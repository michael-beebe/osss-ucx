/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"

#include <bits/wordsize.h>
#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>

/* ------------------------------------------------------------------------ */
/*
 * -- context-free
 * These macros define atomic operations that use the default SHMEM context 
 * (SHMEM_CTX_DEFAULT), which does not require the user to pass in an explicit 
 * context. They are wrappers around their context-based counterparts.
 */
/* ------------------------------------------------------------------------ */

/*
 * API_DEF_CONST_AMO1:
 * Defines an atomic operation for const target values where the operation returns 
 * a result. The target value is not modified, and the operation is context-free 
 * (uses the default context).
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
 * Defines an atomic operation for mutable target values where the operation returns 
 * a result. The target value is allowed to be modified, and the operation is 
 * context-free (uses the default context).
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
 * Defines an atomic operation that takes a target, a value, and returns a result. 
 * The target can be modified with the provided value, and the operation is 
 * context-free (uses the default context).
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
 * Defines an atomic compare-and-swap operation that takes a target, a condition, 
 * and a value, and returns a result. The operation swaps the target with the 
 * value if the condition is met, and it is context-free (uses the default context).
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
 * Defines an atomic operation that modifies a target without returning a result.
 * The target is modified based on the operation, and the operation is context-free 
 * (uses the default context).
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
 * Defines an atomic operation that modifies a target with a value without returning 
 * a result. The target is modified by the value, and the operation is context-free 
 * (uses the default context).
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
