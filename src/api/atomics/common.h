/* For license: see LICENSE file at top-level */

#ifndef SHMEM_AMO_COMMON_H
#define SHMEM_AMO_COMMON_H 1

#include <bits/wordsize.h>
#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>

/* ------------------------------------------------------------------------ */
/*
 * blocking
 * Defines atomic operations that block until the operation is completed.
 */
/* ------------------------------------------------------------------------ */

/*
 * API_DEF_CONST_AMO1:
 * Defines a blocking atomic operation for const target values. 
 * The operation is context-free (uses SHMEM_CTX_DEFAULT).
 * 
 * _op    - Operation (e.g., fetch)
 * _name  - Data type name (e.g., int, float)
 * _type  - C data type (e.g., int, float)
 */
#define API_DEF_CONST_AMO1(_op, _name, _type)                           \
    _type shmem_##_name##_atomic_##_op(const _type *target, int pe)     \
    {                                                                   \
        return shmem_ctx_##_name##_atomic_##_op(SHMEM_CTX_DEFAULT,      \
                                                target, pe);            \
    }

/*
 * API_DEF_AMO1:
 * Defines a blocking atomic operation for mutable target values.
 * The operation is context-free (uses SHMEM_CTX_DEFAULT).
 * 
 * _op    - Operation (e.g., fetch)
 * _name  - Data type name (e.g., int, float)
 * _type  - C data type (e.g., int, float)
 */
#define API_DEF_AMO1(_op, _name, _type)                                 \
    _type shmem_##_name##_atomic_##_op(_type *target, int pe)           \
    {                                                                   \
        return shmem_ctx_##_name##_atomic_##_op(SHMEM_CTX_DEFAULT,      \
                                                target, pe);            \
    }

/*
 * API_DEF_AMO2:
 * Defines a blocking atomic operation that modifies the target with a value 
 * and returns the result. The operation is context-free (uses SHMEM_CTX_DEFAULT).
 * 
 * _op    - Operation (e.g., add)
 * _name  - Data type name (e.g., int, float)
 * _type  - C data type (e.g., int, float)
 */
#define API_DEF_AMO2(_op, _name, _type)                                 \
    _type shmem_##_name##_atomic_##_op(_type *target, _type value,      \
                                       int pe)                          \
    {                                                                   \
        return shmem_ctx_##_name##_atomic_##_op(SHMEM_CTX_DEFAULT,      \
                                                target, value, pe);     \
    }

/*
 * API_DEF_AMO3:
 * Defines a blocking compare-and-swap atomic operation that modifies the 
 * target if a condition is met, and returns the result. 
 * The operation is context-free (uses SHMEM_CTX_DEFAULT).
 * 
 * _op    - Operation (e.g., compare-and-swap)
 * _name  - Data type name (e.g., int, float)
 * _type  - C data type (e.g., int, float)
 */
#define API_DEF_AMO3(_op, _name, _type)                                 \
    _type shmem_##_name##_atomic_##_op(_type *target,                   \
                                       _type cond, _type value,         \
                                       int pe)                          \
    {                                                                   \
        return shmem_ctx_##_name##_atomic_##_op(SHMEM_CTX_DEFAULT,      \
                                                target, cond, value, pe); \
    }

/*
 * API_DEF_VOID_AMO1:
 * Defines a blocking atomic operation that modifies the target but does 
 * not return a result. The operation is context-free (uses SHMEM_CTX_DEFAULT).
 * 
 * _op    - Operation (e.g., increment)
 * _name  - Data type name (e.g., int, float)
 * _type  - C data type (e.g., int, float)
 */
#define API_DEF_VOID_AMO1(_op, _name, _type)                            \
    void shmem_##_name##_atomic_##_op(_type *target, int pe)            \
    {                                                                   \
        shmem_ctx_##_name##_atomic_##_op(SHMEM_CTX_DEFAULT,             \
                                         target, pe);                   \
    }

/*
 * API_DEF_VOID_AMO2:
 * Defines a blocking atomic operation that modifies the target with a value 
 * but does not return a result. The operation is context-free (uses SHMEM_CTX_DEFAULT).
 * 
 * _op    - Operation (e.g., set)
 * _name  - Data type name (e.g., int, float)
 * _type  - C data type (e.g., int, float)
 */
#define API_DEF_VOID_AMO2(_op, _name, _type)                            \
    void shmem_##_name##_atomic_##_op(_type *target, _type value,       \
                                      int pe)                           \
    {                                                                   \
        shmem_ctx_##_name##_atomic_##_op(SHMEM_CTX_DEFAULT,             \
                                         target, value, pe);            \
    }

/*
 * SHMEM_CTX_TYPE_BITWISE:
 * Defines a blocking bitwise atomic operation that modifies the target with a value.
 * The operation uses an explicit SHMEM context.
 * 
 * _opname - Bitwise operation (e.g., xor, or)
 * _name   - Data type name (e.g., int, float)
 * _type   - C data type (e.g., int, float)
 */
#define SHMEM_CTX_TYPE_BITWISE(_opname, _name, _type)                   \
    void                                                                \
    shmem_ctx_##_name##_atomic_##_opname(shmem_ctx_t ctx,               \
                                         _type *target,                 \
                                         _type value, int pe)           \
    {                                                                   \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_##_opname(ctx, target,        \
                                                    &value, sizeof(value), \
                                                    pe));               \
    }

/*
 * SHMEM_CTX_TYPE_FETCH_BITWISE:
 * Defines a blocking bitwise atomic fetch operation that returns the result.
 * The operation uses an explicit SHMEM context.
 * 
 * _opname - Bitwise operation (e.g., xor, or)
 * _name   - Data type name (e.g., int, float)
 * _type   - C data type (e.g., int, float)
 */
#define SHMEM_CTX_TYPE_FETCH_BITWISE(_opname, _name, _type)             \
    _type                                                               \
    shmem_ctx_##_name##_atomic_fetch_##_opname(shmem_ctx_t ctx,         \
                                               _type *target,           \
                                               _type value, int pe)     \
    {                                                                   \
        _type v;                                                        \
                                                                        \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_fetch_##_opname(ctx,          \
                                                          target,       \
                                                          &value,       \
                                                          sizeof(value), \
                                                          pe, &v));     \
        return v;                                                       \
    }

/*
 * non-blocking
 * Defines non-blocking atomic operations that allow for immediate returns 
 * and completion checks at a later point.
 */

/*
 * API_DEF_CONST_AMO1_NBI:
 * Defines a non-blocking atomic operation for const target values.
 * The operation is context-free (uses SHMEM_CTX_DEFAULT).
 * 
 * _op    - Operation (e.g., fetch)
 * _name  - Data type name (e.g., int, float)
 * _type  - C data type (e.g., int, float)
 */
#define API_DEF_CONST_AMO1_NBI(_op, _name, _type)                       \
    void shmem_##_name##_atomic_##_op##_nbi(_type *fetch,               \
                                            const _type *target,        \
                                            int pe)                     \
    {                                                                   \
        shmem_ctx_##_name##_atomic_##_op##_nbi(SHMEM_CTX_DEFAULT,       \
                                               fetch,                   \
                                               target, pe);             \
    }

/*
 * API_DEF_AMO1_NBI:
 * Defines a non-blocking atomic operation for mutable target values.
 * The operation is context-free (uses SHMEM_CTX_DEFAULT).
 * 
 * _op    - Operation (e.g., fetch)
 * _name  - Data type name (e.g., int, float)
 * _type  - C data type (e.g., int, float)
 */
#define API_DEF_AMO1_NBI(_op, _name, _type)                             \
    void shmem_##_name##_atomic_##_op##_nbi(_type *fetch,               \
                                            _type *target,              \
                                            int pe)                     \
    {                                                                   \
        shmem_ctx_##_name##_atomic_##_op##_nbi(SHMEM_CTX_DEFAULT,       \
                                               fetch,                   \
                                               target, pe);             \
    }

/*
 * API_DEF_AMO2_NBI:
 * Defines a non-blocking atomic operation that modifies the target with a value.
 * The operation is context-free (uses SHMEM_CTX_DEFAULT).
 * 
 * _op    - Operation (e.g., add)
 * _name  - Data type name (e.g., int, float)
 * _type  - C data type (e.g., int, float)
 */
#define API_DEF_AMO2_NBI(_op, _name, _type)                             \
    void shmem_##_name##_atomic_##_op##_nbi(_type *fetch,               \
                                            _type *target, _type value, \
                                            int pe)                     \
    {                                                                   \
        shmem_ctx_##_name##_atomic_##_op##_nbi(SHMEM_CTX_DEFAULT,       \
                                               fetch,                   \
                                               target, value, pe);      \
    }

/*
 * API_DEF_AMO3_NBI:
 * Defines a non-blocking compare-and-swap atomic operation.
 * The operation is context-free (uses SHMEM_CTX_DEFAULT).
 * 
 * _op    - Operation (e.g., compare-and-swap)
 * _name  - Data type name (e.g., int, float)
 * _type  - C data type (e.g., int, float)
 */
#define API_DEF_AMO3_NBI(_op, _name, _type)                             \
    void shmem_##_name##_atomic_##_op##_nbi(_type *fetch,               \
                                            _type *target,              \
                                            _type cond,                 \
                                            _type value,                \
                                            int pe)                     \
    {                                                                   \
        shmem_ctx_##_name##_atomic_##_op##_nbi(SHMEM_CTX_DEFAULT,       \
                                               fetch,                   \
                                               target,                  \
                                               cond, value, pe);        \
    }

/*
 * SHMEM_CTX_TYPE_FETCH_BITWISE_NBI:
 * Defines a non-blocking bitwise atomic fetch operation.
 * The operation uses an explicit SHMEM context.
 * 
 * _opname - Bitwise operation (e.g., xor, or)
 * _name   - Data type name (e.g., int, float)
 * _type   - C data type (e.g., int, float)
 */
#define SHMEM_CTX_TYPE_FETCH_BITWISE_NBI(_opname, _name, _type)         \
    void                                                                \
    shmem_ctx_##_name##_atomic_fetch_##_opname##_nbi(shmem_ctx_t ctx,   \
                                                     _type *fetch,      \
                                                     _type *target,     \
                                                     _type value,       \
                                                     int pe)            \
    {                                                                   \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_fetch_##_opname(ctx,          \
                                                          target,       \
                                                          &value,       \
                                                          sizeof(value), \
                                                          pe, fetch));  \
    }

#endif /* ! SHMEM_AMO_COMMON_H */
