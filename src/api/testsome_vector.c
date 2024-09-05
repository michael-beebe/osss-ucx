/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmem_mutex.h"
#include "module.h"
#include "shmemu.h"
#include "shmemc.h"

/*
 * Weak symbol declarations for SHMEM test_some_vector functions.
 * These enable optional profiling (PSHMEM) versions of the test functions.
 * If ENABLE_PSHMEM is defined, the symbols will point to their corresponding 
 * profiling versions. Otherwise, the default SHMEM functions are used.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_short_test_some_vector = pshmem_short_test_some_vector
#define shmem_short_test_some_vector pshmem_short_test_some_vector
#pragma weak shmem_int_test_some_vector = pshmem_int_test_some_vector
#define shmem_int_test_some_vector pshmem_int_test_some_vector
#pragma weak shmem_long_test_some_vector = pshmem_long_test_some_vector
#define shmem_long_test_some_vector pshmem_long_test_some_vector
#pragma weak shmem_longlong_test_some_vector = pshmem_longlong_test_some_vector
#define shmem_longlong_test_some_vector pshmem_longlong_test_some_vector
#pragma weak shmem_ushort_test_some_vector = pshmem_ushort_test_some_vector
#define shmem_ushort_test_some_vector pshmem_ushort_test_some_vector
#pragma weak shmem_uint_test_some_vector = pshmem_uint_test_some_vector
#define shmem_uint_test_some_vector pshmem_uint_test_some_vector
#pragma weak shmem_ulong_test_some_vector = pshmem_ulong_test_some_vector
#define shmem_ulong_test_some_vector pshmem_ulong_test_some_vector
#pragma weak shmem_ulonglong_test_some_vector = pshmem_ulonglong_test_some_vector
#define shmem_ulonglong_test_some_vector pshmem_ulonglong_test_some_vector
#pragma weak shmem_int32_test_some_vector = pshmem_int32_test_some_vector
#define shmem_int32_test_some_vector pshmem_int32_test_some_vector
#pragma weak shmem_int64_test_some_vector = pshmem_int64_test_some_vector
#define shmem_int64_test_some_vector pshmem_int64_test_some_vector
#pragma weak shmem_uint32_test_some_vector = pshmem_uint32_test_some_vector
#define shmem_uint32_test_some_vector pshmem_uint32_test_some_vector
#pragma weak shmem_uint64_test_some_vector = pshmem_uint64_test_some_vector
#define shmem_uint64_test_some_vector pshmem_uint64_test_some_vector
#pragma weak shmem_size_test_some_vector = pshmem_size_test_some_vector
#define shmem_size_test_some_vector pshmem_size_test_some_vector
#pragma weak shmem_ptrdiff_test_some_vector = pshmem_ptrdiff_test_some_vector
#define shmem_ptrdiff_test_some_vector pshmem_ptrdiff_test_some_vector
#endif  /* ENABLE_PSHMEM */

/*
 * Macro for defining SHMEM test_some_vector functions for specific types.
 * 
 * SHMEM_TYPE_TEST_SOME_VECTOR generates functions that test whether some elements
 * in an array (_opname) satisfy the comparison condition (cmp).
 * The function returns the number of matching elements and stores their indices in an array.
 *
 * Parameters:
 * _opname: operation name (e.g., short, int)
 * _type: data type (e.g., short, int)
 * _size: size of the data type in bits (e.g., 16, 32, 64)
 */
#define SHMEM_TYPE_TEST_SOME_VECTOR(_opname, _type, _size)              \
    size_t                                                              \
    shmem_##_opname##_test_some_vector(_type *ivars, size_t nelems,     \
                                       size_t *indices,                 \
                                       const int *status,               \
                                       int cmp, _type *cmp_values)      \
    {                                                                   \
        SHMEMT_MUTEX_PROTECT                                            \
            (                                                           \
             switch (cmp) {                                             \
             case SHMEM_CMP_EQ:                                         \
                 return shmemc_ctx_test_some_vector_eq##_size(SHMEM_CTX_DEFAULT, \
                                                              (int##_size##_t *) ivars, \
                                                              nelems,   \
                                                              indices, status, \
                                                              cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_NE:                                         \
                 return shmemc_ctx_test_some_vector_ne##_size(SHMEM_CTX_DEFAULT, \
                                                              (int##_size##_t *) ivars, \
                                                              nelems,   \
                                                              indices, status, \
                                                              cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_GT:                                         \
                 return shmemc_ctx_test_some_vector_gt##_size(SHMEM_CTX_DEFAULT, \
                                                              (int##_size##_t *) ivars, \
                                                              nelems,   \
                                                              indices, status, \
                                                              cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_LE:                                         \
                 return shmemc_ctx_test_some_vector_le##_size(SHMEM_CTX_DEFAULT, \
                                                              (int##_size##_t *) ivars, \
                                                              nelems,   \
                                                              indices, status, \
                                                              cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_LT:                                         \
                 return shmemc_ctx_test_some_vector_lt##_size(SHMEM_CTX_DEFAULT, \
                                                              (int##_size##_t *) ivars, \
                                                              nelems,   \
                                                              indices, status, \
                                                              cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_GE:                                         \
                 return shmemc_ctx_test_some_vector_ge##_size(SHMEM_CTX_DEFAULT, \
                                                              (int##_size##_t *) ivars, \
                                                              nelems,   \
                                                              indices, status, \
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
 * Implement the test_some_vector function for various types and sizes
 * using the SHMEM_TYPE_TEST_SOME_VECTOR macro. These functions test whether
 * some elements in an array match the comparison condition and return their indices.
 */
SHMEM_TYPE_TEST_SOME_VECTOR(short, short, 16)
SHMEM_TYPE_TEST_SOME_VECTOR(int, int, 32)
SHMEM_TYPE_TEST_SOME_VECTOR(long, long, 64)
SHMEM_TYPE_TEST_SOME_VECTOR(longlong, long long, 64)
SHMEM_TYPE_TEST_SOME_VECTOR(ushort, unsigned short, 16)
SHMEM_TYPE_TEST_SOME_VECTOR(uint, unsigned int, 32)
SHMEM_TYPE_TEST_SOME_VECTOR(ulong, unsigned long, 64)
SHMEM_TYPE_TEST_SOME_VECTOR(ulonglong, unsigned long long, 64)
SHMEM_TYPE_TEST_SOME_VECTOR(int32, int32_t, 32)
SHMEM_TYPE_TEST_SOME_VECTOR(int64, int64_t, 64)
SHMEM_TYPE_TEST_SOME_VECTOR(uint32, uint32_t, 32)
SHMEM_TYPE_TEST_SOME_VECTOR(uint64, uint64_t, 64)
SHMEM_TYPE_TEST_SOME_VECTOR(size, size_t, 64)
SHMEM_TYPE_TEST_SOME_VECTOR(ptrdiff, ptrdiff_t, 64)
