/* For license: see LICENSE file at top-level */

/* If configuration is available, include it */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmem_mutex.h"  /* Mutex protection macros */
#include "shmemu.h"       /* SHMEM utility functions and macros */
#include "shmemc.h"       /* SHMEM communication layer */

/* 
 * Enable weak symbols for profiling, allowing the user to substitute 
 * the SHMEM API functions with custom profiling versions.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_short_wait_until_all_vector = pshmem_short_wait_until_all_vector
#define shmem_short_wait_until_all_vector pshmem_short_wait_until_all_vector
#pragma weak shmem_int_wait_until_all_vector = pshmem_int_wait_until_all_vector
#define shmem_int_wait_until_all_vector pshmem_int_wait_until_all_vector
#pragma weak shmem_long_wait_until_all_vector = pshmem_long_wait_until_all_vector
#define shmem_long_wait_until_all_vector pshmem_long_wait_until_all_vector
#pragma weak shmem_longlong_wait_until_all_vector = pshmem_longlong_wait_until_all_vector
#define shmem_longlong_wait_until_all_vector pshmem_longlong_wait_until_all_vector
#pragma weak shmem_ushort_wait_until_all_vector = pshmem_ushort_wait_until_all_vector
#define shmem_ushort_wait_until_all_vector pshmem_ushort_wait_until_all_vector
#pragma weak shmem_uint_wait_until_all_vector = pshmem_uint_wait_until_all_vector
#define shmem_uint_wait_until_all_vector pshmem_uint_wait_until_all_vector
#pragma weak shmem_ulong_wait_until_all_vector = pshmem_ulong_wait_until_all_vector
#define shmem_ulong_wait_until_all_vector pshmem_ulong_wait_until_all_vector
#pragma weak shmem_ulonglong_wait_until_all_vector = pshmem_ulonglong_wait_until_all_vector
#define shmem_ulonglong_wait_until_all_vector pshmem_ulonglong_wait_until_all_vector
#pragma weak shmem_int32_wait_until_all_vector = pshmem_int32_wait_until_all_vector
#define shmem_int32_wait_until_all_vector pshmem_int32_wait_until_all_vector
#pragma weak shmem_int64_wait_until_all_vector = pshmem_int64_wait_until_all_vector
#define shmem_int64_wait_until_all_vector pshmem_int64_wait_until_all_vector
#pragma weak shmem_uint32_wait_until_all_vector = pshmem_uint32_wait_until_all_vector
#define shmem_uint32_wait_until_all_vector pshmem_uint32_wait_until_all_vector
#pragma weak shmem_uint64_wait_until_all_vector = pshmem_uint64_wait_until_all_vector
#define shmem_uint64_wait_until_all_vector pshmem_uint64_wait_until_all_vector
#pragma weak shmem_size_wait_until_all_vector = pshmem_size_wait_until_all_vector
#define shmem_size_wait_until_all_vector pshmem_size_wait_until_all_vector
#pragma weak shmem_ptrdiff_wait_until_all_vector = pshmem_ptrdiff_wait_until_all_vector
#define shmem_ptrdiff_wait_until_all_vector pshmem_ptrdiff_wait_until_all_vector
#endif  /* ENABLE_PSHMEM */

/*
 * Macro to define wait-until-all-vector function for different data types.
 * This function blocks until all elements in the ivars array match 
 * the comparison condition specified by cmp and cmp_values.
 *
 * @param _opname: Data type name used in the function definition.
 * @param _type: C type of the variables (e.g., int, long, etc.).
 * @param _size: Bit size of the type.
 */
#define SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(_opname, _type, _size)         \
    void                                                                \
    shmem_##_opname##_wait_until_all_vector(_type *ivars, size_t nelems, \
                                            const int *status,          \
                                            int cmp, _type *cmp_values) \
    {                                                                   \
        /* Protect the SHMEM communication call using a mutex */        \
        SHMEMT_MUTEX_PROTECT                                            \
            (                                                           \
             /* Perform the operation based on the comparison type */   \
             switch (cmp) {                                             \
             case SHMEM_CMP_EQ:                                         \
                 shmemc_ctx_wait_until_all_vector_eq##_size(SHMEM_CTX_DEFAULT, \
                                                            (int##_size##_t *) ivars, \
                                                            nelems,     \
                                                            status,     \
                                                            cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_NE:                                         \
                 shmemc_ctx_wait_until_all_vector_ne##_size(SHMEM_CTX_DEFAULT, \
                                                            (int##_size##_t *) ivars, \
                                                            nelems,     \
                                                            status,     \
                                                            cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_GT:                                         \
                 shmemc_ctx_wait_until_all_vector_gt##_size(SHMEM_CTX_DEFAULT, \
                                                            (int##_size##_t *) ivars, \
                                                            nelems,     \
                                                            status,     \
                                                            cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_LE:                                         \
                 shmemc_ctx_wait_until_all_vector_le##_size(SHMEM_CTX_DEFAULT, \
                                                            (int##_size##_t *) ivars, \
                                                            nelems,     \
                                                            status,     \
                                                            cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_LT:                                         \
                 shmemc_ctx_wait_until_all_vector_lt##_size(SHMEM_CTX_DEFAULT, \
                                                            (int##_size##_t *) ivars, \
                                                            nelems,     \
                                                            status,     \
                                                            cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_GE:                                         \
                 shmemc_ctx_wait_until_all_vector_ge##_size(SHMEM_CTX_DEFAULT, \
                                                            (int##_size##_t *) ivars, \
                                                            nelems,     \
                                                            status,     \
                                                            cmp_values); \
                 break;                                                 \
             default:                                                   \
                 /* Handle unknown comparison type */                   \
                 shmemu_fatal("unknown operator (code %d) in \"%s\"",   \
                              cmp,                                      \
                              __func__                                  \
                              );                                        \
                 /* Unreachable code */                                 \
                 break;                                                 \
             }                                                          \
                                                                        ); \
    }

/* Define wait-until-all-vector functions for different types */
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(short, short, 16)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(int, int, 32)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(long, long, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(longlong, long long, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(ushort, unsigned short, 16)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(uint, unsigned int, 32)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(ulong, unsigned long, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(ulonglong, unsigned long long, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(int32, int32_t, 32)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(int64, int64_t, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(uint32, uint32_t, 32)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(uint64, uint64_t, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(size, size_t, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL_VECTOR(ptrdiff, ptrdiff_t, 64)
