/* For license: see LICENSE file at top-level */

/* 
 * Include configuration header if it is present. 
 * This is typically generated during the build process.
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

/* 
 * Include necessary SHMEM headers for mutexes, utilities, core SHMEM
 * functionality, and common definitions.
 */
#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"
#include "common.h"

/* 
 * If ENABLE_PSHMEM is defined, create weak references for the non-blocking
 * atomic fetch-and operations (NBI) for various data types, which allows 
 * these functions to be overridden by prefixed 'pshmem' versions.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_uint_atomic_fetch_and_nbi = pshmem_uint_atomic_fetch_and_nbi
#define shmem_uint_atomic_fetch_and_nbi pshmem_uint_atomic_fetch_and_nbi
#pragma weak shmem_ulong_atomic_fetch_and_nbi = pshmem_ulong_atomic_fetch_and_nbi
#define shmem_ulong_atomic_fetch_and_nbi pshmem_ulong_atomic_fetch_and_nbi
#pragma weak shmem_ulonglong_atomic_fetch_and_nbi = pshmem_ulonglong_atomic_fetch_and_nbi
#define shmem_ulonglong_atomic_fetch_and_nbi pshmem_ulonglong_atomic_fetch_and_nbi
#pragma weak shmem_int32_atomic_fetch_and_nbi = pshmem_int32_atomic_fetch_and_nbi
#define shmem_int32_atomic_fetch_and_nbi pshmem_int32_atomic_fetch_and_nbi
#pragma weak shmem_int64_atomic_fetch_and_nbi = pshmem_int64_atomic_fetch_and_nbi
#define shmem_int64_atomic_fetch_and_nbi pshmem_int64_atomic_fetch_and_nbi
#pragma weak shmem_uint32_atomic_fetch_and_nbi = pshmem_uint32_atomic_fetch_and_nbi
#define shmem_uint32_atomic_fetch_and_nbi pshmem_uint32_atomic_fetch_and_nbi
#pragma weak shmem_uint64_atomic_fetch_and_nbi = pshmem_uint64_atomic_fetch_and_nbi
#define shmem_uint64_atomic_fetch_and_nbi pshmem_uint64_atomic_fetch_and_nbi
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_FETCH_BITWISE_NBI:
 * This macro defines context-based non-blocking atomic fetch-and (NBI) 
 * bitwise operations for various data types. These operations fetch the 
 * value at the target address, perform a bitwise AND operation with the 
 * specified value, and return the original value.
 * 
 * _opname - The operation name (and)
 * _name   - The name of the data type (e.g., uint, ulong)
 * _type   - The actual C data type (e.g., unsigned int, unsigned long)
 */
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(and, uint, unsigned int)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(and, ulong, unsigned long)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(and, ulonglong, unsigned long long)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(and, int32, int32_t)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(and, int64, int64_t)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(and, uint32, uint32_t)
SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(and, uint64, uint64_t)

/*
 * API_DEF_AMO2_NBI:
 * This macro defines non-blocking atomic fetch-and operations (NBI) 
 * without a context, for various data types. It wraps the context-based 
 * operations defined above.
 *
 * _op   - The operation (fetch_and)
 * _name - The name of the data type (e.g., uint, ulong)
 * _type - The actual C data type (e.g., unsigned int, unsigned long)
 */
API_DEF_AMO2_NBI(fetch_and, uint, unsigned int)
API_DEF_AMO2_NBI(fetch_and, ulong, unsigned long)
API_DEF_AMO2_NBI(fetch_and, ulonglong, unsigned long long)
API_DEF_AMO2_NBI(fetch_and, int32, int32_t)
API_DEF_AMO2_NBI(fetch_and, int64, int64_t)
API_DEF_AMO2_NBI(fetch_and, uint32, uint32_t)
API_DEF_AMO2_NBI(fetch_and, uint64, uint64_t)
