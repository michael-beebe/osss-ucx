/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmem_mutex.h"
#include "module.h"
#include "shmemu.h"
#include "shmemc.h"

/*
 * Weak symbol declarations for SHMEM test_any_vector functions.
 * These enable optional profiling (PSHMEM) versions of the test functions.
 * If ENABLE_PSHMEM is defined, the symbols will point to their corresponding 
 * profiling versions. Otherwise, the default SHMEM functions are used.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_short_test_any_vector = pshmem_short_test_any_vector
#define shmem_short_test_any_vector pshmem_short_test_any_vector
#pragma weak shmem_int_test_any_vector = pshmem_int_test_any_vector
#define shmem_int_test_any_vector pshmem_int_test_any_vector
#pragma weak shmem_long_test_any_vector = pshmem_long_test_any_vector
#define shmem_long_test_any_vector pshmem_long_test_any_vector
#pragma weak shmem_longlong_test_any_vector = pshmem_longlong_test_any_vector
#define shmem_longlong_test_any_vector pshmem_longlong_test_any_vector
#pragma weak shmem_ushort_test_any_vector = pshmem_ushort_test_any_vector
#define shmem_ushort_test_any_vector pshmem_ushort_test_any_vector
#pragma weak shmem_uint_test_any_vector = pshmem_uint_test_any_vector
#define shmem_uint_test_any_vector pshmem_uint_test_any_vector
#pragma weak shmem_ulong_test_any_vector = pshmem_ulong_test_any_vector
#define shmem_ulong_test_any_vector pshmem_ulong_test_any_vector
#pragma weak shmem_ulonglong_test_any_vector = pshmem_ulonglong_test_any_vector
#define shmem_ulonglong_test_any_vector pshmem_ulonglong_test_any_vector
#pragma weak shmem_int32_test_any_vector = pshmem_int32_test_any_vector
#define shmem_int32_test_any_vector pshmem_int32_test_any_vector
#pragma weak shmem_int64_test_any_vector = pshmem_int64_test_any_vector
#define shmem_int64_test_any_vector pshmem_int64_test_any_vector
#pragma weak shmem_uint32_test_any_vector = pshmem_uint32_test_any_vector
#define shmem_uint32_test_any_vector pshmem_uint32_test_any_vector
#pragma weak shmem_uint64_test_any_vector = pshmem_uint64_test_any_vector
#define shmem_uint64_test_any_vector pshmem_uint64_test_any_vector
#pragma weak shmem_size_test_any_vector = pshmem_size_test_any_vector
#define shmem_size_test_any_vector pshmem_size_test_any_vector
#pragma weak shmem_ptrdiff_test_any_vector = pshmem_ptrdiff_test_any_vector
#define shmem_ptrdiff_test_any_vector pshmem_ptrdiff_test_any_vector
#endif  /* ENABLE_PSHMEM */

/*
 * Macro for defining SHMEM test_any_vector functions for specific types.
 * 
 * SHMEM_TYPE_TEST_ANY_VECTOR generates functions that perform element-wise
 * comparisons in a vector (array) of elements (_opname). The function 
 * returns the first occurrence of an element that matches the condition specified
 * by the comparison operator (cmp).
 *
 * Parameters:
 * _opname: operation name (e.g., short, int)
 * _type: data type (e.g., short, int)
 * _size: size of the data type in bits (e.g., 16, 32, 64)
 */
#define SHMEM_TYPE_TEST_ANY_VECTOR(_opname, _type, _size)               \
    size_t                                                              \
    shmem_##_opname##_test_any_vector(_type *ivars, size_t nelems,      \
                                      const int *status,                \
                                      int cmp, _type *cmp_values)       \
    {                                                                   \
        SHMEMT_MUTEX_PROTECT                                            \
            (                                                           \
             switch (cmp) {                                             \
             case SHMEM_CMP_EQ:                                         \
                 return shmemc_ctx_test_any_vector_eq##_size(SHMEM_CTX_DEFAULT, \
                                                             (int##_size##_t *) ivars, \
                                                             nelems, status, \
                                                             cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_NE:                                         \
                 return shmemc_ctx_test_any_vector_ne##_size(SHMEM_CTX_DEFAULT, \
                                                             (int##_size##_t *) ivars, \
                                                             nelems, status, \
                                                             cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_GT:                                         \
                 return shmemc_ctx_test_any_vector_gt##_size(SHMEM_CTX_DEFAULT, \
                                                             (int##_size##_t *) ivars, \
                                                             nelems, status, \
                                                             cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_LE:                                         \
                 return shmemc_ctx_test_any_vector_le##_size(SHMEM_CTX_DEFAULT, \
                                                             (int##_size##_t *) ivars, \
                                                             nelems, status, \
                                                             cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_LT:                                         \
                 return shmemc_ctx_test_any_vector_lt##_size(SHMEM_CTX_DEFAULT, \
                                                             (int##_size##_t *) ivars, \
                                                             nelems, status, \
                                                             cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_GE:                                         \
                 return shmemc_ctx_test_any_vector_ge##_size(SHMEM_CTX_DEFAULT, \
                                                             (int##_size##_t *) ivars, \
                                                             nelems, status, \
                                                             cmp_values); \
                 break;                                                 \
             default:                                                   \
                 shmemu_fatal(MODULE ": unknown operator (code %d) in \"%s\"", \
                              cmp,                                      \
                              __func__                                  \
                              );                                        \
                 return -1;                                             \
                 /* NOT REACHED */                                      \
                 break;                                                 \
             }                                                          \
                                                                        ); \
    }

/*
 * Implement the test_any_vector function for various types and sizes
 * using the SHMEM_TYPE_TEST_ANY_VECTOR macro. These functions check
 * if any element in the vector matches the given comparison condition.
 */
SHMEM_TYPE_TEST_ANY_VECTOR(short, short, 16)
SHMEM_TYPE_TEST_ANY_VECTOR(int, int, 32)
SHMEM_TYPE_TEST_ANY_VECTOR(long, long, 64)
SHMEM_TYPE_TEST_ANY_VECTOR(longlong, long long, 64)
SHMEM_TYPE_TEST_ANY_VECTOR(ushort, unsigned short, 16)
SHMEM_TYPE_TEST_ANY_VECTOR(uint, unsigned int, 32)
SHMEM_TYPE_TEST_ANY_VECTOR(ulong, unsigned long, 64)
SHMEM_TYPE_TEST_ANY_VECTOR(ulonglong, unsigned long long, 64)
SHMEM_TYPE_TEST_ANY_VECTOR(int32, int32_t, 32)
SHMEM_TYPE_TEST_ANY_VECTOR(int64, int64_t, 64)
SHMEM_TYPE_TEST_ANY_VECTOR(uint32, uint32_t, 32)
SHMEM_TYPE_TEST_ANY_VECTOR(uint64, uint64_t, 64)
SHMEM_TYPE_TEST_ANY_VECTOR(size, size_t, 64)
SHMEM_TYPE_TEST_ANY_VECTOR(ptrdiff, ptrdiff_t, 64)
