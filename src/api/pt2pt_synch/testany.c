/**
 * @file testany.c
 * @brief Implementation of OpenSHMEM test operations
 *
 * This file provides test operations that check if any elements in an array
 * meet specified comparison criteria. The operations are non-blocking and
 * return immediately.
 *
 * For license: see LICENSE file at top-level
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <shmem/generics.h>
#include "shmem_mutex.h"
#include "module.h"
#include "shmemu.h"
#include "shmemc.h"

#ifdef ENABLE_PSHMEM
#pragma weak shmem_short_test_any = pshmem_short_test_any
#define shmem_short_test_any pshmem_short_test_any
#pragma weak shmem_int_test_any = pshmem_int_test_any
#define shmem_int_test_any pshmem_int_test_any
#pragma weak shmem_long_test_any = pshmem_long_test_any
#define shmem_long_test_any pshmem_long_test_any
#pragma weak shmem_longlong_test_any = pshmem_longlong_test_any
#define shmem_longlong_test_any pshmem_longlong_test_any
#pragma weak shmem_ushort_test_any = pshmem_ushort_test_any
#define shmem_ushort_test_any pshmem_ushort_test_any
#pragma weak shmem_uint_test_any = pshmem_uint_test_any
#define shmem_uint_test_any pshmem_uint_test_any
#pragma weak shmem_ulong_test_any = pshmem_ulong_test_any
#define shmem_ulong_test_any pshmem_ulong_test_any
#pragma weak shmem_ulonglong_test_any = pshmem_ulonglong_test_any
#define shmem_ulonglong_test_any pshmem_ulonglong_test_any
#pragma weak shmem_int32_test_any = pshmem_int32_test_any
#define shmem_int32_test_any pshmem_int32_test_any
#pragma weak shmem_int64_test_any = pshmem_int64_test_any
#define shmem_int64_test_any pshmem_int64_test_any
#pragma weak shmem_uint32_test_any = pshmem_uint32_test_any
#define shmem_uint32_test_any pshmem_uint32_test_any
#pragma weak shmem_uint64_test_any = pshmem_uint64_test_any
#define shmem_uint64_test_any pshmem_uint64_test_any
#pragma weak shmem_size_test_any = pshmem_size_test_any
#define shmem_size_test_any pshmem_size_test_any
#pragma weak shmem_ptrdiff_test_any = pshmem_ptrdiff_test_any
#define shmem_ptrdiff_test_any pshmem_ptrdiff_test_any
#endif /* ENABLE_PSHMEM */

/**
 * @brief Tests if any elements in an array meet specified comparison criteria
 *
 * @param _opname Base name of the operation (e.g. short, int, etc)
 * @param _type C data type for the operation
 * @param _size Size in bits (16, 32, or 64)
 *
 * Tests if any elements meet specified comparison criteria. The function
 * returns immediately without blocking.
 *
 * @param ivars Array of variables to be tested
 * @param nelems Number of elements in the array
 * @param status Array indicating which elements to test (0=test, nonzero=skip)
 * @param cmp Comparison operator (SHMEM_CMP_EQ, NE, GT, LE, LT, GE)
 * @param cmp_value Value to compare against
 *
 * @return Returns an index that satisfies the comparison, or SIZE_MAX if none
 * satisfy it or the test set is empty
 */
#define SHMEM_TYPE_TEST_ANY(_opname, _type, _size)                             \
  size_t shmem_##_opname##_test_any(_type *ivars, size_t nelems,               \
                                    const int *status, int cmp,                \
                                    _type cmp_value) {                         \
    size_t result = 0;                                                         \
    SHMEMT_MUTEX_PROTECT(switch (cmp) {                                        \
      case SHMEM_CMP_EQ:                                                       \
        result = shmemc_ctx_test_any_eq##_size(SHMEM_CTX_DEFAULT,              \
                                               (int##_size##_t *)ivars,        \
                                               nelems, status, cmp_value);     \
        break;                                                                 \
      case SHMEM_CMP_NE:                                                       \
        result = shmemc_ctx_test_any_ne##_size(SHMEM_CTX_DEFAULT,              \
                                               (int##_size##_t *)ivars,        \
                                               nelems, status, cmp_value);     \
        break;                                                                 \
      case SHMEM_CMP_GT:                                                       \
        result = shmemc_ctx_test_any_gt##_size(SHMEM_CTX_DEFAULT,              \
                                               (int##_size##_t *)ivars,        \
                                               nelems, status, cmp_value);     \
        break;                                                                 \
      case SHMEM_CMP_LE:                                                       \
        result = shmemc_ctx_test_any_le##_size(SHMEM_CTX_DEFAULT,              \
                                               (int##_size##_t *)ivars,        \
                                               nelems, status, cmp_value);     \
        break;                                                                 \
      case SHMEM_CMP_LT:                                                       \
        result = shmemc_ctx_test_any_lt##_size(SHMEM_CTX_DEFAULT,              \
                                               (int##_size##_t *)ivars,        \
                                               nelems, status, cmp_value);     \
        break;                                                                 \
      case SHMEM_CMP_GE:                                                       \
        result = shmemc_ctx_test_any_ge##_size(SHMEM_CTX_DEFAULT,              \
                                               (int##_size##_t *)ivars,        \
                                               nelems, status, cmp_value);     \
        break;                                                                 \
      default:                                                                 \
        shmemu_fatal(MODULE ": unknown operator (code %d) in \"%s\"", cmp,     \
                     __func__);                                                \
        result = -1;                                                           \
        /* NOT REACHED */                                                      \
        break;                                                                 \
    });                                                                        \
    return result;                                                             \
  }

#define SHMEM_TYPE_TEST_ANY_HELPER(CTYPE, SHMTYPE)                             \
  SHMEM_APPLY(SHMEM_TYPE_TEST_ANY, SHMTYPE, CTYPE, SHMEM_TYPE_BITSOF_##SHMTYPE)

C11_SHMEM_PT2PT_SYNC_TYPE_TABLE(SHMEM_TYPE_TEST_ANY_HELPER)
