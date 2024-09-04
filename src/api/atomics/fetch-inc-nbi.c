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
 * non-blocking atomic fetch-and-increment (AMO) operations for various data types.
 * These allow the functions to be overridden by prefixed 'pshmem' versions.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_int_atomic_fetch_inc_nbi = pshmem_int_atomic_fetch_inc_nbi
#define shmem_int_atomic_fetch_inc_nbi pshmem_int_atomic_fetch_inc_nbi
#pragma weak shmem_long_atomic_fetch_inc_nbi = pshmem_long_atomic_fetch_inc_nbi
#define shmem_long_atomic_fetch_inc_nbi pshmem_long_atomic_fetch_inc_nbi
#pragma weak shmem_longlong_atomic_fetch_inc_nbi = pshmem_longlong_atomic_fetch_inc_nbi
#define shmem_longlong_atomic_fetch_inc_nbi pshmem_longlong_atomic_fetch_inc_nbi
#pragma weak shmem_uint_atomic_fetch_inc_nbi = pshmem_uint_atomic_fetch_inc_nbi
#define shmem_uint_atomic_fetch_inc_nbi pshmem_uint_atomic_fetch_inc_nbi
#pragma weak shmem_ulong_atomic_fetch_inc_nbi = pshmem_ulong_atomic_fetch_inc_nbi
#define shmem_ulong_atomic_fetch_inc_nbi pshmem_ulong_atomic_fetch_inc_nbi
#pragma weak shmem_ulonglong_atomic_fetch_inc_nbi = pshmem_ulonglong_atomic_fetch_inc_nbi
#define shmem_ulonglong_atomic_fetch_inc_nbi pshmem_ulonglong_atomic_fetch_inc_nbi
#pragma weak shmem_int32_atomic_fetch_inc_nbi = pshmem_int32_atomic_fetch_inc_nbi
#define shmem_int32_atomic_fetch_inc_nbi pshmem_int32_atomic_fetch_inc_nbi
#pragma weak shmem_int64_atomic_fetch_inc_nbi = pshmem_int64_atomic_fetch_inc_nbi
#define shmem_int64_atomic_fetch_inc_nbi pshmem_int64_atomic_fetch_inc_nbi
#pragma weak shmem_uint32_atomic_fetch_inc_nbi = pshmem_uint32_atomic_fetch_inc_nbi
#define shmem_uint32_atomic_fetch_inc_nbi pshmem_uint32_atomic_fetch_inc_nbi
#pragma weak shmem_uint64_atomic_fetch_inc_nbi = pshmem_uint64_atomic_fetch_inc_nbi
#define shmem_uint64_atomic_fetch_inc_nbi pshmem_uint64_atomic_fetch_inc_nbi
#pragma weak shmem_size_atomic_fetch_inc_nbi = pshmem_size_atomic_fetch_inc_nbi
#define shmem_size_atomic_fetch_inc_nbi pshmem_size_atomic_fetch_inc_nbi
#pragma weak shmem_ptrdiff_atomic_fetch_inc_nbi = pshmem_ptrdiff_atomic_fetch_inc_nbi
#define shmem_ptrdiff_atomic_fetch_inc_nbi pshmem_ptrdiff_atomic_fetch_inc_nbi
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_FINC_NBI:
 * Defines context-based non-blocking atomic fetch-and-increment (AMO) operations 
 * for various data types. The function increments the target value and returns 
 * the previous value using the fetch pointer.
 *
 * _name - The name of the data type (e.g., int, long)
 * _type - The actual C data type (e.g., int, long)
 */
#define SHMEM_CTX_TYPE_FINC_NBI(_name, _type)                           \
    void                                                                \
    shmem_ctx_##_name##_atomic_fetch_inc_nbi(shmem_ctx_t ctx,           \
                                             _type *fetch,              \
                                             _type *target,             \
                                             int pe)                    \
    {                                                                   \
        _type one = 1;  /* Constant for increment operation */          \
                                                                        \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_fadd(ctx,                     \
                                               target,                  \
                                               &one, sizeof(one),       \
                                               pe, fetch));             \
    }

/* Instantiate the non-blocking atomic fetch-and-increment function for all required types */
SHMEM_CTX_TYPE_FINC_NBI(int, int)
SHMEM_CTX_TYPE_FINC_NBI(long, long)
SHMEM_CTX_TYPE_FINC_NBI(longlong, long long)
SHMEM_CTX_TYPE_FINC_NBI(uint, unsigned int)
SHMEM_CTX_TYPE_FINC_NBI(ulong, unsigned long)
SHMEM_CTX_TYPE_FINC_NBI(ulonglong, unsigned long long)
SHMEM_CTX_TYPE_FINC_NBI(int32, int32_t)
SHMEM_CTX_TYPE_FINC_NBI(int64, int64_t)
SHMEM_CTX_TYPE_FINC_NBI(uint32, uint32_t)
SHMEM_CTX_TYPE_FINC_NBI(uint64, uint64_t)
SHMEM_CTX_TYPE_FINC_NBI(size, size_t)
SHMEM_CTX_TYPE_FINC_NBI(ptrdiff, ptrdiff_t)

/*
 * API_DEF_AMO1_NBI:
 * This macro defines non-blocking atomic fetch-and-increment (AMO) 
 * operations without context for various data types. It wraps the 
 * context-based operations defined above.
 *
 * _op   - The operation (fetch_inc)
 * _name - The name of the data type (e.g., int, long)
 * _type - The actual C data type (e.g., int, long)
 */
API_DEF_AMO1_NBI(fetch_inc, int, int)
API_DEF_AMO1_NBI(fetch_inc, long, long)
API_DEF_AMO1_NBI(fetch_inc, longlong, long long)
API_DEF_AMO1_NBI(fetch_inc, uint, unsigned int)
API_DEF_AMO1_NBI(fetch_inc, ulong, unsigned long)
API_DEF_AMO1_NBI(fetch_inc, ulonglong, unsigned long long)
API_DEF_AMO1_NBI(fetch_inc, int32, int32_t)
API_DEF_AMO1_NBI(fetch_inc, int64, int64_t)
API_DEF_AMO1_NBI(fetch_inc, uint32, uint32_t)
API_DEF_AMO1_NBI(fetch_inc, uint64, uint64_t)
API_DEF_AMO1_NBI(fetch_inc, size, size_t)
API_DEF_AMO1_NBI(fetch_inc, ptrdiff, ptrdiff_t)
