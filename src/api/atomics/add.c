/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"
#include "common.h"

#ifdef ENABLE_PSHMEM
/* Weak symbol declarations for the public SHMEM atomic add routines. These 
 * enable shared memory implementations across different platforms and compilers. 
 * Each type (int, long, uint32, etc.) gets its own set of weak symbols.
 */
#pragma weak shmem_ctx_int_atomic_add = pshmem_ctx_int_atomic_add
#define shmem_ctx_int_atomic_add pshmem_ctx_int_atomic_add
#pragma weak shmem_ctx_long_atomic_add = pshmem_ctx_long_atomic_add
#define shmem_ctx_long_atomic_add pshmem_ctx_long_atomic_add
#pragma weak shmem_ctx_longlong_atomic_add = pshmem_ctx_longlong_atomic_add
#define shmem_ctx_longlong_atomic_add pshmem_ctx_longlong_atomic_add
#pragma weak shmem_ctx_uint_atomic_add = pshmem_ctx_uint_atomic_add
#define shmem_ctx_uint_atomic_add pshmem_ctx_uint_atomic_add
#pragma weak shmem_ctx_ulong_atomic_add = pshmem_ctx_ulong_atomic_add
#define shmem_ctx_ulong_atomic_add pshmem_ctx_ulong_atomic_add
#pragma weak shmem_ctx_ulonglong_atomic_add = pshmem_ctx_ulonglong_atomic_add
#define shmem_ctx_ulonglong_atomic_add pshmem_ctx_ulonglong_atomic_add
#pragma weak shmem_ctx_int32_atomic_add = pshmem_ctx_int32_atomic_add
#define shmem_ctx_int32_atomic_add pshmem_ctx_int32_atomic_add
#pragma weak shmem_ctx_int64_atomic_add = pshmem_ctx_int64_atomic_add
#define shmem_ctx_int64_atomic_add pshmem_ctx_int64_atomic_add
#pragma weak shmem_ctx_uint32_atomic_add = pshmem_ctx_uint32_atomic_add
#define shmem_ctx_uint32_atomic_add pshmem_ctx_uint32_atomic_add
#pragma weak shmem_ctx_uint64_atomic_add = pshmem_ctx_uint64_atomic_add
#define shmem_ctx_uint64_atomic_add pshmem_ctx_uint64_atomic_add
#pragma weak shmem_ctx_size_atomic_add = pshmem_ctx_size_atomic_add
#define shmem_ctx_size_atomic_add pshmem_ctx_size_atomic_add
#pragma weak shmem_ctx_ptrdiff_atomic_add = pshmem_ctx_ptrdiff_atomic_add
#define shmem_ctx_ptrdiff_atomic_add pshmem_ctx_ptrdiff_atomic_add
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_ADD defines a generic function for performing atomic
 * add operations for a given type. Each type (int, long, etc.) uses
 * this macro to generate its corresponding function.
 *
 * The function takes in a context (ctx), target memory address (target),
 * value to add, and the processing element (pe).
 */
#define SHMEM_CTX_TYPE_ADD(_name, _type)                                \
    void                                                                \
    shmem_ctx_##_name##_atomic_add(shmem_ctx_t ctx,                     \
                                   _type *target, _type value, int pe)  \
    {                                                                   \
        /* Call the core add function from shmemc, with mutex protection. */ \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_add(ctx,                      \
                                              target,                   \
                                              &value, sizeof(value),    \
                                              pe));                     \
    }

/* Generate atomic add functions for different data types */
SHMEM_CTX_TYPE_ADD(int, int)
SHMEM_CTX_TYPE_ADD(long, long)
SHMEM_CTX_TYPE_ADD(longlong, long long)
SHMEM_CTX_TYPE_ADD(uint, unsigned int)
SHMEM_CTX_TYPE_ADD(ulong, unsigned long)
SHMEM_CTX_TYPE_ADD(ulonglong, unsigned long long)
SHMEM_CTX_TYPE_ADD(int32, int32_t)
SHMEM_CTX_TYPE_ADD(int64, int64_t)
SHMEM_CTX_TYPE_ADD(uint32, uint32_t)
SHMEM_CTX_TYPE_ADD(uint64, uint64_t)
SHMEM_CTX_TYPE_ADD(size, size_t)
SHMEM_CTX_TYPE_ADD(ptrdiff, ptrdiff_t)

/*
 * API_DEF_VOID_AMO2 is a utility macro that defines atomic add operations
 * for each data type without returning any value (void). These functions
 * act as part of the API exposed to SHMEM users.
 */
API_DEF_VOID_AMO2(add, int, int)
API_DEF_VOID_AMO2(add, long, long)
API_DEF_VOID_AMO2(add, longlong, long long)
API_DEF_VOID_AMO2(add, uint, unsigned int)
API_DEF_VOID_AMO2(add, ulong, unsigned long)
API_DEF_VOID_AMO2(add, ulonglong, unsigned long long)
API_DEF_VOID_AMO2(add, int32, int32_t)
API_DEF_VOID_AMO2(add, int64, int64_t)
API_DEF_VOID_AMO2(add, uint32, uint32_t)
API_DEF_VOID_AMO2(add, uint64, uint64_t)
API_DEF_VOID_AMO2(add, size, size_t)
API_DEF_VOID_AMO2(add, ptrdiff, ptrdiff_t)

