/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"
#include "shmem_mutex.h"
#include "allocator/memalloc.h"
#include "shmem/api.h"

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

/*
 * -- API --------------------------------------------------------------------
 * This section defines the public memory management API for OpenSHMEM, including
 * functions for memory allocation, reallocation, and freeing of symmetric memory.
 * These functions also ensure proper synchronization across PEs using barriers.
 */

#ifdef ENABLE_PSHMEM
#pragma weak shmem_malloc = pshmem_malloc
#define shmem_malloc pshmem_malloc
#pragma weak shmem_malloc_with_hints = pshmem_malloc_with_hints
#define shmem_malloc_with_hints pshmem_malloc_with_hints
#pragma weak shmem_calloc = pshmem_calloc
#define shmem_calloc pshmem_calloc
#pragma weak shmem_free = pshmem_free
#define shmem_free pshmem_free
#pragma weak shmem_realloc = pshmem_realloc
#define shmem_realloc pshmem_realloc
#pragma weak shmem_align = pshmem_align
#define shmem_align pshmem_align
#endif /* ENABLE_PSHMEM */

/*
 * Internal function to allocate memory.
 * This function is protected by a mutex to ensure thread-safety.
 * A barrier ensures all PEs have completed memory operations before proceeding.
 */
inline static void *
shmem_malloc_private(size_t s)
{
    void *addr;

    if (shmemu_unlikely(s == 0)) {
        return NULL;
    }

    SHMEMT_MUTEX_PROTECT(addr = shmema_malloc(s));

    shmem_barrier_all();

    SHMEMU_CHECK_ALLOC(addr, s);  /* Check if allocation was successful */

    return addr;
}

/*
 * Public routine to allocate memory in symmetric space.
 * Logs the size and address of the allocated memory.
 */
void *
shmem_malloc(size_t s)
{
    void *addr;

    addr = shmem_malloc_private(s);

    logger(LOG_MEMORY,
           "%s(size=%lu) -> %p",
           __func__,
           (unsigned long) s, addr
           );

    return addr;
}

/*
 * Allocates memory with hints (currently unused).
 * Calls the internal memory allocation function and logs the result.
 */
void *
shmem_malloc_with_hints(size_t s, long hints)
{
    void *addr;

    NO_WARN_UNUSED(hints);  /* Ignore hints for now */

    addr = shmem_malloc_private(s);

    logger(LOG_MEMORY,
           "%s(size=%lu) -> %p",
           __func__,
           (unsigned long) s, addr
           );

    return addr;
}

/*
 * Public routine to allocate zero-initialized memory.
 * Calls the calloc function and synchronizes across PEs with a barrier.
 * Logs the allocated memory details.
 */
void *
shmem_calloc(size_t n, size_t s)
{
    void *addr;

    if (shmemu_unlikely((n == 0) || (s == 0))) {
        return NULL;
    }

    SHMEMT_MUTEX_PROTECT(addr = shmema_calloc(n, s));

    shmem_barrier_all();

    logger(LOG_MEMORY,
           "%s(count=%lu, size=%lu) -> %p",
           __func__,
           (unsigned long) n, (unsigned long) s, addr
           );

    SHMEMU_CHECK_ALLOC(addr, s);  /* Check if allocation was successful */

    return addr;
}

/*
 * Public routine to free symmetric memory.
 * A barrier ensures all PEs have completed their operations before freeing the memory.
 */
void
shmem_free(void *p)
{
    shmem_barrier_all();

    SHMEMT_MUTEX_PROTECT(shmema_free(p));

    logger(LOG_MEMORY, "%s(addr=%p)", __func__, p);
}

/*
 * Public routine to reallocate memory.
 * A barrier is used before and after reallocation to ensure memory consistency
 * across PEs, as realloc may cause memory to move.
 * Logs the original and new memory addresses.
 */
void *
shmem_realloc(void *p, size_t s)
{
    void *addr;

    if (shmemu_unlikely(s == 0)) {
        return NULL;
    }

    shmem_barrier_all();

    SHMEMT_MUTEX_PROTECT(addr = shmema_realloc(p, s));

    shmem_barrier_all();

    logger(LOG_MEMORY,
           "%s(addr=%p, size=%lu) -> %p",
           __func__,
           p, (unsigned long) s, addr
           );

    SHMEMU_CHECK_ALLOC(addr, s);  /* Check if allocation was successful */

    return addr;
}

/*
 * Public routine to allocate aligned memory.
 * Ensures memory alignment based on the specified size and alignment.
 * Logs the alignment, size, and address of the allocated memory.
 */
void *
shmem_align(size_t a, size_t s)
{
    void *addr;

    if (shmemu_unlikely(s == 0)) {
        return NULL;
    }

    SHMEMT_MUTEX_PROTECT(addr = shmema_align(a, s));

    shmem_barrier_all();

    logger(LOG_MEMORY,
           "%s(align=%lu, size=%lu) -> %p",
           __func__,
           (unsigned long) a, (unsigned long) s, addr
           );

    SHMEMU_CHECK_ALLOC(addr, s);  /* Check if allocation was successful */

    return addr;
}
