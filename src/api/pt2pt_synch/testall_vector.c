/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <shmem/generics.h>
#include "shmem_mutex.h"
#include "module.h"
#include "shmemu.h"
#include "shmemc.h"

#ifdef ENABLE_PSHMEM
#pragma weak shmem_short_test_all_vector = pshmem_short_test_all_vector
#define shmem_short_test_all_vector pshmem_short_test_all_vector
#pragma weak shmem_int_test_all_vector = pshmem_int_test_all_vector
#define shmem_int_test_all_vector pshmem_int_test_all_vector
#pragma weak shmem_long_test_all_vector = pshmem_long_test_all_vector
#define shmem_long_test_all_vector pshmem_long_test_all_vector
#pragma weak shmem_longlong_test_all_vector = pshmem_longlong_test_all_vector
#define shmem_longlong_test_all_vector pshmem_longlong_test_all_vector
#pragma weak shmem_ushort_test_all_vector = pshmem_ushort_test_all_vector
#define shmem_ushort_test_all_vector pshmem_ushort_test_all_vector
#pragma weak shmem_uint_test_all_vector = pshmem_uint_test_all_vector
#define shmem_uint_test_all_vector pshmem_uint_test_all_vector
#pragma weak shmem_ulong_test_all_vector = pshmem_ulong_test_all_vector
#define shmem_ulong_test_all_vector pshmem_ulong_test_all_vector
#pragma weak shmem_ulonglong_test_all_vector = pshmem_ulonglong_test_all_vector
#define shmem_ulonglong_test_all_vector pshmem_ulonglong_test_all_vector
#pragma weak shmem_int32_test_all_vector = pshmem_int32_test_all_vector
#define shmem_int32_test_all_vector pshmem_int32_test_all_vector
#pragma weak shmem_int64_test_all_vector = pshmem_int64_test_all_vector
#define shmem_int64_test_all_vector pshmem_int64_test_all_vector
#pragma weak shmem_uint32_test_all_vector = pshmem_uint32_test_all_vector
#define shmem_uint32_test_all_vector pshmem_uint32_test_all_vector
#pragma weak shmem_uint64_test_all_vector = pshmem_uint64_test_all_vector
#define shmem_uint64_test_all_vector pshmem_uint64_test_all_vector
#pragma weak shmem_size_test_all_vector = pshmem_size_test_all_vector
#define shmem_size_test_all_vector pshmem_size_test_all_vector
#pragma weak shmem_ptrdiff_test_all_vector = pshmem_ptrdiff_test_all_vector
#define shmem_ptrdiff_test_all_vector pshmem_ptrdiff_test_all_vector
#endif /* ENABLE_PSHMEM */

/**
 * @brief Generic test macro for different data types that tests if all elements
 * in a vector meet specified comparison criteria
 *
 * @param _opname Base name of the operation (e.g. short, int, etc)
 * @param _type C data type for the operation
 * @param _size Size in bits (16, 32, or 64)
 *
 * Tests if all elements in a vector meet specified comparison criteria. The
 * function returns immediately without blocking.
 *
 * @param ivars Array of variables to be tested
 * @param nelems Number of elements in the array
 * @param status Array indicating which elements to test (0=test, nonzero=skip)
 * @param cmp Comparison operator (SHMEM_CMP_EQ, NE, GT, LE, LT, GE)
 * @param cmp_values Array of values to compare against
 *
 * @return Returns 1 if all selected comparisons evaluate to true, 0 if any
 * evaluate to false
 */
#define SHMEM_TYPE_TEST_ALL_VECTOR(_opname, _type, _size)                      \
  int shmem_##_opname##_test_all_vector(_type *ivars, size_t nelems,           \
                                        const int *status, int cmp,            \
                                        const _type *cmp_values) {             \
    int result = 0;                                                            \
    SHMEMT_MUTEX_PROTECT(switch (cmp) {                                        \
      case SHMEM_CMP_EQ:                                                       \
        result = shmemc_ctx_test_all_vector_eq##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      case SHMEM_CMP_NE:                                                       \
        result = shmemc_ctx_test_all_vector_ne##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      case SHMEM_CMP_GT:                                                       \
        result = shmemc_ctx_test_all_vector_gt##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      case SHMEM_CMP_LE:                                                       \
        result = shmemc_ctx_test_all_vector_le##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      case SHMEM_CMP_LT:                                                       \
        result = shmemc_ctx_test_all_vector_lt##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
        break;                                                                 \
      case SHMEM_CMP_GE:                                                       \
        result = shmemc_ctx_test_all_vector_ge##_size(                         \
            SHMEM_CTX_DEFAULT, (int##_size##_t *)ivars, nelems, status,        \
            cmp_values);                                                       \
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

#define SHMEM_TYPE_TEST_ALL_VECTOR_HELPER(CTYPE, SHMTYPE)                      \
  SHMEM_APPLY(SHMEM_TYPE_TEST_ALL_VECTOR, SHMTYPE, CTYPE,                      \
              SHMEM_TYPE_BITSOF_##SHMTYPE)

C11_SHMEM_PT2PT_SYNC_TYPE_TABLE(SHMEM_TYPE_TEST_ALL_VECTOR_HELPER)
