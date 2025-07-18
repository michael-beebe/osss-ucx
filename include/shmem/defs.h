/**
 * @file defs.h
 * @brief Core OpenSHMEM definitions and constants
 *
 * For license: see LICENSE file at top-level
 */

#ifndef _SHMEM_DEFINES_H
#define _SHMEM_DEFINES_H 1

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>

#include <complex.h>
#define COMPLEXIFY(T) T _Complex

/**
 * @brief Check if compiler supports C11
 */
#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 201112L
#define SHMEM_HAS_C11 1
#else
#define SHMEM_HAS_C11 0
#endif /* __STDC_VERSION__ >= 201112L test */
#else
#define SHMEM_HAS_C11 0
#endif /* __STDC_VERSION__ defined test */

/**
 * @brief Define how to annotate functions as non-returning
 */
#if SHMEM_HAS_C11
#define SHMEM_NORETURN _Noreturn
#else
#define SHMEM_NORETURN
#endif /* SHMEM_HAS_C11 */

/**
 * @brief Define warn_unused_result attribute if supported by compiler
 *
 * not all compilers support this annotation
 */
#if defined(__GNUC__) || defined(__PGIC__) || defined(__INTEL_COMPILER) ||     \
    defined(__OPEN64__) || defined(__OPENUH__)
#define _WUR __attribute__((__warn_unused_result__))
#else
#define _WUR
#endif

/**
 * @brief Indirection for macro expansion.
 */
#define SHMEM_APPLY_INTERNAL(_macro, ...) _macro(__VA_ARGS__)
#define SHMEM_APPLY(_macro, ...) SHMEM_APPLY_INTERNAL(_macro, __VA_ARGS__)

/**
 * @brief Constants for comparison operations
 */
enum shmem_cmp_constants {
  SHMEM_CMP_EQ = 0,
  SHMEM_CMP_NE,
  SHMEM_CMP_GT,
  SHMEM_CMP_LE,
  SHMEM_CMP_LT,
  SHMEM_CMP_GE
};

/**
 * @brief Array sizes for collective operations
 */
#define SHMEM_BCAST_SYNC_SIZE 64
#define SHMEM_BARRIER_SYNC_SIZE 2
#define SHMEM_REDUCE_SYNC_SIZE 128
#define SHMEM_REDUCE_MIN_WRKDATA_SIZE 64
#define SHMEM_COLLECT_SYNC_SIZE 128  /* Needs to be at least 66 for bruck algorithm */
#define SHMEM_ALLTOALL_SYNC_SIZE 64
#define SHMEM_ALLTOALLS_SYNC_SIZE 64
/*
 * max(the above)
 */
#define SHMEM_SYNC_SIZE SHMEM_REDUCE_SYNC_SIZE

/**
 * @brief Initial value for sync variables
 */
#define SHMEM_SYNC_VALUE 0L

/**
 * @brief Thread support levels
 */
enum shmem_thread_levels {
  SHMEM_THREAD_SINGLE = 0,
  SHMEM_THREAD_FUNNELED,
  SHMEM_THREAD_SERIALIZED,
  SHMEM_THREAD_MULTIPLE
};

/**
 * @brief Signal operation types
 */
enum shmem_signal_ops { SHMEM_SIGNAL_SET = 0, SHMEM_SIGNAL_ADD };

/**
 * @brief Helper macro for bit operations
 */
#define SHMEM_BIT_SET(_level) (1 << (_level))

/**
 * @brief Context attributes
 */
enum shmem_ctx_attrs {
  SHMEM_CTX_SERIALIZED = SHMEM_BIT_SET(0),
  SHMEM_CTX_PRIVATE = SHMEM_BIT_SET(1),
  SHMEM_CTX_NOSTORE = SHMEM_BIT_SET(2)
};

/**
 * @brief Context type and default/invalid values
 */
typedef void *shmem_ctx_t;

extern shmem_ctx_t SHMEM_CTX_DEFAULT;

#define SHMEM_CTX_INVALID NULL

/**
 * @brief Team context configuration parameters
 */
enum shmem_team_params { SHMEM_TEAM_NUM_CONTEXTS = SHMEM_BIT_SET(0) };

/**
 * @brief Memory allocation hints
 */
enum shmem_malloc_hints {
  SHMEM_MALLOC_ATOMICS_REMOTE = 0,
  SHMEM_MALLOC_SIGNAL_REMOTE
};

/**
 * @brief Macros for declaring atomic memory operations
 */
#define SHMEM_DECL_CONST_AMO1(_op, _name, _type)                               \
  _type shmem_##_name##_atomic_##_op(const _type *target, int pe) _WUR;        \
  _type shmem_ctx_##_name##_atomic_##_op(shmem_ctx_t ctx, const _type *target, \
                                         int pe) _WUR;

#define SHMEM_DECL_AMO1(_op, _name, _type)                                     \
  _type shmem_##_name##_atomic_##_op(_type *target, int pe) _WUR;              \
  _type shmem_ctx_##_name##_atomic_##_op(shmem_ctx_t ctx, _type *target,       \
                                         int pe) _WUR;

#define SHMEM_DECL_AMO2(_op, _name, _type)                                     \
  _type shmem_##_name##_atomic_##_op(_type *target, _type value, int pe) _WUR; \
  _type shmem_ctx_##_name##_atomic_##_op(shmem_ctx_t ctx, _type *target,       \
                                         _type value, int pe) _WUR;

#define SHMEM_DECL_AMO3(_op, _name, _type)                                     \
  _type shmem_##_name##_atomic_##_op(_type *target, _type cond, type value,    \
                                     int pe) _WUR;                             \
  _type shmem_ctx_##_name##_atomic_##_op(                                      \
      shmem_ctx_t ctx, _type *target, _type cond, _type value, int pe) _WUR;

#define SHMEM_DECL_VOID_AMO1(_op, _name, _type)                                \
  void shmem_##_name##_atomic_##_op(_type *target, int pe);                    \
  void shmem_ctx_##_name##_atomic_##_op(shmem_ctx_t ctx, _type *target, int pe);

#define SHMEM_DECL_VOID_AMO2(_op, _name, _type)                                \
  void shmem_##_name##_atomic_##_op(_type *target, _type value, int pe);       \
  void shmem_ctx_##_name##_atomic_##_op(shmem_ctx_t ctx, _type *target,        \
                                        _type value, int pe);

/**
 * @brief Macros for declaring non-blocking atomic memory operations
 */
#define SHMEM_DECL_AMO1_NBI(_op, _name, _type)                                 \
  void shmem_##_name##_atomic_##_op##_nbi(_type *fetch, _type *target,         \
                                          int pe);                             \
  void shmem_ctx_##_name##_atomic_##_op##_nbi(shmem_ctx_t ctx, _type *fetch,   \
                                              _type *target, int pe);

#define SHMEM_DECL_CONST_AMO1_NBI(_op, _name, _type)                           \
  void shmem_##_name##_atomic_##_op##_nbi(_type *fetch, const _type *target,   \
                                          int pe);                             \
  void shmem_ctx_##_name##_atomic_##_op##_nbi(shmem_ctx_t ctx, _type *fetch,   \
                                              const _type *target, int pe);

#define SHMEM_DECL_AMO2_NBI(_op, _name, _type)                                 \
  void shmem_##_name##_atomic_##_op##_nbi(_type *fetch, _type *target,         \
                                          _type value, int pe);                \
  void shmem_ctx_##_name##_atomic_##_op##_nbi(                                 \
      shmem_ctx_t ctx, _type *fetch, _type *target, _type value, int pe);

#if 0

/*
 * under development
 */

#define SHMEM_DECL_AMO3(_op, _name, _type)                                     \
  _type shmem_##_name##_atomic_##_op(_type *target, _type cond, type value,    \
                                     int pe) _WUR;                             \
  _type shmem_ctx_##_name##_atomic_##_op(                                      \
      shmem_ctx_t ctx, _type *target, _type cond, _type value, int pe) _WUR;

#define SHMEM_DECL_VOID_AMO1(_op, _name, _type)                                \
  void shmem_##_name##_atomic_##_op(_type *target, int pe);                    \
  void shmem_ctx_##_name##_atomic_##_op(shmem_ctx_t ctx, _type *target, int pe);

#define SHMEM_DECL_VOID_AMO2(_op, _name, _type)                                \
  void shmem_##_name##_atomic_##_op(_type *target, _type value, int pe);       \
  void shmem_ctx_##_name##_atomic_##_op(shmem_ctx_t ctx, _type *target,        \
                                        _type value, int pe);
#endif

/**
 * @brief Deprecated OpenSHMEM constants
 */
#define _SHMEM_MAJOR_VERSION SHMEM_MAJOR_VERSION
#define _SHMEM_MINOR_VERSION SHMEM_MINOR_VERSION
#define _SHMEM_MAX_NAME_LEN SHMEM_MAX_NAME_LEN
#define _SHMEM_VENDOR_STRING SHMEM_VENDOR_STRING
#define _SHMEM_BCAST_SYNC_SIZE SHMEM_BCAST_SYNC_SIZE
#define _SHMEM_BARRIER_SYNC_SIZE SHMEM_BARRIER_SYNC_SIZE
#define _SHMEM_REDUCE_SYNC_SIZE SHMEM_REDUCE_SYNC_SIZE
#define _SHMEM_REDUCE_MIN_WRKDATA_SIZE SHMEM_REDUCE_MIN_WRKDATA_SIZE
#define _SHMEM_SYNC_VALUE SHMEM_SYNC_VALUE
#define _SHMEM_COLLECT_SYNC_SIZE SHMEM_COLLECT_SYNC_SIZE
#define _SHMEM_CMP_EQ SHMEM_CMP_EQ
#define _SHMEM_CMP_NE SHMEM_CMP_NE
#define _SHMEM_CMP_GT SHMEM_CMP_GT
#define _SHMEM_CMP_LE SHMEM_CMP_LE
#define _SHMEM_CMP_LT SHMEM_CMP_LT
#define _SHMEM_CMP_GE SHMEM_CMP_GE

#endif /* ! _SHMEM_DEFINES_H */
