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
 * atomic fetch operations for various data types. 
 * This allows the functions to be overridden by prefixed 'pshmem' versions.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_int_atomic_fetch = pshmem_ctx_int_atomic_fetch
#define shmem_ctx_int_atomic_fetch pshmem_ctx_int_atomic_fetch
#pragma weak shmem_ctx_long_atomic_fetch = pshmem_ctx_long_atomic_fetch
#define shmem_ctx_long_atomic_fetch pshmem_ctx_long_atomic_fetch
#pragma weak shmem_ctx_longlong_atomic_fetch = pshmem_ctx_longlong_atomic_fetch
#define shmem_ctx_longlong_atomic_fetch pshmem_ctx_longlong_atomic_fetch
#pragma weak shmem_ctx_float_atomic_fetch = pshmem_ctx_float_atomic_fetch
#define shmem_ctx_float_atomic_fetch pshmem_ctx_float_atomic_fetch
#pragma weak shmem_ctx_double_atomic_fetch = pshmem_ctx_double_atomic_fetch
#define shmem_ctx_double_atomic_fetch pshmem_ctx_double_atomic_fetch
#pragma weak shmem_ctx_uint_atomic_fetch = pshmem_ctx_uint_atomic_fetch
#define shmem_ctx_uint_atomic_fetch pshmem_ctx_uint_atomic_fetch
#pragma weak shmem_ctx_ulong_atomic_fetch = pshmem_ctx_ulong_atomic_fetch
#define shmem_ctx_ulong_atomic_fetch pshmem_ctx_ulong_atomic_fetch
#pragma weak shmem_ctx_ulonglong_atomic_fetch = pshmem_ctx_ulonglong_atomic_fetch
#define shmem_ctx_ulonglong_atomic_fetch pshmem_ctx_ulonglong_atomic_fetch
#pragma weak shmem_ctx_int32_atomic_fetch = pshmem_ctx_int32_atomic_fetch
#define shmem_ctx_int32_atomic_fetch pshmem_ctx_int32_atomic_fetch
#pragma weak shmem_ctx_int64_atomic_fetch = pshmem_ctx_int64_atomic_fetch
#define shmem_ctx_int64_atomic_fetch pshmem_ctx_int64_atomic_fetch
#pragma weak shmem_ctx_uint32_atomic_fetch = pshmem_ctx_uint32_atomic_fetch
#define shmem_ctx_uint32_atomic_fetch pshmem_ctx_uint32_atomic_fetch
#pragma weak shmem_ctx_uint64_atomic_fetch = pshmem_ctx_uint64_atomic_fetch
#define shmem_ctx_uint64_atomic_fetch pshmem_ctx_uint64_atomic_fetch
#pragma weak shmem_ctx_size_atomic_fetch = pshmem_ctx_size_atomic_fetch
#define shmem_ctx_size_atomic_fetch pshmem_ctx_size_atomic_fetch
#pragma weak shmem_ctx_ptrdiff_atomic_fetch = pshmem_ctx_ptrdiff_atomic_fetch
#define shmem_ctx_ptrdiff_atomic_fetch pshmem_ctx_ptrdiff_atomic_fetch
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_FETCH:
 * Defines the context-based atomic fetch operation for various data types.
 * Fetches the value of a variable from a specified PE.
 * 
 * _name - The name of the data type (e.g., int, long, float)
 * _type - The actual C data type (e.g., int, long, float)
 */
#define SHMEM_CTX_TYPE_FETCH(_name, _type)                              \
    _type                                                               \
    shmem_ctx_##_name##_atomic_fetch(shmem_ctx_t ctx,                   \
                                     const _type *target, int pe)       \
    {                                                                   \
        _type v;                                                        \
                                                                        \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_fetch(ctx,                    \
                                                (_type *) target,       \
                                                sizeof(*target),        \
                                                pe,                     \
                                                &v));                   \
        return v;                                                       \
    }

/* Implement context-based atomic fetch operations for different data types */
SHMEM_CTX_TYPE_FETCH(float, float)
SHMEM_CTX_TYPE_FETCH(double, double)
SHMEM_CTX_TYPE_FETCH(int, int)
SHMEM_CTX_TYPE_FETCH(long, long)
SHMEM_CTX_TYPE_FETCH(longlong, long long)
SHMEM_CTX_TYPE_FETCH(uint, unsigned int)
SHMEM_CTX_TYPE_FETCH(ulong, unsigned long)
SHMEM_CTX_TYPE_FETCH(ulonglong, unsigned long long)
SHMEM_CTX_TYPE_FETCH(int32, int32_t)
SHMEM_CTX_TYPE_FETCH(int64, int64_t)
SHMEM_CTX_TYPE_FETCH(uint32, uint32_t)
SHMEM_CTX_TYPE_FETCH(uint64, uint64_t)
SHMEM_CTX_TYPE_FETCH(size, size_t)
SHMEM_CTX_TYPE_FETCH(ptrdiff, ptrdiff_t)

/*
 * API_DEF_CONST_AMO1:
 * This macro defines the atomic fetch operation without context for 
 * various data types, wrapping the context-based operations defined above.
 * 
 * _op   - The operation (fetch)
 * _name - The name of the data type (e.g., int, long, float)
 * _type - The actual C data type (e.g., int, long, float)
 */
API_DEF_CONST_AMO1(fetch, float, float)
API_DEF_CONST_AMO1(fetch, double, double)
API_DEF_CONST_AMO1(fetch, int, int)
API_DEF_CONST_AMO1(fetch, long, long)
API_DEF_CONST_AMO1(fetch, longlong, long long)
API_DEF_CONST_AMO1(fetch, uint, unsigned int)
API_DEF_CONST_AMO1(fetch, ulong, unsigned long)
API_DEF_CONST_AMO1(fetch, ulonglong, unsigned long long)
API_DEF_CONST_AMO1(fetch, int32, int32_t)
API_DEF_CONST_AMO1(fetch, int64, int64_t)
API_DEF_CONST_AMO1(fetch, uint32, uint32_t)
API_DEF_CONST_AMO1(fetch, uint64, uint64_t)
API_DEF_CONST_AMO1(fetch, size, size_t)
API_DEF_CONST_AMO1(fetch, ptrdiff, ptrdiff_t)
