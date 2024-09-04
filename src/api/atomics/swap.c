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
 * atomic swap operations. This allows the 'pshmem' versions to override
 * the default versions, if needed.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_float_atomic_swap = pshmem_ctx_float_atomic_swap
#define shmem_ctx_float_atomic_swap pshmem_ctx_float_atomic_swap
#pragma weak shmem_ctx_double_atomic_swap = pshmem_ctx_double_atomic_swap
#define shmem_ctx_double_atomic_swap pshmem_ctx_double_atomic_swap
#pragma weak shmem_ctx_int_atomic_swap = pshmem_ctx_int_atomic_swap
#define shmem_ctx_int_atomic_swap pshmem_ctx_int_atomic_swap
#pragma weak shmem_ctx_long_atomic_swap = pshmem_ctx_long_atomic_swap
#define shmem_ctx_long_atomic_swap pshmem_ctx_long_atomic_swap
#pragma weak shmem_ctx_longlong_atomic_swap = pshmem_ctx_longlong_atomic_swap
#define shmem_ctx_longlong_atomic_swap pshmem_ctx_longlong_atomic_swap
#pragma weak shmem_ctx_uint_atomic_swap = pshmem_ctx_uint_atomic_swap
#define shmem_ctx_uint_atomic_swap pshmem_ctx_uint_atomic_swap
#pragma weak shmem_ctx_ulong_atomic_swap = pshmem_ctx_ulong_atomic_swap
#define shmem_ctx_ulong_atomic_swap pshmem_ctx_ulong_atomic_swap
#pragma weak shmem_ctx_ulonglong_atomic_swap = pshmem_ctx_ulonglong_atomic_swap
#define shmem_ctx_ulonglong_atomic_swap pshmem_ctx_ulonglong_atomic_swap
#pragma weak shmem_ctx_int32_atomic_swap = pshmem_ctx_int32_atomic_swap
#define shmem_ctx_int32_atomic_swap pshmem_ctx_int32_atomic_swap
#pragma weak shmem_ctx_int64_atomic_swap = pshmem_ctx_int64_atomic_swap
#define shmem_ctx_int64_atomic_swap pshmem_ctx_int64_atomic_swap
#pragma weak shmem_ctx_uint32_atomic_swap = pshmem_ctx_uint32_atomic_swap
#define shmem_ctx_uint32_atomic_swap pshmem_ctx_uint32_atomic_swap
#pragma weak shmem_ctx_uint64_atomic_swap = pshmem_ctx_uint64_atomic_swap
#define shmem_ctx_uint64_atomic_swap pshmem_ctx_uint64_atomic_swap
#pragma weak shmem_ctx_size_atomic_swap = pshmem_ctx_size_atomic_swap
#define shmem_ctx_size_atomic_swap pshmem_ctx_size_atomic_swap
#pragma weak shmem_ctx_ptrdiff_atomic_swap = pshmem_ctx_ptrdiff_atomic_swap
#define shmem_ctx_ptrdiff_atomic_swap pshmem_ctx_ptrdiff_atomic_swap
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_SWAP:
 * Define the context-based atomic swap operation for a given data type.
 * The function atomically swaps the value between the calling PE and a target PE.
 * 
 * _name  - The name of the data type (e.g., int, long)
 * _type  - The actual C data type (e.g., int, long)
 */
#define SHMEM_CTX_TYPE_SWAP(_name, _type)                               \
    _type                                                               \
    shmem_ctx_##_name##_atomic_swap(shmem_ctx_t ctx,                    \
                                    _type *target, _type value, int pe) \
    {                                                                   \
        _type v;                                                        \
                                                                        \
        SHMEMU_CHECK_INIT();                                            \
        SHMEMU_CHECK_SYMMETRIC(target, 2);                              \
                                                                        \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_swap(ctx,                     \
                                               target,                  \
                                               &value, sizeof(value),   \
                                               pe, &v));                \
        return v;                                                       \
    }

/* Define context-based atomic swap operations for various data types */
SHMEM_CTX_TYPE_SWAP(int, int)
SHMEM_CTX_TYPE_SWAP(long, long)
SHMEM_CTX_TYPE_SWAP(longlong, long long)
SHMEM_CTX_TYPE_SWAP(float, float)
SHMEM_CTX_TYPE_SWAP(double, double)
SHMEM_CTX_TYPE_SWAP(uint, unsigned int)
SHMEM_CTX_TYPE_SWAP(ulong, unsigned long)
SHMEM_CTX_TYPE_SWAP(ulonglong, unsigned long long)
SHMEM_CTX_TYPE_SWAP(int32, int32_t)
SHMEM_CTX_TYPE_SWAP(int64, int64_t)
SHMEM_CTX_TYPE_SWAP(uint32, uint32_t)
SHMEM_CTX_TYPE_SWAP(uint64, uint64_t)
SHMEM_CTX_TYPE_SWAP(size, size_t)
SHMEM_CTX_TYPE_SWAP(ptrdiff, ptrdiff_t)

/*
 * API_DEF_AMO2:
 * Defines a non-context atomic swap operation for various data types.
 * This function calls the context-based atomic swap operation.
 * 
 * _op    - The operation (swap)
 * _name  - The name of the data type (e.g., int, long)
 * _type  - The actual C data type (e.g., int, long)
 */
API_DEF_AMO2(swap, float, float)
API_DEF_AMO2(swap, double, double)
API_DEF_AMO2(swap, int, int)
API_DEF_AMO2(swap, long, long)
API_DEF_AMO2(swap, longlong, long long)
API_DEF_AMO2(swap, uint, unsigned int)
API_DEF_AMO2(swap, ulong, unsigned long)
API_DEF_AMO2(swap, ulonglong, unsigned long long)
API_DEF_AMO2(swap, int32, int32_t)
API_DEF_AMO2(swap, int64, int64_t)
API_DEF_AMO2(swap, uint32, uint32_t)
API_DEF_AMO2(swap, uint64, uint64_t)
API_DEF_AMO2(swap, size, size_t)
API_DEF_AMO2(swap, ptrdiff, ptrdiff_t)
