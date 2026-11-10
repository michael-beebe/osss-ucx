/**
 * @file shmemu.h
 * @brief OpenSHMEM utility functions and macros
 * @author Tony Curtis, Michael Beebe
 *
 * This file contains utility functions and macros used throughout the OpenSHMEM
 * implementation, including debug checks, logging, and reduction operation
 * declarations.
 */

#ifndef _SHMEM_SHEMU_H
#define _SHMEM_SHEMU_H 1

#include "state.h"
#include "shmemc.h"
#include "shmem/defs.h"
#include "threading.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <stdarg.h>
#include <string.h>

/* ------------------------------------------------------------------ */

#include <values.h>

/**
 * @brief Convert number of bits to number of bytes
 * @param _n Number of bits
 * @return Number of bytes
 */
#define BITS2BYTES(_n) ((_n) / CHARBITS)

/**
 * @brief Branch prediction optimization macros
 *
 * These macros help the compiler optimize branch prediction by indicating
 * which branches are likely/unlikely to be taken.
 */
#ifdef HAVE___BUILTIN_EXPECT
#define shmemu_likely(_expr) __builtin_expect(!!(_expr), 1)
#define shmemu_unlikely(_expr) __builtin_expect(!!(_expr), 0)
#else /* HAVE___BUILTIN_EXPECT */
#define shmemu_likely(_expr) (_expr)
#define shmemu_unlikely(_expr) (_expr)
#endif /* HAVE___BUILTIN_EXPECT */

/**
 * @brief Safe string handling macros
 *
 * Use strlcpy/strlcat if available, otherwise fallback to strncpy/strncat
 */
#ifdef HAVE_STRLCPY
#define STRNCPY_SAFE (void)strlcpy
#else
#define STRNCPY_SAFE (void)strncpy
#endif /* HAVE_STRLCPY */

#ifdef HAVE_STRLCAT
#define STRNCAT_SAFE (void)strlcat
#else
#define STRNCAT_SAFE (void)strncat
#endif /* HAVE_STRLCAT */

/**
 * @brief Communication progress functions
 */
void shmemu_progress_init(void);
void shmemu_progress_finalize(void);
void shmemu_progress_set_delay(long newdelay);

/**
 * @brief Rotate/spread PE communications
 * @param pe Processing element number
 * @return Rotated PE number
 */
inline static int shmemu_shift(int pe) {
  return (pe + proc.li.rank) % proc.li.nranks;
}

/**
 * @brief Check if PE number is valid
 * @param pe Processing element number to check
 * @return Non-zero if valid, 0 otherwise
 */
inline static int shmemu_valid_pe_number(int pe) {
  return (proc.li.nranks > pe) && (pe >= 0);
}

/**
 * @brief Initialize SHMEM utilities
 */
void shmemu_init(void);

/**
 * @brief Finalize SHMEM utilities
 */
void shmemu_finalize(void);

/**
 * @brief Get elapsed time in seconds since program started
 * @return Elapsed time in seconds
 */
double shmemu_timer(void);

#ifdef ENABLE_ALIGNED_ADDRESSES
/**
 * @brief Test for address space range mismatch
 */
void shmemu_test_asr_mismatch(void);
#endif /* ENABLE_ALIGNED_ADDRESSES */

/**
 * @brief Number manipulation functions
 */
int shmemu_parse_size(const char *size_str, size_t *bytes_p);
int shmemu_human_number(double bytes, char *buf, size_t buflen);
const char *shmemu_human_option(int v);
int shmemu_parse_csv(char *str, int **out, size_t *nout);

/**
 * @brief Get plural form of word based on count
 * @param n Count
 * @return String with plural form if n != 1
 */
const char *shmemu_plural(size_t n);

/**
 * @brief Message logging constants
 */
#define LOG_ALL "ALL"
#define LOG_INIT "INIT"
#define LOG_FINALIZE "FINALIZE"
#define LOG_MEMORY "MEMORY"
#define LOG_RMA "RMA"
#define LOG_FENCE "FENCE"
#define LOG_QUIET "QUIET"
#define LOG_HEAPS "HEAPS"
#define LOG_CONTEXTS "CONTEXTS"
#define LOG_RANKS "RANKS"
#define LOG_INFO "INFO"
#define LOG_REDUCTIONS "REDUCTIONS"
#define LOG_COLLECTIVES "COLLECTIVES"
#define LOG_DEPRECATE "DEPRECATE"
#define LOG_LOCKS "LOCKS"
#define LOG_ATOMICS "ATOMICS"
#define LOG_UNKNOWN "UNKNOWN"

/**
 * @brief Fatal error and warning functions
 */
void shmemu_fatal(const char *fmt, ...);
void shmemu_warn(const char *fmt, ...);

/**
 * @brief Version information structure
 */
typedef struct shmemu_version {
  int major; /**< Major version number */
  int minor; /**< Minor version number */
} shmemu_version_t;

#ifdef ENABLE_LOGGING

typedef const char *shmemu_log_t;

typedef shmemu_log_t *shmemu_log_table_t;

/**
 * @brief Log a message
 * @param evt Event type
 * @param fmt Format string
 * @param ... Variable arguments
 */
void shmemu_logger(shmemu_log_t evt, const char *fmt, ...);

/**
 * @brief Log deprecation warning
 * @param fn Function name
 * @param vp Version information
 */
void shmemu_deprecate(const char *fn, const shmemu_version_t *vp);

#define logger(...) shmemu_logger(__VA_ARGS__)

#define deprecate(...) shmemu_deprecate(__VA_ARGS__)

#else /* ENABLE_LOGGING */

#define logger(...)

#define deprecate(...)

#endif /* ENABLE_LOGGING */

/**
 * @brief Thread level translation functions
 */
const char *shmemu_thread_name(int tl);
int shmemu_thread_level(const char *tn);

/**
 * @brief Mark arguments as unused to prevent warnings
 */
#define NO_WARN_UNUSED(_obj) (void)(_obj)

#ifdef ENABLE_DEBUG

/**
 * @brief Assertion check macro
 * @param _cond Condition to check
 * @param ... Arguments passed to shmemu_fatal if condition fails
 */
#define shmemu_assert(_cond, ...)                                              \
  do {                                                                         \
    if (!(_cond)) {                                                            \
      shmemu_fatal(__VA_ARGS__);                                               \
    }                                                                          \
  } while (0)

/**
 * @brief Sanity check macros for debugging
 */
#define SHMEMU_CHECK_PE_ARG_RANGE(_pe, _argpos)                                \
  do {                                                                         \
    const int top_pe = proc.li.nranks - 1;                                     \
                                                                               \
    if (shmemu_unlikely((_pe < 0) || (_pe > top_pe))) {                        \
      shmemu_fatal("In %s(), PE argument #%d is %d: "                          \
                   "outside allocated range [%d, %d]",                         \
                   __func__, _argpos, _pe, 0, top_pe);                         \
      /* NOT REACHED */                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_SYMMETRIC(_addr, _argpos)                                 \
  do {                                                                         \
    if (shmemu_unlikely(!shmemc_addr_accessible(_addr, proc.li.rank))) {       \
      shmemu_fatal("In %s(), address %p in argument #%d "                      \
                   "is not symmetric",                                         \
                   __func__, _addr, _argpos);                                  \
      /* NOT REACHED */                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_INIT()                                                    \
  do {                                                                         \
    if (shmemu_unlikely(__atomic_load_n(&proc.refcount, __ATOMIC_ACQUIRE) <    \
                        1)) {                                                  \
      shmemu_fatal("In %s(), attempt to use OpenSHMEM library"                 \
                   " before initialization",                                   \
                   __func__);                                                  \
      /* NOT REACHED */                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_NOT_NULL(_ptr, _argpos)                                   \
  do {                                                                         \
    if ((_ptr) == NULL) {                                                      \
      shmemu_fatal("In %s(), parameter #%d \"%s\" cannot be NULL", __func__,   \
                   _argpos, #_ptr);                                            \
      /* NOT REACHED */                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_SAME_THREAD(_ctx)                                         \
  do {                                                                         \
    if (shmemu_likely(ctx != SHMEM_CTX_INVALID)) {                             \
      shmemc_context_h ch = (shmemc_context_h)(_ctx);                          \
                                                                               \
      if (ch->attr.privat) {                                                   \
        const threadwrap_thread_t me = threadwrap_thread_id();                 \
        const threadwrap_thread_t cr = ch->creator_thread;                     \
        const unsigned long ulme = (unsigned long)me;                          \
        const unsigned long ulcr = (unsigned long)cr;                          \
                                                                               \
        if (!threadwrap_thread_equal(cr, me)) {                                \
          shmemu_fatal("In %s(), invoking thread #%lu"                         \
                       " not owner thread #%lu"                                \
                       "in private context #%lu",                              \
                       __func__, ulme, ulcr, ch->id);                          \
          /* NOT REACHED */                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_HEAP_INDEX(_idx)                                          \
  do {                                                                         \
    const int top_heap = proc.heaps.nheaps - 1;                                \
                                                                               \
    if (shmemu_unlikely(((_idx) < 0) || ((_idx) > top_heap))) {                \
      shmemu_fatal("In %s(), heap index #%d"                                   \
                   "is outside allocated range [%d, %d]",                      \
                   __func__, _idx, 0, top_heap);                               \
      /* NOT REACHED */                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_ALLOC(_addr, _bytes)                                      \
  do {                                                                         \
    if ((_addr) == NULL) {                                                     \
      char buf_b[16];                                                          \
      char buf_h[16];                                                          \
                                                                               \
      shmemu_human_number(proc.heaps.heapsize[0], buf_h, 15);                  \
      shmemu_human_number(_bytes, buf_b, 15);                                  \
                                                                               \
      shmemu_warn("allocating %s would overrun "                               \
                  "heap size of %s",                                           \
                  buf_b, buf_h);                                               \
      shmemu_warn("try increasing value of "                                   \
                  "environment variable "                                      \
                  "SHMEM_SYMMETRIC_SIZE");                                     \
    }                                                                          \
  } while (0)

/* Additional checks for SHMEM collective routines */
#define SHMEMU_CHECK_TEAM_VALID(_team)                                         \
  do {                                                                         \
    if ((_team) == SHMEM_TEAM_INVALID) {                                       \
      shmemu_fatal("In %s(), team argument is invalid", __func__);             \
      /* NOT REACHED */                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_BUFFER_OVERLAP(_dest, _source, _dest_size, _source_size)  \
  do {                                                                         \
    /* Only check for actual overlap if the base pointers are different */     \
    if ((void *)(_dest) != (void *)(_source)) {                                \
      if ((char *)(_dest) + (_dest_size) > (char *)(_source) &&                \
          (char *)(_source) + (_source_size) > (char *)(_dest)) {              \
        shmemu_fatal(                                                          \
            "In %s(), source buffer (%p) and destination buffer (%p) "         \
            "have an illegal overlap",                                         \
            __func__, (_source), (_dest));                                     \
        /* NOT REACHED */                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

/* check that _a is greater than or equal to _b */
#define SHMEMU_CHECK_GREATER_EQUAL(_a, _b)                                     \
  do {                                                                         \
    if (_a < _b) {                                                             \
      shmemu_fatal("In %s(), argument %d is less than %d, but is requred to "  \
                   "be greater "                                               \
                   "or equal.",                                                \
                   __func__, (_a), (_b));                                      \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_NULL(_ptr, _name)                                         \
  do {                                                                         \
    if ((_ptr) == NULL) {                                                      \
      shmemu_fatal("In %s(), %s cannot be NULL", __func__, (_name));           \
      /* NOT REACHED */                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_POSITIVE(_val, _name)                                     \
  do {                                                                         \
    if ((_val) <= 0) {                                                         \
      shmemu_fatal("In %s(), %s must be positive (got %d)", __func__, (_name), \
                   (_val));                                                    \
      /* NOT REACHED */                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_NON_NEGATIVE(_val, _name)                                 \
  do {                                                                         \
    if ((_val) < 0) {                                                          \
      shmemu_fatal("In %s(), %s must be non-negative (got %d)", __func__,      \
                   (_name), (_val));                                           \
      /* NOT REACHED */                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_ACTIVE_SET_RANGE(_pe_start, _log_pe_stride, _pe_size)     \
  do {                                                                         \
    const int _local_stride = 1 << (_log_pe_stride);                           \
    const int _local_max_pe = (_pe_start) + ((_pe_size) - 1) * _local_stride;  \
    const int _local_n_pes = shmem_n_pes(); /* Cache n_pes call */             \
    if (shmemu_unlikely(_local_max_pe >= _local_n_pes)) {                      \
      shmemu_fatal(                                                            \
          "In %s(), active set PE range ending at PE %d (size %d, start %d, "  \
          "stride 2^%d) exceeds number of PEs (%d)",                           \
          __func__, _local_max_pe, (_pe_size), (_pe_start), (_log_pe_stride),  \
          _local_n_pes);                                                       \
      /* NOT REACHED */                                                        \
    }                                                                          \
  } while (0)

#define SHMEMU_CHECK_TEAM_STRIDE(_stride, _func_name)                          \
  do {                                                                         \
    if (shmemu_unlikely((_stride) <= 0)) {                                     \
      shmemu_warn("In %s(), invalid team stride (%d) <= 0.", _func_name,       \
                  (_stride));                                                  \
    } else if (shmemu_unlikely(((_stride) & ((_stride) - 1)) !=                \
                               0)) { /* Check pow2 */                          \
      shmemu_warn("In %s(), team stride (%d) is not a positive power of 2.",   \
                  _func_name, (_stride));                                      \
    }                                                                          \
  } while (0)

#else /* ! ENABLE_DEBUG */

/*
 * prevent unused-variable warnings
 */
#define shmemu_assert(_cond, ...) NO_WARN_UNUSED(_cond)

#define SHMEMU_CHECK_PE_ARG_RANGE(_pe, _argpos)
#define SHMEMU_CHECK_SYMMETRIC(_addr, _argpos)
#define SHMEMU_CHECK_INIT()
#define SHMEMU_CHECK_NOT_NULL(_ptr, _argpos)
#define SHMEMU_CHECK_SAME_THREAD(_ctx)
#define SHMEMU_CHECK_HEAP_INDEX(_idx)
#define SHMEMU_CHECK_ALLOC(_addr, _bytes)

/* Empty versions of additional checks */
#define SHMEMU_CHECK_TEAM_VALID(_team)
#define SHMEMU_CHECK_BUFFER_OVERLAP(_dest, _source, _dest_size, _source_size)
#define SHMEMU_CHECK_GREATER_EQUAL(_a, _b)
#define SHMEMU_CHECK_NULL(_ptr, _name)
#define SHMEMU_CHECK_POSITIVE(_val, _name)
#define SHMEMU_CHECK_NON_NEGATIVE(_val, _name)
#define SHMEMU_CHECK_ACTIVE_SET_RANGE(_pe_start, _log_pe_stride, _pe_size)
#define SHMEMU_CHECK_TEAM_STRIDE(_stride, _func_name)

#endif /* ENABLE_DEBUG */

#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
#include <complex.h>

/**
 * @brief Declare math reduction functions for a type
 * @param _name Type name suffix
 * @param _type Actual C type
 */
#define SHMEMU_DECL_MATH_FUNC(_name, _type)                                    \
  _type shmemu_sum_##_name##_func(_type _a, _type _b);                         \
  _type shmemu_prod_##_name##_func(_type _a, _type _b);

SHMEMU_DECL_MATH_FUNC(float, float)
SHMEMU_DECL_MATH_FUNC(double, double)
SHMEMU_DECL_MATH_FUNC(short, short)
SHMEMU_DECL_MATH_FUNC(int, int)
SHMEMU_DECL_MATH_FUNC(long, long)
SHMEMU_DECL_MATH_FUNC(longlong, long long)
SHMEMU_DECL_MATH_FUNC(longdouble, long double)
SHMEMU_DECL_MATH_FUNC(uint, unsigned int)
SHMEMU_DECL_MATH_FUNC(ulong, unsigned long)
SHMEMU_DECL_MATH_FUNC(ulonglong, unsigned long long)
SHMEMU_DECL_MATH_FUNC(int32, int32_t)
SHMEMU_DECL_MATH_FUNC(int64, int64_t)
SHMEMU_DECL_MATH_FUNC(uint32, uint32_t)
SHMEMU_DECL_MATH_FUNC(uint64, uint64_t)

/**
 * @brief Declare bitwise logical operation functions for a type
 * @param _name Type name suffix
 * @param _type Actual C type
 */
#define SHMEMU_DECL_LOGIC_FUNC(_name, _type)                                   \
  _type shmemu_and_##_name##_func(_type _a, _type _b);                         \
  _type shmemu_or_##_name##_func(_type _a, _type _b);                          \
  _type shmemu_xor_##_name##_func(_type _a, _type _b);

SHMEMU_DECL_LOGIC_FUNC(short, short)
SHMEMU_DECL_LOGIC_FUNC(int, int)
SHMEMU_DECL_LOGIC_FUNC(long, long)
SHMEMU_DECL_LOGIC_FUNC(longlong, long long)
SHMEMU_DECL_LOGIC_FUNC(uint, unsigned int)
SHMEMU_DECL_LOGIC_FUNC(ulong, unsigned long)
SHMEMU_DECL_LOGIC_FUNC(ulonglong, unsigned long long)
SHMEMU_DECL_LOGIC_FUNC(int32, int32_t)
SHMEMU_DECL_LOGIC_FUNC(int64, int64_t)
SHMEMU_DECL_LOGIC_FUNC(uint32, uint32_t)
SHMEMU_DECL_LOGIC_FUNC(uint64, uint64_t)

/**
 * @brief Declare min/max reduction functions for a type
 * @param _name Type name suffix
 * @param _type Actual C type
 */
#define SHMEMU_DECL_MINIMAX_FUNC(_name, _type)                                 \
  _type shmemu_min_##_name##_func(_type _a, _type _b);                         \
  _type shmemu_max_##_name##_func(_type _a, _type _b);

SHMEMU_DECL_MINIMAX_FUNC(short, short)
SHMEMU_DECL_MINIMAX_FUNC(int, int)
SHMEMU_DECL_MINIMAX_FUNC(long, long)
SHMEMU_DECL_MINIMAX_FUNC(longlong, long long)
SHMEMU_DECL_MINIMAX_FUNC(float, float)
SHMEMU_DECL_MINIMAX_FUNC(double, double)
SHMEMU_DECL_MINIMAX_FUNC(longdouble, long double)
SHMEMU_DECL_MINIMAX_FUNC(uint, unsigned int)
SHMEMU_DECL_MINIMAX_FUNC(ulong, unsigned long)
SHMEMU_DECL_MINIMAX_FUNC(ulonglong, unsigned long long)
SHMEMU_DECL_MINIMAX_FUNC(int32, int32_t)
SHMEMU_DECL_MINIMAX_FUNC(int64, int64_t)
SHMEMU_DECL_MINIMAX_FUNC(uint32, uint32_t)
SHMEMU_DECL_MINIMAX_FUNC(uint64, uint64_t)

#endif /* ! _SHMEM_SHEMU_H */
