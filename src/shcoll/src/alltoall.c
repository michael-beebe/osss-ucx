/*
 * For license: see LICENSE file at top-level
 */

#include "shcoll.h"
#include "shcoll/compat.h"

#include <string.h>
#include <limits.h>
#include <assert.h>
#include <shmem.h>  // Include this header to resolve the shmem_team_sync function declaration

/*
 * edge_color function calculates the peer index based on the number of 
 * PEs (Processing Elements) involved and their indices. This is used 
 * in color-pairwise exchange algorithms to determine the communication
 * pattern between PEs.
 */
inline static int
edge_color(int i, int me, int npes)
{
    int chr_idx;
    int v;

    // Adjust chr_idx for an odd number of PEs
    chr_idx = npes % 2 == 1 ? npes : npes - 1;

    // Calculate peer index for PEs within the chr_idx range
    if (me < chr_idx) {
        v = (i + chr_idx - me) % chr_idx;
    } else {
        v = i % 2 == 1 ? (((i + chr_idx) / 2) % chr_idx) : i / 2;
    }

    // Handle the case when the number of PEs is odd
    if (npes % 2 == 1 && v == me) {
        return -1;
    } else if (v == me) {
        return chr_idx;
    } else {
        return v;
    }
}

/* Number of rounds after which synchronization will occur during alltoall operations */
static int alltoall_rounds_sync = INT32_MAX;

/*
 * Allows setting the number of synchronization rounds for alltoalls operations.
 * Used to fine-tune performance or ensure periodic synchronization.
 */
void
shcoll_set_alltoalls_round_sync(int rounds_sync)
{
    alltoall_rounds_sync = rounds_sync;
}

////////////////////////////////////////////////////////////////////////////////////////
/*
 * ALLTOALL_HELPER_BARRIER_DEFINITION defines a helper function for 
 * all-to-all operations using a barrier synchronization method.
 * The _algo, _peer, and _cond parameters specify the operation name, 
 * peer selection function, and a condition to assert during the operation.
 */
#define ALLTOALL_HELPER_BARRIER_DEFINITION(_typename, _type, _algo, _peer, _cond) \
    inline static void                                                           \
    alltoall_helper_##_typename##_alltoall_##_algo(shmem_team_t team,             \
                                                   _type *dest,                  \
                                                   const _type *source,          \
                                                   size_t nelems) {              \
        const int stride = shmem_team_translate_pe(team, 1, team);               \
        const int me = shmem_my_pe();                                            \
        const int team_size = shmem_team_n_pes(team);                            \
                                                                                 \
        /* Get my index in the active set */                                     \
        const int me_as = shmem_team_translate_pe(team, me, team);               \
                                                                                 \
        _type *const dest_ptr = &dest[me_as * nelems];                           \
        const _type *source_ptr = &source[me_as * nelems];                       \
                                                                                 \
        int i;                                                                   \
        int peer_as;                                                             \
                                                                                 \
        /* Assert the provided condition */                                      \
        assert(_cond);                                                           \
                                                                                 \
        /* Copy data for the current PE */                                       \
        memcpy(dest_ptr, source_ptr, nelems * sizeof(_type));                    \
                                                                                 \
        /* Loop through the other PEs and exchange data */                       \
        for (i = 1; i < team_size; i++) {                                        \
            peer_as = _peer(i, me_as, team_size);                                \
            source_ptr = &source[peer_as * nelems];                              \
                                                                                 \
            /* Non-blocking put operation */                                     \
            shmem_putmem_nbi(dest_ptr, source_ptr, nelems * sizeof(_type),       \
                             shmem_team_translate_pe(team, peer_as, team));      \
                                                                                 \
            /* Synchronize after a set number of rounds */                       \
            if (i % alltoall_rounds_sync == 0) {                                 \
                shmem_team_sync(team);                                           \
            }                                                                    \
        }                                                                        \
                                                                                 \
        /* Final synchronization barrier */                                      \
        shmem_team_sync(team);                                                   \
    }

/*
 * ALLTOALL_HELPER_COUNTER_DEFINITION defines a helper function for 
 * all-to-all operations using a counter-based synchronization.
 */
#define ALLTOALL_HELPER_COUNTER_DEFINITION(_typename, _type, _algo, _peer, _cond) \
    inline static void                                                           \
    alltoall_helper_##_typename##_alltoall_##_algo(shmem_team_t team,             \
                                                   _type *dest,                  \
                                                   const _type *source,          \
                                                   size_t nelems) {              \
        const int stride = shmem_team_translate_pe(team, 1, team);               \
        const int me = shmem_my_pe();                                            \
        const int team_size = shmem_team_n_pes(team);                            \
                                                                                 \
        /* Get my index in the active set */                                     \
        const int me_as = shmem_team_translate_pe(team, me, team);               \
                                                                                 \
        _type *const dest_ptr = &dest[me_as * nelems];                           \
        const _type *source_ptr = NULL;                                          \
                                                                                 \
        int i;                                                                   \
        int peer_as;                                                             \
                                                                                 \
        /* Assert the provided condition */                                      \
        assert(_cond);                                                           \
                                                                                 \
        /* Loop through the other PEs and exchange data */                       \
        for (i = 1; i < team_size; i++) {                                        \
            peer_as = _peer(i, me_as, team_size);                                \
            source_ptr = &source[peer_as * nelems];                              \
                                                                                 \
            /* Non-blocking put operation */                                     \
            shmem_putmem_nbi(dest_ptr, source_ptr, nelems * sizeof(_type),       \
                             shmem_team_translate_pe(team, peer_as, team));      \
        }                                                                        \
                                                                                 \
        /* Copy data for the current PE */                                       \
        source_ptr = &source[me_as * nelems];                                    \
        memcpy(dest_ptr, source_ptr, nelems * sizeof(_type));                    \
                                                                                 \
        /* Fence ensures memory consistency across all PEs */                    \
        shmem_fence();                                                           \
                                                                                 \
        /* Increment the counter for each PE */                                  \
        for (i = 1; i < team_size; i++) {                                        \
            peer_as = _peer(i, me_as, team_size);                                \
            shmem_long_atomic_inc(NULL, shmem_team_translate_pe(team, peer_as, team)); \
        }                                                                        \
                                                                                 \
        /* Wait until all increments are complete */                             \
        shmem_team_sync(team);                                                   \
    }

/*
 * ALLTOALL_HELPER_SIGNAL_DEFINITION defines a helper function for 
 * all-to-all operations using a signal-based synchronization.
 */
#define ALLTOALL_HELPER_SIGNAL_DEFINITION(_typename, _type, _algo, _peer, _cond)  \
    inline static void                                                           \
    alltoall_helper_##_typename##_alltoall_##_algo(shmem_team_t team,             \
                                                   _type *dest,                  \
                                                   const _type *source,          \
                                                   size_t nelems) {              \
        const int stride = shmem_team_translate_pe(team, 1, team);               \
        const int me = shmem_my_pe();                                            \
        const int team_size = shmem_team_n_pes(team);                            \
                                                                                 \
        /* Get my index in the active set */                                     \
        const int me_as = shmem_team_translate_pe(team, me, team);               \
                                                                                 \
        _type *const dest_ptr = &dest[me_as * nelems];                           \
        const _type *source_ptr = NULL;                                          \
                                                                                 \
        int i;                                                                   \
        int peer_as;                                                             \
                                                                                 \
        /* Assert the provided condition */                                      \
        assert(_cond);                                                           \
                                                                                 \
        /* Loop through the other PEs and exchange data */                       \
        for (i = 1; i < team_size; i++) {                                        \
            peer_as = _peer(i, me_as, team_size);                                \
            source_ptr = &source[peer_as * nelems];                              \
                                                                                 \
            /* Non-blocking put operation with signal */                         \
            shmem_putmem_signal_nb(dest_ptr, source_ptr, nelems * sizeof(_type), \
                                   NULL, 1, shmem_team_translate_pe(team, peer_as, team), NULL); \
        }                                                                        \
                                                                                 \
        /* Copy data for the current PE */                                       \
        source_ptr = &source[me_as * nelems];                                    \
        memcpy(dest_ptr, source_ptr, nelems * sizeof(_type));                    \
                                                                                 \
        /* Wait for signals from other PEs */                                    \
        shmem_team_sync(team);                                                   \
    }

/* Corrected shift exchange, XOR, and color-based algorithm peer definitions */

/* Shift exchange peer calculation */
#define SHIFT_PEER(I, ME, NPES) (((ME) + (I)) % (NPES))
ALLTOALL_HELPER_BARRIER_DEFINITION(shift_exchange, float, barrier, SHIFT_PEER, 1)
ALLTOALL_HELPER_COUNTER_DEFINITION(shift_exchange, float, counter, SHIFT_PEER, 1)
ALLTOALL_HELPER_SIGNAL_DEFINITION(shift_exchange, float, signal, SHIFT_PEER, team_size - 1 <= SHCOLL_ALLTOALL_SYNC_SIZE)

/* XOR peer calculation */
#define XOR_PEER(I, ME, NPES) ((I) ^ (ME))
ALLTOALL_HELPER_BARRIER_DEFINITION(xor_pairwise_exchange, float, barrier, XOR_PEER, 1)
ALLTOALL_HELPER_COUNTER_DEFINITION(xor_pairwise_exchange, float, counter, XOR_PEER, 1)
ALLTOALL_HELPER_SIGNAL_DEFINITION(xor_pairwise_exchange, float, signal, XOR_PEER, (team_size - 1 <= SHCOLL_ALLTOALL_SYNC_SIZE))

/* Color-based peer calculation */
#define COLOR_PEER(I, ME, NPES) edge_color(I, ME, NPES)
ALLTOALL_HELPER_BARRIER_DEFINITION(color_pairwise_exchange, float, barrier, COLOR_PEER, 1)
ALLTOALL_HELPER_COUNTER_DEFINITION(color_pairwise_exchange, float, counter, COLOR_PEER, 1)
ALLTOALL_HELPER_SIGNAL_DEFINITION(color_pairwise_exchange, float, signal, COLOR_PEER, (team_size - 1 <= SHCOLL_ALLTOALL_SYNC_SIZE))

// @formatter:on

/*
 * Macro to define the alltoall functions for different types and algorithms.
 */
#define SHCOLL_ALLTOALL_DEFINITION(_typename, _type, _algo)                      \
    void                                                                         \
    shcoll_##_typename##_alltoall_##_algo(shmem_team_t team,                     \
                                          _type *dest,                           \
                                          const _type *source,                   \
                                          size_t nelems) {                       \
        alltoall_helper_##_typename##_alltoall_##_algo(team, dest, source, nelems); \
    }

// @formatter:off

/* Define alltoall functions for shift exchange algorithms */
SHCOLL_ALLTOALL_DEFINITION(float, float, shift_exchange_barrier)
SHCOLL_ALLTOALL_DEFINITION(double, double, shift_exchange_barrier)
SHCOLL_ALLTOALL_DEFINITION(int, int, shift_exchange_barrier)

SHCOLL_ALLTOALL_DEFINITION(float, float, shift_exchange_counter)
SHCOLL_ALLTOALL_DEFINITION(double, double, shift_exchange_counter)
SHCOLL_ALLTOALL_DEFINITION(int, int, shift_exchange_counter)

SHCOLL_ALLTOALL_DEFINITION(float, float, shift_exchange_signal)
SHCOLL_ALLTOALL_DEFINITION(double, double, shift_exchange_signal)
SHCOLL_ALLTOALL_DEFINITION(int, int, shift_exchange_signal)

SHCOLL_ALLTOALL_DEFINITION(float, float, xor_pairwise_exchange_barrier)
SHCOLL_ALLTOALL_DEFINITION(double, double, xor_pairwise_exchange_barrier)
SHCOLL_ALLTOALL_DEFINITION(int, int, xor_pairwise_exchange_barrier)

SHCOLL_ALLTOALL_DEFINITION(float, float, xor_pairwise_exchange_counter)
SHCOLL_ALLTOALL_DEFINITION(double, double, xor_pairwise_exchange_counter)
SHCOLL_ALLTOALL_DEFINITION(int, int, xor_pairwise_exchange_counter)

SHCOLL_ALLTOALL_DEFINITION(float, float, xor_pairwise_exchange_signal)
SHCOLL_ALLTOALL_DEFINITION(double, double, xor_pairwise_exchange_signal)
SHCOLL_ALLTOALL_DEFINITION(int, int, xor_pairwise_exchange_signal)

///////////////////////////////////////////////////////////////////////////////

// @formatter:on
