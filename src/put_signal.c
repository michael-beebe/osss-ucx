/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemc.h"
#include "shmem/api.h"

#include "put_signal.h"

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

SHMEM_CTX_TYPED_PUT_SIGNAL(float, float)
SHMEM_CTX_TYPED_PUT_SIGNAL(double, double)
SHMEM_CTX_TYPED_PUT_SIGNAL(longdouble, long double)
SHMEM_CTX_TYPED_PUT_SIGNAL(char, char)
SHMEM_CTX_TYPED_PUT_SIGNAL(schar, signed char)
SHMEM_CTX_TYPED_PUT_SIGNAL(short, short)
SHMEM_CTX_TYPED_PUT_SIGNAL(int, int)
SHMEM_CTX_TYPED_PUT_SIGNAL(long, long)
SHMEM_CTX_TYPED_PUT_SIGNAL(longlong, long long)
SHMEM_CTX_TYPED_PUT_SIGNAL(uchar, unsigned char)
SHMEM_CTX_TYPED_PUT_SIGNAL(ushort, unsigned short)
SHMEM_CTX_TYPED_PUT_SIGNAL(uint, unsigned int)
SHMEM_CTX_TYPED_PUT_SIGNAL(ulong, unsigned long)
SHMEM_CTX_TYPED_PUT_SIGNAL(ulonglong, unsigned long long)
SHMEM_CTX_TYPED_PUT_SIGNAL(int8, int8_t)
SHMEM_CTX_TYPED_PUT_SIGNAL(int16, int16_t)
SHMEM_CTX_TYPED_PUT_SIGNAL(int32, int32_t)
SHMEM_CTX_TYPED_PUT_SIGNAL(int64, int64_t)
SHMEM_CTX_TYPED_PUT_SIGNAL(uint8, uint8_t)
SHMEM_CTX_TYPED_PUT_SIGNAL(uint16, uint16_t)
SHMEM_CTX_TYPED_PUT_SIGNAL(uint32, uint32_t)
SHMEM_CTX_TYPED_PUT_SIGNAL(uint64, uint64_t)
SHMEM_CTX_TYPED_PUT_SIGNAL(size, size_t)
SHMEM_CTX_TYPED_PUT_SIGNAL(ptrdiff, ptrdiff_t)

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

API_DECL_TYPED_PUT_SIGNAL(float, float)
API_DECL_TYPED_PUT_SIGNAL(double, double)
API_DECL_TYPED_PUT_SIGNAL(longdouble, long double)
API_DECL_TYPED_PUT_SIGNAL(schar, signed char)
API_DECL_TYPED_PUT_SIGNAL(char, char)
API_DECL_TYPED_PUT_SIGNAL(short, short)
API_DECL_TYPED_PUT_SIGNAL(int, int)
API_DECL_TYPED_PUT_SIGNAL(long, long)
API_DECL_TYPED_PUT_SIGNAL(longlong, long long)
API_DECL_TYPED_PUT_SIGNAL(uchar, unsigned char)
API_DECL_TYPED_PUT_SIGNAL(ushort, unsigned short)
API_DECL_TYPED_PUT_SIGNAL(uint, unsigned int)
API_DECL_TYPED_PUT_SIGNAL(ulong, unsigned long)
API_DECL_TYPED_PUT_SIGNAL(ulonglong, unsigned long long)
API_DECL_TYPED_PUT_SIGNAL(int8, int8_t)
API_DECL_TYPED_PUT_SIGNAL(int16, int16_t)
API_DECL_TYPED_PUT_SIGNAL(int32, int32_t)
API_DECL_TYPED_PUT_SIGNAL(int64, int64_t)
API_DECL_TYPED_PUT_SIGNAL(uint8, uint8_t)
API_DECL_TYPED_PUT_SIGNAL(uint16, uint16_t)
API_DECL_TYPED_PUT_SIGNAL(uint32, uint32_t)
API_DECL_TYPED_PUT_SIGNAL(uint64, uint64_t)
API_DECL_TYPED_PUT_SIGNAL(size, size_t)
API_DECL_TYPED_PUT_SIGNAL(ptrdiff, ptrdiff_t)
