/* For license: see LICENSE file at top-level */

#ifndef _SHMEM_PUTGET_SIGNAL_H
#define _SHMEM_PUTGET_SIGNAL_H 1

#include "shmemu.h"
#include "shmemc.h"
#include "shmem_mutex.h"

/*
 * Macro to define context-specific signal-based put operations
 * for a given data type '_type'. The function will transfer 
 * 'nelems' elements from 'src' to 'dest' on PE 'pe', and update 
 * the signal at 'sig_addr' using the value 'signal' and the 
 * operation 'sig_op'.
 */
#define SHMEM_CTX_TYPED_PUT_SIGNAL(_name, _type)                    \
    void                                                            \
    shmem_ctx_##_name##_put_signal(shmem_ctx_t ctx,                 \
                                   _type *dest, const _type *src,   \
                                   size_t nelems,                   \
                                   uint64_t *sig_addr,              \
                                   uint64_t signal,                 \
                                   int sig_op,                      \
                                   int pe)                          \
    {                                                               \
        const size_t nb = sizeof(_type) * nelems;                   \
                                                                    \
        SHMEMU_CHECK_INIT();                                        \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 8);                           \
        SHMEMU_CHECK_SYMMETRIC(dest, 2);                            \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 5);                        \
                                                                    \
        SHMEMT_MUTEX_PROTECT(shmemc_ctx_put_signal(ctx,             \
                                                   dest, src,       \
                                                   nb,              \
                                                   sig_addr,        \
                                                   signal,          \
                                                   sig_op,          \
                                                   pe));            \
    }

/*
 * Macro to define signal-based put operations for a given 
 * data type '_type' without specifying the context. The 
 * function transfers 'nelems' elements from 'src' to 'dest' 
 * and updates the signal at 'sig_addr' using the value 'signal' 
 * and the operation 'sig_op'. Uses the default context.
 */
#define API_DECL_TYPED_PUT_SIGNAL(_name, _type)                         \
    void                                                                \
    shmem_##_name##_put_signal(_type *dest, const _type *src,           \
                               size_t nelems,                           \
                               uint64_t *sig_addr,                      \
                               uint64_t signal,                         \
                               int sig_op,                              \
                               int pe)                                  \
    {                                                                   \
        const size_t nb = sizeof(_type) * nelems;                       \
                                                                        \
        SHMEMU_CHECK_INIT();                                            \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 7);                               \
        SHMEMU_CHECK_SYMMETRIC(dest, 1);                                \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 4);                            \
                                                                        \
        SHMEMT_MUTEX_PROTECT(shmemc_ctx_put_signal(SHMEM_CTX_DEFAULT,   \
                                                   dest, src,           \
                                                   nb,                  \
                                                   sig_addr,            \
                                                   signal,              \
                                                   sig_op,              \
                                                   pe));                \
    }

/*
 * Macro to declare context-specific signal-based put operations 
 * for a specific size '_size' in bits. Converts the size to 
 * bytes and uses it to transfer 'nelems' elements from 'src' to 'dest'. 
 * Updates the signal using the value 'signal' and the operation 'sig_op'.
 */
#define SHMEM_CTX_DECL_SIZED_PUT_SIGNAL(_size)                          \
    void                                                                \
    shmem_ctx_put##_size##_signal(shmem_ctx_t ctx,                      \
                                  void *dest, const void *src,          \
                                  size_t nelems,                        \
                                  uint64_t *sig_addr,                   \
                                  uint64_t signal,                      \
                                  int sig_op,                           \
                                  int pe)                               \
    {                                                                   \
        const size_t nb = BITS2BYTES(_size) * nelems;                   \
                                                                        \
        SHMEMU_CHECK_INIT();                                            \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 8);                               \
        SHMEMU_CHECK_SYMMETRIC(dest, 2);                                \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 5);                            \
                                                                        \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_put_signal(ctx,               \
                                                     dest, src,         \
                                                     nb,                \
                                                     sig_addr,          \
                                                     signal,            \
                                                     sig_op,            \
                                                     pe));              \
    }

/*
 * Macro to declare API-level signal-based put operations for 
 * a specific size '_size' in bits. Converts the size to bytes 
 * and transfers 'nelems' elements from 'src' to 'dest'. Updates 
 * the signal using the value 'signal' and the operation 'sig_op'.
 * Uses the default context.
 */
#define API_DECL_SIZED_PUT_SIGNAL(_size)                                \
    void                                                                \
    shmem_put##_size##_signal(void *dest, const void *src,              \
                              size_t nelems,                            \
                              uint64_t *sig_addr,                       \
                              uint64_t signal,                          \
                              int sig_op,                               \
                              int pe)                                   \
    {                                                                   \
        const size_t nb = BITS2BYTES(_size) * nelems;                   \
                                                                        \
        SHMEMU_CHECK_INIT();                                            \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 7);                               \
        SHMEMU_CHECK_SYMMETRIC(dest, 1);                                \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 4);                            \
                                                                        \
        SHMEMT_MUTEX_PROTECT(shmemc_ctx_put_signal(SHMEM_CTX_DEFAULT,   \
                                                   dest, src,           \
                                                   nb,                  \
                                                   sig_addr,            \
                                                   signal,              \
                                                   sig_op,              \
                                                   pe));                \
    }

/*
 * Macro to declare a context-specific signal-based put operation 
 * for memory regions without specifying a data type. Transfers 
 * 'nelems' bytes from 'src' to 'dest' and updates the signal at 
 * 'sig_addr' using the value 'signal' and the operation 'sig_op'.
 */
#define SHMEM_CTX_DECL_PUTMEM_SIGNAL()                                  \
    void                                                                \
    shmem_ctx_putmem_signal(shmem_ctx_t ctx,                            \
                            void *dest, const void *src,                \
                            size_t nelems,                              \
                            uint64_t *sig_addr,                         \
                            uint64_t signal,                            \
                            int sig_op,                                 \
                            int pe)                                     \
    {                                                                   \
        SHMEMU_CHECK_INIT();                                            \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 8);                               \
        SHMEMU_CHECK_SYMMETRIC(dest, 2);                                \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 5);                            \
                                                                        \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_put_signal(ctx,               \
                                                     dest, src,         \
                                                     nelems,            \
                                                     sig_addr,          \
                                                     signal,            \
                                                     sig_op,            \
                                                     pe));              \
    }

/*
 * Macro to declare an API-level signal-based put operation for 
 * memory regions without specifying a data type. Transfers 'nelems' 
 * bytes from 'src' to 'dest' and updates the signal using the value 
 * 'signal' and the operation 'sig_op'. Uses the default context.
 */
#define API_DECL_PUTMEM_SIGNAL()                                        \
    void                                                                \
    shmem_putmem_signal(void *dest, const void *src,                    \
                        size_t nelems,                                  \
                        uint64_t *sig_addr,                             \
                        uint64_t signal,                                \
                        int sig_op,                                     \
                        int pe)                                         \
    {                                                                   \
        SHMEMU_CHECK_INIT();                                            \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 7);                               \
        SHMEMU_CHECK_SYMMETRIC(dest, 1);                                \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 4);                            \
                                                                        \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_put_signal(SHMEM_CTX_DEFAULT, \
                                                     dest, src,         \
                                                     nelems,            \
                                                     sig_addr,          \
                                                     signal,            \
                                                     sig_op,            \
                                                     pe));              \
    }

/*
 * Macros for non-blocking signal-based put operations, which
 * work similarly to their blocking counterparts but initiate
 * non-blocking data transfer.
 */

/*
 * Macro to define context-specific non-blocking signal-based put 
 * operations for a given data type '_type'. The function transfers 
 * 'nelems' elements from 'src' to 'dest' and updates the signal at 
 * 'sig_addr' using the value 'signal' and the operation 'sig_op'.
 */
#define SHMEM_CTX_TYPED_PUT_SIGNAL_NBI(_name, _type)                \
    void                                                            \
    shmem_ctx_##_name##_put_signal_nbi(shmem_ctx_t ctx,             \
                                       _type *dest,                 \
                                       const _type *src,            \
                                       size_t nelems,               \
                                       uint64_t *sig_addr,          \
                                       uint64_t signal,             \
                                       int sig_op,                  \
                                       int pe)                      \
    {                                                               \
        const size_t nb = sizeof(_type) * nelems;                   \
                                                                    \
        SHMEMU_CHECK_INIT();                                        \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 8);                           \
        SHMEMU_CHECK_SYMMETRIC(dest, 2);                            \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 5);                        \
                                                                    \
        SHMEMT_MUTEX_PROTECT(shmemc_ctx_put_signal_nbi(ctx,         \
                                                       dest,        \
                                                       src,         \
                                                       nb,          \
                                                       sig_addr,    \
                                                       signal,      \
                                                       sig_op,      \
                                                       pe));        \
    }

/*
 * Macro to define API-level non-blocking signal-based put operations 
 * for a given data type '_type'. The function transfers 'nelems' 
 * elements from 'src' to 'dest' and updates the signal at 'sig_addr' 
 * using the value 'signal' and the operation 'sig_op'. Uses the default context.
 */
#define API_DECL_TYPED_PUT_SIGNAL_NBI(_name, _type)                     \
    void                                                                \
    shmem_##_name##_put_signal_nbi(_type *dest, const _type *src,       \
                                   size_t nelems,                       \
                                   uint64_t *sig_addr,                  \
                                   uint64_t signal,                     \
                                   int sig_op,                          \
                                   int pe)                              \
    {                                                                   \
        const size_t nb = sizeof(_type) * nelems;                       \
                                                                        \
        SHMEMU_CHECK_INIT();                                            \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 7);                               \
        SHMEMU_CHECK_SYMMETRIC(dest, 1);                                \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 4);                            \
                                                                        \
        SHMEMT_MUTEX_PROTECT(shmemc_ctx_put_signal_nbi(SHMEM_CTX_DEFAULT, \
                                                       dest, src,       \
                                                       nb,              \
                                                       sig_addr,        \
                                                       signal,          \
                                                       sig_op,          \
                                                       pe));            \
    }

/*
 * Macro to declare context-specific non-blocking signal-based put 
 * operations for a specific size '_size' in bits. Converts the size 
 * to bytes and transfers 'nelems' elements from 'src' to 'dest'.
 * Updates the signal using the value 'signal' and the operation 'sig_op'.
 */
#define SHMEM_CTX_DECL_SIZED_PUT_SIGNAL_NBI(_size)                  \
    void                                                            \
    shmem_ctx_put##_size##_signal_nbi(shmem_ctx_t ctx,              \
                                      void *dest, const void *src,  \
                                      size_t nelems,                \
                                      uint64_t *sig_addr,           \
                                      uint64_t signal,              \
                                      int sig_op,                   \
                                      int pe)                       \
    {                                                               \
        const size_t nb = BITS2BYTES(_size) * nelems;               \
                                                                    \
        SHMEMU_CHECK_INIT();                                        \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 8);                           \
        SHMEMU_CHECK_SYMMETRIC(dest, 2);                            \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 5);                        \
                                                                    \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_put_signal_nbi(ctx,       \
                                                         dest, src, \
                                                         nb,        \
                                                         sig_addr,  \
                                                         signal,    \
                                                         sig_op,    \
                                                         pe));      \
    }

/*
 * Macro to declare API-level non-blocking signal-based put operations 
 * for a specific size '_size' in bits. Converts the size to bytes 
 * and transfers 'nelems' elements from 'src' to 'dest'. Updates the 
 * signal using the value 'signal' and the operation 'sig_op'. 
 * Uses the default context.
 */
#define API_DECL_SIZED_PUT_SIGNAL_NBI(_size)                            \
    void                                                                \
    shmem_put##_size##_signal_nbi(void *dest, const void *src,          \
                                  size_t nelems,                        \
                                  uint64_t *sig_addr,                   \
                                  uint64_t signal,                      \
                                  int sig_op,                           \
                                  int pe)                               \
    {                                                                   \
        const size_t nb = BITS2BYTES(_size) * nelems;                   \
                                                                        \
        SHMEMU_CHECK_INIT();                                            \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 7);                               \
        SHMEMU_CHECK_SYMMETRIC(dest, 1);                                \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 4);                            \
                                                                        \
        SHMEMT_MUTEX_PROTECT(shmemc_ctx_put_signal_nbi(SHMEM_CTX_DEFAULT, \
                                                       dest, src,       \
                                                       nb,              \
                                                       sig_addr,        \
                                                       signal,          \
                                                       sig_op,          \
                                                       pe));            \
    }

/*
 * Macro to declare context-specific non-blocking signal-based put 
 * operations for memory regions without specifying a data type.
 * Transfers 'nelems' bytes from 'src' to 'dest' and updates the 
 * signal at 'sig_addr' using the value 'signal' and the operation 'sig_op'.
 */
#define SHMEM_CTX_DECL_PUTMEM_SIGNAL_NBI()                          \
    void                                                            \
    shmem_ctx_putmem_signal_nbi(shmem_ctx_t ctx,                    \
                                void *dest, const void *src,        \
                                size_t nelems,                      \
                                uint64_t *sig_addr,                 \
                                uint64_t signal,                    \
                                int sig_op,                         \
                                int pe)                             \
    {                                                               \
        SHMEMU_CHECK_INIT();                                        \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 8);                           \
        SHMEMU_CHECK_SYMMETRIC(dest, 2);                            \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 5);                        \
                                                                    \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_put_signal_nbi(ctx,       \
                                                         dest, src, \
                                                         nelems,    \
                                                         sig_addr,  \
                                                         signal,    \
                                                         sig_op,    \
                                                         pe));      \
    }

/*
 * Macro to declare API-level non-blocking signal-based put operations 
 * for memory regions without specifying a data type. Transfers 'nelems' 
 * bytes from 'src' to 'dest' and updates the signal at 'sig_addr' 
 * using the value 'signal' and the operation 'sig_op'. Uses the default context.
 */
#define API_DECL_PUTMEM_SIGNAL_NBI()                                    \
    void                                                                \
    shmem_putmem_signal_nbi(void *dest, const void *src,                \
                            size_t nelems,                              \
                            uint64_t *sig_addr,                         \
                            uint64_t signal,                            \
                            int sig_op,                                 \
                            int pe)                                     \
    {                                                                   \
        SHMEMU_CHECK_INIT();                                            \
        SHMEMU_CHECK_PE_ARG_RANGE(pe, 7);                               \
        SHMEMU_CHECK_SYMMETRIC(dest, 1);                                \
        SHMEMU_CHECK_SYMMETRIC(sig_addr, 4);                            \
                                                                        \
        SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_put_signal_nbi(SHMEM_CTX_DEFAULT, \
                                                         dest, src,     \
                                                         nelems,        \
                                                         sig_addr,      \
                                                         signal,        \
                                                         sig_op,        \
                                                         pe));          \
    }

#endif  /* ! _SHMEM_PUTGET_SIGNAL_H */
