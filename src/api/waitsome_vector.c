/* For license: see LICENSE file at top-level */

/* Include config header if available */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

/* Include necessary SHMEM headers */
#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"

/* 
 * Enable weak symbols for SHMEM API functions when profiling is enabled. 
 * This allows the profiling versions of the functions to be used if available.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_short_wait_until_some_vector = pshmem_short_wait_until_some_vector
#define shmem_short_wait_until_some_vector pshmem_short_wait_until_some_vector
#pragma weak shmem_int_wait_until_some_vector = pshmem_int_wait_until_some_vector
#define shmem_int_wait_until_some_vector pshmem_int_wait_until_some_vector
#pragma weak shmem_long_wait_until_some_vector = pshmem_long_wait_until_some_vector
#define shmem_long_wait_until_some_vector pshmem_long_wait_until_some_vector
#pragma weak shmem_longlong_wait_until_some_vector = pshmem_longlong_wait_until_some_vector
#define shmem_longlong_wait_until_some_vector pshmem_longlong_wait_until_some_vector
#pragma weak shmem_ushort_wait_until_some_vector = pshmem_ushort_wait_until_some_vector
#define shmem_ushort_wait_until_some_vector pshmem_ushort_wait_until_some_vector
#pragma weak shmem_uint_wait_until_some_vector = pshmem_uint_wait_until_some_vector
#define shmem_uint_wait_until_some_vector pshmem_uint_wait_until_some_vector
#pragma weak shmem_ulong_wait_until_some_vector = pshmem_ulong_wait_until_some_vector
#define shmem_ulong_wait_until_some_vector pshmem_ulong_wait_until_some_vector
#pragma weak shmem_ulonglong_wait_until_some_vector = pshmem_ulonglong_wait_until_some_vector
#define shmem_ulonglong_wait_until_some_vector pshmem_ulonglong_wait_until_some_vector
#pragma weak shmem_int32_wait_until_some_vector = pshmem_int32_wait_until_some_vector
#define shmem_int32_wait_until_some_vector pshmem_int32_wait_until_some_vector
#pragma weak shmem_int64_wait_until_some_vector = pshmem_int64_wait_until_some_vector
#define shmem_int64_wait_until_some_vector pshmem_int64_wait_until_some_vector
#pragma weak shmem_uint32_wait_until_some_vector = pshmem_uint32_wait_until_some_vector
#define shmem_uint32_wait_until_some_vector pshmem_uint32_wait_until_some_vector
#pragma weak shmem_uint64_wait_until_some_vector = pshmem_uint64_wait_until_some_vector
#define shmem_uint64_wait_until_some_vector pshmem_uint64_wait_until_some_vector
#pragma weak shmem_size_wait_until_some_vector = pshmem_size_wait_until_some_vector
#define shmem_size_wait_until_some_vector pshmem_size_wait_until_some_vector
#pragma weak shmem_ptrdiff_wait_until_some_vector = pshmem_ptrdiff_wait_until_some_vector
#define shmem_ptrdiff_wait_until_some_vector pshmem_ptrdiff_wait_until_some_vector
#endif  /* ENABLE_PSHMEM */

/*
 * Macro to define wait-until-some-vector function for different data types.
 * The function waits until some elements in the ivars array satisfy the 
 * specified comparison condition (cmp) with the provided cmp_values.
 *
 * @param _opname: Data type name used in the function definition.
 * @param _type: The C type of the variable (e.g., int, long, etc.).
 * @param _size: Bit size of the type (e.g., 32, 64).
 */
#define SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(_opname, _type, _size)        \
    size_t                                                              \
    shmem_##_opname##_wait_until_some_vector(_type *ivars, size_t nelems, \
                                             size_t *idxs,              \
                                             const int *status,         \
                                             int cmp, _type *cmp_values) \
    {                                                                   \
        /* Protect the SHMEM communication call with a mutex */         \
        SHMEMT_MUTEX_PROTECT                                            \
            (                                                           \
             /* Perform the operation based on the comparison type */   \
             switch (cmp) {                                             \
             case SHMEM_CMP_EQ:                                         \
                 return shmemc_ctx_wait_until_some_vector_eq##_size(SHMEM_CTX_DEFAULT, \
                                                                    (int##_size##_t *) ivars, \
                                                                    nelems, \
                                                                    idxs, \
                                                                    status, \
                                                                    cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_NE:                                         \
                 return shmemc_ctx_wait_until_some_vector_ne##_size(SHMEM_CTX_DEFAULT, \
                                                                    (int##_size##_t *) ivars, \
                                                                    nelems, \
                                                                    idxs, \
                                                                    status, \
                                                                    cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_GT:                                         \
                 return shmemc_ctx_wait_until_some_vector_gt##_size(SHMEM_CTX_DEFAULT, \
                                                                    (int##_size##_t *) ivars, \
                                                                    nelems, \
                                                                    idxs, \
                                                                    status, \
                                                                    cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_LE:                                         \
                 return shmemc_ctx_wait_until_some_vector_le##_size(SHMEM_CTX_DEFAULT, \
                                                                    (int##_size##_t *) ivars, \
                                                                    nelems, \
                                                                    idxs, \
                                                                    status, \
                                                                    cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_LT:                                         \
                 return shmemc_ctx_wait_until_some_vector_lt##_size(SHMEM_CTX_DEFAULT, \
                                                                    (int##_size##_t *) ivars, \
                                                                    nelems, \
                                                                    idxs, \
                                                                    status, \
                                                                    cmp_values); \
                 break;                                                 \
             case SHMEM_CMP_GE:                                         \
                 return shmemc_ctx_wait_until_some_vector_ge##_size(SHMEM_CTX_DEFAULT, \
                                                                    (int##_size##_t *) ivars, \
                                                                    nelems, \
                                                                    idxs, \
                                                                    status, \
                                                                    cmp_values); \
                 break;                                                 \
             default:                                                   \
                 shmemu_fatal("unknown operator (code %d) in \"%s\"",   \
                              cmp,                                      \
                              __func__                                  \
                              );                                        \
                 return -1;                                             \
                 /* NOT REACHED */                                      \
                 break;                                                 \
             }                                                          \
                                                                        ); \
    }

/* Define wait-until-some-vector functions for different types */
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(short, short, 16)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(int, int, 32)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(long, long, 64)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(longlong, long long, 64)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(ushort, unsigned short, 16)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(uint, unsigned int, 32)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(ulong, unsigned long, 64)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(ulonglong, unsigned long long, 64)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(int32, int32_t, 32)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(int64, int64_t, 64)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(uint32, uint32_t, 32)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(uint64, uint64_t, 64)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(size, size_t, 64)
SHMEM_TYPE_WAIT_UNTIL_SOME_VECTOR(ptrdiff, ptrdiff_t, 64)
