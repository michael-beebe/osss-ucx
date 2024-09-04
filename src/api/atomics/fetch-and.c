/* For license: see LICENSE file at top-level */

/* 
 * Include configuration header if available. 
 * This ensures that platform-specific configurations are applied.
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
 * atomic fetch-and operations (AMO) for various data types. 
 * These allow the functions to be overridden by prefixed 'pshmem' versions.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_uint_atomic_fetch_and = pshmem_ctx_uint_atomic_fetch_and
#define shmem_ctx_uint_atomic_fetch_and pshmem_ctx_uint_atomic_fetch_and
#pragma weak shmem_ctx_ulong_atomic_fetch_and = pshmem_ctx_ulong_atomic_fetch_and
#define shmem_ctx_ulong_atomic_fetch_and pshmem_ctx_ulong_atomic_fetch_and
#pragma weak shmem_ctx_ulonglong_atomic_fetch_and = pshmem_ctx_ulonglong_atomic_fetch_and
#define shmem_ctx_ulonglong_atomic_fetch_and pshmem_ctx_ulonglong_atomic_fetch_and
#pragma weak shmem_ctx_int32_atomic_fetch_and = pshmem_ctx_int32_atomic_fetch_and
#define shmem_ctx_int32_atomic_fetch_and pshmem_ctx_int32_atomic_fetch_and
#pragma weak shmem_ctx_int64_atomic_fetch_and = pshmem_ctx_int64_atomic_fetch_and
#define shmem_ctx_int64_atomic_fetch_and pshmem_ctx_int64_atomic_fetch_and
#pragma weak shmem_ctx_uint32_atomic_fetch_and = pshmem_ctx_uint32_atomic_fetch_and
#define shmem_ctx_uint32_atomic_fetch_and pshmem_ctx_uint32_atomic_fetch_and
#pragma weak shmem_ctx_uint64_atomic_fetch_and = pshmem_ctx_uint64_atomic_fetch_and
#define shmem_ctx_uint64_atomic_fetch_and pshmem_ctx_uint64_atomic_fetch_and
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_FETCH_BITWISE:
 * Defines context-based blocking atomic fetch-and (AMO) operations 
 * for various data types, using the bitwise AND operation.
 *
 * _opname - The operation name (and)
 * _name   - The name of the data type (e.g., uint, ulong)
 * _type   - The actual C data type (e.g., unsigned int, unsigned long)
 */
SHMEM_CTX_TYPE_FETCH_BITWISE(and, uint, unsigned int)
SHMEM_CTX_TYPE_FETCH_BITWISE(and, ulong, unsigned long)
SHMEM_CTX_TYPE_FETCH_BITWISE(and, ulonglong, unsigned long long)
SHMEM_CTX_TYPE_FETCH_BITWISE(and, int32, int32_t)
SHMEM_CTX_TYPE_FETCH_BITWISE(and, int64, int64_t)
SHMEM_CTX_TYPE_FETCH_BITWISE(and, uint32, uint32_t)
SHMEM_CTX_TYPE_FETCH_BITWISE(and, uint64, uint64_t)

/*
 * API_DEF_AMO2:
 * This macro defines blocking atomic fetch-and operations (AMO) 
 * without context for various data types. It wraps the context-based 
 * operations defined above.
 *
 * _op   - The operation (fetch_and)
 * _name - The name of the data type (e.g., uint, ulong)
 * _type - The actual C data type (e.g., unsigned int, unsigned long)
 */
API_DEF_AMO2(fetch_and, uint, unsigned int)
API_DEF_AMO2(fetch_and, ulong, unsigned long)
API_DEF_AMO2(fetch_and, ulonglong, unsigned long long)
API_DEF_AMO2(fetch_and, int32, int32_t)
API_DEF_AMO2(fetch_and, int64, int64_t)
API_DEF_AMO2(fetch_and, uint32, uint32_t)
API_DEF_AMO2(fetch_and, uint64, uint64_t)
