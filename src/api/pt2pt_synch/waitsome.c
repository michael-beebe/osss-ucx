/**
 * @file waitsome.c
 * @brief Implementation of OpenSHMEM wait operations
 *
 * This file provides wait operations that block until some elements in an array
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
#pragma weak shmem_short_wait_until_some = pshmem_short_wait_until_some
#define shmem_short_wait_until_some pshmem_short_wait_until_some
#pragma weak shmem_int_wait_until_some = pshmem_int_wait_until_some
#define shmem_int_wait_until_some pshmem_int_wait_until_some
#pragma weak shmem_long_wait_until_some = pshmem_long_wait_until_some
#define shmem_long_wait_until_some pshmem_long_wait_until_some
#pragma weak shmem_longlong_wait_until_some = pshmem_longlong_wait_until_some
#define shmem_longlong_wait_until_some pshmem_longlong_wait_until_some
#pragma weak shmem_ushort_wait_until_some = pshmem_ushort_wait_until_some
#define shmem_ushort_wait_until_some pshmem_ushort_wait_until_some
#pragma weak shmem_uint_wait_until_some = pshmem_uint_wait_until_some
#define shmem_uint_wait_until_some pshmem_uint_wait_until_some
#pragma weak shmem_ulong_wait_until_some = pshmem_ulong_wait_until_some
#define shmem_ulong_wait_until_some pshmem_ulong_wait_until_some
#pragma weak shmem_ulonglong_wait_until_some = pshmem_ulonglong_wait_until_some
#define shmem_ulonglong_wait_until_some pshmem_ulonglong_wait_until_some
#pragma weak shmem_int32_wait_until_some = pshmem_int32_wait_until_some
#define shmem_int32_wait_until_some pshmem_int32_wait_until_some
#pragma weak shmem_int64_wait_until_some = pshmem_int64_wait_until_some
#define shmem_int64_wait_until_some pshmem_int64_wait_until_some
#pragma weak shmem_uint32_wait_until_some = pshmem_uint32_wait_until_some
#define shmem_uint32_wait_until_some pshmem_uint32_wait_until_some
#pragma weak shmem_uint64_wait_until_some = pshmem_uint64_wait_until_some
#define shmem_uint64_wait_until_some pshmem_uint64_wait_until_some
#pragma weak shmem_size_wait_until_some = pshmem_size_wait_until_some
#define shmem_size_wait_until_some pshmem_size_wait_until_some
#pragma weak shmem_ptrdiff_wait_until_some = pshmem_ptrdiff_wait_until_some
#define shmem_ptrdiff_wait_until_some pshmem_ptrdiff_wait_until_some
#endif /* ENABLE_PSHMEM */

/**
 * @brief Waits until some elements in an array meet specified comparison
 * criteria
 *
 * @param _opname Base name of the operation (e.g. short, int, etc)
 * @param _type C data type for the operation
 * @param _size Size in bits (16, 32, or 64)
 *
 * Blocks until some elements in an array meet specified comparison criteria.
 * Returns the number of elements that satisfied the criteria and stores their
 * indices.
 *
 * @param ivars Array of variables to be tested
 * @param nelems Number of elements in the array
 * @param idxs Array to store indices of elements that satisfied criteria
 * @param status Array indicating which elements to test (0=test, nonzero=skip)
 * @param cmp Comparison operator (SHMEM_CMP_EQ, NE, GT, LE, LT, GE)
 * @param cmp_value Value to compare against
 *
 * @return Number of elements that satisfied the criteria, or -1 on error
 */
#define SHMEM_TYPE_WAIT_UNTIL_SOME(_opname, _type, _size)                      \
  size_t shmem_##_opname##_wait_until_some(_type *ivars, size_t nelems,        \
                                           size_t *idxs, const int *status,    \
                                           int cmp, _type cmp_value) {         \
    size_t result = 0;                                                         \
    SHMEMT_MUTEX_PROTECT(switch (cmp) {                                        \
      case SHMEM_CMP_EQ:                                                       \
        result = shmemc_ctx_wait_until_some_eq##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, idxs, status,  \
            cmp_value);                                                        \
        break;                                                                 \
      case SHMEM_CMP_NE:                                                       \
        result = shmemc_ctx_wait_until_some_ne##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, idxs, status,  \
            cmp_value);                                                        \
        break;                                                                 \
      case SHMEM_CMP_GT:                                                       \
        result = shmemc_ctx_wait_until_some_gt##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, idxs, status,  \
            cmp_value);                                                        \
        break;                                                                 \
      case SHMEM_CMP_LE:                                                       \
        result = shmemc_ctx_wait_until_some_le##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, idxs, status,  \
            cmp_value);                                                        \
        break;                                                                 \
      case SHMEM_CMP_LT:                                                       \
        result = shmemc_ctx_wait_until_some_lt##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, idxs, status,  \
            cmp_value);                                                        \
        break;                                                                 \
      case SHMEM_CMP_GE:                                                       \
        result = shmemc_ctx_wait_until_some_ge##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, idxs, status,  \
            cmp_value);                                                        \
        break;                                                                 \
      default:                                                                 \
        shmemu_fatal("unknown operator (code %d) in \"%s\"", cmp, __func__);   \
        result = -1;                                                           \
        /* NOT REACHED */                                                      \
        break;                                                                 \
    });                                                                        \
    return result;                                                             \
  }

#define SHMEM_TYPE_WAIT_UNTIL_SOME_HELPER(CTYPE, SHMTYPE)                      \
  SHMEM_APPLY(SHMEM_TYPE_WAIT_UNTIL_SOME, SHMTYPE, CTYPE,                      \
              SHMEM_TYPE_BITSOF_##SHMTYPE)

C11_SHMEM_PT2PT_SYNC_TYPE_TABLE(SHMEM_TYPE_WAIT_UNTIL_SOME_HELPER)
