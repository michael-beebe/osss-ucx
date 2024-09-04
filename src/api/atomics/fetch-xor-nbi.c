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
 * If ENABLE_PSHMEM is defined, create weak references for the non-blocking 
 * atomic fetch 'xor' operations (AMO) for various data types.
 * These allow the functions to be overridden by prefixed 'pshmem' versions.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_uint_atomic_fetch_xor_nbi = pshmem_uint_atomic_fetch_xor_nbi
#define shmem_uint_atomic_fetch_xor_nbi pshmem_uint_atomic_fetch_xor_nbi
#pragma weak shmem_ulong_atomic_fetch_xor_nbi = pshmem_ulong_atomic_fetch_xor_nbi
#define shmem_ulong_atomic_fetch_xor_nbi pshmem_ulong_atomic_fetch_xor_nbi
#pragma weak shmem_ulonglong_atomic_fetch_xor_nbi = pshmem_ulonglong_atomic_fetch_xor_nbi
#define shmem_ulonglong_atomic_fetch_xor_nbi pshmem_ulonglong_atomic_fetch_xor_nbi
#pragma weak shmem_int32_atomic_fetch_xor_nbi = pshmem_int32_atomic_fetch_xor_nbi
#define shmem_int32_atomic_fetch_xor_nbi pshmem_int32_atomic_fetch_xor_nbi
#pragma weak shmem_int64_atomic_fetch_xor_nbi = pshmem_int64_atomic_fetch_xor_nbi
#define shmem_int64_atomic_fetch_xor_nbi pshmem_int64_atomic_fetch_xor_nbi
#pragma weak shmem_uint32_atomic_fetch_xor_nbi = pshmem_uint32_atomic_fetch_xor_nbi
#define shmem_uint32_atomic_fetch_xor_nbi pshmem_uint32_atomic_fetch_xor_nbi
#pragma weak shmem_uint64_atomic_fetch_xor_nbi = pshmem_uint64_atomic_fetch_xor_nbi
#define shmem_uint64_atomic_fetch_xor_nbi pshmem_uint64_atomic_fetch_xor_nbi
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_FETCH_BITWISE_NBI:
 * Defines the context-based atomic fetch 'xor' non-blocking operation for 
 * various data types. This function fetches the bitwise XOR value from a 
 * remote PE and returns it non-blocking.
 * 
 * _opname - The operation name (xor)
 * _name   - The name of the data type (e.g., uint, int32)
 * _type   - The actual C data type (e.g., unsigned int, int32_t)
 */
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(xor, uint, unsigned int)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(xor, ulong, unsigned long)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(xor, ulonglong, unsigned long long)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(xor, int32, int32_t)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(xor, int64, int64_t)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(xor, uint32, uint32_t)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(xor, uint64, uint64_t)

/*
 * API_DEF_AMO2_NBI:
 * This macro defines the non-blocking atomic fetch 'xor' operation without
 * context for various data types. It wraps the context-based operations 
 * defined above.
 * 
 * _op   - The operation (fetch_xor)
 * _name - The name of the data type (e.g., uint, ulong)
 * _type - The actual C data type (e.g., unsigned int, unsigned long)
 */
API_DEF_AMO2_NBI(fetch_xor, uint, unsigned int)
API_DEF_AMO2_NBI(fetch_xor, ulong, unsigned long)
API_DEF_AMO2_NBI(fetch_xor, ulonglong, unsigned long long)
API_DEF_AMO2_NBI(fetch_xor, int32, int32_t)
API_DEF_AMO2_NBI(fetch_xor, int64, int64_t)
API_DEF_AMO2_NBI(fetch_xor, uint32, uint32_t)
API_DEF_AMO2_NBI(fetch_xor, uint64, uint64_t)
