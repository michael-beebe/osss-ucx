/* For license: see LICENSE file at top-level */

/*
 * If the configuration header is available, include it.
 * This may define platform-specific configurations or flags.
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

/*
 * Include SHMEM headers for mutex, utility functions,
 * core SHMEM functionality, and common definitions.
 */
#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"
#include "common.h"

/*
 * Define weak symbols for context-based atomic bitwise OR operations.
 * This allows these functions to be overridden by 'pshmem' prefixed versions 
 * when ENABLE_PSHMEM is defined.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_uint_atomic_or = pshmem_ctx_uint_atomic_or
#define shmem_ctx_uint_atomic_or pshmem_ctx_uint_atomic_or
#pragma weak shmem_ctx_ulong_atomic_or = pshmem_ctx_ulong_atomic_or
#define shmem_ctx_ulong_atomic_or pshmem_ctx_ulong_atomic_or
#pragma weak shmem_ctx_ulonglong_atomic_or = pshmem_ctx_ulonglong_atomic_or
#define shmem_ctx_ulonglong_atomic_or pshmem_ctx_ulonglong_atomic_or
#pragma weak shmem_ctx_int32_atomic_or = pshmem_ctx_int32_atomic_or
#define shmem_ctx_int32_atomic_or pshmem_ctx_int32_atomic_or
#pragma weak shmem_ctx_int64_atomic_or = pshmem_ctx_int64_atomic_or
#define shmem_ctx_int64_atomic_or pshmem_ctx_int64_atomic_or
#pragma weak shmem_ctx_uint32_atomic_or = pshmem_ctx_uint32_atomic_or
#define shmem_ctx_uint32_atomic_or pshmem_ctx_uint32_atomic_or
#pragma weak shmem_ctx_uint64_atomic_or = pshmem_ctx_uint64_atomic_or
#define shmem_ctx_uint64_atomic_or pshmem_ctx_uint64_atomic_or
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_BITWISE:
 * Defines a context-based atomic OR operation for a specific data type.
 * 
 * _opname - The bitwise operation name (in this case 'or')
 * _name   - The name of the data type (e.g., uint, ulong)
 * _type   - The actual C data type (e.g., unsigned int, unsigned long)
 */
SHMEM_CTX_TYPE_BITWISE(or, uint, unsigned int)
SHMEM_CTX_TYPE_BITWISE(or, ulong, unsigned long)
SHMEM_CTX_TYPE_BITWISE(or, ulonglong, unsigned long long)
SHMEM_CTX_TYPE_BITWISE(or, int32, int32_t)
SHMEM_CTX_TYPE_BITWISE(or, int64, int64_t)
SHMEM_CTX_TYPE_BITWISE(or, uint32, uint32_t)
SHMEM_CTX_TYPE_BITWISE(or, uint64, uint64_t)

/*
 * API_DEF_VOID_AMO2:
 * Defines a non-context atomic OR operation for various data types.
 * This wraps the context-based OR functions defined above.
 * 
 * _op   - The operation (or)
 * _name - The name of the data type (e.g., uint, ulong)
 * _type - The actual C data type (e.g., unsigned int, unsigned long)
 */
API_DEF_VOID_AMO2(or, uint, unsigned int)
API_DEF_VOID_AMO2(or, ulong, unsigned long)
API_DEF_VOID_AMO2(or, ulonglong, unsigned long long)
API_DEF_VOID_AMO2(or, int32, int32_t)
API_DEF_VOID_AMO2(or, int64, int64_t)
API_DEF_VOID_AMO2(or, uint32, uint32_t)
API_DEF_VOID_AMO2(or, uint64, uint64_t)
