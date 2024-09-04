/* For license: see LICENSE file at top-level */

#ifndef _COLLECTIVES_DEFAULTS_H
#define _COLLECTIVES_DEFAULTS_H 1

/*
 * This header file defines the default algorithms or methods
 * used for various collective operations in SHMEM.
 *
 * Each collective operation (e.g., alltoall, barrier, broadcast)
 * can be implemented using different algorithms.
 * This file sets the default algorithm to be used for each operation.
 */

/* Default algorithm for the all-to-all communication pattern */
#define COLLECTIVES_DEFAULT_ALLTOALL         "shift_exchange_barrier"

/* Default algorithm for the all-to-all strided communication pattern */
#define COLLECTIVES_DEFAULT_ALLTOALLS        "shift_exchange_barrier"

/* Default algorithm for the barrier synchronization operation */
#define COLLECTIVES_DEFAULT_BARRIER          "binomial_tree"

/* Default algorithm for the barrier_all synchronization operation */
#define COLLECTIVES_DEFAULT_BARRIER_ALL      "binomial_tree"

/* Default algorithm for the sync synchronization operation */
#define COLLECTIVES_DEFAULT_SYNC             "binomial_tree"

/* Default algorithm for the sync_all synchronization operation */
#define COLLECTIVES_DEFAULT_SYNC_ALL         "binomial_tree"

/* Default algorithm for the broadcast operation */
#define COLLECTIVES_DEFAULT_BROADCAST        "binomial_tree"

/* Default algorithm for the collect operation */
#define COLLECTIVES_DEFAULT_COLLECT          "bruck"

/* Default algorithm for the fcollect (familiar collect) operation */
#define COLLECTIVES_DEFAULT_FCOLLECT         "bruck_inplace"

/* Default algorithm for reductions (e.g., sum, max) operations */
#define COLLECTIVES_DEFAULT_REDUCTIONS       "rec_dbl"

#endif /* ! _COLLECTIVES_DEFAULTS_H */
