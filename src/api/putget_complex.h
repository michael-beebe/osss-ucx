/* For license: see LICENSE file at top-level */

#ifndef _SHMEM_PUTGET_COMPLEX_H
#define _SHMEM_PUTGET_COMPLEX_H 1

/*
 * Macro to declare function prototypes for context-based put routines 
 * for a given type '_type'. These routines allow putting (transferring) 
 * 'nelems' elements from 'src' to 'dest' on a specified PE, either 
 * with blocking, non-blocking, or strided interfaces.
 */
#define DECL_TYPED_CTX_PUT_PROTO(_name, _type)                      \
    void shmem_ctx_##_name##_put(shmem_ctx_t ctx,                   \
                                 _type *dest, const _type *src,     \
                                 size_t nelems, int pe);            \
    void shmem_ctx_##_name##_put_nbi(shmem_ctx_t ctx,               \
                                     _type *dest,                   \
                                     const _type *src,              \
                                     size_t nelems, int pe);        \
    void shmem_ctx_##_name##_iput(shmem_ctx_t ctx,                  \
                                  _type *dest,                      \
                                  const _type *src,                 \
                                  ptrdiff_t tst, ptrdiff_t sst,     \
                                  size_t nelems, int pe);

/* Declare context-based put routines for complex float and complex double types */
DECL_TYPED_CTX_PUT_PROTO(complexf, COMPLEXIFY(float))
DECL_TYPED_CTX_PUT_PROTO(complexd, COMPLEXIFY(double))

/*
 * Macro to declare function prototypes for context-based get routines 
 * for a given type '_type'. These routines allow getting (retrieving) 
 * 'nelems' elements from 'src' to 'dest' on a specified PE, either 
 * with blocking, non-blocking, or strided interfaces.
 */
#define DECL_TYPED_CTX_GET_PROTO(_name, _type)                      \
    void shmem_ctx_##_name##_get(shmem_ctx_t ctx,                   \
                                 _type *dest, const _type *src,     \
                                 size_t nelems, int pe);            \
    void shmem_ctx_##_name##_get_nbi(shmem_ctx_t ctx,               \
                                     _type *dest,                   \
                                     const _type *src,              \
                                     size_t nelems, int pe);        \
    void shmem_ctx_##_name##_iget(shmem_ctx_t ctx,                  \
                                  _type *dest,                      \
                                  const _type *src,                 \
                                  ptrdiff_t tst, ptrdiff_t sst,     \
                                  size_t nelems, int pe);

/* Declare context-based get routines for complex float and complex double types */
DECL_TYPED_CTX_GET_PROTO(complexf, COMPLEXIFY(float))
DECL_TYPED_CTX_GET_PROTO(complexd, COMPLEXIFY(double))

/*
 * Macro to declare function prototypes for context-based put single element 
 * (P) routines for a given type '_type'. These routines allow putting a single 
 * value 'val' to a specific memory address 'addr' on the specified PE.
 */
#define DECL_TYPED_CTX_P_PROTO(_name, _type)                    \
    void shmem_ctx_##_name##_p(shmem_ctx_t ctx,                 \
                               _type *addr, _type val, int pe);

/* Declare context-based put single element (P) routines for complex float and complex double types */
DECL_TYPED_CTX_P_PROTO(complexf, COMPLEXIFY(float))
DECL_TYPED_CTX_P_PROTO(complexd, COMPLEXIFY(double))

/*
 * Macro to declare function prototypes for context-based get single element 
 * (G) routines for a given type '_type'. These routines allow getting a 
 * single value from a specific memory address 'addr' on the specified PE.
 */
#define DECL_TYPED_CTX_G_PROTO(_name, _type)                \
    _type shmem_ctx_##_name##_g(shmem_ctx_t ctx,            \
                                const _type *addr, int pe);

/* Declare context-based get single element (G) routines for complex float and complex double types */
DECL_TYPED_CTX_G_PROTO(complexf, COMPLEXIFY(float))
DECL_TYPED_CTX_G_PROTO(complexd, COMPLEXIFY(double))

#endif  /* ! _SHMEM_PUTGET_COMPLEX_H */
