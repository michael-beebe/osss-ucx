/* For license: see LICENSE file at top-level */

/*
 * This file defines a table-based registry system for the collective
 * operations available in SHCOLL (SHMEM collectives). The operations are
 * organized into tables of known algorithms for various collective
 * operations like all-to-all, broadcast, and barrier.
 */

#include "shcoll.h"
#include "table.h"

#include <stdio.h>
#include <string.h>

/* Macros for registering collective algorithms */

/* Register a sized algorithm (32-bit and 64-bit variants) */
#define SIZED_REG(_typename, _algo)                \
    { #_algo,                                      \
      shcoll_##_typename##32##_##_algo,            \
      shcoll_##_typename##64##_##_algo }

/* End of a sized operation table */
#define SIZED_LAST                                 \
    { "", NULL, NULL }

/* Register an unsized algorithm (no 32/64-bit distinction) */
#define UNSIZED_REG(_typename, _algo)              \
    { #_algo, shcoll_##_typename##_##_algo }

/* End of an unsized operation table */
#define UNSIZED_LAST                               \
    { "", NULL }

/* Register a typed algorithm for all-to-all */
#define UNSIZED_TYPED_REG(_typename, _type, _algo) \
    { #_algo, #_type, shcoll_##_type##_##_typename##_##_algo }

/* End of a typed unsized operation table */
#define UNSIZED_TYPED_LAST                         \
    { "", "", NULL }

/* All-to-all typed algorithm table */
static unsized_typed_op_t alltoall_tab[] = {
    UNSIZED_TYPED_REG(alltoall, float, shift_exchange_barrier),
    UNSIZED_TYPED_REG(alltoall, double, shift_exchange_barrier),
    UNSIZED_TYPED_REG(alltoall, int, shift_exchange_barrier),

    UNSIZED_TYPED_REG(alltoall, float, shift_exchange_counter),
    UNSIZED_TYPED_REG(alltoall, double, shift_exchange_counter),
    UNSIZED_TYPED_REG(alltoall, int, shift_exchange_counter),

    UNSIZED_TYPED_REG(alltoall, float, shift_exchange_signal),
    UNSIZED_TYPED_REG(alltoall, double, shift_exchange_signal),
    UNSIZED_TYPED_REG(alltoall, int, shift_exchange_signal),

    UNSIZED_TYPED_REG(alltoall, float, xor_pairwise_exchange_barrier),
    UNSIZED_TYPED_REG(alltoall, double, xor_pairwise_exchange_barrier),
    UNSIZED_TYPED_REG(alltoall, int, xor_pairwise_exchange_barrier),
    
    UNSIZED_TYPED_REG(alltoall, float, xor_pairwise_exchange_counter),
    UNSIZED_TYPED_REG(alltoall, double, xor_pairwise_exchange_counter),
    UNSIZED_TYPED_REG(alltoall, int, xor_pairwise_exchange_counter),
    
    UNSIZED_TYPED_REG(alltoall, float, xor_pairwise_exchange_signal),
    UNSIZED_TYPED_REG(alltoall, double, xor_pairwise_exchange_signal),
    UNSIZED_TYPED_REG(alltoall, int, xor_pairwise_exchange_signal),

    UNSIZED_TYPED_LAST
};

/* Broadcast algorithms */
static sized_op_t broadcast_tab[] = {
    SIZED_REG(broadcast, linear),
    SIZED_REG(broadcast, complete_tree),
    SIZED_REG(broadcast, binomial_tree),
    SIZED_REG(broadcast, knomial_tree),
    SIZED_REG(broadcast, knomial_tree_signal),
    SIZED_REG(broadcast, scatter_collect),
    SIZED_LAST
};

/* All-to-all strided algorithms */
static sized_op_t alltoalls_tab[] = {
    SIZED_REG(alltoalls, shift_exchange_barrier),
    SIZED_REG(alltoalls, shift_exchange_counter),
    SIZED_REG(alltoalls, shift_exchange_barrier_nbi),
    SIZED_REG(alltoalls, shift_exchange_counter_nbi),
    SIZED_REG(alltoalls, xor_pairwise_exchange_barrier),
    SIZED_REG(alltoalls, xor_pairwise_exchange_counter),
    SIZED_REG(alltoalls, xor_pairwise_exchange_barrier_nbi),
    SIZED_REG(alltoalls, xor_pairwise_exchange_counter_nbi),
    SIZED_REG(alltoalls, color_pairwise_exchange_barrier),
    SIZED_REG(alltoalls, color_pairwise_exchange_counter),
    SIZED_REG(alltoalls, color_pairwise_exchange_barrier_nbi),
    SIZED_REG(alltoalls, color_pairwise_exchange_counter_nbi),
    SIZED_LAST
};

/* Collect algorithms */
static sized_op_t collect_tab[] = {
    SIZED_REG(collect, linear),
    SIZED_REG(collect, all_linear),
    SIZED_REG(collect, all_linear1),
    SIZED_REG(collect, rec_dbl),
    SIZED_REG(collect, rec_dbl_signal),
    SIZED_REG(collect, ring),
    SIZED_REG(collect, bruck),
    SIZED_REG(collect, bruck_no_rotate),
    SIZED_LAST
};

/* Familiar collect algorithms */
static sized_op_t fcollect_tab[] = {
    SIZED_REG(fcollect, linear),
    SIZED_REG(fcollect, all_linear),
    SIZED_REG(fcollect, all_linear1),
    SIZED_REG(fcollect, rec_dbl),
    SIZED_REG(fcollect, ring),
    SIZED_REG(fcollect, bruck),
    SIZED_REG(fcollect, bruck_no_rotate),
    SIZED_REG(fcollect, bruck_signal),
    SIZED_REG(fcollect, bruck_inplace),
    SIZED_REG(fcollect, neighbor_exchange),
    SIZED_LAST
};

/* Barrier_all algorithms (not size-dependent) */
static unsized_op_t barrier_all_tab[] = {
    UNSIZED_REG(barrier_all, linear),
    UNSIZED_REG(barrier_all, complete_tree),
    UNSIZED_REG(barrier_all, binomial_tree),
    UNSIZED_REG(barrier_all, knomial_tree),
    UNSIZED_REG(barrier_all, dissemination),
    UNSIZED_LAST
};

/* Sync_all algorithms (not size-dependent) */
static unsized_op_t sync_all_tab[] = {
    UNSIZED_REG(sync_all, linear),
    UNSIZED_REG(sync_all, complete_tree),
    UNSIZED_REG(sync_all, binomial_tree),
    UNSIZED_REG(sync_all, knomial_tree),
    UNSIZED_REG(sync_all, dissemination),
    UNSIZED_LAST
};

/* Barrier algorithms (not size-dependent) */
static unsized_op_t barrier_tab[] = {
    UNSIZED_REG(barrier, linear),
    UNSIZED_REG(barrier, complete_tree),
    UNSIZED_REG(barrier, binomial_tree),
    UNSIZED_REG(barrier, knomial_tree),
    UNSIZED_REG(barrier, dissemination),
    UNSIZED_LAST
};

/* Sync algorithms (not size-dependent) */
static unsized_op_t sync_tab[] = {
    UNSIZED_REG(sync, linear),
    UNSIZED_REG(sync, complete_tree),
    UNSIZED_REG(sync, binomial_tree),
    UNSIZED_REG(sync, knomial_tree),
    UNSIZED_REG(sync, dissemination),
    UNSIZED_LAST
};

/* Function to register a sized operation */
static int register_sized(sized_op_t *tabp, const char *op, coll_fn_t *fn32, coll_fn_t *fn64)
{
    for (sized_op_t *p = tabp; p->f32 != NULL; ++p) {
        if (strncmp(op, p->op, COLL_NAME_MAX) == 0) {
            *fn32 = p->f32;
            *fn64 = p->f64;
            return 0;
        }
    }
    return -1;
}

/* Function to register an unsized operation */
static int register_unsized(unsized_op_t *tabp, const char *op, coll_fn_t *fn)
{
    for (unsized_op_t *p = tabp; p->f != NULL; ++p) {
        if (strncmp(op, p->op, COLL_NAME_MAX) == 0) {
            *fn = p->f;
            return 0;
        }
    }
    return -1;
}

/* Function to register a typed unsized operation */
static int register_unsized_typed(unsized_typed_op_t *tabp, const char *op, const char *type, coll_fn_t *fn)
{
    for (unsized_typed_op_t *p = tabp; p->f != NULL; ++p) {
        if (strncmp(op, p->op, COLL_NAME_MAX) == 0 && strncmp(type, p->type, COLL_NAME_MAX) == 0) {
            *fn = p->f;
            return 0;
        }
    }
    return -1;
}

/* Macro to register all-to-all with typed support */
#define REGISTER_UNSIZED_TYPED(_coll)                                  \
    int register_##_coll(const char *name, const char *type)           \
    {                                                                  \
        return register_unsized_typed(_coll##_tab, name, type, &colls._coll.f); \
    }

/* Register all-to-all operations */
REGISTER_UNSIZED_TYPED(alltoall)

/* Other collective operation registrations remain unchanged... */

/* Macro to register sized collective operations */
#define REGISTER_SIZED(_coll)                                           \
    int register_##_coll(const char *name)                              \
    {                                                                   \
        return register_sized(_coll##_tab, name, &colls._coll.f32, &colls._coll.f64); \
    }

/* Macro to register unsized collective operations */
#define REGISTER_UNSIZED(_coll)                                         \
    int register_##_coll(const char *name)                              \
    {                                                                   \
        return register_unsized(_coll##_tab, name, &colls._coll.f);     \
    }

/* Register other collectives */
REGISTER_SIZED(alltoalls)
REGISTER_SIZED(broadcast)
REGISTER_SIZED(collect)
REGISTER_SIZED(fcollect)
REGISTER_UNSIZED(barrier)
REGISTER_UNSIZED(barrier_all)
REGISTER_UNSIZED(sync)
REGISTER_UNSIZED(sync_all)

/*
 * TODO: Implement reductions
 */
