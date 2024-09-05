/* For license: see LICENSE file at top-level */

/* Include the configuration file if it exists */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

/* Include necessary headers for mutex, utility functions, and core SHMEM routines */
#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"

/*
 * Enable weak symbols for SHMEM API functions to allow profiling versions to
 * be substituted, where applicable, in a profiling or debugging environment.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_short_wait_until_all = pshmem_short_wait_until_all
#define shmem_short_wait_until_all pshmem_short_wait_until_all
#pragma weak shmem_int_wait_until_all = pshmem_int_wait_until_all
#define shmem_int_wait_until_all pshmem_int_wait_until_all
#pragma weak shmem_long_wait_until_all = pshmem_long_wait_until_all
#define shmem_long_wait_until_all pshmem_long_wait_until_all
#pragma weak shmem_longlong_wait_until_all = pshmem_longlong_wait_until_all
#define shmem_longlong_wait_until_all pshmem_longlong_wait_until_all
#pragma weak shmem_ushort_wait_until_all = pshmem_ushort_wait_until_all
#define shmem_ushort_wait_until_all pshmem_ushort_wait_until_all
#pragma weak shmem_uint_wait_until_all = pshmem_uint_wait_until_all
#define shmem_uint_wait_until_all pshmem_uint_wait_until_all
#pragma weak shmem_ulong_wait_until_all = pshmem_ulong_wait_until_all
#define shmem_ulong_wait_until_all pshmem_ulong_wait_until_all
#pragma weak shmem_ulonglong_wait_until_all = pshmem_ulonglong_wait_until_all
#define shmem_ulonglong_wait_until_all pshmem_ulonglong_wait_until_all
#pragma weak shmem_int32_wait_until_all = pshmem_int32_wait_until_all
#define shmem_int32_wait_until_all pshmem_int32_wait_until_all
#pragma weak shmem_int64_wait_until_all = pshmem_int64_wait_until_all
#define shmem_int64_wait_until_all pshmem_int64_wait_until_all
#pragma weak shmem_uint32_wait_until_all = pshmem_uint32_wait_until_all
#define shmem_uint32_wait_until_all pshmem_uint32_wait_until_all
#pragma weak shmem_uint64_wait_until_all = pshmem_uint64_wait_until_all
#define shmem_uint64_wait_until_all pshmem_uint64_wait_until_all
#pragma weak shmem_size_wait_until_all = pshmem_size_wait_until_all
#define shmem_size_wait_until_all pshmem_size_wait_until_all
#pragma weak shmem_ptrdiff_wait_until_all = pshmem_ptrdiff_wait_until_all
#define shmem_ptrdiff_wait_until_all pshmem_ptrdiff_wait_until_all
#endif  /* ENABLE_PSHMEM */

/*
 * Macro to define wait-until-all function for different data types.
 * The function waits until all elements in the ivars array satisfy 
 * the specified comparison condition (cmp) with the provided cmp_value.
 *
 * @param _opname: Data type name used in the function definition.
 * @param _type: The C type of the variable (e.g., int, long, etc.).
 * @param _size: Bit size of the type (e.g., 32, 64).
 */
#define SHMEM_TYPE_WAIT_UNTIL_ALL(_opname, _type, _size)                \
    void                                                                \
    shmem_##_opname##_wait_until_all(_type *ivars, size_t nelems,       \
                                     const int *status,                 \
                                     int cmp, _type cmp_value)          \
    {                                                                   \
        /* Protect the SHMEM communication call with a mutex */         \
        SHMEMT_MUTEX_PROTECT                                            \
            (                                                           \
             /* Perform the operation based on the comparison type */   \
             switch (cmp) {                                             \
             case SHMEM_CMP_EQ:                                         \
             shmemc_ctx_wait_until_all_eq##_size(SHMEM_CTX_DEFAULT,     \
                                                 (int##_size##_t *) ivars, \
                                                 nelems,                \
                                                 status,                \
                                                 cmp_value);            \
             break;                                                     \
             case SHMEM_CMP_NE:                                         \
             shmemc_ctx_wait_until_all_ne##_size(SHMEM_CTX_DEFAULT,     \
                                                 (int##_size##_t *) ivars, \
                                                 nelems,                \
                                                 status,                \
                                                 cmp_value);            \
             break;                                                     \
             case SHMEM_CMP_GT:                                         \
             shmemc_ctx_wait_until_all_gt##_size(SHMEM_CTX_DEFAULT,     \
                                                 (int##_size##_t *) ivars, \
                                                 nelems,                \
                                                 status,                \
                                                 cmp_value);            \
             break;                                                     \
             case SHMEM_CMP_LE:                                         \
             shmemc_ctx_wait_until_all_le##_size(SHMEM_CTX_DEFAULT,     \
                                                 (int##_size##_t *) ivars, \
                                                 nelems,                \
                                                 status,                \
                                                 cmp_value);            \
             break;                                                     \
             case SHMEM_CMP_LT:                                         \
             shmemc_ctx_wait_until_all_lt##_size(SHMEM_CTX_DEFAULT,     \
                                                 (int##_size##_t *) ivars, \
                                                 nelems,                \
                                                 status,                \
                                                 cmp_value);            \
             break;                                                     \
             case SHMEM_CMP_GE:                                         \
             shmemc_ctx_wait_until_all_ge##_size(SHMEM_CTX_DEFAULT,     \
                                                 (int##_size##_t *) ivars, \
                                                 nelems,                \
                                                 status,                \
                                                 cmp_value);            \
             break;                                                     \
             default:                                                   \
             /* Handle unknown comparison type */                       \
             shmemu_fatal("unknown operator (code %d) in \"%s\"",       \
                          cmp,                                          \
                          __func__                                      \
                          );                                            \
             /* Unreachable code */                                     \
             break;                                                     \
             }                                                          \
                                                                        ); \
    }

/* Define wait-until-all functions for different types */
SHMEM_TYPE_WAIT_UNTIL_ALL(short, short, 16)
SHMEM_TYPE_WAIT_UNTIL_ALL(int, int, 32)
SHMEM_TYPE_WAIT_UNTIL_ALL(long, long, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL(longlong, long long, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL(ushort, unsigned short, 16)
SHMEM_TYPE_WAIT_UNTIL_ALL(uint, unsigned int, 32)
SHMEM_TYPE_WAIT_UNTIL_ALL(ulong, unsigned long, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL(ulonglong, unsigned long long, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL(int32, int32_t, 32)
SHMEM_TYPE_WAIT_UNTIL_ALL(int64, int64_t, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL(uint32, uint32_t, 32)
SHMEM_TYPE_WAIT_UNTIL_ALL(uint64, uint64_t, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL(size, size_t, 64)
SHMEM_TYPE_WAIT_UNTIL_ALL(ptrdiff, ptrdiff_t, 64)
