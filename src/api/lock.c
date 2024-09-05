/* For license: see LICENSE file at top-level */

/*
 * Rewrite of an original MCS lock code by:
 *
 *    Copyright (c) 1996-2002 by Quadrics Supercomputers World Ltd.
 *    Copyright (c) 2003-2005 by Quadrics Ltd.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"
#include "shmem.h"
#include "shmem_mutex.h"

#include <stdint.h>
#include <sys/types.h>

/*
 * This structure overlays an opaque blob we can move around with AMOs,
 * containing the signaling/PE parts.
 *
 * TODO: "short" PE in this setup, should be "int".
 */
enum {
    SHMEM_LOCK_FREE = -1,
    SHMEM_LOCK_RESET = 0,       /* Matches lock initializer in spec */
    SHMEM_LOCK_ACQUIRED
};

typedef union shmem_lock {
    struct data_split {
        int16_t locked;
        int16_t next;
    } d;
    int32_t blob;               /* For AMOs & owner transfer */
} shmem_lock_t;

/*
 * ----------------------------------------
 * Helper routine: get_owner_spread
 * ----------------------------------------
 * Spread lock ownership across PEs based on the address.
 */
inline static int
get_owner_spread(uint64_t addr)
{
    return (addr >> 3) % shmemc_n_pes();
}

/*
 * ----------------------------------------
 * Helper routine: lock_owner
 * ----------------------------------------
 * Determines the PE that owns the lock.
 */
inline static int
lock_owner(void *addr)
{
    const uint64_t la = (const uint64_t) addr;
    int owner;

#ifdef ENABLE_ALIGNED_ADDRESSES
    owner = get_owner_spread(la);
#else
    if (shmemc_global_address(la)) {
        owner = get_owner_spread(la);
    }
    else {
        /* Avoid PE 0 as it is often used for work allocation */
        owner = shmemc_n_pes() - 1;
    }
#endif /* ENABLE_ALIGNED_ADDRESSES */

    return owner;
}

/*
 * ----------------------------------------
 * Lock action helper routines
 * ----------------------------------------
 * These routines facilitate lock manipulation such as requesting or clearing locks.
 */

/*
 * Common lock action routine.
 */
inline static void
try_lock_action(shmem_lock_t *lock, int cond, int value, shmem_lock_t *cmp)
{
    cmp->blob = shmem_int_atomic_compare_swap(&(lock->blob), cond, value, lock_owner(lock));
}

/*
 * Attempt to request a lock.
 */
inline static void
try_request_lock(shmem_lock_t *lock, int me, shmem_lock_t *cmp)
{
    const shmem_lock_t tmp = { .d.locked = SHMEM_LOCK_ACQUIRED, .d.next = me };
    try_lock_action(lock, SHMEM_LOCK_RESET, tmp.blob, cmp);
}

/*
 * Attempt to clear a lock.
 */
inline static void
try_clear_lock(shmem_lock_t *lock, int me, shmem_lock_t *cmp)
{
    const shmem_lock_t tmp = { .d.locked = SHMEM_LOCK_ACQUIRED, .d.next = me };
    try_lock_action(lock, tmp.blob, SHMEM_LOCK_RESET, cmp);
}

/*
 * ----------------------------------------
 * Lock request routines
 * ----------------------------------------
 * These routines handle the lock request phase for each lock operation.
 */

/*
 * Set a lock request.
 */
inline static void
set_lock_request(shmem_lock_t *lock, int me, shmem_lock_t *cmp)
{
    do {
        try_request_lock(lock, me, cmp);
    } while (cmp->blob != SHMEM_LOCK_RESET);
}

/*
 * Test a lock request.
 */
inline static void
test_lock_request(shmem_lock_t *lock, int me, shmem_lock_t *cmp)
{
    try_request_lock(lock, me, cmp);
}

/*
 * Clear a lock request.
 */
inline static void
clear_lock_request(shmem_lock_t *node, shmem_lock_t *lock, int me, shmem_lock_t *cmp)
{
    if (node->d.next == SHMEM_LOCK_FREE) {
        try_clear_lock(lock, me, cmp);
    }
}

/*
 * ----------------------------------------
 * Lock execution routines
 * ----------------------------------------
 * These routines handle the lock execution phase for each lock operation.
 */

/*
 * Execute a lock.
 */
inline static void
set_lock_execute(shmem_lock_t *node, int me, shmem_lock_t *cmp)
{
    node->d.next = SHMEM_LOCK_FREE;

    if (cmp->d.locked == SHMEM_LOCK_ACQUIRED) {
        node->d.locked = SHMEM_LOCK_ACQUIRED;
        shmem_short_p(&(node->d.next), me, cmp->d.next);

        do {
            shmemc_progress();
        } while (node->d.locked == SHMEM_LOCK_ACQUIRED);
    }
}

/*
 * Test a lock execution.
 */
inline static int
test_lock_execute(shmem_lock_t *node, int me, shmem_lock_t *cmp)
{
    if (cmp->blob == SHMEM_LOCK_RESET) {
        set_lock_execute(node, me, cmp);
        return 0;
    }
    return 1;
}

/*
 * Clear a lock execution.
 */
inline static void
clear_lock_execute(shmem_lock_t *node, int me, shmem_lock_t *cmp)
{
    if (cmp->d.next == me) {
        return;
    }

    do {
        shmemc_progress();
    } while (node->d.next == SHMEM_LOCK_FREE);

    shmem_short_p(&(node->d.locked), SHMEM_LOCK_RESET, node->d.next);
}

/*
 * ----------------------------------------
 * Internal blocking calls
 * ----------------------------------------
 * These routines block until the lock is acquired or released.
 */

/*
 * Set a lock.
 */
inline static void
set_lock(shmem_lock_t *node, shmem_lock_t *lock, int me)
{
    shmem_lock_t t;
    set_lock_request(lock, me, &t);
    set_lock_execute(node, me, &t);
}

/*
 * Clear a lock.
 */
inline static void
clear_lock(shmem_lock_t *node, shmem_lock_t *lock, int me)
{
    shmem_lock_t t;
    shmemc_quiet();
    clear_lock_request(node, lock, me, &t);
    clear_lock_execute(node, me, &t);
}

/*
 * Test a lock.
 */
inline static int
test_lock(shmem_lock_t *node, shmem_lock_t *lock, int me)
{
    shmem_lock_t t;
    test_lock_request(lock, me, &t);
    return test_lock_execute(node, me, &t);
}

/*
 * ----------------------------------------
 * API routines
 * ----------------------------------------
 * These routines are part of the OpenSHMEM API for lock handling.
 */

#ifdef ENABLE_PSHMEM
#pragma weak shmem_set_lock = pshmem_set_lock
#define shmem_set_lock pshmem_set_lock
#pragma weak shmem_test_lock = pshmem_test_lock
#define shmem_test_lock pshmem_test_lock
#pragma weak shmem_clear_lock = pshmem_clear_lock
#define shmem_clear_lock pshmem_clear_lock
#endif /* ENABLE_PSHMEM */

#define UNPACK()                                        \
    shmem_lock_t *base = (shmem_lock_t *) lp;           \
    shmem_lock_t *node = base + 1;                      \
    shmem_lock_t *lock = base + 0

/*
 * Routine: shmem_set_lock
 * -----------------------
 * Blocks until the lock is acquired.
 */
void
shmem_set_lock(long *lp)
{
    UNPACK();

    SHMEMU_CHECK_INIT();
    SHMEMU_CHECK_NOT_NULL(lp, 1);
    SHMEMU_CHECK_SYMMETRIC(lp, 1);

    logger(LOG_LOCKS, "%s(lock=%p)", __func__, lock);

    SHMEMT_MUTEX_NOPROTECT(set_lock(node, lock, shmemc_my_pe()));
}

/*
 * Routine: shmem_clear_lock
 * -------------------------
 * Releases the lock.
 */
void
shmem_clear_lock(long *lp)
{
    UNPACK();

    SHMEMU_CHECK_INIT();
    SHMEMU_CHECK_NOT_NULL(lp, 1);
    SHMEMU_CHECK_SYMMETRIC(lp, 1);

    logger(LOG_LOCKS, "%s(lock=%p)", __func__, lock);

    SHMEMT_MUTEX_NOPROTECT(clear_lock(node, lock, shmemc_my_pe()));
}

/*
 * Routine: shmem_test_lock
 * ------------------------
 * Tests if the lock is available. Returns 0 if acquired, 1 otherwise.
 */
int
shmem_test_lock(long *lp)
{
    int ret;
    UNPACK();

    SHMEMU_CHECK_INIT();
    SHMEMU_CHECK_NOT_NULL(lp, 1);
    SHMEMU_CHECK_SYMMETRIC(lp, 1);

    logger(LOG_LOCKS, "%s(lock=%p)", __func__, lock);

    SHMEMT_MUTEX_NOPROTECT(ret = test_lock(node, lock, shmemc_my_pe()));

    return ret;
}
