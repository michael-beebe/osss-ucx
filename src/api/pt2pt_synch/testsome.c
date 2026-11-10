/**
 * @file testsome.c
 * @brief Implementation of OpenSHMEM test operations
 *
 * This file provides test operations that check if some elements in an array
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
#pragma weak shmem_short_test_some = pshmem_short_test_some
#define shmem_short_test_some pshmem_short_test_some
#pragma weak shmem_int_test_some = pshmem_int_test_some
#define shmem_int_test_some pshmem_int_test_some
#pragma weak shmem_long_test_some = pshmem_long_test_some
#define shmem_long_test_some pshmem_long_test_some
#pragma weak shmem_longlong_test_some = pshmem_longlong_test_some
#define shmem_longlong_test_some pshmem_longlong_test_some
#pragma weak shmem_ushort_test_some = pshmem_ushort_test_some
#define shmem_ushort_test_some pshmem_ushort_test_some
#pragma weak shmem_uint_test_some = pshmem_uint_test_some
#define shmem_uint_test_some pshmem_uint_test_some
#pragma weak shmem_ulong_test_some = pshmem_ulong_test_some
#define shmem_ulong_test_some pshmem_ulong_test_some
#pragma weak shmem_ulonglong_test_some = pshmem_ulonglong_test_some
#define shmem_ulonglong_test_some pshmem_ulonglong_test_some
#pragma weak shmem_int32_test_some = pshmem_int32_test_some
#define shmem_int32_test_some pshmem_int32_test_some
#pragma weak shmem_int64_test_some = pshmem_int64_test_some
#define shmem_int64_test_some pshmem_int64_test_some
#pragma weak shmem_uint32_test_some = pshmem_uint32_test_some
#define shmem_uint32_test_some pshmem_uint32_test_some
#pragma weak shmem_uint64_test_some = pshmem_uint64_test_some
#define shmem_uint64_test_some pshmem_uint64_test_some
#pragma weak shmem_size_test_some = pshmem_size_test_some
#define shmem_size_test_some pshmem_size_test_some
#pragma weak shmem_ptrdiff_test_some = pshmem_ptrdiff_test_some
#define shmem_ptrdiff_test_some pshmem_ptrdiff_test_some
#endif /* ENABLE_PSHMEM */

/**
 * @brief Tests if some elements in an array meet specified comparison criteria
 *
 * @param _opname Base name of the operation (e.g. short, int, etc)
 * @param _type C data type for the operation
 * @param _size Size in bits (16, 32, or 64)
 *
 * Tests if some elements meet specified comparison criteria. The function
 * returns immediately without blocking.
 *
 * @param ivars Array of variables to be tested
 * @param nelems Number of elements in the array
 * @param indices Array to store indices of elements that evaluate to true
 * @param status Array indicating which elements to test (0=test, nonzero=skip)
 * @param cmp Comparison operator (SHMEM_CMP_EQ, NE, GT, LE, LT, GE)
 * @param cmp_value Value to compare against
 *
 * @return Returns number of elements that evaluated to true
 */
#define SHMEM_TYPE_TEST_SOME(_opname, _type, _size)                            \
  size_t shmem_##_opname##_test_some(_type *ivars, size_t nelems,              \
                                     size_t *indices, const int *status,       \
                                     int cmp, _type cmp_value) {               \
    size_t result = 0;                                                         \
    SHMEMT_MUTEX_PROTECT(switch (cmp) {                                        \
      case SHMEM_CMP_EQ:                                                       \
        result = shmemc_ctx_test_some_eq##_size(                               \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, indices,       \
            status, cmp_value);                                                \
        break;                                                                 \
      case SHMEM_CMP_NE:                                                       \
        result = shmemc_ctx_test_some_ne##_size(                               \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, indices,       \
            status, cmp_value);                                                \
        break;                                                                 \
      case SHMEM_CMP_GT:                                                       \
        result = shmemc_ctx_test_some_gt##_size(                               \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, indices,       \
            status, cmp_value);                                                \
        break;                                                                 \
      case SHMEM_CMP_LE:                                                       \
        result = shmemc_ctx_test_some_le##_size(                               \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, indices,       \
            status, cmp_value);                                                \
        break;                                                                 \
      case SHMEM_CMP_LT:                                                       \
        result = shmemc_ctx_test_some_lt##_size(                               \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, indices,       \
            status, cmp_value);                                                \
        break;                                                                 \
      case SHMEM_CMP_GE:                                                       \
        result = shmemc_ctx_test_some_ge##_size(                               \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, indices,       \
            status, cmp_value);                                                \
        break;                                                                 \
      default:                                                                 \
        shmemu_fatal(MODULE ":unknown operator (code %d) in \"%s\"", cmp,      \
                     __func__);                                                \
        result = -1;                                                           \
        /* NOT REACHED */                                                      \
        break;                                                                 \
    });                                                                        \
    return result;                                                             \
  }

#define SHMEM_TYPE_TEST_SOME_HELPER(CTYPE, SHMTYPE)                            \
  SHMEM_APPLY(SHMEM_TYPE_TEST_SOME, SHMTYPE, CTYPE, SHMEM_TYPE_BITSOF_##SHMTYPE)

C11_SHMEM_PT2PT_SYNC_TYPE_TABLE(SHMEM_TYPE_TEST_SOME_HELPER)
