/* For license: see LICENSE file at top-level */

/* 
 * Include configuration header if available. 
 * This ensures platform-specific configurations are applied.
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

/* 
 * Include necessary SHMEM headers for mutex, utility functions, core SHMEM 
 * functionality, and common definitions.
 */
#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"
#include "common.h"

/* 
 * If ENABLE_PSHMEM is defined, create weak references for the context-based 
 * non-blocking atomic fetch-and-increment (AMO) operations for various data types.
 * These allow the functions to be overridden by prefixed 'pshmem' versions.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_int_atomic_fetch_inc = pshmem_ctx_int_atomic_fetch_inc
#define shmem_ctx_int_atomic_fetch_inc pshmem_ctx_int_atomic_fetch_inc
#pragma weak shmem_ctx_long_atomic_fetch_inc = pshmem_ctx_long_atomic_fetch_inc
#define shmem_ctx_long_atomic_fetch_inc pshmem_ctx_long_atomic_fetch_inc
#pragma weak shmem_ctx_longlong_atomic_fetch_inc = pshmem_ctx_longlong_atomic_fetch_inc
#define shmem_ctx_longlong_atomic_fetch_inc pshmem_ctx_longlong_atomic_fetch_inc
#pragma weak shmem_ctx_uint_atomic_fetch_inc = pshmem_ctx_uint_atomic_fetch_inc
#define shmem_ctx_uint_atomic_fetch_inc pshmem_ctx_uint_atomic_fetch_inc
#pragma weak shmem_ctx_ulong_atomic_fetch_inc = pshmem_ctx_ulong_atomic_fetch_inc
#define shmem_ctx_ulong_atomic_fetch_inc pshmem_ctx_ulong_atomic_fetch_inc
#pragma weak shmem_ctx_ulonglong_atomic_fetch_inc = pshmem_ctx_ulonglong_atomic_fetch_inc
#define shmem_ctx_ulonglong_atomic_fetch_inc pshmem_ctx_ulonglong_atomic_fetch_inc
#pragma weak shmem_ctx_int32_atomic_fetch_inc = pshmem_ctx_int32_atomic_fetch_inc
#define shmem_ctx_int32_atomic_fetch_inc pshmem_ctx_int32_atomic_fetch_inc
#pragma weak shmem_ctx_int64_atomic_fetch_inc = pshmem_ctx_int64_atomic_fetch_inc
#define shmem_ctx_int64_atomic_fetch_inc pshmem_ctx_int64_atomic_fetch_inc
#pragma weak shmem_ctx_uint32_atomic_fetch_inc = pshmem_ctx_uint32_atomic_fetch_inc
#define shmem_ctx_uint32_atomic_fetch_inc pshmem_ctx_uint32_atomic_fetch_inc
#pragma weak shmem_ctx_uint64_atomic_fetch_inc = pshmem_ctx_uint64_atomic_fetch_inc
#define shmem_ctx_uint64_atomic_fetch_inc pshmem_ctx_uint64_atomic_fetch_inc
#pragma weak shmem_ctx_size_atomic_fetch_inc = pshmem_ctx_size_atomic_fetch_inc
#define shmem_ctx_size_atomic_fetch_inc pshmem_ctx_size_atomic_fetch_inc
#pragma weak shmem_ctx_ptrdiff_atomic_fetch_inc = pshmem_ctx_ptrdiff_atomic_fetch_inc
#define shmem_ctx_ptrdiff_atomic_fetch_inc pshmem_ctx_ptrdiff_atomic_fetch_inc
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_FINC:
 * Defines context-based atomic fetch-and-increment (AMO) operations for various 
 * data types. The function increments the target value by 1 and returns the 
 * previous value.
 *
 * _name - The name of the data type (e.g., int, long)
 * _type - The actual C data type (e.g., int, long)
 */
#define SHMEM_CTX_TYPE_FINC(_name, _type)                               \
    _type                                                               \
    shmem_ctx_##_name##_atomic_fetch_inc(shmem_ctx_t ctx,               \
                                         _type *target,                 \
                                         int pe)                        \
    {                                                                   \
        _type one = 1;  /* Constant for increment operation */          \
        _type v;                                                        \
                                                                        \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_fadd(ctx,                     \
                                               target,                  \
                                               &one, sizeof(one),       \
                                               pe, &v));                \
        return v;                                                       \
    }

/* Instantiate the atomic fetch-and-increment function for all required types */
SHMEM_CTX_TYPE_FINC(int, int)
SHMEM_CTX_TYPE_FINC(long, long)
SHMEM_CTX_TYPE_FINC(longlong, long long)
SHMEM_CTX_TYPE_FINC(uint, unsigned int)
SHMEM_CTX_TYPE_FINC(ulong, unsigned long)
SHMEM_CTX_TYPE_FINC(ulonglong, unsigned long long)
SHMEM_CTX_TYPE_FINC(int32, int32_t)
SHMEM_CTX_TYPE_FINC(int64, int64_t)
SHMEM_CTX_TYPE_FINC(uint32, uint32_t)
SHMEM_CTX_TYPE_FINC(uint64, uint64_t)
SHMEM_CTX_TYPE_FINC(size, size_t)
SHMEM_CTX_TYPE_FINC(ptrdiff, ptrdiff_t)

/*
 * API_DEF_AMO1:
 * This macro defines atomic fetch-and-increment (AMO) operations without context 
 * for various data types. It wraps the context-based operations defined above.
 *
 * _op   - The operation (fetch_inc)
 * _name - The name of the data type (e.g., int, long)
 * _type - The actual C data type (e.g., int, long)
 */
API_DEF_AMO1(fetch_inc, int, int)
API_DEF_AMO1(fetch_inc, long, long)
API_DEF_AMO1(fetch_inc, longlong, long long)
API_DEF_AMO1(fetch_inc, uint, unsigned int)
API_DEF_AMO1(fetch_inc, ulong, unsigned long)
API_DEF_AMO1(fetch_inc, ulonglong, unsigned long long)
API_DEF_AMO1(fetch_inc, int32, int32_t)
API_DEF_AMO1(fetch_inc, int64, int64_t)
API_DEF_AMO1(fetch_inc, uint32, uint32_t)
API_DEF_AMO1(fetch_inc, uint64, uint64_t)
API_DEF_AMO1(fetch_inc, size, size_t)
API_DEF_AMO1(fetch_inc, ptrdiff, ptrdiff_t)
