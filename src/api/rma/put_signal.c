/**
 * @file put_signal.c
 * @brief Implements signal-based put routines for SHMEM.
 *
 * This file provides implementations for both blocking and non-blocking
 * remote memory put operations with signaling. It also sets up the PSHMEM
 * interface using weak symbol definitions. In particular, it defines routines
 * such as shmem_signal_fetch that atomically fetch the current signal value.
 */

/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include "shmemc.h"
#include "put_signal.h"
#include <shmem/api_types.h>

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_float_put_signal = pshmem_ctx_float_put_signal
#define shmem_ctx_float_put_signal pshmem_ctx_float_put_signal
#pragma weak shmem_ctx_double_put_signal = pshmem_ctx_double_put_signal
#define shmem_ctx_double_put_signal pshmem_ctx_double_put_signal
#pragma weak shmem_ctx_longdouble_put_signal = pshmem_ctx_longdouble_put_signal
#define shmem_ctx_longdouble_put_signal pshmem_ctx_longdouble_put_signal
#pragma weak shmem_ctx_char_put_signal = pshmem_ctx_char_put_signal
#define shmem_ctx_char_put_signal pshmem_ctx_char_put_signal
#pragma weak shmem_ctx_schar_put_signal = pshmem_ctx_schar_put_signal
#define shmem_ctx_schar_put_signal pshmem_ctx_schar_put_signal
#pragma weak shmem_ctx_short_put_signal = pshmem_ctx_short_put_signal
#define shmem_ctx_short_put_signal pshmem_ctx_short_put_signal
#pragma weak shmem_ctx_int_put_signal = pshmem_ctx_int_put_signal
#define shmem_ctx_int_put_signal pshmem_ctx_int_put_signal
#pragma weak shmem_ctx_long_put_signal = pshmem_ctx_long_put_signal
#define shmem_ctx_long_put_signal pshmem_ctx_long_put_signal
#pragma weak shmem_ctx_longlong_put_signal = pshmem_ctx_longlong_put_signal
#define shmem_ctx_longlong_put_signal pshmem_ctx_longlong_put_signal
#pragma weak shmem_ctx_uchar_put_signal = pshmem_ctx_uchar_put_signal
#define shmem_ctx_uchar_put_signal pshmem_ctx_uchar_put_signal
#pragma weak shmem_ctx_ushort_put_signal = pshmem_ctx_ushort_put_signal
#define shmem_ctx_ushort_put_signal pshmem_ctx_ushort_put_signal
#pragma weak shmem_ctx_uint_put_signal = pshmem_ctx_uint_put_signal
#define shmem_ctx_uint_put_signal pshmem_ctx_uint_put_signal
#pragma weak shmem_ctx_ulong_put_signal = pshmem_ctx_ulong_put_signal
#define shmem_ctx_ulong_put_signal pshmem_ctx_ulong_put_signal
#pragma weak shmem_ctx_ulonglong_put_signal = pshmem_ctx_ulonglong_put_signal
#define shmem_ctx_ulonglong_put_signal pshmem_ctx_ulonglong_put_signal
#pragma weak shmem_ctx_int8_put_signal = pshmem_ctx_int8_put_signal
#define shmem_ctx_int8_put_signal pshmem_ctx_int8_put_signal
#pragma weak shmem_ctx_int16_put_signal = pshmem_ctx_int16_put_signal
#define shmem_ctx_int16_put_signal pshmem_ctx_int16_put_signal
#pragma weak shmem_ctx_int32_put_signal = pshmem_ctx_int32_put_signal
#define shmem_ctx_int32_put_signal pshmem_ctx_int32_put_signal
#pragma weak shmem_ctx_int64_put_signal = pshmem_ctx_int64_put_signal
#define shmem_ctx_int64_put_signal pshmem_ctx_int64_put_signal
#pragma weak shmem_ctx_uint8_put_signal = pshmem_ctx_uint8_put_signal
#define shmem_ctx_uint8_put_signal pshmem_ctx_uint8_put_signal
#pragma weak shmem_ctx_uint16_put_signal = pshmem_ctx_uint16_put_signal
#define shmem_ctx_uint16_put_signal pshmem_ctx_uint16_put_signal
#pragma weak shmem_ctx_uint32_put_signal = pshmem_ctx_uint32_put_signal
#define shmem_ctx_uint32_put_signal pshmem_ctx_uint32_put_signal
#pragma weak shmem_ctx_uint64_put_signal = pshmem_ctx_uint64_put_signal
#define shmem_ctx_uint64_put_signal pshmem_ctx_uint64_put_signal
#endif /* ENABLE_PSHMEM */

#define PUT_SIGNAL_TYPE_HELPER(_type, _typename)                               \
  SHMEM_CTX_TYPED_PUT_SIGNAL(_typename, _type)

SHMEM_STANDARD_RMA_TYPE_TABLE(PUT_SIGNAL_TYPE_HELPER)
#undef PUT_SIGNAL_TYPE_HELPER

#ifdef ENABLE_PSHMEM
#pragma weak shmem_float_put_signal = pshmem_float_put_signal
#define shmem_float_put_signal pshmem_float_put_signal
#pragma weak shmem_double_put_signal = pshmem_double_put_signal
#define shmem_double_put_signal pshmem_double_put_signal
#pragma weak shmem_longdouble_put_signal = pshmem_longdouble_put_signal
#define shmem_longdouble_put_signal pshmem_longdouble_put_signal
#pragma weak shmem_char_put_signal = pshmem_char_put_signal
#define shmem_char_put_signal pshmem_char_put_signal
#pragma weak shmem_schar_put_signal = pshmem_schar_put_signal
#define shmem_schar_put_signal pshmem_schar_put_signal
#pragma weak shmem_short_put_signal = pshmem_short_put_signal
#define shmem_short_put_signal pshmem_short_put_signal
#pragma weak shmem_int_put_signal = pshmem_int_put_signal
#define shmem_int_put_signal pshmem_int_put_signal
#pragma weak shmem_long_put_signal = pshmem_long_put_signal
#define shmem_long_put_signal pshmem_long_put_signal
#pragma weak shmem_longlong_put_signal = pshmem_longlong_put_signal
#define shmem_longlong_put_signal pshmem_longlong_put_signal
#pragma weak shmem_uchar_put_signal = pshmem_uchar_put_signal
#define shmem_uchar_put_signal pshmem_uchar_put_signal
#pragma weak shmem_ushort_put_signal = pshmem_ushort_put_signal
#define shmem_ushort_put_signal pshmem_ushort_put_signal
#pragma weak shmem_uint_put_signal = pshmem_uint_put_signal
#define shmem_uint_put_signal pshmem_uint_put_signal
#pragma weak shmem_ulong_put_signal = pshmem_ulong_put_signal
#define shmem_ulong_put_signal pshmem_ulong_put_signal
#pragma weak shmem_ulonglong_put_signal = pshmem_ulonglong_put_signal
#define shmem_ulonglong_put_signal pshmem_ulonglong_put_signal
#pragma weak shmem_int8_put_signal = pshmem_int8_put_signal
#define shmem_int8_put_signal pshmem_int8_put_signal
#pragma weak shmem_int16_put_signal = pshmem_int16_put_signal
#define shmem_int16_put_signal pshmem_int16_put_signal
#pragma weak shmem_int32_put_signal = pshmem_int32_put_signal
#define shmem_int32_put_signal pshmem_int32_put_signal
#pragma weak shmem_int64_put_signal = pshmem_int64_put_signal
#define shmem_int64_put_signal pshmem_int64_put_signal
#pragma weak shmem_uint8_put_signal = pshmem_uint8_put_signal
#define shmem_uint8_put_signal pshmem_uint8_put_signal
#pragma weak shmem_uint16_put_signal = pshmem_uint16_put_signal
#define shmem_uint16_put_signal pshmem_uint16_put_signal
#pragma weak shmem_uint32_put_signal = pshmem_uint32_put_signal
#define shmem_uint32_put_signal pshmem_uint32_put_signal
#pragma weak shmem_uint64_put_signal = pshmem_uint64_put_signal
#define shmem_uint64_put_signal pshmem_uint64_put_signal
#pragma weak shmem_size_put_signal = pshmem_size_put_signal
#define shmem_size_put_signal pshmem_size_put_signal
#pragma weak shmem_ptrdiff_put_signal = pshmem_ptrdiff_put_signal
#define shmem_ptrdiff_put_signal pshmem_ptrdiff_put_signal
#endif /* ENABLE_PSHMEM */

#define API_PUT_SIGNAL_TYPE_HELPER(_type, _typename)                           \
  API_DECL_TYPED_PUT_SIGNAL(_typename, _type)

SHMEM_STANDARD_RMA_TYPE_TABLE(API_PUT_SIGNAL_TYPE_HELPER)
#undef API_PUT_SIGNAL_TYPE_HELPER

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_put8_signal = pshmem_ctx_put8_signal
#define shmem_ctx_put8_signal pshmem_ctx_put8_signal
#pragma weak shmem_ctx_put16_signal = pshmem_ctx_put16_signal
#define shmem_ctx_put16_signal pshmem_ctx_put16_signal
#pragma weak shmem_ctx_put32_signal = pshmem_ctx_put32_signal
#define shmem_ctx_put32_signal pshmem_ctx_put32_signal
#pragma weak shmem_ctx_put64_signal = pshmem_ctx_put64_signal
#define shmem_ctx_put64_signal pshmem_ctx_put64_signal
#pragma weak shmem_ctx_put128_signal = pshmem_ctx_put128_signal
#define shmem_ctx_put128_signal pshmem_ctx_put128_signal

#pragma weak shmem_put8_signal = pshmem_put8_signal
#define shmem_put8_signal pshmem_put8_signal
#pragma weak shmem_put16_signal = pshmem_put16_signal
#define shmem_put16_signal pshmem_put16_signal
#pragma weak shmem_put32_signal = pshmem_put32_signal
#define shmem_put32_signal pshmem_put32_signal
#pragma weak shmem_put64_signal = pshmem_put64_signal
#define shmem_put64_signal pshmem_put64_signal
#pragma weak shmem_put128_signal = pshmem_put128_signal
#define shmem_put128_signal pshmem_put128_signal
#endif /* ENABLE_PSHMEM */

SHMEM_CTX_DECL_SIZED_PUT_SIGNAL(8)
SHMEM_CTX_DECL_SIZED_PUT_SIGNAL(16)
SHMEM_CTX_DECL_SIZED_PUT_SIGNAL(32)
SHMEM_CTX_DECL_SIZED_PUT_SIGNAL(64)
SHMEM_CTX_DECL_SIZED_PUT_SIGNAL(128)

API_DECL_SIZED_PUT_SIGNAL(8)
API_DECL_SIZED_PUT_SIGNAL(16)
API_DECL_SIZED_PUT_SIGNAL(32)
API_DECL_SIZED_PUT_SIGNAL(64)
API_DECL_SIZED_PUT_SIGNAL(128)

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_putmem_signal = pshmem_ctx_putmem_signal
#define shmem_ctx_putmem_signal pshmem_ctx_putmem_signal
#endif /* ENABLE_PSHMEM */

SHMEM_CTX_DECL_PUTMEM_SIGNAL()
API_DECL_PUTMEM_SIGNAL()

/*
 * non-blocking
 */

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_float_put_signal_nbi = pshmem_ctx_float_put_signal_nbi
#define shmem_ctx_float_put_signal_nbi pshmem_ctx_float_put_signal_nbi
#pragma weak shmem_ctx_double_put_signal_nbi = pshmem_ctx_double_put_signal_nbi
#define shmem_ctx_double_put_signal_nbi pshmem_ctx_double_put_signal_nbi
#pragma weak shmem_ctx_longdouble_put_signal_nbi =                             \
    pshmem_ctx_longdouble_put_signal_nbi
#define shmem_ctx_longdouble_put_signal_nbi pshmem_ctx_longdouble_put_signal_nbi
#pragma weak shmem_ctx_char_put_signal_nbi = pshmem_ctx_char_put_signal_nbi
#define shmem_ctx_char_put_signal_nbi pshmem_ctx_char_put_signal_nbi
#pragma weak shmem_ctx_schar_put_signal_nbi = pshmem_ctx_schar_put_signal_nbi
#define shmem_ctx_schar_put_signal_nbi pshmem_ctx_schar_put_signal_nbi
#pragma weak shmem_ctx_short_put_signal_nbi = pshmem_ctx_short_put_signal_nbi
#define shmem_ctx_short_put_signal_nbi pshmem_ctx_short_put_signal_nbi
#pragma weak shmem_ctx_int_put_signal_nbi = pshmem_ctx_int_put_signal_nbi
#define shmem_ctx_int_put_signal_nbi pshmem_ctx_int_put_signal_nbi
#pragma weak shmem_ctx_long_put_signal_nbi = pshmem_ctx_long_put_signal_nbi
#define shmem_ctx_long_put_signal_nbi pshmem_ctx_long_put_signal_nbi
#pragma weak shmem_ctx_longlong_put_signal_nbi =                               \
    pshmem_ctx_longlong_put_signal_nbi
#define shmem_ctx_longlong_put_signal_nbi pshmem_ctx_longlong_put_signal_nbi
#pragma weak shmem_ctx_uchar_put_signal_nbi = pshmem_ctx_uchar_put_signal_nbi
#define shmem_ctx_uchar_put_signal_nbi pshmem_ctx_uchar_put_signal_nbi
#pragma weak shmem_ctx_ushort_put_signal_nbi = pshmem_ctx_ushort_put_signal_nbi
#define shmem_ctx_ushort_put_signal_nbi pshmem_ctx_ushort_put_signal_nbi
#pragma weak shmem_ctx_uint_put_signal_nbi = pshmem_ctx_uint_put_signal_nbi
#define shmem_ctx_uint_put_signal_nbi pshmem_ctx_uint_put_signal_nbi
#pragma weak shmem_ctx_ulong_put_signal_nbi = pshmem_ctx_ulong_put_signal_nbi
#define shmem_ctx_ulong_put_signal_nbi pshmem_ctx_ulong_put_signal_nbi
#pragma weak shmem_ctx_ulonglong_put_signal_nbi =                              \
    pshmem_ctx_ulonglong_put_signal_nbi
#define shmem_ctx_ulonglong_put_signal_nbi pshmem_ctx_ulonglong_put_signal_nbi
#pragma weak shmem_ctx_int8_put_signal_nbi = pshmem_ctx_int8_put_signal_nbi
#define shmem_ctx_int8_put_signal_nbi pshmem_ctx_int8_put_signal_nbi
#pragma weak shmem_ctx_int16_put_signal_nbi = pshmem_ctx_int16_put_signal_nbi
#define shmem_ctx_int16_put_signal_nbi pshmem_ctx_int16_put_signal_nbi
#pragma weak shmem_ctx_int32_put_signal_nbi = pshmem_ctx_int32_put_signal_nbi
#define shmem_ctx_int32_put_signal_nbi pshmem_ctx_int32_put_signal_nbi
#pragma weak shmem_ctx_int64_put_signal_nbi = pshmem_ctx_int64_put_signal_nbi
#define shmem_ctx_int64_put_signal_nbi pshmem_ctx_int64_put_signal_nbi
#pragma weak shmem_ctx_uint8_put_signal_nbi = pshmem_ctx_uint8_put_signal_nbi
#define shmem_ctx_uint8_put_signal_nbi pshmem_ctx_uint8_put_signal_nbi
#pragma weak shmem_ctx_uint16_put_signal_nbi = pshmem_ctx_uint16_put_signal_nbi
#define shmem_ctx_uint16_put_signal_nbi pshmem_ctx_uint16_put_signal_nbi
#pragma weak shmem_ctx_uint32_put_signal_nbi = pshmem_ctx_uint32_put_signal_nbi
#define shmem_ctx_uint32_put_signal_nbi pshmem_ctx_uint32_put_signal_nbi
#pragma weak shmem_ctx_uint64_put_signal_nbi = pshmem_ctx_uint64_put_signal_nbi
#define shmem_ctx_uint64_put_signal_nbi pshmem_ctx_uint64_put_signal_nbi
#endif /* ENABLE_PSHMEM */

#define CTX_PUT_SIGNAL_NBI_TYPE_HELPER(_type, _typename)                       \
  SHMEM_CTX_TYPED_PUT_SIGNAL_NBI(_typename, _type)

SHMEM_STANDARD_RMA_TYPE_TABLE(CTX_PUT_SIGNAL_NBI_TYPE_HELPER)
#undef CTX_PUT_SIGNAL_NBI_TYPE_HELPER

#ifdef ENABLE_PSHMEM
#pragma weak shmem_float_put_signal_nbi = pshmem_float_put_signal_nbi
#define shmem_float_put_signal_nbi pshmem_float_put_signal_nbi
#pragma weak shmem_double_put_signal_nbi = pshmem_double_put_signal_nbi
#define shmem_double_put_signal_nbi pshmem_double_put_signal_nbi
#pragma weak shmem_longdouble_put_signal_nbi = pshmem_longdouble_put_signal_nbi
#define shmem_longdouble_put_signal_nbi pshmem_longdouble_put_signal_nbi
#pragma weak shmem_char_put_signal_nbi = pshmem_char_put_signal_nbi
#define shmem_char_put_signal_nbi pshmem_char_put_signal_nbi
#pragma weak shmem_schar_put_signal_nbi = pshmem_schar_put_signal_nbi
#define shmem_schar_put_signal_nbi pshmem_schar_put_signal_nbi
#pragma weak shmem_short_put_signal_nbi = pshmem_short_put_signal_nbi
#define shmem_short_put_signal_nbi pshmem_short_put_signal_nbi
#pragma weak shmem_int_put_signal_nbi = pshmem_int_put_signal_nbi
#define shmem_int_put_signal_nbi pshmem_int_put_signal_nbi
#pragma weak shmem_long_put_signal_nbi = pshmem_long_put_signal_nbi
#define shmem_long_put_signal_nbi pshmem_long_put_signal_nbi
#pragma weak shmem_longlong_put_signal_nbi = pshmem_longlong_put_signal_nbi
#define shmem_longlong_put_signal_nbi pshmem_longlong_put_signal_nbi
#pragma weak shmem_uchar_put_signal_nbi = pshmem_uchar_put_signal_nbi
#define shmem_uchar_put_signal_nbi pshmem_uchar_put_signal_nbi
#pragma weak shmem_ushort_put_signal_nbi = pshmem_ushort_put_signal_nbi
#define shmem_ushort_put_signal_nbi pshmem_ushort_put_signal_nbi
#pragma weak shmem_uint_put_signal_nbi = pshmem_uint_put_signal_nbi
#define shmem_uint_put_signal_nbi pshmem_uint_put_signal_nbi
#pragma weak shmem_ulong_put_signal_nbi = pshmem_ulong_put_signal_nbi
#define shmem_ulong_put_signal_nbi pshmem_ulong_put_signal_nbi
#pragma weak shmem_ulonglong_put_signal_nbi = pshmem_ulonglong_put_signal_nbi
#define shmem_ulonglong_put_signal_nbi pshmem_ulonglong_put_signal_nbi
#pragma weak shmem_int8_put_signal_nbi = pshmem_int8_put_signal_nbi
#define shmem_int8_put_signal_nbi pshmem_int8_put_signal_nbi
#pragma weak shmem_int16_put_signal_nbi = pshmem_int16_put_signal_nbi
#define shmem_int16_put_signal_nbi pshmem_int16_put_signal_nbi
#pragma weak shmem_int32_put_signal_nbi = pshmem_int32_put_signal_nbi
#define shmem_int32_put_signal_nbi pshmem_int32_put_signal_nbi
#pragma weak shmem_int64_put_signal_nbi = pshmem_int64_put_signal_nbi
#define shmem_int64_put_signal_nbi pshmem_int64_put_signal_nbi
#pragma weak shmem_uint8_put_signal_nbi = pshmem_uint8_put_signal_nbi
#define shmem_uint8_put_signal_nbi pshmem_uint8_put_signal_nbi
#pragma weak shmem_uint16_put_signal_nbi = pshmem_uint16_put_signal_nbi
#define shmem_uint16_put_signal_nbi pshmem_uint16_put_signal_nbi
#pragma weak shmem_uint32_put_signal_nbi = pshmem_uint32_put_signal_nbi
#define shmem_uint32_put_signal_nbi pshmem_uint32_put_signal_nbi
#pragma weak shmem_uint64_put_signal_nbi = pshmem_uint64_put_signal_nbi
#define shmem_uint64_put_signal_nbi pshmem_uint64_put_signal_nbi
#pragma weak shmem_size_put_signal_nbi = pshmem_size_put_signal_nbi
#define shmem_size_put_signal_nbi pshmem_size_put_signal_nbi
#pragma weak shmem_ptrdiff_put_signal_nbi = pshmem_ptrdiff_put_signal_nbi
#define shmem_ptrdiff_put_signal_nbi pshmem_ptrdiff_put_signal_nbi
#endif /* ENABLE_PSHMEM */

#define API_PUT_SIGNAL_NBI_TYPE_HELPER(_type, _typename)                       \
  API_DECL_TYPED_PUT_SIGNAL_NBI(_typename, _type)

SHMEM_STANDARD_RMA_TYPE_TABLE(API_PUT_SIGNAL_NBI_TYPE_HELPER)
#undef API_PUT_SIGNAL_NBI_TYPE_HELPER

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_put8_signal_nbi = pshmem_ctx_put8_signal_nbi
#define shmem_ctx_put8_signal_nbi pshmem_ctx_put8_signal_nbi
#pragma weak shmem_ctx_put16_signal_nbi = pshmem_ctx_put16_signal_nbi
#define shmem_ctx_put16_signal_nbi pshmem_ctx_put16_signal_nbi
#pragma weak shmem_ctx_put32_signal_nbi = pshmem_ctx_put32_signal_nbi
#define shmem_ctx_put32_signal_nbi pshmem_ctx_put32_signal_nbi
#pragma weak shmem_ctx_put64_signal_nbi = pshmem_ctx_put64_signal_nbi
#define shmem_ctx_put64_signal_nbi pshmem_ctx_put64_signal_nbi
#pragma weak shmem_ctx_put128_signal_nbi = pshmem_ctx_put128_signal_nbi
#define shmem_ctx_put128_signal_nbi pshmem_ctx_put128_signal_nbi

#pragma weak shmem_put8_signal_nbi = pshmem_put8_signal_nbi
#define shmem_put8_signal_nbi pshmem_put8_signal_nbi
#pragma weak shmem_put16_signal_nbi = pshmem_put16_signal_nbi
#define shmem_put16_signal_nbi pshmem_put16_signal_nbi
#pragma weak shmem_put32_signal_nbi = pshmem_put32_signal_nbi
#define shmem_put32_signal_nbi pshmem_put32_signal_nbi
#pragma weak shmem_put64_signal_nbi = pshmem_put64_signal_nbi
#define shmem_put64_signal_nbi pshmem_put64_signal_nbi
#pragma weak shmem_put128_signal_nbi = pshmem_put128_signal_nbi
#define shmem_put128_signal_nbi pshmem_put128_signal_nbi
#endif /* ENABLE_PSHMEM */

SHMEM_CTX_DECL_SIZED_PUT_SIGNAL_NBI(8)
SHMEM_CTX_DECL_SIZED_PUT_SIGNAL_NBI(16)
SHMEM_CTX_DECL_SIZED_PUT_SIGNAL_NBI(32)
SHMEM_CTX_DECL_SIZED_PUT_SIGNAL_NBI(64)
SHMEM_CTX_DECL_SIZED_PUT_SIGNAL_NBI(128)

API_DECL_SIZED_PUT_SIGNAL_NBI(8)
API_DECL_SIZED_PUT_SIGNAL_NBI(16)
API_DECL_SIZED_PUT_SIGNAL_NBI(32)
API_DECL_SIZED_PUT_SIGNAL_NBI(64)
API_DECL_SIZED_PUT_SIGNAL_NBI(128)

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_putmem_signal_nbi = pshmem_ctx_putmem_signal_nbi
#define shmem_ctx_putmem_signal_nbi pshmem_ctx_putmem_signal_nbi
#endif /* ENABLE_PSHMEM */

SHMEM_CTX_DECL_PUTMEM_SIGNAL_NBI()
API_DECL_PUTMEM_SIGNAL_NBI()

#ifdef ENABLE_PSHMEM
#pragma weak shmem_signal_fetch = pshmem_signal_fetch
#define shmem_signal_fetch pshmem_signal_fetch
#endif /* ENABLE_PSHMEM */



/**
 * @brief Atomically fetch the current signal value.
 *
 * This routine atomically retrieves the value stored at the signaling address.
 * It uses the default SHMEM context and performs the fetch operation with mutex
 * protection.
 *
 * @param sig_addr Pointer to the signal variable.
 *
 * @return The current value of the signal.
 */
uint64_t shmem_signal_fetch(const uint64_t *sig_addr) {
  uint64_t v;
  SHMEMU_CHECK_SYMMETRIC(sig_addr, 1);                                           \
  SHMEMT_MUTEX_NOPROTECT(
      shmemc_ctx_fetch(SHMEM_CTX_DEFAULT, (uint64_t *)sig_addr,
                       sizeof(*sig_addr), shmemc_my_pe(), &v));
  return v;
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_signal_set = pshmem_ctx_signal_set
#define shmem_ctx_signal_set pshmem_ctx_signal_set
#pragma weak shmem_signal_set = pshmem_signal_set
#define shmem_signal_set pshmem_signal_set
#endif /* ENABLE_PSHMEM */

/**
 * @brief Atomically sets the given signal value.
 *
 * This routine atomically sets the value stored at the signaling address.
 *
 * @param ctx context handle of the operation
 * @param sig_addr Pointer to the signal variable.
 * @param signal value to be set for signal
 * @param pe processing element on which to set signal
 */
void shmem_ctx_signal_set(shmem_ctx_t ctx, uint64_t *sig_addr,
      uint64_t signal, int pe) {
  SHMEMU_CHECK_PE_ARG_RANGE(pe, 4);
  SHMEMU_CHECK_SYMMETRIC(sig_addr, 2);

  SHMEMT_MUTEX_NOPROTECT(
      shmemc_ctx_set(ctx, sig_addr, sizeof(*sig_addr), &signal, sizeof(signal),
                   pe));
}

/**
 * @brief Atomically sets the given signal value.
 *
 * This routine atomically sets the value stored at the signaling address.
 * It uses the default SHMEM context.
 *
 * @param sig_addr Pointer to the signal variable.
 * @param signal value to be set for signal
 * @param pe processing element on which to set signal
 */
void shmem_signal_set(uint64_t *sig_addr, uint64_t signal, 
    int pe) {
  SHMEMU_CHECK_PE_ARG_RANGE(pe, 3);
  SHMEMU_CHECK_SYMMETRIC(sig_addr, 1);

  SHMEMT_MUTEX_NOPROTECT(
      shmemc_ctx_set(SHMEM_CTX_DEFAULT, sig_addr, sizeof(*sig_addr), &signal, sizeof(signal),
                   pe));
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_signal_add = pshmem_ctx_signal_add
#define shmem_ctx_signal_add pshmem_ctx_signal_add
#pragma weak shmem_signal_add = pshmem_signal_add
#define shmem_signal_add pshmem_signal_add
#endif /* ENABLE_PSHMEM */

/**
 * @brief Atomically adds the given signal value.
 *
 * This routine atomically adds to the signal at sig_addr
 *
 * @param ctx context handle of the operation
 * @param sig_addr Pointer to the signal variable.
 * @param signal value to be add for signal
 * @param pe processing element on which to add signal
 */
void shmem_ctx_signal_add(shmem_ctx_t ctx, uint64_t *sig_addr,
      uint64_t signal, int pe) {
  SHMEMU_CHECK_PE_ARG_RANGE(pe, 4);
  SHMEMU_CHECK_SYMMETRIC(sig_addr, 2);

  SHMEMT_MUTEX_NOPROTECT(
    shmemc_ctx_add(ctx, sig_addr, &signal, sizeof(signal), pe));
}

/**
 * @brief Atomically adds the given signal value.
 *
 * This routine atomically adds to the signal at sig_addr
 * This operation happpens on the Default Context
 *
 * @param sig_addr Pointer to the signal variable.
 * @param signal value to be add for signal
 * @param pe processing element on which to add signal
 */
void shmem_signal_add(uint64_t *sig_addr, uint64_t signal, 
    int pe) {
  SHMEMU_CHECK_PE_ARG_RANGE(pe, 3);
  SHMEMU_CHECK_SYMMETRIC(sig_addr, 1);

  SHMEMT_MUTEX_NOPROTECT(
    shmemc_ctx_add(SHMEM_CTX_DEFAULT, sig_addr, &signal, sizeof(signal), pe));
}
