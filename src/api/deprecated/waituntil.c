/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmem_mutex.h"
#include "shmemu.h"

/*
 * Deprecated at OpenSHMEM version 1.4
 *
 * This file implements deprecated wait routines. These wait routines
 * (e.g., `shmem_int_wait()`) were simplified forms of the more 
 * general `shmem_wait_until()` and checked for inequality or change.
 * 
 * The modern equivalents are non-blocking variants and use more general
 * context-aware calls.
 */

static const shmemu_version_t v = { .major = 1, .minor = 4 };

/* 
 * PSHMEM weak symbol declarations for profiling.
 * This allows profiling tools to intercept calls to the deprecated wait routines.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_short_wait = pshmem_short_wait
#define shmem_short_wait pshmem_short_wait
#pragma weak shmem_int_wait = pshmem_int_wait
#define shmem_int_wait pshmem_int_wait
#pragma weak shmem_long_wait = pshmem_long_wait
#define shmem_long_wait pshmem_long_wait
#pragma weak shmem_longlong_wait = pshmem_longlong_wait
#define shmem_longlong_wait pshmem_longlong_wait
#pragma weak shmem_ushort_wait = pshmem_ushort_wait
#define shmem_ushort_wait pshmem_ushort_wait
#pragma weak shmem_uint_wait = pshmem_uint_wait
#define shmem_uint_wait pshmem_uint_wait
#pragma weak shmem_ulong_wait = pshmem_ulong_wait
#define shmem_ulong_wait pshmem_ulong_wait
#pragma weak shmem_ulonglong_wait = pshmem_ulonglong_wait
#define shmem_ulonglong_wait pshmem_ulonglong_wait
#pragma weak shmem_int32_wait = pshmem_int32_wait
#define shmem_int32_wait pshmem_int32_wait
#pragma weak shmem_int64_wait = pshmem_int64_wait
#define shmem_int64_wait pshmem_int64_wait
#pragma weak shmem_uint32_wait = pshmem_uint32_wait
#define shmem_uint32_wait pshmem_uint32_wait
#pragma weak shmem_uint64_wait = pshmem_uint64_wait
#define shmem_uint64_wait pshmem_uint64_wait
#pragma weak shmem_size_wait = pshmem_size_wait
#define shmem_size_wait pshmem_size_wait
#pragma weak shmem_ptrdiff_wait = pshmem_ptrdiff_wait
#define shmem_ptrdiff_wait pshmem_ptrdiff_wait
#endif  /* ENABLE_PSHMEM */

/*
 * Macro for defining deprecated wait routines. The wait routines block 
 * until the value pointed to by `ivar` becomes unequal to `cmp_value`.
 *
 * `_name`: Type of the wait routine (e.g., short, int, long).
 * `_type`: Data type associated with the routine (e.g., int32_t, uint64_t).
 * `_size`: Bit size of the data type (e.g., 32, 64).
 * 
 * The macro defines functions like `shmem_int_wait()` and logs a 
 * deprecation warning before calling the updated version.
 */
#define SHMEM_TYPE_WAIT(_name, _type, _size)                        \
    void                                                            \
    shmem_##_name##_wait(_type *ivar, _type cmp_value)              \
    {                                                               \
        /* Log deprecation warning for the routine */               \
        deprecate(__func__, &v);                                    \
                                                                    \
        /* Call the newer version using a 16, 32, or 64-bit type */  \
        shmemc_ctx_wait_until_ne##_size(SHMEM_CTX_DEFAULT,          \
                                        (int##_size##_t *) ivar,    \
                                        cmp_value);                 \
    }

/*
 * Define deprecated wait functions for various data types (int, long, etc.).
 * These functions wait until the value in `ivar` is not equal to `cmp_value`.
 * 
 * Examples:
 * - `shmem_int_wait()` waits for a 32-bit integer.
 * - `shmem_long_wait()` waits for a 64-bit long integer.
 * 
 * The `shmemc_ctx_wait_until_ne*_size()` function is called to handle
 * the wait operation in the modern context-aware API.
 */
SHMEM_TYPE_WAIT(short, short, 16)
SHMEM_TYPE_WAIT(int, int, 32)
SHMEM_TYPE_WAIT(long, long, 64)
SHMEM_TYPE_WAIT(longlong, long long, 64)
SHMEM_TYPE_WAIT(ushort, unsigned short, 16)
SHMEM_TYPE_WAIT(uint, unsigned int, 32)
SHMEM_TYPE_WAIT(ulong, unsigned long, 64)
SHMEM_TYPE_WAIT(ulonglong, unsigned long long, 64)
SHMEM_TYPE_WAIT(int32, int32_t, 32)
SHMEM_TYPE_WAIT(int64, int64_t, 64)
SHMEM_TYPE_WAIT(uint32, uint32_t, 32)
SHMEM_TYPE_WAIT(uint64, uint64_t, 64)
SHMEM_TYPE_WAIT(size, size_t, 64)
SHMEM_TYPE_WAIT(ptrdiff, ptrdiff_t, 64)
