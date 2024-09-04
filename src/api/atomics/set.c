/* For license: see LICENSE file at top-level */

/*
 * If the configuration header exists, include it.
 * This might contain platform-specific settings.
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

/*
 * Include the necessary headers for mutex functionality, utility functions,
 * core SHMEM operations, and common definitions.
 */
#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"
#include "common.h"

/*
 * Define weak symbols for context-based atomic set operations.
 * This allows these functions to be overridden by 'pshmem' prefixed versions
 * when ENABLE_PSHMEM is defined.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_int_atomic_set = pshmem_ctx_int_atomic_set
#define shmem_ctx_int_atomic_set pshmem_ctx_int_atomic_set
#pragma weak shmem_ctx_long_atomic_set = pshmem_ctx_long_atomic_set
#define shmem_ctx_long_atomic_set pshmem_ctx_long_atomic_set
#pragma weak shmem_ctx_longlong_atomic_set = pshmem_ctx_longlong_atomic_set
#define shmem_ctx_longlong_atomic_set pshmem_ctx_longlong_atomic_set
#pragma weak shmem_ctx_float_atomic_set = pshmem_ctx_float_atomic_set
#define shmem_ctx_float_atomic_set pshmem_ctx_float_atomic_set
#pragma weak shmem_ctx_double_atomic_set = pshmem_ctx_double_atomic_set
#define shmem_ctx_double_atomic_set pshmem_ctx_double_atomic_set
#pragma weak shmem_ctx_uint_atomic_set = pshmem_ctx_uint_atomic_set
#define shmem_ctx_uint_atomic_set pshmem_ctx_uint_atomic_set
#pragma weak shmem_ctx_ulong_atomic_set = pshmem_ctx_ulong_atomic_set
#define shmem_ctx_ulong_atomic_set pshmem_ctx_ulong_atomic_set
#pragma weak shmem_ctx_ulonglong_atomic_set = pshmem_ctx_ulonglong_atomic_set
#define shmem_ctx_ulonglong_atomic_set pshmem_ctx_ulonglong_atomic_set
#pragma weak shmem_ctx_int32_atomic_set = pshmem_ctx_int32_atomic_set
#define shmem_ctx_int32_atomic_set pshmem_ctx_int32_atomic_set
#pragma weak shmem_ctx_int64_atomic_set = pshmem_ctx_int64_atomic_set
#define shmem_ctx_int64_atomic_set pshmem_ctx_int64_atomic_set
#pragma weak shmem_ctx_uint32_atomic_set = pshmem_ctx_uint32_atomic_set
#define shmem_ctx_uint32_atomic_set pshmem_ctx_uint32_atomic_set
#pragma weak shmem_ctx_uint64_atomic_set = pshmem_ctx_uint64_atomic_set
#define shmem_ctx_uint64_atomic_set pshmem_ctx_uint64_atomic_set
#pragma weak shmem_ctx_size_atomic_set = pshmem_ctx_size_atomic_set
#define shmem_ctx_size_atomic_set pshmem_ctx_size_atomic_set
#pragma weak shmem_ctx_ptrdiff_atomic_set = pshmem_ctx_ptrdiff_atomic_set
#define shmem_ctx_ptrdiff_atomic_set pshmem_ctx_ptrdiff_atomic_set
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_SET:
 * Defines a context-based atomic set operation for a specific data type.
 * This function sets the value of a variable atomically at a target PE.
 * 
 * _name   - The name of the data type (e.g., int, long)
 * _type   - The actual C data type (e.g., int, long)
 */
#define SHMEM_CTX_TYPE_SET(_name, _type)                                \
    void                                                                \
    shmem_ctx_##_name##_atomic_set(shmem_ctx_t ctx,                     \
                                   _type *target, _type value, int pe)  \
    {                                                                   \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_set(ctx,                      \
                                              target, sizeof(*target),  \
                                              &value, sizeof(value),    \
                                              pe));                     \
    }

/* Define context-based atomic set operations for various data types */
SHMEM_CTX_TYPE_SET(float, float)
SHMEM_CTX_TYPE_SET(double, double)
SHMEM_CTX_TYPE_SET(int, int)
SHMEM_CTX_TYPE_SET(long, long)
SHMEM_CTX_TYPE_SET(longlong, long long)
SHMEM_CTX_TYPE_SET(uint, unsigned int)
SHMEM_CTX_TYPE_SET(ulong, unsigned long)
SHMEM_CTX_TYPE_SET(ulonglong, unsigned long long)
SHMEM_CTX_TYPE_SET(int32, int32_t)
SHMEM_CTX_TYPE_SET(int64, int64_t)
SHMEM_CTX_TYPE_SET(uint32, uint32_t)
SHMEM_CTX_TYPE_SET(uint64, uint64_t)
SHMEM_CTX_TYPE_SET(size, size_t)
SHMEM_CTX_TYPE_SET(ptrdiff, ptrdiff_t)

/*
 * API_DEF_VOID_AMO2:
 * Defines a non-context atomic set operation for various data types.
 * This function calls the context-based atomic set operation.
 * 
 * _op   - The operation (set)
 * _name - The name of the data type (e.g., int, long)
 * _type - The actual C data type (e.g., int, long)
 */
API_DEF_VOID_AMO2(set, float, float)
API_DEF_VOID_AMO2(set, double, double)
API_DEF_VOID_AMO2(set, int, int)
API_DEF_VOID_AMO2(set, long, long)
API_DEF_VOID_AMO2(set, longlong, long long)
API_DEF_VOID_AMO2(set, uint, unsigned int)
API_DEF_VOID_AMO2(set, ulong, unsigned long)
API_DEF_VOID_AMO2(set, ulonglong, unsigned long long)
API_DEF_VOID_AMO2(set, int32, int32_t)
API_DEF_VOID_AMO2(set, int64, int64_t)
API_DEF_VOID_AMO2(set, uint32, uint32_t)
API_DEF_VOID_AMO2(set, uint64, uint64_t)
API_DEF_VOID_AMO2(set, size, size_t)
API_DEF_VOID_AMO2(set, ptrdiff, ptrdiff_t)
