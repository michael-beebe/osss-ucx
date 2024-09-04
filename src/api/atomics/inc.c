/* For license: see LICENSE file at top-level */

/* 
 * If the configuration header is available, include it.
 * This may define platform-specific configurations or flags.
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

/* 
 * Include necessary SHMEM headers for mutex, utility functions,
 * core SHMEM functionality, and common definitions.
 */
#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"
#include "common.h"

/* 
 * If ENABLE_PSHMEM is defined, define weak symbols for context-based atomic
 * increment functions, allowing them to be overridden by 'pshmem' prefixed
 * versions if needed.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_int_atomic_inc = pshmem_ctx_int_atomic_inc
#define shmem_ctx_int_atomic_inc pshmem_ctx_int_atomic_inc
#pragma weak shmem_ctx_long_atomic_inc = pshmem_ctx_long_atomic_inc
#define shmem_ctx_long_atomic_inc pshmem_ctx_long_atomic_inc
#pragma weak shmem_ctx_longlong_atomic_inc = pshmem_ctx_longlong_atomic_inc
#define shmem_ctx_longlong_atomic_inc pshmem_ctx_longlong_atomic_inc
#pragma weak shmem_ctx_uint_atomic_inc = pshmem_ctx_uint_atomic_inc
#define shmem_ctx_uint_atomic_inc pshmem_ctx_uint_atomic_inc
#pragma weak shmem_ctx_ulong_atomic_inc = pshmem_ctx_ulong_atomic_inc
#define shmem_ctx_ulong_atomic_inc pshmem_ctx_ulong_atomic_inc
#pragma weak shmem_ctx_ulonglong_atomic_inc = pshmem_ctx_ulonglong_atomic_inc
#define shmem_ctx_ulonglong_atomic_inc pshmem_ctx_ulonglong_atomic_inc
#pragma weak shmem_ctx_int32_atomic_inc = pshmem_ctx_int32_atomic_inc
#define shmem_ctx_int32_atomic_inc pshmem_ctx_int32_atomic_inc
#pragma weak shmem_ctx_int64_atomic_inc = pshmem_ctx_int64_atomic_inc
#define shmem_ctx_int64_atomic_inc pshmem_ctx_int64_atomic_inc
#pragma weak shmem_ctx_uint32_atomic_inc = pshmem_ctx_uint32_atomic_inc
#define shmem_ctx_uint32_atomic_inc pshmem_ctx_uint32_atomic_inc
#pragma weak shmem_ctx_uint64_atomic_inc = pshmem_ctx_uint64_atomic_inc
#define shmem_ctx_uint64_atomic_inc pshmem_ctx_uint64_atomic_inc
#pragma weak shmem_ctx_size_atomic_inc = pshmem_ctx_size_atomic_inc
#define shmem_ctx_size_atomic_inc pshmem_ctx_size_atomic_inc
#pragma weak shmem_ctx_ptrdiff_atomic_inc = pshmem_ctx_ptrdiff_atomic_inc
#define shmem_ctx_ptrdiff_atomic_inc pshmem_ctx_ptrdiff_atomic_inc
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_INC:
 * Defines a context-based atomic increment operation for a specific data type.
 * This function increments the value of the target variable on the specified PE.
 * 
 * _name - The name of the data type (e.g., int, long)
 * _type - The actual C data type (e.g., int, long)
 */
#define SHMEM_CTX_TYPE_INC(_name, _type)                                \
    void                                                                \
    shmem_ctx_##_name##_atomic_inc(shmem_ctx_t ctx,                     \
                                   _type *target,                       \
                                   int pe)                              \
    {                                                                   \
        _type one = 1; /* Define a variable 'one' to use for increment */\
                                                                        \
        /* Use SHMEMT_MUTEX_NOPROTECT to perform the atomic increment */\
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_add(ctx,                      \
                                              target,                   \
                                              &one, sizeof(one),        \
                                              pe));                     \
    }

/* Define atomic increment operations for different data types */
SHMEM_CTX_TYPE_INC(int, int)
SHMEM_CTX_TYPE_INC(long, long)
SHMEM_CTX_TYPE_INC(longlong, long long)
SHMEM_CTX_TYPE_INC(uint, unsigned int)
SHMEM_CTX_TYPE_INC(ulong, unsigned long)
SHMEM_CTX_TYPE_INC(ulonglong, unsigned long long)
SHMEM_CTX_TYPE_INC(int32, int32_t)
SHMEM_CTX_TYPE_INC(int64, int64_t)
SHMEM_CTX_TYPE_INC(uint32, uint32_t)
SHMEM_CTX_TYPE_INC(uint64, uint64_t)
SHMEM_CTX_TYPE_INC(size, size_t)
SHMEM_CTX_TYPE_INC(ptrdiff, ptrdiff_t)

/*
 * API_DEF_VOID_AMO1:
 * Defines atomic increment operations without context for various data types.
 * This wraps the context-based increment functions defined above.
 * 
 * _op   - The operation (inc)
 * _name - The name of the data type (e.g., int, long)
 * _type - The actual C data type (e.g., int, long)
 */
API_DEF_VOID_AMO1(inc, int, int)
API_DEF_VOID_AMO1(inc, long, long)
API_DEF_VOID_AMO1(inc, longlong, long long)
API_DEF_VOID_AMO1(inc, uint, unsigned int)
API_DEF_VOID_AMO1(inc, ulong, unsigned long)
API_DEF_VOID_AMO1(inc, ulonglong, unsigned long long)
API_DEF_VOID_AMO1(inc, int32, int32_t)
API_DEF_VOID_AMO1(inc, int64, int64_t)
API_DEF_VOID_AMO1(inc, uint32, uint32_t)
API_DEF_VOID_AMO1(inc, uint64, uint64_t)
API_DEF_VOID_AMO1(inc, size, size_t)
API_DEF_VOID_AMO1(inc, ptrdiff, ptrdiff_t)
