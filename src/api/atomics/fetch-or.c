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
 * atomic fetch 'or' operations (AMO) for various data types.
 * These allow the functions to be overridden by prefixed 'pshmem' versions.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_uint_atomic_fetch_or = pshmem_ctx_uint_atomic_fetch_or
#define shmem_ctx_uint_atomic_fetch_or pshmem_ctx_uint_atomic_fetch_or
#pragma weak shmem_ctx_ulong_atomic_fetch_or = pshmem_ctx_ulong_atomic_fetch_or
#define shmem_ctx_ulong_atomic_fetch_or pshmem_ctx_ulong_atomic_fetch_or
#pragma weak shmem_ctx_ulonglong_atomic_fetch_or = pshmem_ctx_ulonglong_atomic_fetch_or
#define shmem_ctx_ulonglong_atomic_fetch_or pshmem_ctx_ulonglong_atomic_fetch_or
#pragma weak shmem_ctx_int32_atomic_fetch_or = pshmem_ctx_int32_atomic_fetch_or
#define shmem_ctx_int32_atomic_fetch_or pshmem_ctx_int32_atomic_fetch_or
#pragma weak shmem_ctx_int64_atomic_fetch_or = pshmem_ctx_int64_atomic_fetch_or
#define shmem_ctx_int64_atomic_fetch_or pshmem_ctx_int64_atomic_fetch_or
#pragma weak shmem_ctx_uint32_atomic_fetch_or = pshmem_ctx_uint32_atomic_fetch_or
#define shmem_ctx_uint32_atomic_fetch_or pshmem_ctx_uint32_atomic_fetch_or
#pragma weak shmem_ctx_uint64_atomic_fetch_or = pshmem_ctx_uint64_atomic_fetch_or
#define shmem_ctx_uint64_atomic_fetch_or pshmem_ctx_uint64_atomic_fetch_or
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_FETCH_BITWISE:
 * Defines the context-based atomic fetch 'or' operation for various data types.
 * This function fetches the bitwise OR value from a remote PE and returns it.
 * 
 * _opname - The operation name (or)
 * _name   - The name of the data type (e.g., uint, int32)
 * _type   - The actual C data type (e.g., unsigned int, int32_t)
 */
SHMEM_CTX_TYPE_FETCH_BITWISE(or, uint, unsigned int)
SHMEM_CTX_TYPE_FETCH_BITWISE(or, ulong, unsigned long)
SHMEM_CTX_TYPE_FETCH_BITWISE(or, ulonglong, unsigned long long)
SHMEM_CTX_TYPE_FETCH_BITWISE(or, int32, int32_t)
SHMEM_CTX_TYPE_FETCH_BITWISE(or, int64, int64_t)
SHMEM_CTX_TYPE_FETCH_BITWISE(or, uint32, uint32_t)
SHMEM_CTX_TYPE_FETCH_BITWISE(or, uint64, uint64_t)

/*
 * API_DEF_AMO2:
 * This macro defines the atomic fetch 'or' operation without context for 
 * various data types. It wraps the context-based operations defined above.
 * 
 * _op   - The operation (fetch_or)
 * _name - The name of the data type (e.g., uint, ulong)
 * _type - The actual C data type (e.g., unsigned int, unsigned long)
 */
API_DEF_AMO2(fetch_or, uint, unsigned int)
API_DEF_AMO2(fetch_or, ulong, unsigned long)
API_DEF_AMO2(fetch_or, ulonglong, unsigned long long)
API_DEF_AMO2(fetch_or, int32, int32_t)
API_DEF_AMO2(fetch_or, int64, int64_t)
API_DEF_AMO2(fetch_or, uint32, uint32_t)
API_DEF_AMO2(fetch_or, uint64, uint64_t)
