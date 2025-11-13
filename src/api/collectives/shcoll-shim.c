/**
 * @file shcoll-shim.c
 * @brief Implementation of OpenSHMEM collective operations
 *
 * This file contains the implementation of OpenSHMEM collective operations
 * including:
 * - Initialization and finalization of collectives
 * - All-to-all operations (alltoall, alltoalls)
 * - Collection operations (collect, fcollect)
 * - Barrier and synchronization operations
 * - Broadcast operations
 * - Reduction operations
 */

#include "thispe.h"
#include "shmemu.h"
#include "collectives/table.h"
#include "shmem/teams.h"

#include "shmem/api_types.h"

/**
 * @brief Helper macro to register collective operations
 * @param _cname Name of the collective operation to register
 */
#define TRY(_cname)                                                            \
  {                                                                            \
    const int s = register_##_cname(proc.env.coll._cname);                     \
                                                                               \
    if (s != 0) {                                                              \
      shmemu_fatal("couldn't register collective "                             \
                   "\"%s\" (s = %d)",                                          \
                   #_cname, s);                                                \
    }                                                                          \
  }

/**
 * @brief Helper macro to call a typed collective operation
 * @param CONFIG The collective operation name
 * @param TYPENAME The type name
 * @param ... The arguments to the collective operation
 */
#define TYPED_CALL(CONFIG, TYPENAME, ...)                                      \
  do {                                                                         \
    char opstr[COLL_NAME_MAX * 2];                                             \
    const char *base = proc.env.coll.CONFIG;                                   \
    if (strchr(base, ':') == NULL) {                                           \
      snprintf(opstr, sizeof(opstr), "%s:%s", base, TYPENAME);                 \
    } else {                                                                   \
      strncpy(opstr, base, sizeof(opstr) - 1);                                 \
      opstr[sizeof(opstr) - 1] = '\0';                                         \
    }                                                                          \
    int _rc = register_##CONFIG(opstr);                                        \
    if (_rc != 0) {                                                            \
      shmemu_fatal("couldn't register typed collective '%s' (s = %d)", opstr,  \
                   _rc);                                                       \
    }                                                                          \
    return colls.CONFIG.f(__VA_ARGS__);                                        \
  } while (0)

/**
 * @brief Macro for to_all typed call operations with void return type
 * @param CONFIG The collective configuration
 * @param TYPENAME The type name string
 * @param ... Additional arguments to pass to the operation
 */
#define TO_ALL_TYPED_CALL(CONFIG, TYPENAME, ...)                               \
  do {                                                                         \
    char opstr[COLL_NAME_MAX * 2];                                             \
    const char *base = proc.env.coll.CONFIG;                                   \
    if (strchr(base, ':') == NULL) {                                           \
      snprintf(opstr, sizeof(opstr), "%s:%s", base, TYPENAME);                 \
    } else {                                                                   \
      strncpy(opstr, base, sizeof(opstr) - 1);                                 \
      opstr[sizeof(opstr) - 1] = '\0';                                         \
    }                                                                          \
    int _rc = register_##CONFIG(opstr);                                        \
    if (_rc != 0) {                                                            \
      shmemu_fatal("couldn't register typed collective '%s' (s = %d)", opstr,  \
                   _rc);                                                       \
    }                                                                          \
    colls.CONFIG.f(__VA_ARGS__);                                               \
    return;                                                                    \
  } while (0)

/**
 * @brief Initialize all collective operations
 *
 * Registers implementations for all collective operations including:
 * alltoall, alltoalls, collect, fcollect, barrier, sync, and broadcast
 */
void collectives_init(void) {
  TRY(alltoall_type);
  TRY(alltoall_mem);
  TRY(alltoall_size);

  TRY(alltoalls_type);
  TRY(alltoalls_mem);
  TRY(alltoalls_size);

  TRY(collect_type);
  TRY(collect_mem);
  TRY(collect_size);

  TRY(fcollect_type);
  TRY(fcollect_mem);
  TRY(fcollect_size);

  TRY(broadcast_type);
  TRY(broadcast_mem);
  TRY(broadcast_size);

  TRY(barrier);
  TRY(barrier_all);
  TRY(sync);
  TRY(team_sync);
  TRY(sync_all);

  TRY(and_to_all);
  TRY(or_to_all);
  TRY(xor_to_all);
  TRY(max_to_all);
  TRY(min_to_all);
  TRY(sum_to_all);
  TRY(prod_to_all);

  TRY(and_reduce);
  TRY(or_reduce);
  TRY(xor_reduce);
  TRY(max_reduce);
  TRY(min_reduce);
  TRY(sum_reduce);
  TRY(prod_reduce);
}

/**
 * @brief Cleanup and finalize collective operations
 */
void collectives_finalize(void) { return; }

/**
 * @defgroup alltoall All-to-all Operations
 * @{
 */

#ifdef ENABLE_PSHMEM
#pragma weak shmem_int_alltoall = pshmem_int_alltoall
#define shmem_int_alltoall pshmem_int_alltoall
#pragma weak shmem_long_alltoall = pshmem_long_alltoall
#define shmem_long_alltoall pshmem_long_alltoall
#pragma weak shmem_longlong_alltoall = pshmem_longlong_alltoall
#define shmem_longlong_alltoall pshmem_longlong_alltoall
#pragma weak shmem_float_alltoall = pshmem_float_alltoall
#define shmem_float_alltoall pshmem_float_alltoall
#pragma weak shmem_double_alltoall = pshmem_double_alltoall
#define shmem_double_alltoall pshmem_double_alltoall
#pragma weak shmem_longdouble_alltoall = pshmem_longdouble_alltoall
#define shmem_longdouble_alltoall pshmem_longdouble_alltoall
#pragma weak shmem_uint_alltoall = pshmem_uint_alltoall
#define shmem_uint_alltoall pshmem_uint_alltoall
#pragma weak shmem_ulong_alltoall = pshmem_ulong_alltoall
#define shmem_ulong_alltoall pshmem_ulong_alltoall
#pragma weak shmem_ulonglong_alltoall = pshmem_ulonglong_alltoall
#define shmem_ulonglong_alltoall pshmem_ulonglong_alltoall
#pragma weak shmem_int32_alltoall = pshmem_int32_alltoall
#define shmem_int32_alltoall pshmem_int32_alltoall
#pragma weak shmem_int64_alltoall = pshmem_int64_alltoall
#define shmem_int64_alltoall pshmem_int64_alltoall
#pragma weak shmem_uint32_alltoall = pshmem_uint32_alltoall
#define shmem_uint32_alltoall pshmem_uint32_alltoall
#pragma weak shmem_uint64_alltoall = pshmem_uint64_alltoall
#define shmem_uint64_alltoall pshmem_uint64_alltoall
#pragma weak shmem_size_alltoall = pshmem_size_alltoall
#define shmem_size_alltoall pshmem_size_alltoall
#pragma weak shmem_ptrdiff_alltoall = pshmem_ptrdiff_alltoall
#define shmem_ptrdiff_alltoall pshmem_ptrdiff_alltoall
#endif /* ENABLE_PSHMEM */

/**
 * @brief Macro to generate typed all-to-all collective operations
 * @param _type The C data type
 * @param _typename The type name string
 */
#undef SHMEM_TYPENAME_ALLTOALL
#define SHMEM_TYPENAME_ALLTOALL(_type, _typename)                              \
  int shmem_##_typename##_alltoall(shmem_team_t team, _type *dest,             \
                                   const _type *source, size_t nelems) {       \
    logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %zu)", __func__, team, dest,       \
           source, nelems);                                                    \
    TYPED_CALL(alltoall_type, #_typename, team, dest, source, nelems);         \
  }

#define DECL_SHIM_ALLTOALL(_type, _typename)                                   \
  SHMEM_TYPENAME_ALLTOALL(_type, _typename)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_SHIM_ALLTOALL)
#undef DECL_SHIM_ALLTOALL
#undef SHMEM_TYPENAME_ALLTOALL

#ifdef ENABLE_PSHMEM
#pragma weak shmem_alltoallmem = pshmem_alltoallmem
#define shmem_alltoallmem pshmem_alltoallmem
#endif /* ENABLE_PSHMEM */

/**
 * @brief Generic memory alltoall routine (deprecated)
 *
 * @param team    The team over which to alltoall
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array on root PE
 * @param nelems  Number of elements to alltoall
 * @return        Zero on success, non-zero on failure
 */
int shmem_alltoallmem(shmem_team_t team, void *dest, const void *source,
                      size_t nelems) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %zu)", __func__, team, dest, source,
         nelems);
  colls.alltoall_mem.f(team, dest, source, nelems);
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_alltoall32 = pshmem_alltoall32
#define shmem_alltoall32 pshmem_alltoall32
#pragma weak shmem_alltoall64 = pshmem_alltoall64
#define shmem_alltoall64 pshmem_alltoall64
#endif /* ENABLE_PSHMEM */

/**
 * @brief Performs an all-to-all exchange of 32-bit data
 *
 * @param target Symmetric destination array
 * @param source Symmetric source array
 * @param nelems Number of elements to exchange
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_alltoall32(void *target, const void *source, size_t nelems,
                      int PE_start, int logPE_stride, int PE_size,
                      long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %zu, %d, %d, %d, %p)", __func__, target,
         source, nelems, PE_start, logPE_stride, PE_size, pSync);

  colls.alltoall_size.f32(target, source, nelems, PE_start, logPE_stride,
                          PE_size, pSync);
}

/**
 * @brief Performs an all-to-all exchange of 64-bit data
 *
 * @param target Symmetric destination array
 * @param source Symmetric source array
 * @param nelems Number of elements to exchange
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_alltoall64(void *target, const void *source, size_t nelems,
                      int PE_start, int logPE_stride, int PE_size,
                      long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %zu, %d, %d, %d, %p)", __func__, target,
         source, nelems, PE_start, logPE_stride, PE_size, pSync);

  colls.alltoall_size.f64(target, source, nelems, PE_start, logPE_stride,
                          PE_size, pSync);
}

/** @} */

/**
 * @defgroup alltoalls Strided All-to-all Operations
 * @{
 */

#ifdef ENABLE_PSHMEM
#pragma weak shmem_int_alltoalls = pshmem_int_alltoalls
#define shmem_int_alltoalls pshmem_int_alltoalls
#pragma weak shmem_long_alltoalls = pshmem_long_alltoalls
#define shmem_long_alltoalls pshmem_long_alltoalls
#pragma weak shmem_longlong_alltoalls = pshmem_longlong_alltoalls
#define shmem_longlong_alltoalls pshmem_longlong_alltoalls
#pragma weak shmem_float_alltoalls = pshmem_float_alltoalls
#define shmem_float_alltoalls pshmem_float_alltoalls
#pragma weak shmem_double_alltoalls = pshmem_double_alltoalls
#define shmem_double_alltoalls pshmem_double_alltoalls
#pragma weak shmem_longdouble_alltoalls = pshmem_longdouble_alltoalls
#define shmem_longdouble_alltoalls pshmem_longdouble_alltoalls
#pragma weak shmem_uint_alltoalls = pshmem_uint_alltoalls
#define shmem_uint_alltoalls pshmem_uint_alltoalls
#pragma weak shmem_ulong_alltoalls = pshmem_ulong_alltoalls
#define shmem_ulong_alltoalls pshmem_ulong_alltoalls
#pragma weak shmem_ulonglong_alltoalls = pshmem_ulonglong_alltoalls
#define shmem_ulonglong_alltoalls pshmem_ulonglong_alltoalls
#pragma weak shmem_int32_alltoalls = pshmem_int32_alltoalls
#define shmem_int32_alltoalls pshmem_int32_alltoalls
#pragma weak shmem_int64_alltoalls = pshmem_int64_alltoalls
#define shmem_int64_alltoalls pshmem_int64_alltoalls
#pragma weak shmem_uint32_alltoalls = pshmem_uint32_alltoalls
#define shmem_uint32_alltoalls pshmem_uint32_alltoalls
#pragma weak shmem_uint64_alltoalls = pshmem_uint64_alltoalls
#define shmem_uint64_alltoalls pshmem_uint64_alltoalls
#pragma weak shmem_size_alltoalls = pshmem_size_alltoalls
#define shmem_size_alltoalls pshmem_size_alltoalls
#pragma weak shmem_ptrdiff_alltoalls = pshmem_ptrdiff_alltoalls
#define shmem_ptrdiff_alltoalls pshmem_ptrdiff_alltoalls
#endif /* ENABLE_PSHMEM */

/**
 * @brief Macro to generate typed strided all-to-all collective operations
 * @param _type The C data type
 * @param _typename The type name string
 */
#define SHMEM_TYPENAME_ALLTOALLS(_type, _typename)                             \
  int shmem_##_typename##_alltoalls(shmem_team_t team, _type *dest,            \
                                    const _type *source, ptrdiff_t dst,        \
                                    ptrdiff_t sst, size_t nelems) {            \
    logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %td, %td, %zu)", __func__, team,   \
           dest, source, dst, sst, nelems);                                    \
    TYPED_CALL(alltoalls_type, #_typename, team, dest, source, dst, sst,       \
               nelems);                                                        \
  }

#define DECL_SHIM_ALLTOALLS(_type, _typename)                                  \
  SHMEM_TYPENAME_ALLTOALLS(_type, _typename)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_SHIM_ALLTOALLS)
#undef DECL_SHIM_ALLTOALLS
#undef SHMEM_TYPENAME_ALLTOALLS

#ifdef ENABLE_PSHMEM
#pragma weak shmem_alltoallsmem = pshmem_alltoallsmem
#define shmem_alltoallsmem pshmem_alltoallsmem
#endif /* ENABLE_PSHMEM */

/**
 * @brief Generic memory alltoall routine (deprecated)
 *
 * @param team    The team over which to alltoall
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array on root PE
 * @param dst     Destination array on root PE
 * @param sst     Source array on root PE
 * @param nelems  Number of elements to alltoall
 * @return        Zero on success, non-zero on failure
 */
int shmem_alltoallsmem(shmem_team_t team, void *dest, const void *source,
                       ptrdiff_t dst, ptrdiff_t sst, size_t nelems) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %td, %td, %zu)", __func__, team, dest,
         source, dst, sst, nelems);
  colls.alltoalls_mem.f(team, dest, source, dst, sst, nelems);
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_alltoalls32 = pshmem_alltoalls32
#define shmem_alltoalls32 pshmem_alltoalls32
#pragma weak shmem_alltoalls64 = pshmem_alltoalls64
#define shmem_alltoalls64 pshmem_alltoalls64
#endif /* ENABLE_PSHMEM */

/**
 * @brief Performs a strided all-to-all exchange of 32-bit data
 *
 * @param target Symmetric destination array
 * @param source Symmetric source array
 * @param dst Target array element stride
 * @param sst Source array element stride
 * @param nelems Number of elements to exchange
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_alltoalls32(void *target, const void *source, ptrdiff_t dst,
                       ptrdiff_t sst, size_t nelems, int PE_start,
                       int logPE_stride, int PE_size, long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %td, %td, %zu, %d, %d, %d, %p)", __func__,
         target, source, dst, sst, nelems, PE_start, logPE_stride, PE_size,
         pSync);

  colls.alltoalls_size.f32(target, source, dst, sst, nelems, PE_start,
                           logPE_stride, PE_size, pSync);
}

/**
 * @brief Performs a strided all-to-all exchange of 64-bit data
 *
 * @param target Symmetric destination array
 * @param source Symmetric source array
 * @param dst Target array element stride
 * @param sst Source array element stride
 * @param nelems Number of elements to exchange
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_alltoalls64(void *target, const void *source, ptrdiff_t dst,
                       ptrdiff_t sst, size_t nelems, int PE_start,
                       int logPE_stride, int PE_size, long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %td, %td, %zu, %d, %d, %d, %p)", __func__,
         target, source, dst, sst, nelems, PE_start, logPE_stride, PE_size,
         pSync);

  colls.alltoalls_size.f64(target, source, dst, sst, nelems, PE_start,
                           logPE_stride, PE_size, pSync);
}

/** @} */

/**
 * @defgroup collect Collection Operations
 * @{
 */

#ifdef ENABLE_PSHMEM
#pragma weak shmem_float_collect = pshmem_float_collect
#define shmem_float_collect pshmem_float_collect
#pragma weak shmem_double_collect = pshmem_double_collect
#define shmem_double_collect pshmem_double_collect
#pragma weak shmem_longdouble_collect = pshmem_longdouble_collect
#define shmem_longdouble_collect pshmem_longdouble_collect
#pragma weak shmem_char_collect = pshmem_char_collect
#define shmem_char_collect pshmem_char_collect
#pragma weak shmem_schar_collect = pshmem_schar_collect
#define shmem_schar_collect pshmem_schar_collect
#pragma weak shmem_short_collect = pshmem_short_collect
#define shmem_short_collect pshmem_short_collect
#pragma weak shmem_int_collect = pshmem_int_collect
#define shmem_int_collect pshmem_int_collect
#pragma weak shmem_long_collect = pshmem_long_collect
#define shmem_long_collect pshmem_long_collect
#pragma weak shmem_longlong_collect = pshmem_longlong_collect
#define shmem_longlong_collect pshmem_longlong_collect
#pragma weak shmem_uchar_collect = pshmem_uchar_collect
#define shmem_uchar_collect pshmem_uchar_collect
#pragma weak shmem_ushort_collect = pshmem_ushort_collect
#define shmem_ushort_collect pshmem_ushort_collect
#pragma weak shmem_uint_collect = pshmem_uint_collect
#define shmem_uint_collect pshmem_uint_collect
#pragma weak shmem_ulong_collect = pshmem_ulong_collect
#define shmem_ulong_collect pshmem_ulong_collect
#pragma weak shmem_ulonglong_collect = pshmem_ulonglong_collect
#define shmem_ulonglong_collect pshmem_ulonglong_collect
#pragma weak shmem_int8_collect = pshmem_int8_collect
#define shmem_int8_collect pshmem_int8_collect
#pragma weak shmem_int16_collect = pshmem_int16_collect
#define shmem_int16_collect pshmem_int16_collect
#pragma weak shmem_int32_collect = pshmem_int32_collect
#define shmem_int32_collect pshmem_int32_collect
#pragma weak shmem_int64_collect = pshmem_int64_collect
#define shmem_int64_collect pshmem_int64_collect
#pragma weak shmem_uint8_collect = pshmem_uint8_collect
#define shmem_uint8_collect pshmem_uint8_collect
#pragma weak shmem_uint16_collect = pshmem_uint16_collect
#define shmem_uint16_collect pshmem_uint16_collect
#pragma weak shmem_uint32_collect = pshmem_uint32_collect
#define shmem_uint32_collect pshmem_uint32_collect
#pragma weak shmem_uint64_collect = pshmem_uint64_collect
#define shmem_uint64_collect pshmem_uint64_collect
#pragma weak shmem_size_collect = pshmem_size_collect
#define shmem_size_collect pshmem_size_collect
#pragma weak shmem_ptrdiff_collect = pshmem_ptrdiff_collect
#define shmem_ptrdiff_collect pshmem_ptrdiff_collect
#endif /* ENABLE_PSHMEM */

/**
 * @brief Macro to generate typed collect operations
 * @param _type The C data type
 * @param _typename The type name string
 */
#define SHMEM_TYPENAME_COLLECT(_type, _typename)                               \
  int shmem_##_typename##_collect(shmem_team_t team, _type *dest,              \
                                  const _type *source, size_t nelems) {        \
    logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %zu)", __func__, team, dest,       \
           source, nelems);                                                    \
    TYPED_CALL(collect_type, #_typename, team, dest, source, nelems);          \
  }

#define DECL_SHIM_COLLECT(_type, _typename)                                    \
  SHMEM_TYPENAME_COLLECT(_type, _typename)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_SHIM_COLLECT)
#undef DECL_SHIM_COLLECT
#undef SHMEM_TYPENAME_COLLECT

#ifdef ENABLE_PSHMEM
#pragma weak shmem_collectmem = pshmem_collectmem
#define shmem_collectmem pshmem_collectmem
#endif /* ENABLE_PSHMEM */

/**
 * @brief Generic memory collect routine (deprecated)
 *
 * @param team    The team over which to collect
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array on root PE
 * @param nelems  Number of elements to collect
 * @return        Zero on success, non-zero on failure
 */
int shmem_collectmem(shmem_team_t team, void *dest, const void *source,
                     size_t nelems) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %zu)", __func__, team, dest, source,
         nelems);
  colls.collect_mem.f(team, dest, source, nelems);
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_collect32 = pshmem_collect32
#define shmem_collect32 pshmem_collect32
#pragma weak shmem_collect64 = pshmem_collect64
#define shmem_collect64 pshmem_collect64
#endif /* ENABLE_PSHMEM */

/**
 * @brief Concatenates 32-bit data from multiple PEs to an array in ascending PE
 * order
 *
 * @param target Symmetric destination array
 * @param source Symmetric source array
 * @param nelems Number of elements to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_collect32(void *target, const void *source, size_t nelems,
                     int PE_start, int logPE_stride, int PE_size, long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %zu, %d, %d, %d, %p)", __func__, target,
         source, nelems, PE_start, logPE_stride, PE_size, pSync);

  colls.collect_size.f32(target, source, nelems, PE_start, logPE_stride,
                         PE_size, pSync);
}

/**
 * @brief Concatenates 64-bit data from multiple PEs to an array in ascending PE
 * order
 *
 * @param target Symmetric destination array
 * @param source Symmetric source array
 * @param nelems Number of elements to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_collect64(void *target, const void *source, size_t nelems,
                     int PE_start, int logPE_stride, int PE_size, long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %zu, %d, %d, %d, %p)", __func__, target,
         source, nelems, PE_start, logPE_stride, PE_size, pSync);

  colls.collect_size.f64(target, source, nelems, PE_start, logPE_stride,
                         PE_size, pSync);
}

/** @} */

/**
 * @defgroup fcollect Fixed-Length Collection Operations
 * @{
 */

#ifdef ENABLE_PSHMEM
#pragma weak shmem_float_fcollect = pshmem_float_fcollect
#define shmem_float_fcollect pshmem_float_fcollect
#pragma weak shmem_double_fcollect = pshmem_double_fcollect
#define shmem_double_fcollect pshmem_double_fcollect
#pragma weak shmem_longdouble_fcollect = pshmem_longdouble_fcollect
#define shmem_longdouble_fcollect pshmem_longdouble_fcollect
#pragma weak shmem_char_fcollect = pshmem_char_fcollect
#define shmem_char_fcollect pshmem_char_fcollect
#pragma weak shmem_schar_fcollect = pshmem_schar_fcollect
#define shmem_schar_fcollect pshmem_schar_fcollect
#pragma weak shmem_short_fcollect = pshmem_short_fcollect
#define shmem_short_fcollect pshmem_short_fcollect
#pragma weak shmem_int_fcollect = pshmem_int_fcollect
#define shmem_int_fcollect pshmem_int_fcollect
#pragma weak shmem_long_fcollect = pshmem_long_fcollect
#define shmem_long_fcollect pshmem_long_fcollect
#pragma weak shmem_longlong_fcollect = pshmem_longlong_fcollect
#define shmem_longlong_fcollect pshmem_longlong_fcollect
#pragma weak shmem_uchar_fcollect = pshmem_uchar_fcollect
#define shmem_uchar_fcollect pshmem_uchar_fcollect
#pragma weak shmem_ushort_fcollect = pshmem_ushort_fcollect
#define shmem_ushort_fcollect pshmem_ushort_fcollect
#pragma weak shmem_uint_fcollect = pshmem_uint_fcollect
#define shmem_uint_fcollect pshmem_uint_fcollect
#pragma weak shmem_ulong_fcollect = pshmem_ulong_fcollect
#define shmem_ulong_fcollect pshmem_ulong_fcollect
#pragma weak shmem_ulonglong_fcollect = pshmem_ulonglong_fcollect
#define shmem_ulonglong_fcollect pshmem_ulonglong_fcollect
#pragma weak shmem_int8_fcollect = pshmem_int8_fcollect
#define shmem_int8_fcollect pshmem_int8_fcollect
#pragma weak shmem_int16_fcollect = pshmem_int16_fcollect
#define shmem_int16_fcollect pshmem_int16_fcollect
#pragma weak shmem_int32_fcollect = pshmem_int32_fcollect
#define shmem_int32_fcollect pshmem_int32_fcollect
#pragma weak shmem_int64_fcollect = pshmem_int64_fcollect
#define shmem_int64_fcollect pshmem_int64_fcollect
#pragma weak shmem_uint8_fcollect = pshmem_uint8_fcollect
#define shmem_uint8_fcollect pshmem_uint8_fcollect
#pragma weak shmem_uint16_fcollect = pshmem_uint16_fcollect
#define shmem_uint16_fcollect pshmem_uint16_fcollect
#pragma weak shmem_uint32_fcollect = pshmem_uint32_fcollect
#define shmem_uint32_fcollect pshmem_uint32_fcollect
#pragma weak shmem_uint64_fcollect = pshmem_uint64_fcollect
#define shmem_uint64_fcollect pshmem_uint64_fcollect
#pragma weak shmem_size_fcollect = pshmem_size_fcollect
#define shmem_size_fcollect pshmem_size_fcollect
#pragma weak shmem_ptrdiff_fcollect = pshmem_ptrdiff_fcollect
#define shmem_ptrdiff_fcollect pshmem_ptrdiff_fcollect
#endif /* ENABLE_PSHMEM */

/**
 * @brief Macro to generate typed fixed-length collect operations
 * @param _type The C data type
 * @param _typename The type name string
 */
#define SHMEM_TYPENAME_FCOLLECT(_type, _typename)                              \
  int shmem_##_typename##_fcollect(shmem_team_t team, _type *dest,             \
                                   const _type *source, size_t nelems) {       \
    logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %zu)", __func__, team, dest,       \
           source, nelems);                                                    \
    TYPED_CALL(fcollect_type, #_typename, team, dest, source, nelems);         \
  }

#define DECL_SHIM_FCOLLECT(_type, _typename)                                   \
  SHMEM_TYPENAME_FCOLLECT(_type, _typename)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_SHIM_FCOLLECT)
#undef DECL_SHIM_FCOLLECT
#undef SHMEM_TYPENAME_FCOLLECT

#ifdef ENABLE_PSHMEM
#pragma weak shmem_fcollectmem = pshmem_fcollectmem
#define shmem_fcollectmem pshmem_fcollectmem
#endif /* ENABLE_PSHMEM */

/**
 * @brief Generic memory collect routine (deprecated)
 *
 * @param team    The team over which to collect
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array on root PE
 * @param nelems  Number of elements to collect
 * @return        Zero on success, non-zero on failure
 */
int shmem_fcollectmem(shmem_team_t team, void *dest, const void *source,
                      size_t nelems) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %zu)", __func__, team, dest, source,
         nelems);
  colls.fcollect_mem.f(team, dest, source, nelems);
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_fcollect32 = pshmem_fcollect32
#define shmem_fcollect32 pshmem_fcollect32
#pragma weak shmem_fcollect64 = pshmem_fcollect64
#define shmem_fcollect64 pshmem_fcollect64
#endif /* ENABLE_PSHMEM */

/**
 * @brief Concatenates fixed-length 32-bit data from multiple PEs to an array in
 * ascending PE order
 *
 * @param target Symmetric destination array
 * @param source Symmetric source array
 * @param nelems Number of elements to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_fcollect32(void *target, const void *source, size_t nelems,
                      int PE_start, int logPE_stride, int PE_size,
                      long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %zu, %d, %d, %d, %p)", __func__, target,
         source, nelems, PE_start, logPE_stride, PE_size, pSync);

  colls.fcollect_size.f32(target, source, nelems, PE_start, logPE_stride,
                          PE_size, pSync);
}

/**
 * @brief Concatenates fixed-length 64-bit data from multiple PEs to an array in
 * ascending PE order
 *
 * @param target Symmetric destination array
 * @param source Symmetric source array
 * @param nelems Number of elements to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_fcollect64(void *target, const void *source, size_t nelems,
                      int PE_start, int logPE_stride, int PE_size,
                      long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %zu, %d, %d, %d, %p)", __func__, target,
         source, nelems, PE_start, logPE_stride, PE_size, pSync);

  colls.fcollect_size.f64(target, source, nelems, PE_start, logPE_stride,
                          PE_size, pSync);
}

/** @} */

/**
 * @defgroup Broadcast Operations
 * @{
 */

#ifdef ENABLE_PSHMEM
#pragma weak shmem_float_broadcast = pshmem_float_broadcast
#define shmem_float_broadcast pshmem_float_broadcast
#pragma weak shmem_double_broadcast = pshmem_double_broadcast
#define shmem_double_broadcast pshmem_double_broadcast
#pragma weak shmem_longdouble_broadcast = pshmem_longdouble_broadcast
#define shmem_longdouble_broadcast pshmem_longdouble_broadcast
#pragma weak shmem_char_broadcast = pshmem_char_broadcast
#define shmem_char_broadcast pshmem_char_broadcast
#pragma weak shmem_schar_broadcast = pshmem_schar_broadcast
#define shmem_schar_broadcast pshmem_schar_broadcast
#pragma weak shmem_short_broadcast = pshmem_short_broadcast
#define shmem_short_broadcast pshmem_short_broadcast
#pragma weak shmem_int_broadcast = pshmem_int_broadcast
#define shmem_int_broadcast pshmem_int_broadcast
#pragma weak shmem_long_broadcast = pshmem_long_broadcast
#define shmem_long_broadcast pshmem_long_broadcast
#pragma weak shmem_longlong_broadcast = pshmem_longlong_broadcast
#define shmem_longlong_broadcast pshmem_longlong_broadcast
#pragma weak shmem_uchar_broadcast = pshmem_uchar_broadcast
#define shmem_uchar_broadcast pshmem_uchar_broadcast
#pragma weak shmem_ushort_broadcast = pshmem_ushort_broadcast
#define shmem_ushort_broadcast pshmem_ushort_broadcast
#pragma weak shmem_uint_broadcast = pshmem_uint_broadcast
#define shmem_uint_broadcast pshmem_uint_broadcast
#pragma weak shmem_ulong_broadcast = pshmem_ulong_broadcast
#define shmem_ulong_broadcast pshmem_ulong_broadcast
#pragma weak shmem_ulonglong_broadcast = pshmem_ulonglong_broadcast
#define shmem_ulonglong_broadcast pshmem_ulonglong_broadcast
#pragma weak shmem_int8_broadcast = pshmem_int8_broadcast
#define shmem_int8_broadcast pshmem_int8_broadcast
#pragma weak shmem_int16_broadcast = pshmem_int16_broadcast
#define shmem_int16_broadcast pshmem_int16_broadcast
#pragma weak shmem_int32_broadcast = pshmem_int32_broadcast
#define shmem_int32_broadcast pshmem_int32_broadcast
#pragma weak shmem_int64_broadcast = pshmem_int64_broadcast
#define shmem_int64_broadcast pshmem_int64_broadcast
#pragma weak shmem_uint8_broadcast = pshmem_uint8_broadcast
#define shmem_uint8_broadcast pshmem_uint8_broadcast
#pragma weak shmem_uint16_broadcast = pshmem_uint16_broadcast
#define shmem_uint16_broadcast pshmem_uint16_broadcast
#pragma weak shmem_uint32_broadcast = pshmem_uint32_broadcast
#define shmem_uint32_broadcast pshmem_uint32_broadcast
#pragma weak shmem_uint64_broadcast = pshmem_uint64_broadcast
#define shmem_uint64_broadcast pshmem_uint64_broadcast
#pragma weak shmem_size_broadcast = pshmem_size_broadcast
#define shmem_size_broadcast pshmem_size_broadcast
#pragma weak shmem_ptrdiff_broadcast = pshmem_ptrdiff_broadcast
#define shmem_ptrdiff_broadcast pshmem_ptrdiff_broadcast
#endif /* ENABLE_PSHMEM */

/**
 * @brief Macro to generate typed fixed-length broadcast operations
 * @param _type The C data type
 * @param _typename The type name string
 */
#define SHMEM_TYPENAME_BROADCAST(_type, _typename)                             \
  int shmem_##_typename##_broadcast(shmem_team_t team, _type *dest,            \
                                    const _type *source, size_t nelems,        \
                                    int PE_root) {                             \
    logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %zu, %d)", __func__, team, dest,   \
           source, nelems, PE_root);                                           \
    TYPED_CALL(broadcast_type, #_typename, team, dest, source, nelems,         \
               PE_root);                                                       \
  }

#define DECL_SHIM_BROADCAST(_type, _typename)                                  \
  SHMEM_TYPENAME_BROADCAST(_type, _typename)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_SHIM_BROADCAST)
#undef DECL_SHIM_BROADCAST
#undef SHMEM_TYPENAME_BROADCAST

#ifdef ENABLE_PSHMEM
#pragma weak shmem_broadcastmem = pshmem_broadcastmem
#define shmem_broadcastmem pshmem_broadcastmem
#endif /* ENABLE_PSHMEM */

/**
 * @brief Generic memory broadcast routine (deprecated)
 *
 * @param team    The team over which to broadcast
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array on root PE
 * @param nelems  Number of elements to broadcast
 * @param PE_root The root PE
 * @return        Zero on success, non-zero on failure
 */
int shmem_broadcastmem(shmem_team_t team, void *dest, const void *source,
                       size_t nelems, int PE_root) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %zu, %d)", __func__, team, dest,
         source, nelems, PE_root);
  colls.broadcast_mem.f(team, dest, source, nelems, PE_root);
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_broadcast32 = pshmem_broadcast32
#define shmem_broadcast32 pshmem_broadcast32
#pragma weak shmem_broadcast64 = pshmem_broadcast64
#define shmem_broadcast64 pshmem_broadcast64
#endif /* ENABLE_PSHMEM */

/**
 * @brief Broadcasts 32-bit data from a source PE to all other PEs in a group
 *
 * @param target Symmetric destination array
 * @param source Symmetric source array
 * @param nelems Number of elements to broadcast
 * @param PE_root Source PE for the broadcast
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_broadcast32(void *target, const void *source, size_t nelems,
                       int PE_root, int PE_start, int logPE_stride, int PE_size,
                       long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %zu, %d, %d, %d, %p)", __func__, target,
         source, nelems, PE_start, logPE_stride, PE_size, pSync);

  colls.broadcast_size.f32(target, source, nelems, PE_root, PE_start,
                           logPE_stride, PE_size, pSync);
}

/**
 * @brief Broadcasts 64-bit data from a source PE to all other PEs in a group
 *
 * @param target Symmetric destination array
 * @param source Symmetric source array
 * @param nelems Number of elements to broadcast
 * @param PE_root Source PE for the broadcast
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_broadcast64(void *target, const void *source, size_t nelems,
                       int PE_root, int PE_start, int logPE_stride, int PE_size,
                       long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%p, %p, %zu, %d, %d, %d, %p)", __func__, target,
         source, nelems, PE_start, logPE_stride, PE_size, pSync);

  colls.broadcast_size.f64(target, source, nelems, PE_root, PE_start,
                           logPE_stride, PE_size, pSync);
}

/** @} */
///////////////////////////////////////////////////////////////////////

/**
 * @brief Declares a to_all operation for a given type and operation
 *
 * @param _typename The type name
 * @param _type The type
 * @param _op The operation
 */
#define SHMEM_TYPENAME_OP_TO_ALL(_typename, _type, _op)                        \
  void shmem_##_typename##_##_op##_to_all(                                     \
      _type *dest, const _type *source, int nreduce, int PE_start,             \
      int logPE_stride, int PE_size, _type *pWrk, long *pSync) {               \
    logger(LOG_COLLECTIVES, "%s(%p, %p, %d, %d, %d, %d, %p, %p)", __func__,    \
           dest, source, nreduce, PE_start, logPE_stride, PE_size, pWrk,       \
           pSync);                                                             \
    TO_ALL_TYPED_CALL(_op##_to_all, #_typename, dest, source, nreduce,         \
                      PE_start, logPE_stride, PE_size, pWrk, pSync);           \
  }

#ifdef ENABLE_PSHMEM
#pragma weak shmem_int_and_to_all = pshmem_int_and_to_all
#define shmem_int_and_to_all pshmem_int_and_to_all
#pragma weak shmem_long_and_to_all = pshmem_long_and_to_all
#define shmem_long_and_to_all pshmem_long_and_to_all
#pragma weak shmem_longlong_and_to_all = pshmem_longlong_and_to_all
#define shmem_longlong_and_to_all pshmem_longlong_and_to_all
#pragma weak shmem_short_and_to_all = pshmem_short_and_to_all
#define shmem_short_and_to_all pshmem_short_and_to_all
#pragma weak shmem_int_or_to_all = pshmem_int_or_to_all
#define shmem_int_or_to_all pshmem_int_or_to_all
#pragma weak shmem_long_or_to_all = pshmem_long_or_to_all
#define shmem_long_or_to_all pshmem_long_or_to_all
#pragma weak shmem_longlong_or_to_all = pshmem_longlong_or_to_all
#define shmem_longlong_or_to_all pshmem_longlong_or_to_all
#pragma weak shmem_short_or_to_all = pshmem_short_or_to_all
#define shmem_short_or_to_all pshmem_short_or_to_all
#pragma weak shmem_int_xor_to_all = pshmem_int_xor_to_all
#define shmem_int_xor_to_all pshmem_int_xor_to_all
#pragma weak shmem_long_xor_to_all = pshmem_long_xor_to_all
#define shmem_long_xor_to_all pshmem_long_xor_to_all
#pragma weak shmem_longlong_xor_to_all = pshmem_longlong_xor_to_all
#define shmem_longlong_xor_to_all pshmem_longlong_xor_to_all
#pragma weak shmem_short_xor_to_all = pshmem_short_xor_to_all
#define shmem_short_xor_to_all pshmem_short_xor_to_all
#endif /* ENABLE_PSHMEM */

/* shmem_and_to_all */
#define DECL_SHIM_AND_TO_ALL(_typename, _type)                                 \
  SHMEM_TYPENAME_OP_TO_ALL(_type, _typename, and)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(DECL_SHIM_AND_TO_ALL)
#undef DECL_SHIM_AND_TO_ALL

/* shmem_or_to_all */
#define DECL_SHIM_OR_TO_ALL(_typename, _type)                                  \
  SHMEM_TYPENAME_OP_TO_ALL(_type, _typename, or)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(DECL_SHIM_OR_TO_ALL)
#undef DECL_SHIM_OR_TO_ALL

/* shmem_xor_to_all */
#define DECL_SHIM_XOR_TO_ALL(_typename, _type)                                 \
  SHMEM_TYPENAME_OP_TO_ALL(_type, _typename, xor)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(DECL_SHIM_XOR_TO_ALL)
#undef DECL_SHIM_XOR_TO_ALL

#ifdef ENABLE_PSHMEM
#pragma weak shmem_int_max_to_all = pshmem_int_max_to_all
#define shmem_int_max_to_all pshmem_int_max_to_all
#pragma weak shmem_long_max_to_all = pshmem_long_max_to_all
#define shmem_long_max_to_all pshmem_long_max_to_all
#pragma weak shmem_longlong_max_to_all = pshmem_longlong_max_to_all
#define shmem_longlong_max_to_all pshmem_longlong_max_to_all
#pragma weak shmem_short_max_to_all = pshmem_short_max_to_all
#define shmem_short_max_to_all pshmem_short_max_to_all
#pragma weak shmem_longdouble_max_to_all = pshmem_longdouble_max_to_all
#define shmem_longdouble_max_to_all pshmem_longdouble_max_to_all
#pragma weak shmem_float_max_to_all = pshmem_float_max_to_all
#define shmem_float_max_to_all pshmem_float_max_to_all
#pragma weak shmem_double_max_to_all = pshmem_double_max_to_all
#define shmem_double_max_to_all pshmem_double_max_to_all
#pragma weak shmem_int_min_to_all = pshmem_int_min_to_all
#define shmem_int_min_to_all pshmem_int_min_to_all
#pragma weak shmem_long_min_to_all = pshmem_long_min_to_all
#define shmem_long_min_to_all pshmem_long_min_to_all
#pragma weak shmem_longlong_min_to_all = pshmem_longlong_min_to_all
#define shmem_longlong_min_to_all pshmem_longlong_min_to_all
#pragma weak shmem_short_min_to_all = pshmem_short_min_to_all
#define shmem_short_min_to_all pshmem_short_min_to_all
#pragma weak shmem_longdouble_min_to_all = pshmem_longdouble_min_to_all
#define shmem_longdouble_min_to_all pshmem_longdouble_min_to_all
#pragma weak shmem_float_min_to_all = pshmem_float_min_to_all
#define shmem_float_min_to_all pshmem_float_min_to_all
#pragma weak shmem_double_min_to_all = pshmem_double_min_to_all
#define shmem_double_min_to_all pshmem_double_min_to_all
#endif /* ENABLE_PSHMEM */

/* shmem_min_to_all */
#define DECL_SHIM_MIN_TO_ALL(_typename, _type)                                 \
  SHMEM_TYPENAME_OP_TO_ALL(_type, _typename, min)
SHMEM_TO_ALL_MINMAX_TYPE_TABLE(DECL_SHIM_MIN_TO_ALL)
#undef DECL_SHIM_MIN_TO_ALL

/* shmem_max_to_all */
#define DECL_SHIM_MAX_TO_ALL(_typename, _type)                                 \
  SHMEM_TYPENAME_OP_TO_ALL(_type, _typename, max)
SHMEM_TO_ALL_MINMAX_TYPE_TABLE(DECL_SHIM_MAX_TO_ALL)
#undef DECL_SHIM_MAX_TO_ALL

#ifdef ENABLE_PSHMEM
// #pragma weak shmem_complexd_sum_to_all = pshmem_complexd_sum_to_all
// #define shmem_complexd_sum_to_all pshmem_complexd_sum_to_all
// #pragma weak shmem_complexf_sum_to_all = pshmem_complexf_sum_to_all
// #define shmem_complexf_sum_to_all pshmem_complexf_sum_to_all
#pragma weak shmem_double_sum_to_all = pshmem_double_sum_to_all
#define shmem_double_sum_to_all pshmem_double_sum_to_all
#pragma weak shmem_float_sum_to_all = pshmem_float_sum_to_all
#define shmem_float_sum_to_all pshmem_float_sum_to_all
#pragma weak shmem_int_sum_to_all = pshmem_int_sum_to_all
#define shmem_int_sum_to_all pshmem_int_sum_to_all
#pragma weak shmem_long_sum_to_all = pshmem_long_sum_to_all
#define shmem_long_sum_to_all pshmem_long_sum_to_all
#pragma weak shmem_longdouble_sum_to_all = pshmem_longdouble_sum_to_all
#define shmem_longdouble_sum_to_all pshmem_longdouble_sum_to_all
#pragma weak shmem_longlong_sum_to_all = pshmem_longlong_sum_to_all
#define shmem_longlong_sum_to_all pshmem_longlong_sum_to_all
#pragma weak shmem_short_sum_to_all = pshmem_short_sum_to_all
#define shmem_short_sum_to_all pshmem_short_sum_to_all
// #pragma weak shmem_complexd_prod_to_all = pshmem_complexd_prod_to_all
// #define shmem_complexd_prod_to_all pshmem_complexd_prod_to_all
// #pragma weak shmem_complexf_prod_to_all = pshmem_complexf_prod_to_all
// #define shmem_complexf_prod_to_all pshmem_complexf_prod_to_all
#pragma weak shmem_double_prod_to_all = pshmem_double_prod_to_all
#define shmem_double_prod_to_all pshmem_double_prod_to_all
#pragma weak shmem_float_prod_to_all = pshmem_float_prod_to_all
#define shmem_float_prod_to_all pshmem_float_prod_to_all
#pragma weak shmem_int_prod_to_all = pshmem_int_prod_to_all
#define shmem_int_prod_to_all pshmem_int_prod_to_all
#pragma weak shmem_long_prod_to_all = pshmem_long_prod_to_all
#define shmem_long_prod_to_all pshmem_long_prod_to_all
#pragma weak shmem_longdouble_prod_to_all = pshmem_longdouble_prod_to_all
#define shmem_longdouble_prod_to_all pshmem_longdouble_prod_to_all
#pragma weak shmem_longlong_prod_to_all = pshmem_longlong_prod_to_all
#define shmem_longlong_prod_to_all pshmem_longlong_prod_to_all
#pragma weak shmem_short_prod_to_all = pshmem_short_prod_to_all
#define shmem_short_prod_to_all pshmem_short_prod_to_all
#endif /* ENABLE_PSHMEM */

/* shmem_sum_to_all */
#define DECL_SHIM_SUM_TO_ALL(_typename, _type)                                 \
  SHMEM_TYPENAME_OP_TO_ALL(_type, _typename, sum)
SHMEM_TO_ALL_ARITH_TYPE_TABLE(DECL_SHIM_SUM_TO_ALL)
#undef DECL_SHIM_SUM_TO_ALL

/* shmem_prod_to_all */
#define DECL_SHIM_PROD_TO_ALL(_typename, _type)                                \
  SHMEM_TYPENAME_OP_TO_ALL(_type, _typename, prod)
SHMEM_TO_ALL_ARITH_TYPE_TABLE(DECL_SHIM_PROD_TO_ALL)
#undef DECL_SHIM_PROD_TO_ALL

#undef SHMEM_TYPENAME_OP_TO_ALL

/**
 * @brief Declares a reduce operation for a given type and operation
 *
 * @param _typename The type name
 * @param _type The type
 * @param _op The operation
 */
#define SHMEM_TYPENAME_OP_REDUCE(_typename, _type, _op)                        \
  int shmem_##_typename##_##_op##_reduce(                                      \
      shmem_team_t team, _type *dest, const _type *source, size_t nreduce) {   \
    logger(LOG_COLLECTIVES, "%s(%p, %p, %p, %zu)", __func__, team, dest,       \
           source, nreduce);                                                   \
    TYPED_CALL(_op##_reduce, #_typename, team, dest, source, nreduce);         \
  }

#ifdef ENABLE_PSHMEM
/* and */
#pragma weak shmem_uchar_and_reduce = pshmem_uchar_and_reduce
#define shmem_uchar_and_reduce pshmem_uchar_and_reduce
#pragma weak shmem_ushort_and_reduce = pshmem_ushort_and_reduce
#define shmem_ushort_and_reduce pshmem_ushort_and_reduce
#pragma weak shmem_uint_and_reduce = pshmem_uint_and_reduce
#define shmem_uint_and_reduce pshmem_uint_and_reduce
#pragma weak shmem_ulong_and_reduce = pshmem_ulong_and_reduce
#define shmem_ulong_and_reduce pshmem_ulong_and_reduce
#pragma weak shmem_ulonglong_and_reduce = pshmem_ulonglong_and_reduce
#define shmem_ulonglong_and_reduce pshmem_ulonglong_and_reduce
#pragma weak shmem_int8_and_reduce = pshmem_int8_and_reduce
#define shmem_int8_and_reduce pshmem_int8_and_reduce
#pragma weak shmem_int16_and_reduce = pshmem_int16_and_reduce
#define shmem_int16_and_reduce pshmem_int16_and_reduce
#pragma weak shmem_int32_and_reduce = pshmem_int32_and_reduce
#define shmem_int32_and_reduce pshmem_int32_and_reduce
#pragma weak shmem_int64_and_reduce = pshmem_int64_and_reduce
#define shmem_int64_and_reduce pshmem_int64_and_reduce
#pragma weak shmem_uint8_and_reduce = pshmem_uint8_and_reduce
#define shmem_uint8_and_reduce pshmem_uint8_and_reduce
#pragma weak shmem_uint16_and_reduce = pshmem_uint16_and_reduce
#define shmem_uint16_and_reduce pshmem_uint16_and_reduce
#pragma weak shmem_uint32_and_reduce = pshmem_uint32_and_reduce
#define shmem_uint32_and_reduce pshmem_uint32_and_reduce
#pragma weak shmem_uint64_and_reduce = pshmem_uint64_and_reduce
#define shmem_uint64_and_reduce pshmem_uint64_and_reduce
/* or */
#pragma weak shmem_uchar_or_reduce = pshmem_uchar_or_reduce
#define shmem_uchar_or_reduce pshmem_uchar_or_reduce
#pragma weak shmem_ushort_or_reduce = pshmem_ushort_or_reduce
#define shmem_ushort_or_reduce pshmem_ushort_or_reduce
#pragma weak shmem_uint_or_reduce = pshmem_uint_or_reduce
#define shmem_uint_or_reduce pshmem_uint_or_reduce
#pragma weak shmem_ulong_or_reduce = pshmem_ulong_or_reduce
#define shmem_ulong_or_reduce pshmem_ulong_or_reduce
#pragma weak shmem_ulonglong_or_reduce = pshmem_ulonglong_or_reduce
#define shmem_ulonglong_or_reduce pshmem_ulonglong_or_reduce
#pragma weak shmem_int8_or_reduce = pshmem_int8_or_reduce
#define shmem_int8_or_reduce pshmem_int8_or_reduce
#pragma weak shmem_int16_or_reduce = pshmem_int16_or_reduce
#define shmem_int16_or_reduce pshmem_int16_or_reduce
#pragma weak shmem_int32_or_reduce = pshmem_int32_or_reduce
#define shmem_int32_or_reduce pshmem_int32_or_reduce
#pragma weak shmem_int64_or_reduce = pshmem_int64_or_reduce
#define shmem_int64_or_reduce pshmem_int64_or_reduce
#pragma weak shmem_uint8_or_reduce = pshmem_uint8_or_reduce
#define shmem_uint8_or_reduce pshmem_uint8_or_reduce
#pragma weak shmem_uint16_or_reduce = pshmem_uint16_or_reduce
#define shmem_uint16_or_reduce pshmem_uint16_or_reduce
#pragma weak shmem_uint32_or_reduce = pshmem_uint32_or_reduce
#define shmem_uint32_or_reduce pshmem_uint32_or_reduce
#pragma weak shmem_uint64_or_reduce = pshmem_uint64_or_reduce
#define shmem_uint64_or_reduce pshmem_uint64_or_reduce
#pragma weak shmem_size_or_reduce = pshmem_size_or_reduce
#define shmem_size_or_reduce pshmem_size_or_reduce
/* xor */
#pragma weak shmem_uchar_xor_reduce = pshmem_uchar_xor_reduce
#define shmem_uchar_xor_reduce pshmem_uchar_xor_reduce
#pragma weak shmem_ushort_xor_reduce = pshmem_ushort_xor_reduce
#define shmem_ushort_xor_reduce pshmem_ushort_xor_reduce
#pragma weak shmem_uint_xor_reduce = pshmem_uint_xor_reduce
#define shmem_uint_xor_reduce pshmem_uint_xor_reduce
#pragma weak shmem_ulong_xor_reduce = pshmem_ulong_xor_reduce
#define shmem_ulong_xor_reduce pshmem_ulong_xor_reduce
#pragma weak shmem_ulonglong_xor_reduce = pshmem_ulonglong_xor_reduce
#define shmem_ulonglong_xor_reduce pshmem_ulonglong_xor_reduce
#pragma weak shmem_int8_xor_reduce = pshmem_int8_xor_reduce
#define shmem_int8_xor_reduce pshmem_int8_xor_reduce
#pragma weak shmem_int16_xor_reduce = pshmem_int16_xor_reduce
#define shmem_int16_xor_reduce pshmem_int16_xor_reduce
#pragma weak shmem_int32_xor_reduce = pshmem_int32_xor_reduce
#define shmem_int32_xor_reduce pshmem_int32_xor_reduce
#pragma weak shmem_int64_xor_reduce = pshmem_int64_xor_reduce
#define shmem_int64_xor_reduce pshmem_int64_xor_reduce
#pragma weak shmem_uint8_xor_reduce = pshmem_uint8_xor_reduce
#define shmem_uint8_xor_reduce pshmem_uint8_xor_reduce
#pragma weak shmem_uint16_xor_reduce = pshmem_uint16_xor_reduce
#define shmem_uint16_xor_reduce pshmem_uint16_xor_reduce
#pragma weak shmem_uint32_xor_reduce = pshmem_uint32_xor_reduce
#define shmem_uint32_xor_reduce pshmem_uint32_xor_reduce
#pragma weak shmem_uint64_xor_reduce = pshmem_uint64_xor_reduce
#define shmem_uint64_xor_reduce pshmem_uint64_xor_reduce
#pragma weak shmem_size_xor_reduce = pshmem_size_xor_reduce
#define shmem_size_xor_reduce pshmem_size_xor_reduce

#endif /* ENABLE_PSHMEM */

/* shmem_and_reduce */
#define DECL_SHIM_AND_REDUCE(_typename, _type)                                 \
  SHMEM_TYPENAME_OP_REDUCE(_type, _typename, and)
SHMEM_REDUCE_BITWISE_TYPE_TABLE(DECL_SHIM_AND_REDUCE)
#undef DECL_SHIM_AND_REDUCE

/* shmem_or_reduce */
#define DECL_SHIM_OR_REDUCE(_typename, _type)                                  \
  SHMEM_TYPENAME_OP_REDUCE(_type, _typename, or)
SHMEM_REDUCE_BITWISE_TYPE_TABLE(DECL_SHIM_OR_REDUCE)
#undef DECL_SHIM_OR_REDUCE

/* shmem_xor_reduce */
#define DECL_SHIM_XOR_REDUCE(_typename, _type)                                 \
  SHMEM_TYPENAME_OP_REDUCE(_type, _typename, xor)
SHMEM_REDUCE_BITWISE_TYPE_TABLE(DECL_SHIM_XOR_REDUCE)
#undef DECL_SHIM_XOR_REDUCE

#ifdef ENABLE_PSHMEM
/* max */
#pragma weak shmem_char_max_reduce = pshmem_char_max_reduce
#define shmem_char_max_reduce pshmem_char_max_reduce
#pragma weak shmem_schar_max_reduce = pshmem_schar_max_reduce
#define shmem_schar_max_reduce pshmem_schar_max_reduce
#pragma weak shmem_short_max_reduce = pshmem_short_max_reduce
#define shmem_short_max_reduce pshmem_short_max_reduce
#pragma weak shmem_int_max_reduce = pshmem_int_max_reduce
#define shmem_int_max_reduce pshmem_int_max_reduce
#pragma weak shmem_long_max_reduce = pshmem_long_max_reduce
#define shmem_long_max_reduce pshmem_long_max_reduce
#pragma weak shmem_longlong_max_reduce = pshmem_longlong_max_reduce
#define shmem_longlong_max_reduce pshmem_longlong_max_reduce
#pragma weak shmem_ptrdiff_max_reduce = pshmem_ptrdiff_max_reduce
#define shmem_ptrdiff_max_reduce pshmem_ptrdiff_max_reduce
#pragma weak shmem_uchar_max_reduce = pshmem_uchar_max_reduce
#define shmem_uchar_max_reduce pshmem_uchar_max_reduce
#pragma weak shmem_ushort_max_reduce = pshmem_ushort_max_reduce
#define shmem_ushort_max_reduce pshmem_ushort_max_reduce
#pragma weak shmem_uint_max_reduce = pshmem_uint_max_reduce
#define shmem_uint_max_reduce pshmem_uint_max_reduce
#pragma weak shmem_ulong_max_reduce = pshmem_ulong_max_reduce
#define shmem_ulong_max_reduce pshmem_ulong_max_reduce
#pragma weak shmem_ulonglong_max_reduce = pshmem_ulonglong_max_reduce
#define shmem_ulonglong_max_reduce pshmem_ulonglong_max_reduce
#pragma weak shmem_int8_max_reduce = pshmem_int8_max_reduce
#define shmem_int8_max_reduce pshmem_int8_max_reduce
#pragma weak shmem_int16_max_reduce = pshmem_int16_max_reduce
#define shmem_int16_max_reduce pshmem_int16_max_reduce
#pragma weak shmem_int32_max_reduce = pshmem_int32_max_reduce
#define shmem_int32_max_reduce pshmem_int32_max_reduce
#pragma weak shmem_int64_max_reduce = pshmem_int64_max_reduce
#define shmem_int64_max_reduce pshmem_int64_max_reduce
#pragma weak shmem_uint8_max_reduce = pshmem_uint8_max_reduce
#define shmem_uint8_max_reduce pshmem_uint8_max_reduce
#pragma weak shmem_uint16_max_reduce = pshmem_uint16_max_reduce
#define shmem_uint16_max_reduce pshmem_uint16_max_reduce
#pragma weak shmem_uint32_max_reduce = pshmem_uint32_max_reduce
#define shmem_uint32_max_reduce pshmem_uint32_max_reduce
#pragma weak shmem_uint64_max_reduce = pshmem_uint64_max_reduce
#define shmem_uint64_max_reduce pshmem_uint64_max_reduce
#pragma weak shmem_size_max_reduce = pshmem_size_max_reduce
#define shmem_size_max_reduce pshmem_size_max_reduce
#pragma weak shmem_float_max_reduce = pshmem_float_max_reduce
#define shmem_float_max_reduce pshmem_float_max_reduce
#pragma weak shmem_double_max_reduce = pshmem_double_max_reduce
#define shmem_double_max_reduce pshmem_double_max_reduce
#pragma weak shmem_longdouble_max_reduce = pshmem_longdouble_max_reduce
#define shmem_longdouble_max_reduce pshmem_longdouble_max_reduce
/* min */
#pragma weak shmem_char_min_reduce = pshmem_char_min_reduce
#define shmem_char_min_reduce pshmem_char_min_reduce
#pragma weak shmem_schar_min_reduce = pshmem_schar_min_reduce
#define shmem_schar_min_reduce pshmem_schar_min_reduce
#pragma weak shmem_short_min_reduce = pshmem_short_min_reduce
#define shmem_short_min_reduce pshmem_short_min_reduce
#pragma weak shmem_int_min_reduce = pshmem_int_min_reduce
#define shmem_int_min_reduce pshmem_int_min_reduce
#pragma weak shmem_long_min_reduce = pshmem_long_min_reduce
#define shmem_long_min_reduce pshmem_long_min_reduce
#pragma weak shmem_longlong_min_reduce = pshmem_longlong_min_reduce
#define shmem_longlong_min_reduce pshmem_longlong_min_reduce
#pragma weak shmem_ptrdiff_min_reduce = pshmem_ptrdiff_min_reduce
#define shmem_ptrdiff_min_reduce pshmem_ptrdiff_min_reduce
#pragma weak shmem_uchar_min_reduce = pshmem_uchar_min_reduce
#define shmem_uchar_min_reduce pshmem_uchar_min_reduce
#pragma weak shmem_ushort_min_reduce = pshmem_ushort_min_reduce
#define shmem_ushort_min_reduce pshmem_ushort_min_reduce
#pragma weak shmem_uint_min_reduce = pshmem_uint_min_reduce
#define shmem_uint_min_reduce pshmem_uint_min_reduce
#pragma weak shmem_ulong_min_reduce = pshmem_ulong_min_reduce
#define shmem_ulong_min_reduce pshmem_ulong_min_reduce
#pragma weak shmem_ulonglong_min_reduce = pshmem_ulonglong_min_reduce
#define shmem_ulonglong_min_reduce pshmem_ulonglong_min_reduce
#pragma weak shmem_int8_min_reduce = pshmem_int8_min_reduce
#define shmem_int8_min_reduce pshmem_int8_min_reduce
#pragma weak shmem_int16_min_reduce = pshmem_int16_min_reduce
#define shmem_int16_min_reduce pshmem_int16_min_reduce
#pragma weak shmem_int32_min_reduce = pshmem_int32_min_reduce
#define shmem_int32_min_reduce pshmem_int32_min_reduce
#pragma weak shmem_int64_min_reduce = pshmem_int64_min_reduce
#define shmem_int64_min_reduce pshmem_int64_min_reduce
#pragma weak shmem_uint8_min_reduce = pshmem_uint8_min_reduce
#define shmem_uint8_min_reduce pshmem_uint8_min_reduce
#pragma weak shmem_uint16_min_reduce = pshmem_uint16_min_reduce
#define shmem_uint16_min_reduce pshmem_uint16_min_reduce
#pragma weak shmem_uint32_min_reduce = pshmem_uint32_min_reduce
#define shmem_uint32_min_reduce pshmem_uint32_min_reduce
#pragma weak shmem_uint64_min_reduce = pshmem_uint64_min_reduce
#define shmem_uint64_min_reduce pshmem_uint64_min_reduce
#pragma weak shmem_size_min_reduce = pshmem_size_min_reduce
#define shmem_size_min_reduce pshmem_size_min_reduce
#pragma weak shmem_float_min_reduce = pshmem_float_min_reduce
#define shmem_float_min_reduce pshmem_float_min_reduce
#pragma weak shmem_double_min_reduce = pshmem_double_min_reduce
#define shmem_double_min_reduce pshmem_double_min_reduce
#pragma weak shmem_longdouble_min_reduce = pshmem_longdouble_min_reduce
#define shmem_longdouble_min_reduce pshmem_longdouble_min_reduce

#endif /* ENABLE_PSHMEM */

#define DECL_SHIM_MAX_REDUCE(_typename, _type)                                 \
  SHMEM_TYPENAME_OP_REDUCE(_type, _typename, max)
SHMEM_REDUCE_MINMAX_TYPE_TABLE(DECL_SHIM_MAX_REDUCE)
#undef DECL_SHIM_MAX_REDUCE

#define DECL_SHIM_MIN_REDUCE(_typename, _type)                                 \
  SHMEM_TYPENAME_OP_REDUCE(_type, _typename, min)
SHMEM_REDUCE_MINMAX_TYPE_TABLE(DECL_SHIM_MIN_REDUCE)
#undef DECL_SHIM_MIN_REDUCE

#ifdef ENABLE_PSHMEM
/* sum */
#pragma weak shmem_char_sum_reduce = pshmem_char_sum_reduce
#define shmem_char_sum_reduce pshmem_char_sum_reduce
#pragma weak shmem_schar_sum_reduce = pshmem_schar_sum_reduce
#define shmem_schar_sum_reduce pshmem_schar_sum_reduce
#pragma weak shmem_short_sum_reduce = pshmem_short_sum_reduce
#define shmem_short_sum_reduce pshmem_short_sum_reduce
#pragma weak shmem_int_sum_reduce = pshmem_int_sum_reduce
#define shmem_int_sum_reduce pshmem_int_sum_reduce
#pragma weak shmem_long_sum_reduce = pshmem_long_sum_reduce
#define shmem_long_sum_reduce pshmem_long_sum_reduce
#pragma weak shmem_longlong_sum_reduce = pshmem_longlong_sum_reduce
#define shmem_longlong_sum_reduce pshmem_longlong_sum_reduce
#pragma weak shmem_ptrdiff_sum_reduce = pshmem_ptrdiff_sum_reduce
#define shmem_ptrdiff_sum_reduce pshmem_ptrdiff_sum_reduce
#pragma weak shmem_uchar_sum_reduce = pshmem_uchar_sum_reduce
#define shmem_uchar_sum_reduce pshmem_uchar_sum_reduce
#pragma weak shmem_ushort_sum_reduce = pshmem_ushort_sum_reduce
#define shmem_ushort_sum_reduce pshmem_ushort_sum_reduce
#pragma weak shmem_uint_sum_reduce = pshmem_uint_sum_reduce
#define shmem_uint_sum_reduce pshmem_uint_sum_reduce
#pragma weak shmem_ulong_sum_reduce = pshmem_ulong_sum_reduce
#define shmem_ulong_sum_reduce pshmem_ulong_sum_reduce
#pragma weak shmem_ulonglong_sum_reduce = pshmem_ulonglong_sum_reduce
#define shmem_ulonglong_sum_reduce pshmem_ulonglong_sum_reduce
#pragma weak shmem_int8_sum_reduce = pshmem_int8_sum_reduce
#define shmem_int8_sum_reduce pshmem_int8_sum_reduce
#pragma weak shmem_int16_sum_reduce = pshmem_int16_sum_reduce
#define shmem_int16_sum_reduce pshmem_int16_sum_reduce
#pragma weak shmem_int32_sum_reduce = pshmem_int32_sum_reduce
#define shmem_int32_sum_reduce pshmem_int32_sum_reduce
#pragma weak shmem_int64_sum_reduce = pshmem_int64_sum_reduce
#define shmem_int64_sum_reduce pshmem_int64_sum_reduce
#pragma weak shmem_uint8_sum_reduce = pshmem_uint8_sum_reduce
#define shmem_uint8_sum_reduce pshmem_uint8_sum_reduce
#pragma weak shmem_uint16_sum_reduce = pshmem_uint16_sum_reduce
#define shmem_uint16_sum_reduce pshmem_uint16_sum_reduce
#pragma weak shmem_uint32_sum_reduce = pshmem_uint32_sum_reduce
#define shmem_uint32_sum_reduce pshmem_uint32_sum_reduce
#pragma weak shmem_uint64_sum_reduce = pshmem_uint64_sum_reduce
#define shmem_uint64_sum_reduce pshmem_uint64_sum_reduce
#pragma weak shmem_size_sum_reduce = pshmem_size_sum_reduce
#define shmem_size_sum_reduce pshmem_size_sum_reduce
#pragma weak shmem_float_sum_reduce = pshmem_float_sum_reduce
#define shmem_float_sum_reduce pshmem_float_sum_reduce
#pragma weak shmem_double_sum_reduce = pshmem_double_sum_reduce
#define shmem_double_sum_reduce pshmem_double_sum_reduce
#pragma weak shmem_longdouble_sum_reduce = pshmem_longdouble_sum_reduce
#define shmem_longdouble_sum_reduce pshmem_longdouble_sum_reduce
#pragma weak shmem_complexd_sum_reduce = pshmem_complexd_sum_reduce
#define shmem_complexd_sum_reduce pshmem_complexd_sum_reduce
#pragma weak shmem_complexf_sum_reduce = pshmem_complexf_sum_reduce
#define shmem_complexf_sum_reduce pshmem_complexf_sum_reduce
/* prod */
#pragma weak shmem_char_prod_reduce = pshmem_char_prod_reduce
#define shmem_char_prod_reduce pshmem_char_prod_reduce
#pragma weak shmem_schar_prod_reduce = pshmem_schar_prod_reduce
#define shmem_schar_prod_reduce pshmem_schar_prod_reduce
#pragma weak shmem_short_prod_reduce = pshmem_short_prod_reduce
#define shmem_short_prod_reduce pshmem_short_prod_reduce
#pragma weak shmem_int_prod_reduce = pshmem_int_prod_reduce
#define shmem_int_prod_reduce pshmem_int_prod_reduce
#pragma weak shmem_long_prod_reduce = pshmem_long_prod_reduce
#define shmem_long_prod_reduce pshmem_long_prod_reduce
#pragma weak shmem_longlong_prod_reduce = pshmem_longlong_prod_reduce
#define shmem_longlong_prod_reduce pshmem_longlong_prod_reduce
#pragma weak shmem_ptrdiff_prod_reduce = pshmem_ptrdiff_prod_reduce
#define shmem_ptrdiff_prod_reduce pshmem_ptrdiff_prod_reduce
#pragma weak shmem_uchar_prod_reduce = pshmem_uchar_prod_reduce
#define shmem_uchar_prod_reduce pshmem_uchar_prod_reduce
#pragma weak shmem_ushort_prod_reduce = pshmem_ushort_prod_reduce
#define shmem_ushort_prod_reduce pshmem_ushort_prod_reduce
#pragma weak shmem_uint_prod_reduce = pshmem_uint_prod_reduce
#define shmem_uint_prod_reduce pshmem_uint_prod_reduce
#pragma weak shmem_ulong_prod_reduce = pshmem_ulong_prod_reduce
#define shmem_ulong_prod_reduce pshmem_ulong_prod_reduce
#pragma weak shmem_ulonglong_prod_reduce = pshmem_ulonglong_prod_reduce
#define shmem_ulonglong_prod_reduce pshmem_ulonglong_prod_reduce
#pragma weak shmem_int8_prod_reduce = pshmem_int8_prod_reduce
#define shmem_int8_prod_reduce pshmem_int8_prod_reduce
#pragma weak shmem_int16_prod_reduce = pshmem_int16_prod_reduce
#define shmem_int16_prod_reduce pshmem_int16_prod_reduce
#pragma weak shmem_int32_prod_reduce = pshmem_int32_prod_reduce
#define shmem_int32_prod_reduce pshmem_int32_prod_reduce
#pragma weak shmem_int64_prod_reduce = pshmem_int64_prod_reduce
#define shmem_int64_prod_reduce pshmem_int64_prod_reduce
#pragma weak shmem_uint8_prod_reduce = pshmem_uint8_prod_reduce
#define shmem_uint8_prod_reduce pshmem_uint8_prod_reduce
#pragma weak shmem_uint16_prod_reduce = pshmem_uint16_prod_reduce
#define shmem_uint16_prod_reduce pshmem_uint16_prod_reduce
#pragma weak shmem_uint32_prod_reduce = pshmem_uint32_prod_reduce
#define shmem_uint32_prod_reduce pshmem_uint32_prod_reduce
#pragma weak shmem_uint64_prod_reduce = pshmem_uint64_prod_reduce
#define shmem_uint64_prod_reduce pshmem_uint64_prod_reduce
#pragma weak shmem_size_prod_reduce = pshmem_size_prod_reduce
#define shmem_size_prod_reduce pshmem_size_prod_reduce
#pragma weak shmem_float_prod_reduce = pshmem_float_prod_reduce
#define shmem_float_prod_reduce pshmem_float_prod_reduce
#pragma weak shmem_double_prod_reduce = pshmem_double_prod_reduce
#define shmem_double_prod_reduce pshmem_double_prod_reduce
#pragma weak shmem_longdouble_prod_reduce = pshmem_longdouble_prod_reduce
#define shmem_longdouble_prod_reduce pshmem_longdouble_prod_reduce
#pragma weak shmem_complexd_prod_reduce = pshmem_complexd_prod_reduce
#define shmem_complexd_prod_reduce pshmem_complexd_prod_reduce
#pragma weak shmem_complexf_prod_reduce = pshmem_complexf_prod_reduce
#define shmem_complexf_prod_reduce pshmem_complexf_prod_reduce

#endif /* ENABLE_PSHMEM */

#define DECL_SHIM_SUM_REDUCE(_typename, _type)                                 \
  SHMEM_TYPENAME_OP_REDUCE(_type, _typename, sum)
SHMEM_REDUCE_ARITH_TYPE_TABLE(DECL_SHIM_SUM_REDUCE)
#undef DECL_SHIM_SUM_REDUCE

#define DECL_SHIM_PROD_REDUCE(_typename, _type)                                \
  SHMEM_TYPENAME_OP_REDUCE(_type, _typename, prod)
SHMEM_REDUCE_ARITH_TYPE_TABLE(DECL_SHIM_PROD_REDUCE)
#undef DECL_SHIM_PROD_REDUCE

/** @} */

/** @} */

///////////////////////////////////////////////////////////////////////
/**
 * @defgroup barrier Barrier Operations
 * @{
 */

/*
 * sync variables supplied by me
 */
extern long *shmemc_barrier_all_psync;
extern long *shmemc_sync_all_psync;

#ifdef ENABLE_PSHMEM
#pragma weak shmem_barrier_all = pshmem_barrier_all
#define shmem_barrier_all pshmem_barrier_all
#endif /* ENABLE_PSHMEM */

/**
 * @brief Barrier synchronization across all PEs
 */
void shmem_barrier_all(void) {
  logger(LOG_COLLECTIVES, "%s()", __func__);

  colls.barrier_all.f(shmemc_barrier_all_psync);
}

/** @} */

///////////////////////////////////////////////////////////////////////

#ifdef ENABLE_PSHMEM
#pragma weak shmem_sync_all = pshmem_sync_all
#define shmem_sync_all pshmem_sync_all
#endif /* ENABLE_PSHMEM */

/**
 * @brief Synchronize across all PEs
 */
void shmem_sync_all(void) {
  logger(LOG_COLLECTIVES, "%s()", __func__);

  colls.sync_all.f(shmemc_sync_all_psync);
}

/** @} */

///////////////////////////////////////////////////////////////////////

/**
 * @defgroup sync Deprecated Synchronization Operations
 * @{
 */

// #ifdef ENABLE_PSHMEM
// #pragma weak shmem_sync = pshmem_sync
// #define shmem_sync pshmem_sync
// #endif /* ENABLE_PSHMEM */

/**
 * @brief Synchronizes a subset of PEs
 *
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_sync_deprecated(int PE_start, int logPE_stride, int PE_size,
                           long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%d, %d, %zu, %p)", __func__, PE_start,
         logPE_stride, PE_size, pSync);
  colls.sync.f(PE_start, logPE_stride, PE_size, pSync);
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_barrier = pshmem_barrier
#define shmem_barrier pshmem_barrier
#endif /* ENABLE_PSHMEM */

/**
 * @brief Performs a barrier synchronization across a subset of PEs
 *
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
void shmem_barrier(int PE_start, int logPE_stride, int PE_size, long *pSync) {
  logger(LOG_COLLECTIVES, "%s(%d, %d, %zu, %p)", __func__, PE_start,
         logPE_stride, PE_size, pSync);

  colls.barrier.f(PE_start, logPE_stride, PE_size, pSync);
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_team_sync = pshmem_team_sync
#define shmem_team_sync pshmem_team_sync
#endif /* ENABLE_PSHMEM */

/**
 * @brief Synchronizes a team of PEs
 *
 * @param team The team to synchronize
 */
int shmem_team_sync(shmem_team_t team) {
  logger(LOG_COLLECTIVES, "%s(%p)", __func__, team);

  colls.team_sync.f(team);
}

/** @} */
