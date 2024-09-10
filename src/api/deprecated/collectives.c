// /* For license: see LICENSE file at top-level */

// /*
//  * This file provides the SHMEM API functions for collective operations
//  * and dispatches them to the appropriate internal implementations.
//  * The functions in this file interface with the SHMEM user-level API
//  * and map the API calls to the corresponding internal collective operations.
//  */

// #include "thispe.h"
// #include "shmemu.h"
// #include "collectives/table.h"

// /*
//  * Macro to register a collective operation.
//  * If the registration fails (s != 0), the program will terminate with an error.
//  */
// #define TRY(_cname)                                             \
//     {                                                           \
//         const int s = register_##_cname(proc.env.coll._cname);  \
//                                                                 \
//         if (s != 0) {                                           \
//             shmemu_fatal("couldn't register collective "        \
//                          "\"%s\" (s = %d)",                     \
//                          #_cname, s);                           \
//         }                                                       \
//     }

// /*
//  * Initialization function for collective operations.
//  * Registers the default algorithms for each collective operation.
//  */
// void
// collectives_init(void)
// {
//     TRY(alltoall);       /* Register the all-to-all operation */
//     TRY(alltoalls);      /* Register the all-to-all strided operation */
//     TRY(collect);        /* Register the collect operation */
//     TRY(fcollect);       /* Register the familiar collect operation */
//     TRY(barrier);        /* Register the barrier operation */
//     TRY(barrier_all);    /* Register the barrier_all operation */
//     TRY(sync);           /* Register the sync operation */
//     TRY(sync_all);       /* Register the sync_all operation */
//     TRY(broadcast);      /* Register the broadcast operation */

//     /* TODO: Register reduction operations (e.g., sum, max) */
// }

// /*
//  * Finalization function for collective operations.
//  * Currently, it does nothing but is a placeholder for future clean-up operations.
//  */
// void
// collectives_finalize(void)
// {
//     return;
// }

// /*
//  * The following sections provide the SHMEM API functions for various
//  * collective operations. Each function logs its invocation and then
//  * dispatches the call to the corresponding internal implementation
//  * via the `colls` structure.
//  */

// #ifdef ENABLE_PSHMEM
// #pragma weak shmem_alltoall32 = pshmem_alltoall32
// #define shmem_alltoall32 pshmem_alltoall32
// #pragma weak shmem_alltoall64 = pshmem_alltoall64
// #define shmem_alltoall64 pshmem_alltoall64
// #endif /* ENABLE_PSHMEM */

// /*
//  * SHMEM all-to-all communication for 32-bit data.
//  * Distributes data from all PEs to all other PEs.
//  */
// void
// shmem_alltoall32(void *target, const void *source, size_t nelems,
//                  int PE_start, int logPE_stride, int PE_size, long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%p, %p, %lu, %d, %d, %d, %p)",
//            __func__,
//            target, source, nelems, PE_start, logPE_stride, PE_size, pSync);

//     colls.alltoall.f32(target, source, nelems,
//                        PE_start, logPE_stride, PE_size, pSync);
// }

// /*
//  * SHMEM all-to-all communication for 64-bit data.
//  * Distributes data from all PEs to all other PEs.
//  */
// void
// shmem_alltoall64(void *target, const void *source, size_t nelems,
//                  int PE_start, int logPE_stride, int PE_size, long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%p, %p, %lu, %d, %d, %d, %p)",
//            __func__,
//            target, source, nelems, PE_start, logPE_stride, PE_size, pSync);

//     colls.alltoall.f64(target, source, nelems,
//                        PE_start, logPE_stride, PE_size, pSync);
// }

// #ifdef ENABLE_PSHMEM
// #pragma weak shmem_alltoalls32 = pshmem_alltoalls32
// #define shmem_alltoalls32 pshmem_alltoalls32
// #pragma weak shmem_alltoalls64 = pshmem_alltoalls64
// #define shmem_alltoalls64 pshmem_alltoalls64
// #endif /* ENABLE_PSHMEM */

// /*
//  * SHMEM all-to-all strided communication for 32-bit data.
//  * Similar to all-to-all but allows strided access to data.
//  */
// void
// shmem_alltoalls32(void *target, const void *source,
//                   ptrdiff_t dst, ptrdiff_t sst, size_t nelems,
//                   int PE_start, int logPE_stride, int PE_size,
//                   long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%p, %p, %lu, %lu, %lu, %d, %d, %d, %p)",
//            __func__,
//            target, source,
//            dst, sst, nelems,
//            PE_start, logPE_stride, PE_size,
//            pSync);

//     colls.alltoalls.f32(target, source,
//                         dst, sst, nelems,
//                         PE_start, logPE_stride, PE_size,
//                         pSync);
// }

// /*
//  * SHMEM all-to-all strided communication for 64-bit data.
//  * Similar to all-to-all but allows strided access to data.
//  */
// void
// shmem_alltoalls64(void *target, const void *source,
//                   ptrdiff_t dst, ptrdiff_t sst, size_t nelems,
//                   int PE_start, int logPE_stride, int PE_size,
//                   long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%p, %p, %lu, %lu, %lu, %d, %d, %d, %p)",
//            __func__,
//            target, source,
//            dst, sst, nelems,
//            PE_start, logPE_stride, PE_size,
//            pSync);

//     colls.alltoalls.f64(target, source,
//                         dst, sst, nelems,
//                         PE_start, logPE_stride, PE_size,
//                         pSync);
// }

// #ifdef ENABLE_PSHMEM
// #pragma weak shmem_collect32 = pshmem_collect32
// #define shmem_collect32 pshmem_collect32
// #pragma weak shmem_collect64 = pshmem_collect64
// #define shmem_collect64 pshmem_collect64
// #endif /* ENABLE_PSHMEM */

// /*
//  * SHMEM collect operation for 32-bit data.
//  * Gathers data from all PEs into a single target buffer.
//  */
// void
// shmem_collect32(void *target, const void *source, size_t nelems,
//                 int PE_start, int logPE_stride, int PE_size, long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%p, %p, %lu, %d, %d, %d, %p)",
//            __func__,
//            target, source, nelems, PE_start, logPE_stride, PE_size, pSync);

//     colls.collect.f32(target, source, nelems,
//                       PE_start, logPE_stride, PE_size, pSync);
// }

// /*
//  * SHMEM collect operation for 64-bit data.
//  * Gathers data from all PEs into a single target buffer.
//  */
// void
// shmem_collect64(void *target, const void *source, size_t nelems,
//                 int PE_start, int logPE_stride, int PE_size, long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%p, %p, %lu, %d, %d, %d, %p)",
//            __func__,
//            target, source, nelems, PE_start, logPE_stride, PE_size, pSync);

//     colls.collect.f64(target, source, nelems,
//                       PE_start, logPE_stride, PE_size, pSync);
// }

// #ifdef ENABLE_PSHMEM
// #pragma weak shmem_fcollect32 = pshmem_fcollect32
// #define shmem_fcollect32 pshmem_fcollect32
// #pragma weak shmem_fcollect64 = pshmem_fcollect64
// #define shmem_fcollect64 pshmem_fcollect64
// #endif /* ENABLE_PSHMEM */

// /*
//  * SHMEM familiar collect operation for 32-bit data.
//  * Similar to collect but with certain familiar properties.
//  */
// void
// shmem_fcollect32(void *target, const void *source, size_t nelems,
//                  int PE_start, int logPE_stride, int PE_size, long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%p, %p, %lu, %d, %d, %d, %p)",
//            __func__,
//            target, source, nelems, PE_start, logPE_stride, PE_size, pSync);

//     colls.fcollect.f32(target, source, nelems,
//                        PE_start, logPE_stride, PE_size, pSync);
// }

// /*
//  * SHMEM familiar collect operation for 64-bit data.
//  * Similar to collect but with certain familiar properties.
//  */
// void
// shmem_fcollect64(void *target, const void *source, size_t nelems,
//                  int PE_start, int logPE_stride, int PE_size, long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%p, %p, %lu, %d, %d, %d, %p)",
//            __func__,
//            target, source, nelems, PE_start, logPE_stride, PE_size, pSync);

//     colls.fcollect.f64(target, source, nelems,
//                        PE_start, logPE_stride, PE_size, pSync);
// }

// #ifdef ENABLE_PSHMEM
// #pragma weak shmem_barrier = pshmem_barrier
// #define shmem_barrier pshmem_barrier
// #endif /* ENABLE_PSHMEM */

// /*
//  * SHMEM barrier operation.
//  * Synchronizes all PEs in the specified set.
//  */
// void
// shmem_barrier(int PE_start, int logPE_stride, int PE_size, long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%d, %d, %d, %p)",
//            __func__,
//            PE_start, logPE_stride, PE_size, pSync);

//     colls.barrier.f(PE_start, logPE_stride, PE_size, pSync);
// }

// /*
//  * External synchronization variables used in barrier and sync operations.
//  */
// extern long *shmemc_barrier_all_psync;
// extern long *shmemc_sync_all_psync;

// #ifdef ENABLE_PSHMEM
// #pragma weak shmem_barrier_all = pshmem_barrier_all
// #define shmem_barrier_all pshmem_barrier_all
// #endif /* ENABLE_PSHMEM */

// /*
//  * SHMEM barrier_all operation.
//  * Synchronizes all PEs in the program.
//  */
// void
// shmem_barrier_all(void)
// {
//     logger(LOG_COLLECTIVES, "%s()", __func__);

//     colls.barrier_all.f(shmemc_barrier_all_psync);
// }

// #ifdef ENABLE_PSHMEM
// #pragma weak shmem_sync = pshmem_sync
// #define shmem_sync pshmem_sync
// #endif /* ENABLE_PSHMEM */

// /*
//  * SHMEM sync operation.
//  * Synchronizes a subset of PEs.
//  */
// void
// shmem_sync(int PE_start, int logPE_stride, int PE_size, long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%d, %d, %d, %p)",
//            __func__,
//            PE_start, logPE_stride, PE_size, pSync);

//     colls.sync.f(PE_start, logPE_stride, PE_size, pSync);
// }

// #ifdef ENABLE_PSHMEM
// #pragma weak shmem_sync_all = pshmem_sync_all
// #define shmem_sync_all pshmem_sync_all
// #endif /* ENABLE_PSHMEM */

// /*
//  * SHMEM sync_all operation.
//  * Synchronizes all PEs in the program.
//  */
// void
// shmem_sync_all(void)
// {
//     logger(LOG_COLLECTIVES, "%s()", __func__);

//     colls.sync_all.f(shmemc_sync_all_psync);
// }

// #ifdef ENABLE_PSHMEM
// #pragma weak shmem_broadcast32 = pshmem_broadcast32
// #define shmem_broadcast32 pshmem_broadcast32
// #pragma weak shmem_broadcast64 = pshmem_broadcast64
// #define shmem_broadcast64 pshmem_broadcast64
// #endif /* ENABLE_PSHMEM */

// /*
//  * SHMEM broadcast operation for 32-bit data.
//  * Broadcasts data from a root PE to all other PEs.
//  */
// void
// shmem_broadcast32(void *target, const void *source,
//                   size_t nelems, int PE_root, int PE_start,
//                   int logPE_stride, int PE_size, long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%p, %p, %lu, %d, %d, %d, %p)",
//            __func__,
//            target, source, nelems, PE_start, logPE_stride, PE_size, pSync);

//     colls.broadcast.f32(target, source,
//                         nelems, PE_root, PE_start,
//                         logPE_stride, PE_size, pSync);
// }

// /*
//  * SHMEM broadcast operation for 64-bit data.
//  * Broadcasts data from a root PE to all other PEs.
//  */
// void
// shmem_broadcast64(void *target, const void *source,
//                   size_t nelems, int PE_root, int PE_start,
//                   int logPE_stride, int PE_size, long *pSync)
// {
//     logger(LOG_COLLECTIVES,
//            "%s(%p, %p, %lu, %d, %d, %d, %p)",
//            __func__,
//            target, source, nelems, PE_start, logPE_stride, PE_size, pSync);

//     colls.broadcast.f64(target, source,
//                         nelems, PE_root, PE_start,
//                         logPE_stride, PE_size, pSync);
// }

// /*
//  * -- WIP ----------------------------------------------------------
//  */

// #include "collectives/reductions.h"

// /*
//  * Placeholder for reduction operations.
//  * This section of the code is work-in-progress (WIP) and
//  * implements various reduction algorithms (e.g., linear, binomial).
//  */
// SHIM_REDUCE_ALL(rec_dbl)
