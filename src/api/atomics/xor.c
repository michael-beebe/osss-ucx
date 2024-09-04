/* For license: see LICENSE file at top-level */

/*
 * Include the configuration header if available.
 * This may contain platform-specific configurations.
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

/*
 * Include headers for mutexes, utility functions, core SHMEM operations,
 * and common functionality shared across different SHMEM operations.
 */
#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"
#include "common.h"

/*
 * If ENABLE_PSHMEM is defined, declare weak symbols for the context-based
 * atomic XOR operations. This allows the 'pshmem' versions to override
 * the default versions, if needed.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_uint_atomic_xor = pshmem_ctx_uint_atomic_xor
#define shmem_ctx_uint_atomic_xor pshmem_ctx_uint_atomic_xor
#pragma weak shmem_ctx_ulong_atomic_xor = pshmem_ctx_ulong_atomic_xor
#define shmem_ctx_ulong_atomic_xor pshmem_ctx_ulong_atomic_xor
#pragma weak shmem_ctx_ulonglong_atomic_xor = pshmem_ctx_ulonglong_atomic_xor
#define shmem_ctx_ulonglong_atomic_xor pshmem_ctx_ulonglong_atomic_xor
#pragma weak shmem_ctx_int32_atomic_xor = pshmem_ctx_int32_atomic_xor
#define shmem_ctx_int32_atomic_xor pshmem_ctx_int32_atomic_xor
#pragma weak shmem_ctx_int64_atomic_xor = pshmem_ctx_int64_atomic_xor
#define shmem_ctx_int64_atomic_xor pshmem_ctx_int64_atomic_xor
#pragma weak shmem_ctx_uint32_atomic_xor = pshmem_ctx_uint32_atomic_xor
#define shmem_ctx_uint32_atomic_xor pshmem_ctx_uint32_atomic_xor
#pragma weak shmem_ctx_uint64_atomic_xor = pshmem_ctx_uint64_atomic_xor
#define shmem_ctx_uint64_atomic_xor pshmem_ctx_uint64_atomic_xor
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_BITWISE:
 * Defines the context-based atomic XOR operation for a given data type.
 * The function atomically performs an XOR operation between the value in
 * the calling PE and the value in a target PE.
 * 
 * _opname - The bitwise operation (xor)
 * _name   - The name of the data type (e.g., uint, ulong)
 * _type   - The actual C data type (e.g., unsigned int, unsigned long)
 */
SHMEM_CTX_TYPE_BITWISE(xor, uint, unsigned int)
SHMEM_CTX_TYPE_BITWISE(xor, ulong, unsigned long)
SHMEM_CTX_TYPE_BITWISE(xor, ulonglong, unsigned long long)
SHMEM_CTX_TYPE_BITWISE(xor, int32, int32_t)
SHMEM_CTX_TYPE_BITWISE(xor, int64, int64_t)
SHMEM_CTX_TYPE_BITWISE(xor, uint32, uint32_t)
SHMEM_CTX_TYPE_BITWISE(xor, uint64, uint64_t)

/*
 * API_DEF_VOID_AMO2:
 * Defines a non-context atomic XOR operation for various data types.
 * This function calls the context-based atomic XOR operation.
 * 
 * _op    - The operation (xor)
 * _name  - The name of the data type (e.g., uint, ulong)
 * _type  - The actual C data type (e.g., unsigned int, unsigned long)
 */
API_DEF_VOID_AMO2(xor, uint, unsigned int)
API_DEF_VOID_AMO2(xor, ulong, unsigned long)
API_DEF_VOID_AMO2(xor, ulonglong, unsigned long long)
API_DEF_VOID_AMO2(xor, int32, int32_t)
API_DEF_VOID_AMO2(xor, int64, int64_t)
API_DEF_VOID_AMO2(xor, uint32, uint32_t)
API_DEF_VOID_AMO2(xor, uint64, uint64_t)
