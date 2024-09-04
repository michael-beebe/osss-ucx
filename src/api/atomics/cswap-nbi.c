/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmem_mutex.h"   /* Include SHMEM mutex support */
#include "shmemu.h"        /* Include SHMEM utility functions */
#include "shmemc.h"        /* Include SHMEM core functions */
#include "common.h"        /* Include common definitions */

/*
 * The following section handles enabling support for weak symbols 
 * for atomic compare-and-swap non-blocking operations (NBI) 
 * in case of ENABLE_PSHMEM being defined.
 */

#ifdef ENABLE_PSHMEM
#pragma weak shmem_int_atomic_compare_swap_nbi = pshmem_int_atomic_compare_swap_nbi
#define shmem_int_atomic_compare_swap_nbi pshmem_int_atomic_compare_swap_nbi
#pragma weak shmem_long_atomic_compare_swap_nbi = pshmem_long_atomic_compare_swap_nbi
#define shmem_long_atomic_compare_swap_nbi pshmem_long_atomic_compare_swap_nbi
#pragma weak shmem_longlong_atomic_compare_swap_nbi = pshmem_longlong_atomic_compare_swap_nbi
#define shmem_longlong_atomic_compare_swap_nbi pshmem_longlong_atomic_compare_swap_nbi
#pragma weak shmem_uint_atomic_compare_swap_nbi = pshmem_uint_atomic_compare_swap_nbi
#define shmem_uint_atomic_compare_swap_nbi pshmem_uint_atomic_compare_swap_nbi
#pragma weak shmem_ulong_atomic_compare_swap_nbi = pshmem_ulong_atomic_compare_swap_nbi
#define shmem_ulong_atomic_compare_swap_nbi pshmem_ulong_atomic_compare_swap_nbi
#pragma weak shmem_ulonglong_atomic_compare_swap_nbi = pshmem_ulonglong_atomic_compare_swap_nbi
#define shmem_ulonglong_atomic_compare_swap_nbi pshmem_ulonglong_atomic_compare_swap_nbi
#pragma weak shmem_int32_atomic_compare_swap_nbi = pshmem_int32_atomic_compare_swap_nbi
#define shmem_int32_atomic_compare_swap_nbi pshmem_int32_atomic_compare_swap_nbi
#pragma weak shmem_int64_atomic_compare_swap_nbi = pshmem_int64_atomic_compare_swap_nbi
#define shmem_int64_atomic_compare_swap_nbi pshmem_int64_atomic_compare_swap_nbi
#pragma weak shmem_uint32_atomic_compare_swap_nbi = pshmem_uint32_atomic_compare_swap_nbi
#define shmem_uint32_atomic_compare_swap_nbi pshmem_uint32_atomic_compare_swap_nbi
#pragma weak shmem_uint64_atomic_compare_swap_nbi = pshmem_uint64_atomic_compare_swap_nbi
#define shmem_uint64_atomic_compare_swap_nbi pshmem_uint64_atomic_compare_swap_nbi
#pragma weak shmem_size_atomic_compare_swap_nbi = pshmem_size_atomic_compare_swap_nbi
#define shmem_size_atomic_compare_swap_nbi pshmem_size_atomic_compare_swap_nbi
#pragma weak shmem_ptrdiff_atomic_compare_swap_nbi = pshmem_ptrdiff_atomic_compare_swap_nbi
#define shmem_ptrdiff_atomic_compare_swap_nbi pshmem_ptrdiff_atomic_compare_swap_nbi
#endif /* ENABLE_PSHMEM */

/*
 * SHMEM_CTX_TYPE_CSWAP_NBI:
 * Macro to define the context-based atomic compare-and-swap non-blocking (NBI) operation
 * for a given data type.
 *
 * _name  - The data type's name (e.g., int, long)
 * _type  - The actual C data type (e.g., int, long)
 */
#define SHMEM_CTX_TYPE_CSWAP_NBI(_name, _type)                          \
    void                                                                \
    shmem_ctx_##_name##_atomic_compare_swap_nbi(shmem_ctx_t ctx,        \
                                                _type *fetch,           \
                                                _type *target,          \
                                                _type cond, _type value, \
                                                int pe)                 \
    {                                                                   \
        SHMEMU_CHECK_INIT();  /* Check that SHMEM has been initialized */ \
        SHMEMU_CHECK_SYMMETRIC(target, 3);  /* Ensure the target is symmetric */ \
                                                                        \
        /* Perform the compare-and-swap operation */                    \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_cswap(ctx,                    \
                                                target,                 \
                                                &cond,                  \
                                                &value, sizeof(value),  \
                                                pe, fetch));            \
    }

/* Define atomic compare-and-swap NBI operations for various data types */
SHMEM_CTX_TYPE_CSWAP_NBI(int, int)
SHMEM_CTX_TYPE_CSWAP_NBI(long, long)
SHMEM_CTX_TYPE_CSWAP_NBI(longlong, long long)
SHMEM_CTX_TYPE_CSWAP_NBI(uint, unsigned int)
SHMEM_CTX_TYPE_CSWAP_NBI(ulong, unsigned long)
SHMEM_CTX_TYPE_CSWAP_NBI(ulonglong, unsigned long long)
SHMEM_CTX_TYPE_CSWAP_NBI(int32, int32_t)
SHMEM_CTX_TYPE_CSWAP_NBI(int64, int64_t)
SHMEM_CTX_TYPE_CSWAP_NBI(uint32, uint32_t)
SHMEM_CTX_TYPE_CSWAP_NBI(uint64, uint64_t)
SHMEM_CTX_TYPE_CSWAP_NBI(size, size_t)
SHMEM_CTX_TYPE_CSWAP_NBI(ptrdiff, ptrdiff_t)

/*
 * API_DEF_AMO3_NBI:
 * Macro to define context-free atomic compare-and-swap non-blocking (NBI) operations.
 *
 * _op   - The operation (e.g., compare_swap)
 * _name - The data type's name (e.g., int, long)
 * _type - The actual C data type (e.g., int, long)
 */
API_DEF_AMO3_NBI(compare_swap, int, int)
API_DEF_AMO3_NBI(compare_swap, long, long)
API_DEF_AMO3_NBI(compare_swap, longlong, long long)
API_DEF_AMO3_NBI(compare_swap, uint, unsigned int)
API_DEF_AMO3_NBI(compare_swap, ulong, unsigned long)
API_DEF_AMO3_NBI(compare_swap, ulonglong, unsigned long long)
API_DEF_AMO3_NBI(compare_swap, int32, int32_t)
API_DEF_AMO3_NBI(compare_swap, int64, int64_t)
API_DEF_AMO3_NBI(compare_swap, uint32, uint32_t)
API_DEF_AMO3_NBI(compare_swap, uint64, uint64_t)
API_DEF_AMO3_NBI(compare_swap, size, size_t)
API_DEF_AMO3_NBI(compare_swap, ptrdiff, ptrdiff_t)
