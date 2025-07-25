/**
 * @file waitall_vector.c
 * @brief Implementation of OpenSHMEM wait operations for vectors
 *
 * This file provides wait operations for vectors that block until all elements
 * meet specified comparison criteria.
 *
 * For license: see LICENSE file at top-level
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <shmem/generics.h>
#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"

#ifdef ENABLE_PSHMEM
#pragma weak shmem_short_wait_until_all_vector =                               \
    pshmem_short_wait_until_all_vector
#define shmem_short_wait_until_all_vector pshmem_short_wait_until_all_vector
#pragma weak shmem_int_wait_until_all_vector = pshmem_int_wait_until_all_vector
#define shmem_int_wait_until_all_vector pshmem_int_wait_until_all_vector
#pragma weak shmem_long_wait_until_all_vector =                                \
    pshmem_long_wait_until_all_vector
#define shmem_long_wait_until_all_vector pshmem_long_wait_until_all_vector
#pragma weak shmem_longlong_wait_until_all_vector =                            \
    pshmem_longlong_wait_until_all_vector
#define shmem_longlong_wait_until_all_vector                                   \
  pshmem_longlong_wait_until_all_vector
#pragma weak shmem_ushort_wait_until_all_vector =                              \
    pshmem_ushort_wait_until_all_vector
#define shmem_ushort_wait_until_all_vector pshmem_ushort_wait_until_all_vector
#pragma weak shmem_uint_wait_until_all_vector =                                \
    pshmem_uint_wait_until_all_vector
#define shmem_uint_wait_until_all_vector pshmem_uint_wait_until_all_vector
#pragma weak shmem_ulong_wait_until_all_vector =                               \
    pshmem_ulong_wait_until_all_vector
#define shmem_ulong_wait_until_all_vector pshmem_ulong_wait_until_all_vector
#pragma weak shmem_ulonglong_wait_until_all_vector =                           \
    pshmem_ulonglong_wait_until_all_vector
#define shmem_ulonglong_wait_until_all_vector                                  \
  pshmem_ulonglong_wait_until_all_vector
#pragma weak shmem_int32_wait_until_all_vector =                               \
    pshmem_int32_wait_until_all_vector
#define shmem_int32_wait_until_all_vector pshmem_int32_wait_until_all_vector
#pragma weak shmem_int64_wait_until_all_vector =                               \
    pshmem_int64_wait_until_all_vector
#define shmem_int64_wait_until_all_vector pshmem_int64_wait_until_all_vector
#pragma weak shmem_uint32_wait_until_all_vector =                              \
    pshmem_uint32_wait_until_all_vector
#define shmem_uint32_wait_until_all_vector pshmem_uint32_wait_until_all_vector
#pragma weak shmem_uint64_wait_until_all_vector =                              \
    pshmem_uint64_wait_until_all_vector
#define shmem_uint64_wait_until_all_vector pshmem_uint64_wait_until_all_vector
#pragma weak shmem_size_wait_until_all_vector =                                \
    pshmem_size_wait_until_all_vector
#define shmem_size_wait_until_all_vector pshmem_size_wait_until_all_vector
#pragma weak shmem_ptrdiff_wait_until_all_vector =                             \
    pshmem_ptrdiff_wait_until_all_vector
#define shmem_ptrdiff_wait_until_all_vector pshmem_ptrdiff_wait_until_all_vector
#endif /* ENABLE_PSHMEM */

/**
 * @brief Waits until all elements in a vector meet specified comparison
 * criteria
 *
 * @param _opname Base name of the operation (e.g. short, int, etc)
 * @param _type C data type for the operation
 * @param _size Size in bits (16, 32, or 64)
 *
 * Blocks until all elements in a vector meet specified comparison criteria.
 *
 * @param ivars Array of variables to be tested
 * @param nelems Number of elements in the array
 * @param status Array indicating which elements to test (1=test, 0=skip)
 * @param cmp Comparison operator (SHMEM_CMP_EQ, NE, GT, LE, LT, GE)
 * @param cmp_values Array of values to compare against
 */
#define SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(_opname, _type, _size)                \
  void shmem_##_opname##_wait_until_all_vector(_type *ivars, size_t nelems,    \
                                               const int *status, int cmp,     \
                                               _type *cmp_values) {            \
    SHMEMT_MUTEX_PROTECT(switch (cmp) {                                        \
      case SHMEM_CMP_EQ:                                                       \
        shmemc_ctx_wait_until_all_vector_eq##_size(                            \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      case SHMEM_CMP_NE:                                                       \
        shmemc_ctx_wait_until_all_vector_ne##_size(                            \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      case SHMEM_CMP_GT:                                                       \
        shmemc_ctx_wait_until_all_vector_gt##_size(                            \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      case SHMEM_CMP_LE:                                                       \
        shmemc_ctx_wait_until_all_vector_le##_size(                            \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      case SHMEM_CMP_LT:                                                       \
        shmemc_ctx_wait_until_all_vector_lt##_size(                            \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      case SHMEM_CMP_GE:                                                       \
        shmemc_ctx_wait_until_all_vector_ge##_size(                            \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      default:                                                                 \
        shmemu_fatal("unknown operator (code %d) in \"%s\"", cmp, __func__);   \
        /* NOT REACHED */                                                      \
        break;                                                                 \
    });                                                                        \
  }

#define SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR_HELPER(CTYPE, SHMTYPE)                \
  SHMEM_APPLY(SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR, SHMTYPE, CTYPE,                \
              SHMEM_TYPE_BITSOF_##SHMTYPE)

C11_SHMEM_PT2PT_SYNC_TYPE_TABLE(SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR_HELPER)
