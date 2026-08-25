/* For license: see LICENSE file at top-level */

#ifndef _SHMEM_API_H
#define _SHMEM_API_H 1

#include <shmem/defs.h>
#include <shmem/depr.h>
#include <shmem/teams.h>
#include <sys/types.h>
#include <stddef.h> /* ptrdiff_t */
#include <stdint.h> /* sized int types */
#include <stdarg.h>
#include <shmem/api_types.h>

/*
 * for handling the "I" (upper-case eye) macro for complex numbers
 *
 * and see end of file for tidy-up
 */
#ifdef I
#define shmemi_h_I_already_defined__
#endif /* I */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

////////////////////////////////////////////////////////////////////////////////
/*
 * start/stop & query
 */

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief initializes the OpenSHMEM environment on the calling PE.
 *
 * @page start_pes
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void start_pes(int npes);
 @endcode
 *
 * @param npes the number of PEs participating in the program.  This
 * is ignored and should be set to 0.
 *
 * @section Effect
 * Initializes the OpenSHMEM environment on the calling PE.
 *
 * @section Return
 * None.
 *
 * @section Deprecated
 * by \ref shmem_init()
 *
 */
void start_pes(int npes) _DEPRECATED_BY(shmem_init, 1.2);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief initializes the OpenSHMEM environment on the calling PE.
 * @page shmem_init
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_init(void);
 @endcode
 *
 * @section Effect
 * Initializes the OpenSHMEM environment on the calling PE.
 *
 * @section Return
 * None.
 *
 */
void shmem_init(void);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief initializes the OpenSHMEM environment on the calling PE
 * and requests a threading support level
 *
 * @page shmem_init_thread
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_init_thread(int requested, int *provided);
 @endcode
 *
 * @section Effect
 *
 * Initializes the OpenSHMEM environment on the calling PE,
 * requesting thread support level "requested".  Routine returns
 * thread support level "provided".
 *
 * @section Return zero on success, non-zero otherwise
 *
 */
int shmem_init_thread(int requested, int *provided);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief finalizes the OpenSHMEM environment on the calling PE.
 * @page shmem_finalize
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_finalize(void);
 @endcode
 *
 * @section Effect
 * A collective finalization of the OpenSHMEM environment on the
 * calling PE.  After a finalize call, no further OpenSHMEM calls
 * are permitted.  Any subsequent use has undefined effects.
 *
 * @section Return
 * None.
 *
 */
void shmem_finalize(void);

////////////////////////////////////////////////////////////////////////////////
#if 1

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief is the OpenSHMEM environment initialized on the calling PE?
 * @page shmem_initialized
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_initialized(void);
 @endcode
 *
 * @section Effect Checks if the OpenSHMEM environment has been
 * initialized on the calling PE.
 *
 * @section Return
 * 1 if initialized, 0 if not.
 *
 */
int shmem_initialized(void);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief has the openSHMEM environment been finalized on the calling PE?
 * @page shmem_finalized
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_finalized(void);
 @endcode
 *
 * @section Effect Checks if the OpenSHMEM environment has been
 * finalized on the calling PE.
 *
 * @section Return
 * 1 if finalized, 0 if not.
 *
 */
int shmem_finalized(void);

#endif /* PR470 */

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief causes immediate exit from the OpenSHMEM program on all PEs.
 * @page shmem_global_exit
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_global_exit(int status);
 @endcode
 *
 * @section Effect
 * Called on 1 or more PEs, Causes immediate termination of the
 * program on all PEs.  Pending communication is flushed, files are
 * closed.  "status" allows the call to pass back information to the
 * execution environment.
 *
 * @section Return
 * None.
 *
 */
void shmem_global_exit(int status);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief returns the "rank" or identity of the calling PE
 * @page _my_pe
 * @section Deprecated
 * by \ref shmem_my_pe()
 *
 */
int _my_pe(void) _WUR _DEPRECATED_BY(shmem_my_pe, 1.2);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief returns the "rank" or identity of the calling PE
 * @page shmem_my_pe
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_my_pe(void);
 @endcode
 *
 * @section Effect
 * None.
 *
 * @section Return
 * Rank of calling PE
 *
 */
int shmem_my_pe(void) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines return the number of PEs in the program
 *
 * @section Deprecated by by shmem_n_pes()
 *
 */
int _num_pes(void) _WUR _DEPRECATED_BY(shmem_n_pes, 1.2);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief returns the number of PEs in the program
 *
 * @page shmem_n_pes
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_n_pes(void);
 @endcode
 *
 * @section Effect
 * None.
 *
 * @section Return
 * Number of PEs in program
 *
 */
int shmem_n_pes(void) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Supplies the supported threading level to the caller
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_query_thread(int *provided);
 @endcode
 *
 * @section Effect
 * Supplies the supported threading level to the caller
 *
 * @section Return
 * None.
 *
 */
void shmem_query_thread(int *provided);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief determines the major.minor version numbers of this release.
 * @page shmem_info_get_version
 * @section Synopsis
 *
 * @subsection c C
 @code
 void shmem_info_get_version(int *maj, int *min);
 @endcode
 *
 * @param[out] maj set to the release's major version number
 * @param[out] min set to the release's minor version number
 *
 * @section Effect
 * None.
 *
 * @section Return
 * None.
 *
 */
void shmem_info_get_version(int *major, int *minor);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief determines a vandor-supplied name for this release.
 * @page shmem_info_get_name
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_info_get_name(char *name);
 @endcode
 *
 * @param[out] name contains the vendor-supplied release name
 *
 * @section Effect
 * None.
 *
 * @section Return
 * None.
 *
 */
void shmem_info_get_name(char *name);

////////////////////////////////////////////////////////////////////////////////
#ifdef PR463
/*
 * https://github.com/openshmem-org/specification/issues/463
 *
 */

void shmem_info_get_version_number(int *version);
void shmem_info_get_vendor_version(int *major, int *minor, int *patch);
void shmem_info_get_vendor_version_number(int *version);
#endif

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Allows the user to control profiling
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_pcontrol(const int level, ...)
 @endcode
 *
 * @param[in] level Profilong level to use
 *
 * @section Effect
 * Sets the library's internal profiling behavior.
 *
 * @section Return
 * None.
 *
 */
void shmem_pcontrol(const int level, ...);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares context-based put/get operations for typed data
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_ctx_typename_put(shmem_ctx_t ctx, type *dest, const type *src,
 * size_t nelems, int pe); void shmem_ctx_typename_iput(shmem_ctx_t ctx, type
 * *dest, const type *src, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe);
 * void shmem_ctx_typename_put_nbi(shmem_ctx_t ctx, type *dest, const type *src,
 * size_t nelems, int pe);
 * @endcode
 *
 * @param[in] ctx     The context on which to perform the operation
 * @param[out] dest   Symmetric destination array on remote PE
 * @param[in] src     Local source array
 * @param[in] nelems  Number of elements to transfer
 * @param[in] pe      PE number of remote PE
 * @param[in] tst     Stride between elements in target array
 * @param[in] sst     Stride between elements in source array
 *
 * @section Effect
 * Transfers data from local array to remote PE's symmetric array
 */
#define API_DECL_CTX_PUTGET(_opname, _typename, _type)                         \
  void shmem_ctx_##_typename##_##_opname(                                      \
      shmem_ctx_t ctx, _type *dest, const _type *src, size_t nelems, int pe);  \
  void shmem_ctx_##_typename##_i##_opname(                                     \
      shmem_ctx_t ctx, _type *dest, const _type *src, ptrdiff_t tst,           \
      ptrdiff_t sst, size_t nelems, int pe);                                   \
  void shmem_ctx_##_typename##_ib##_opname(                                    \
      shmem_ctx_t ctx, _type *dest, const _type *src, ptrdiff_t tst,           \
      ptrdiff_t sst, size_t bsize, size_t nblocks, int pe);                    \
  void shmem_ctx_##_typename##_##_opname##_nbi(                                \
      shmem_ctx_t ctx, _type *dest, const _type *src, size_t nelems, int pe);

#define DECL_CTX_PUT(_type, _typename)                                         \
  API_DECL_CTX_PUTGET(put, _typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_CTX_PUT)
#undef DECL_CTX_PUT

#define DECL_CTX_GET(_type, _typename)                                         \
  API_DECL_CTX_PUTGET(get, _typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_CTX_GET)
#undef DECL_CTX_GET

#undef API_DECL_CTX_PUTGET

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares non-context put/get operations for typed data
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_typename_put(type *dest, const type *src, size_t nelems, int pe);
 * void shmem_typename_iput(type *dest, const type *src, ptrdiff_t tst,
 * ptrdiff_t sst, size_t nelems, int pe); void shmem_typename_put_nbi(type
 * *dest, const type *src, size_t nelems, int pe);
 * @endcode
 *
 * @param[out] dest   Symmetric destination array on remote PE
 * @param[in] src     Local source array
 * @param[in] nelems  Number of elements to transfer
 * @param[in] pe      PE number of remote PE
 * @param[in] tst     Stride between elements in target array
 * @param[in] sst     Stride between elements in source array
 *
 * @section Effect
 * Transfers data from local array to remote PE's symmetric array
 */
#define API_DECL_PUTGET(_opname, _typename, _type)                             \
  void shmem_##_typename##_##_opname(_type *dest, const _type *src,            \
                                     size_t nelems, int pe);                   \
  void shmem_##_typename##_i##_opname(_type *dest, const _type *src,           \
                                      ptrdiff_t tst, ptrdiff_t sst,            \
                                      size_t nelems, int pe);                  \
  void shmem_##_typename##_ib##_opname(_type *dest, const _type *src,          \
                                      ptrdiff_t tst, ptrdiff_t sst,            \
                                      size_t bsize, size_t nblocks, int pe);   \
  void shmem_##_typename##_##_opname##_nbi(_type *dest, const _type *src,      \
                                           size_t nelems, int pe);

#define DECL_PUT(_type, _typename) API_DECL_PUTGET(put, _typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_PUT)
#undef DECL_PUT

#define DECL_GET(_type, _typename) API_DECL_PUTGET(get, _typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_GET)
#undef DECL_GET

#undef API_DECL_PUTGET

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares context-based put/get operations for fixed-size data
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_ctx_putX(shmem_ctx_t ctx, void *dest, const void *src, size_t
 * nelems, int pe); void shmem_ctx_iputX(shmem_ctx_t ctx, void *dest, const void
 * *src, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe); void
 * shmem_ctx_putX_nbi(shmem_ctx_t ctx, void *dest, const void *src, size_t
 * nelems, int pe);
 * @endcode
 * where X is one of: 8, 16, 32, 64, 128
 *
 * @param[in] ctx     The context on which to perform the operation
 * @param[out] dest   Symmetric destination array on remote PE
 * @param[in] src     Local source array
 * @param[in] nelems  Number of elements to transfer
 * @param[in] pe      PE number of remote PE
 * @param[in] tst     Stride between elements in target array
 * @param[in] sst     Stride between elements in source array
 *
 * @section Effect
 * Transfers fixed-size data from local array to remote PE's symmetric array
 */
#define API_DECL_CTX_PUTGET_SIZE(_opname, _size)                               \
  void shmem_ctx_##_opname##_size(shmem_ctx_t ctx, void *dest,                 \
                                  const void *src, size_t nelems, int pe);     \
  void shmem_ctx_i##_opname##_size(shmem_ctx_t ctx, void *dest,                \
                                   const void *src, ptrdiff_t tst,             \
                                   ptrdiff_t sst, size_t nelems, int pe);      \
  void shmem_ctx_ib##_opname##_size(shmem_ctx_t ctx, void *dest,               \
                                   const void *src, ptrdiff_t tst,             \
                                   ptrdiff_t sst, size_t bsize, size_t nblocks,\
                                   int pe);                                    \
  void shmem_ctx_##_opname##_size##_nbi(                                       \
      shmem_ctx_t ctx, void *dest, const void *src, size_t nelems, int pe);

API_DECL_CTX_PUTGET_SIZE(put, 8)
API_DECL_CTX_PUTGET_SIZE(put, 16)
API_DECL_CTX_PUTGET_SIZE(put, 32)
API_DECL_CTX_PUTGET_SIZE(put, 64)
API_DECL_CTX_PUTGET_SIZE(put, 128)

API_DECL_CTX_PUTGET_SIZE(get, 8)
API_DECL_CTX_PUTGET_SIZE(get, 16)
API_DECL_CTX_PUTGET_SIZE(get, 32)
API_DECL_CTX_PUTGET_SIZE(get, 64)
API_DECL_CTX_PUTGET_SIZE(get, 128)

#undef API_DECL_CTX_PUTGET_SIZE

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares non-context put/get operations for fixed-size data
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_putX(void *dest, const void *src, size_t nelems, int pe);
 * void shmem_iputX(void *dest, const void *src, ptrdiff_t tst, ptrdiff_t sst,
 * size_t nelems, int pe); void shmem_putX_nbi(void *dest, const void *src,
 * size_t nelems, int pe);
 * @endcode
 * where X is one of: 8, 16, 32, 64, 128
 *
 * @param[out] dest   Symmetric destination array on remote PE
 * @param[in] src     Local source array
 * @param[in] nelems  Number of elements to transfer
 * @param[in] pe      PE number of remote PE
 * @param[in] tst     Stride between elements in target array
 * @param[in] sst     Stride between elements in source array
 *
 * @section Effect
 * Transfers fixed-size data from local array to remote PE's symmetric array
 */
#define API_DECL_PUTGET_SIZE(_opname, _size)                                   \
  void shmem_##_opname##_size(void *dest, const void *src, size_t nelems,      \
                              int pe);                                         \
  void shmem_i##_opname##_size(void *dest, const void *src, ptrdiff_t tst,     \
                               ptrdiff_t sst, size_t nelems, int pe);          \
  void shmem_ib##_opname##_size(void *dest, const void *src, ptrdiff_t tst,    \
                               ptrdiff_t sst, size_t bsize,                    \
                               size_t nblocks, int pe);                        \
  void shmem_##_opname##_size##_nbi(void *dest, const void *src,               \
                                    size_t nelems, int pe);

API_DECL_PUTGET_SIZE(put, 8)
API_DECL_PUTGET_SIZE(put, 16)
API_DECL_PUTGET_SIZE(put, 32)
API_DECL_PUTGET_SIZE(put, 64)
API_DECL_PUTGET_SIZE(put, 128)

API_DECL_PUTGET_SIZE(get, 8)
API_DECL_PUTGET_SIZE(get, 16)
API_DECL_PUTGET_SIZE(get, 32)
API_DECL_PUTGET_SIZE(get, 64)
API_DECL_PUTGET_SIZE(get, 128)

#undef API_DECL_PUTGET_SIZE

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares context-based put/get operations for memory blocks
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_ctx_putmem(shmem_ctx_t ctx, void *dest, const void *src, size_t
 * nelems, int pe); void shmem_ctx_putmem_nbi(shmem_ctx_t ctx, void *dest, const
 * void *src, size_t nelems, int pe);
 * @endcode
 *
 * @param[in] ctx     The context on which to perform the operation
 * @param[out] dest   Symmetric destination array on remote PE
 * @param[in] src     Local source array
 * @param[in] nelems  Number of elements to transfer
 * @param[in] pe      PE number of remote PE
 *
 * @section Effect
 * Transfers memory blocks from local array to remote PE's symmetric array
 */
#define API_DECL_CTX_PUTGET_MEM(_opname)                                       \
  void shmem_ctx_##_opname##mem(shmem_ctx_t ctx, void *dest, const void *src,  \
                                size_t nelems, int pe);                        \
  void shmem_ctx_##_opname##mem##_nbi(shmem_ctx_t ctx, void *dest,             \
                                      const void *src, size_t nelems, int pe);

API_DECL_CTX_PUTGET_MEM(put)
API_DECL_CTX_PUTGET_MEM(get)

#undef API_DECL_CTX_PUTGET_MEM

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares non-context put/get operations for memory blocks
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_putmem(void *dest, const void *src, size_t nelems, int pe);
 * void shmem_putmem_nbi(void *dest, const void *src, size_t nelems, int pe);
 * @endcode
 *
 * @param[out] dest   Symmetric destination array on remote PE
 * @param[in] src     Local source array
 * @param[in] nelems  Number of elements to transfer
 * @param[in] pe      PE number of remote PE
 *
 * @section Effect
 * Transfers memory blocks from local array to remote PE's symmetric array
 */
#define API_DECL_PUTGET_MEM(_opname)                                           \
  void shmem_##_opname##mem(void *dest, const void *src, size_t nelems,        \
                            int pe);                                           \
  void shmem_##_opname##mem_nbi(void *dest, const void *src, size_t nelems,    \
                                int pe);

API_DECL_PUTGET_MEM(put)
API_DECL_PUTGET_MEM(get)

#undef API_DECL_PUTGET_MEM

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares context-based put operations for single values
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_ctx_typename_p(shmem_ctx_t ctx, type *dest, type src, int pe);
 * @endcode
 *
 * @param[in] ctx     The context on which to perform the operation
 * @param[out] dest   Symmetric destination variable on remote PE
 * @param[in] src     Local source value
 * @param[in] pe      PE number of remote PE
 *
 * @section Effect
 * Transfers a single value from local variable to remote PE's symmetric
 * variable
 */
#define API_CTX_DECL_P(_typename, _type)                                       \
  void shmem_ctx_##_typename##_p(shmem_ctx_t ctx, _type *dest, _type src,      \
                                 int pe);

#define DECL_CTX_P(_type, _typename) API_CTX_DECL_P(_typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_CTX_P)
#undef DECL_CTX_P

#undef API_CTX_DECL_P

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares non-context put operations for single values
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_typename_p(type *dest, type src, int pe);
 * @endcode
 *
 * @param[out] dest   Symmetric destination variable on remote PE
 * @param[in] src     Local source value
 * @param[in] pe      PE number of remote PE
 *
 * @section Effect
 * Transfers a single value from local variable to remote PE's symmetric
 * variable
 */
#define API_DECL_P(_typename, _type)                                           \
  void shmem_##_typename##_p(_type *dest, _type src, int pe);

#define DECL_P(_type, _typename) API_DECL_P(_typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_P)
#undef DECL_P

#undef API_DECL_P

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares context-based get operations for single values
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * type shmem_ctx_typename_g(shmem_ctx_t ctx, const type *src, int pe);
 * @endcode
 *
 * @param[in] ctx     The context on which to perform the operation
 * @param[in] src     Symmetric source variable on remote PE
 * @param[in] pe      PE number of remote PE
 *
 * @section Return
 * The value from the remote PE
 *
 * @section Effect
 * Gets a single value from remote PE's symmetric variable
 */
#define API_CTX_DECL_G(_typename, _type)                                       \
  _type shmem_ctx_##_typename##_g(shmem_ctx_t ctx, const _type *src, int pe);

#define DECL_CTX_G(_type, _typename) API_CTX_DECL_G(_typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_CTX_G)
#undef DECL_CTX_G

#undef API_CTX_DECL_G

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares non-context get operations for single values
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * type shmem_typename_g(const type *src, int pe);
 * @endcode
 *
 * @param[in] src     Symmetric source variable on remote PE
 * @param[in] pe      PE number of remote PE
 *
 * @section Return
 * The value from the remote PE
 *
 * @section Effect
 * Gets a single value from remote PE's symmetric variable
 */
#define API_DECL_G(_typename, _type)                                           \
  _type shmem_##_typename##_g(const _type *src, int pe);

#define DECL_G(_type, _typename) API_DECL_G(_typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_G)
#undef DECL_G

#undef API_DECL_G

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares context-based put operations with signal
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_ctx_typename_put_signal(shmem_ctx_t ctx, type *dest, const type
 * *src, size_t nelems, uint64_t *sig_addr, uint64_t signal, int sig_op, int
 * pe); void shmem_ctx_typename_put_signal_nbi(shmem_ctx_t ctx, type *dest,
 * const type *src, size_t nelems, uint64_t *sig_addr, uint64_t signal, int
 * sig_op, int pe);
 * @endcode
 *
 * @param[in] ctx       The context on which to perform the operation
 * @param[out] dest     Symmetric destination array on remote PE
 * @param[in] src       Local source array
 * @param[in] nelems    Number of elements to transfer
 * @param[out] sig_addr Address of signal variable on remote PE
 * @param[in] signal    Signal value to write after transfer
 * @param[in] sig_op    Signal operation to perform
 * @param[in] pe        PE number of remote PE
 *
 * @section Effect
 * Transfers data and signals completion to remote PE
 */
#define API_DECL_CTX_PUT_SIGNAL(_typename, _type)                              \
  void shmem_ctx_##_typename##_put_signal(                                     \
      shmem_ctx_t ctx, _type *dest, const _type *src, size_t nelems,           \
      uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);

#define DECL_CTX_PUT_SIGNAL(_type, _typename)                                  \
  API_DECL_CTX_PUT_SIGNAL(_typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_CTX_PUT_SIGNAL)
#undef DECL_CTX_PUT_SIGNAL

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares context-based non-blocking put operations with signal
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_ctx_typename_put_signal_nbi(shmem_ctx_t ctx, type *dest, const
 * type *src, size_t nelems, uint64_t *sig_addr, uint64_t signal, int sig_op,
 * int pe);
 * @endcode
 *
 * @param[in] ctx       The context on which to perform the operation
 * @param[out] dest     Symmetric destination array on remote PE
 * @param[in] src       Local source array
 * @param[in] nelems    Number of elements to transfer
 * @param[out] sig_addr Address of signal variable on remote PE
 * @param[in] signal    Signal value to write after transfer
 * @param[in] sig_op    Signal operation to perform
 * @param[in] pe        PE number of remote PE
 *
 * @section Effect
 * Initiates non-blocking transfer of data and signals completion to remote PE
 */
#define API_DECL_CTX_PUT_SIGNAL_NBI(_typename, _type)                          \
  void shmem_ctx_##_typename##_put_signal_nbi(                                 \
      shmem_ctx_t ctx, _type *dest, const _type *src, size_t nelems,           \
      uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);

#define DECL_CTX_PUT_SIGNAL_NBI(_type, _typename)                              \
  API_DECL_CTX_PUT_SIGNAL_NBI(_typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_CTX_PUT_SIGNAL_NBI)
#undef DECL_CTX_PUT_SIGNAL_NBI

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares non-context put operations with signal
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_typename_put_signal(type *dest, const type *src, size_t nelems,
 * uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);
 * @endcode
 *
 * @param[out] dest     Symmetric destination array on remote PE
 * @param[in] src       Local source array
 * @param[in] nelems    Number of elements to transfer
 * @param[out] sig_addr Address of signal variable on remote PE
 * @param[in] signal    Signal value to write after transfer
 * @param[in] sig_op    Signal operation to perform
 * @param[in] pe        PE number of remote PE
 *
 * @section Effect
 * Transfers data and signals completion to remote PE
 */
#define API_DECL_PUT_SIGNAL(_typename, _type)                                  \
  void shmem_##_typename##_put_signal(_type *dest, const _type *src,           \
                                      size_t nelems, uint64_t *sig_addr,       \
                                      uint64_t signal, int sig_op, int pe);

#define DECL_PUT_SIGNAL(_type, _typename) API_DECL_PUT_SIGNAL(_typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_PUT_SIGNAL)
#undef DECL_PUT_SIGNAL

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares non-context non-blocking put operations with signal
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_typename_put_signal_nbi(type *dest, const type *src, size_t
 * nelems, uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);
 * @endcode
 *
 * @param[out] dest     Symmetric destination array on remote PE
 * @param[in] src       Local source array
 * @param[in] nelems    Number of elements to transfer
 * @param[out] sig_addr Address of signal variable on remote PE
 * @param[in] signal    Signal value to write after transfer
 * @param[in] sig_op    Signal operation to perform
 * @param[in] pe        PE number of remote PE
 *
 * @section Effect
 * Initiates non-blocking transfer of data and signals completion to remote PE
 */
#define API_DECL_PUT_SIGNAL_NBI(_typename, _type)                              \
  void shmem_##_typename##_put_signal_nbi(                                     \
      _type *dest, const _type *src, size_t nelems, uint64_t *sig_addr,        \
      uint64_t signal, int sig_op, int pe);

#define DECL_PUT_SIGNAL_NBI(_type, _typename)                                  \
  API_DECL_PUT_SIGNAL_NBI(_typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_PUT_SIGNAL_NBI)
#undef DECL_PUT_SIGNAL_NBI

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares context-based put operations with signal for fixed sizes
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_ctx_putX_signal(shmem_ctx_t ctx, void *dest, const void *src,
 * size_t nelems, uint64_t *sig_addr, uint64_t signal, int sig_op, int pe); void
 * shmem_ctx_putX_signal_nbi(shmem_ctx_t ctx, void *dest, const void *src,
 * size_t nelems, uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);
 * @endcode
 * where X is one of: 8, 16, 32, 64, 128
 *
 * @param[in] ctx       The context on which to perform the operation
 * @param[out] dest     Symmetric destination array on remote PE
 * @param[in] src       Local source array
 * @param[in] nelems    Number of elements to transfer
 * @param[out] sig_addr Address of signal variable on remote PE
 * @param[in] signal    Signal value to write after transfer
 * @param[in] sig_op    Signal operation to perform
 * @param[in] pe        PE number of remote PE
 *
 * @section Effect
 * Transfers fixed-size data and signals completion to remote PE
 */
#define API_DECL_CTX_PUT_SIGNAL_SIZE(_size)                                    \
  void shmem_ctx_put##_size##_signal(                                          \
      shmem_ctx_t ctx, void *dest, const void *src, size_t nelems,             \
      uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);                \
  void shmem_ctx_put##_size##_signal_nbi(                                      \
      shmem_ctx_t ctx, void *dest, const void *src, size_t nelems,             \
      uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);

API_DECL_CTX_PUT_SIGNAL_SIZE(8)
API_DECL_CTX_PUT_SIGNAL_SIZE(16)
API_DECL_CTX_PUT_SIGNAL_SIZE(32)
API_DECL_CTX_PUT_SIGNAL_SIZE(64)
API_DECL_CTX_PUT_SIGNAL_SIZE(128)

#undef API_DECL_CTX_PUT_SIGNAL_SIZE

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares context-based put operations with signal for memory blocks
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_ctx_putmem_signal(shmem_ctx_t ctx, void *dest, const void *src,
 * size_t nelems, uint64_t *sig_addr, uint64_t signal, int sig_op, int pe); void
 * shmem_ctx_putmem_signal_nbi(shmem_ctx_t ctx, void *dest, const void *src,
 * size_t nelems, uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);
 * @endcode
 *
 * @param[in] ctx       The context on which to perform the operation
 * @param[out] dest     Symmetric destination array on remote PE
 * @param[in] src       Local source array
 * @param[in] nelems    Number of elements to transfer
 * @param[out] sig_addr Address of signal variable on remote PE
 * @param[in] signal    Signal value to write after transfer
 * @param[in] sig_op    Signal operation to perform
 * @param[in] pe        PE number of remote PE
 *
 * @section Effect
 * Transfers memory blocks and signals completion to remote PE
 */
#define API_DECL_CTX_PUTMEM_SIGNAL()                                           \
  void shmem_ctx_putmem_signal(shmem_ctx_t ctx, void *dest, const void *src,   \
                               size_t nelems, uint64_t *sig_addr,              \
                               uint64_t signal, int sig_op, int pe);           \
  void shmem_ctx_putmem_signal_nbi(                                            \
      shmem_ctx_t ctx, void *dest, const void *src, size_t nelems,             \
      uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);

API_DECL_CTX_PUTMEM_SIGNAL()

#undef API_DECL_CTX_PUTMEM_SIGNAL

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares non-context put operations with signal for fixed-size data
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_putX_signal(void *dest, const void *src, size_t nelems,
 * uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);
 * void shmem_putX_signal_nbi(void *dest, const void *src, size_t nelems,
 * uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);
 * @endcode
 * where X is one of: 8, 16, 32, 64, 128
 *
 * @param[out] dest     Symmetric destination array on remote PE
 * @param[in] src       Local source array
 * @param[in] nelems    Number of elements to transfer
 * @param[out] sig_addr Address of signal variable on remote PE
 * @param[in] signal    Signal value to write after transfer
 * @param[in] sig_op    Signal operation to perform
 * @param[in] pe        PE number of remote PE
 *
 * @section Effect
 * Transfers fixed-size data and signals completion to remote PE
 */
#define API_DECL_PUT_SIGNAL_SIZE(_size)                                        \
  void shmem_put##_size##_signal(void *dest, const void *src, size_t nelems,   \
                                 uint64_t *sig_addr, uint64_t signal,          \
                                 int sig_op, int pe);                          \
  void shmem_put##_size##_signal_nbi(void *dest, const void *src,              \
                                     size_t nelems, uint64_t *sig_addr,        \
                                     uint64_t signal, int sig_op, int pe);

API_DECL_PUT_SIGNAL_SIZE(8)
API_DECL_PUT_SIGNAL_SIZE(16)
API_DECL_PUT_SIGNAL_SIZE(32)
API_DECL_PUT_SIGNAL_SIZE(64)
API_DECL_PUT_SIGNAL_SIZE(128)

#undef API_DECL_PUT_SIGNAL_SIZE

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Declares non-context put operations with signal for memory blocks
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_putmem_signal(void *dest, const void *src, size_t nelems,
 * uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);
 * void shmem_putmem_signal_nbi(void *dest, const void *src, size_t nelems,
 * uint64_t *sig_addr, uint64_t signal, int sig_op, int pe);
 * @endcode
 *
 * @param[out] dest     Symmetric destination array on remote PE
 * @param[in] src       Local source array
 * @param[in] nelems    Number of elements to transfer
 * @param[out] sig_addr Address of signal variable on remote PE
 * @param[in] signal    Signal value to write after transfer
 * @param[in] sig_op    Signal operation to perform
 * @param[in] pe        PE number of remote PE
 *
 * @section Effect
 * Transfers memory blocks and signals completion to remote PE
 */
#define API_DECL_PUTMEM_SIGNAL()                                               \
  void shmem_putmem_signal(void *dest, const void *src, size_t nelems,         \
                           uint64_t *sig_addr, uint64_t signal, int sig_op,    \
                           int pe);                                            \
  void shmem_putmem_signal_nbi(void *dest, const void *src, size_t nelems,     \
                               uint64_t *sig_addr, uint64_t signal,            \
                               int sig_op, int pe);

API_DECL_PUTMEM_SIGNAL()

#undef API_DECL_PUTMEM_SIGNAL

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief fetches value of the signal object.
 * @page shmem_signal_fetch
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 uint64_t shmem_signal_fetch(const uint64_t *sig_addr);
 @endcode
 *
 * @param[in] sig_addr The address of the remotely accessible signal object
 *
 * @section Effect
 * None.
 *
 * @section Return
 * The data value.
 *
 */
uint64_t shmem_signal_fetch(const uint64_t *sig_addr);


////////////////////////////////////////////////////////////////////////////////
/**
 * @brief sets value of the signal object.
 * @page shmem_ctx_signal_set
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_ctx_signal_set(shmem_ctx_t ctx, const uint64_t *sig_addr,
      uint64_t signal, int pe) {
 @endcode
 *
 * @param[in] ctx Context on which to execute operation
 * @param[in] sig_addr The address of the remotely accessible signal object
 * @param[in] signal New value for signal
 * @param[in] pe Processing element to set signal
 *
 * @section Effect
 * None.
 *
 * @section Return
 * None.
 *
 */
void shmem_ctx_signal_set(shmem_ctx_t ctx, const uint64_t *sig_addr,
      uint64_t signal, int pe);
  
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief sets value of the signal object.
 * @page shmem_signal_set
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_signal_set(const uint64_t *sig_addr, uint64_t signal, int pe);
 @endcode
 *
 * @param[in] sig_addr The address of the remotely accessible signal object
 * @param[in] signal New value for signal
 * @param[in] pe Processing element to set signal
 *
 * @section Effect
 * None.
 *
 * @section Return
 * None.
 *
 */
void shmem_signal_set(const uint64_t *sig_addr, uint64_t signal, int pe);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief adds value to the signal object.
 * @page shmem_ctx_signal_add
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_ctx_signal_add(shmem_ctx_t ctx, const uint64_t *sig_addr,
      uint64_t signal, int pe) {
 @endcode
 *
 * @param[in] ctx Context on which to execute operation
 * @param[in] sig_addr The address of the remotely accessible signal object
 * @param[in] signal Value to add to signal
 * @param[in] pe Processing element to add to
 *
 * @section Effect
 * None.
 *
 * @section Return
 * None.
 *
 */
void shmem_ctx_signal_add(shmem_ctx_t ctx, const uint64_t *sig_addr,
      uint64_t signal, int pe);
  
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief adds value of the signal object.
 * @page shmem_signal_add
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_signal_add(const uint64_t *sig_addr, uint64_t signal, int pe);
 @endcode
 *
 * @param[in] sig_addr The address of the remotely accessible signal object
 * @param[in] signal New value for signal
 * @param[in] pe Processing element to add signal
 *
 * @section Effect
 * None.
 *
 * @section Return
 * None.
 *
 */
void shmem_signal_add(const uint64_t *sig_addr, uint64_t signal, int pe);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief waits for signal object to change value
 * @page shmem_signal_wait_until
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 uint64_t shmem_signal_wait_until(uint64_t *sig_addr, int cmp, uint64_t
 cmp_value));
 @endcode
 *
 * @param[inout] sig_addr The address of the remotely accessible signal object
 * @param[in] cmp The comparison operator
 * @param[in] cmp_value Value to compare against
 *
 * @section Effect
 * Waits for contents of sig_addr to change
 *
 * @section Return
 * The value in sig_addr on calling PE
 *
 */
uint64_t shmem_signal_wait_until(uint64_t *sig_addr, int cmp,
                                 uint64_t cmp_value);

////////////////////////////////////////////////////////////////////////////////
/**
  * @brief causes an active set of PEs to synchronize.  Local memory stores
  * complete.
  * @page shmem_sync
  * @section Synopsis
  *
  * @subsection c C/C++
  @code
  void shmem_sync(int PE_start, int logPE_stride, int PE_size,
                  long *pSync);
  @endcode
  *
  * @param[in] PE_start first PE of the active set
  * @param[in] logPE_stride log2 of stride between PEs
  * @param[in] PE_size number of PEs in the active set
  * @param[in, out] pSync symmetric work array
  *
  * @section Effect
  * PEs in the active set defined by (PE_start, logPE_stride,
  * PE_size) synchronize: no PE from this active set can leave the
  * global barrier until all have arrived.  Local memory loads and store
  * complete before return.  PEs not in the active set do not call
  * shmem_sync().  pSync must be initialized everywhere before
  * use, and, if modified, must be reset to its state before the
  * call.
  *
  * @section Return
  * None.
  *
  */

void shmem_sync_deprecated(int PE_start, int logPE_stride, int PE_size,
                           long *pSync) _DEPRECATED_BY(shmem_team_sync, 1.5);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief causes all PEs to synchronize
 * @page shmem_sync_all
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_sync_all(void);
 @endcode
 *
 * @section Effect
 * All PEs synchronize: no PE can leave the global barrier until all
 * have arrived.  Local memory loads and store complete before return.
 *
 * @section Return
 * None.
 *
 */
void shmem_sync_all(void);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief causes an active set of PEs to synchronize
 * @page shmem_barrier
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_barrier(int PE_start, int logPE_stride, int PE_size,
                    long *pSync);
 @endcode
 *
 * @param[in] PE_start first PE of the active set
 * @param[in] logPE_stride log2 of stride between PEs
 * @param[in] PE_size number of PEs in the active set
 * @param[in, out] pSync symmetric work array
 *
 * @section Effect
 * PEs in the active set defined by (PE_start, logPE_stride,
 * PE_size) synchronize: no PE from this active set can leave the
 * global barrier until all have arrived.  Communication is also
 * flushed before return.  PEs not in the active set do not call
 * shmem_barrier().  pSync must be initialized everywhere before
 * use, and, if modified, must be reset to its state before the
 * call.
 *
 * @section Return
 * None.
 *
 */
void shmem_barrier(int PE_start, int logPE_stride, int PE_size, long *pSync)
    _DEPRECATED_BY(shmem_ctx_quiet then shmem_team_sync, 1.5);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief causes all PEs to synchronize
 * @page shmem_barrier_all
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_barrier_all(void);
 @endcode
 *
 * @section Effect
 * All PEs synchronize: no PE can leave the global barrier until all
 * have arrived.
 *
 * @section Return
 * None.
 *
 */
void shmem_barrier_all(void);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief outbound communication completes before any subsequent
 * communication is sent.
 * @page shmem_ctx_fence
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_ctx_fence(shmem_ctx_t ctx);
 void shmem_fence(void);
 @endcode
 *
 * @section Effect
 * Local ordering
 *
 * @section Return
 * None.
 *
 */
void shmem_ctx_fence(shmem_ctx_t ctx);
void shmem_fence(void);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief causes outbound communication to complete before
 * subsequent puts are sent.
 * @page shmem_ctx_quiet
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_ctx_quiet(shmem_ctx_t ctx);
 void shmem_quiet(void);
 @endcode
 *
 * @section Effect
 * Remote completion
 *
 * @section Return
 * None.
 *
 */
void shmem_ctx_quiet(shmem_ctx_t ctx);
void shmem_quiet(void);


/**
 * @brief causes outbound communication to complete before
 * subsequent puts are sent for a subset of PEs.
 * @page shmem_ctx_quiet
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_ctx_pe_quiet(shmem_ctx_t ctx, const int *target_pes, size_t npes);
 void shmem_pe_quiet(const int *target_pes, size_t npes);
 @endcode
 *
 * @section Effect
 * Remote completion
 *
 * @section Return
 * None.
 *
 */
 void shmem_ctx_pe_quiet(shmem_ctx_t ctx, const int *target_pes, size_t npes);
 void shmem_pe_quiet(const int *target_pes, size_t npes);

////////////////////////////////////////////////////////////////////////////////
/*
 * accessibility
 */

/**
 * @brief checks whether the caller PE can communicate with the named PE
 * @page shmem_pe_accessible
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_pe_accessible(int pe);
 @endcode
 *
 * @section Effect
 * None
 *
 * @section Return
 * non-zero if "pe" can be communicated with.  0 if not.
 *
 */
int shmem_pe_accessible(int pe) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief checks whether the caller PE can communicate with a memory
 * address on the named PE
 * @page shmem_addr_accessible
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_addr_accessible(const void *addr, int pe);
 @endcode
 *
 * @param addr address to check
 * @param pe PE to check
 *
 * @section Effect
 * None
 *
 * @section Return
 * non-zero if address "addr" can be used for communication
 * on PE "pe".  0 if not.
 *
 */
int shmem_addr_accessible(const void *addr, int pe) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief checks whether an address on a target PE can be accessed
 * with a simple load/store operation.
 * @page shmem_ptr
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void *shmem_ptr(const void *addr, int pe);
 @endcode
 *
 * @section Effect
 * None
 *
 * @section Return
 * a pointer to a memory location corresponding to the
 * address on the target PE if that address can be accessed with
 * load/store operations by the calling PE.  NULL if not.
 *
 */
void *shmem_ptr(const void *target, int pe) _WUR;

////////////////////////////////////////////////////////////////////////////////
/*
 * symmetric memory management
 */

/* deprecated calls from 1.2 ++ */

/**
 * @brief dynamically allocates symmetric memory
 *
 * @section Deprecated by by \ref shmem_malloc()
 *
 */
void *shmalloc(size_t size) _WUR _DEPRECATED_BY(shmem_malloc, 1.2);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief dynamically allocates symmetric memory
 *
 * @section Deprecated by by \ref shmem_free()
 *
 */
void shfree(void *ptr) _DEPRECATED_BY(shmem_free, 1.2);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief dynamically allocates symmetric memory
 *
 * @section Deprecated by by \ref shmem_realloc()
 *
 */
void *shrealloc(void *ptr, size_t size) _WUR _DEPRECATED_BY(shmem_realloc, 1.2);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief dynamically allocates symmetric memory
 *
 * @section Deprecated by by \ref shmem_align()
 *
 */
void *shmemalign(size_t alignment, size_t size) _WUR
    _DEPRECATED_BY(shmem_align, 1.2);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief dynamically allocates symmetric memory
 * @page shmem_malloc
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void *shmem_malloc(size_t size);
 @endcode
 *
 * @param size number of bytes requested
 *
 * @section Effect
 * Allocates "size" bytes of contiguous memory from the PE's
 * symmetric heap.
 *
 * @section Return
 * a pointer to the requested memory location, or NULL if
 * the requested memory is not available.
 *
 */
void *shmem_malloc(size_t size) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief dynamically allocates zeroed symmetric memory
 * @page shmem_calloc
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void *shmem_calloc(size_t count, size_t size);
 @endcode
 *
 * @param count number of elements to allcoate
 * @param size how big each element is (bytes)
 *
 * @section Effect
 * Allocates "count" lots of "size" bytes of contiguous memory
 * from the PE's symmetric heap.  Memory is zeroed before return.
 *
 * @section Return
 * a pointer to the requested memory location, or NULL if
 * the requested memory is not available.
 *
 */
void *shmem_calloc(size_t count, size_t size) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief dynamically allocates symmetric memory
 * @page shmem_free
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_free(void *ptr);
 @endcode
 *
 * @param ptr symmetric memory pointer
 *
 * @section Effect
 * Frees a previous symmetric allocation.
 *
 * @section Return
 * None.
 */
void shmem_free(void *ptr);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief dynamically allocates symmetric memory
 * @page shmem_realloc
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void *shmem_realloc(void *ptr, size_t size);
 @endcode
 *
 * @param ptr symmetric memory pointer
 * @param size number of bytes
 *
 * @section Effect
 * Resizes a previous symmetric memory allocation starting at "ptr"
 * to "size" bytes.
 *
 * @section Return
 * a pointer to the resized area, or NULL if this is not
 * possible.
 *
 */
void *shmem_realloc(void *ptr, size_t size) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief aligns already allocated symmetric memory
 * @page shmem_align
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void *shmem_align(void *ptr, size_t size);
 @endcode
 *
 * @param ptr symmetric memory pointer
 * @param size number of bytes
 *
 * @section Effect
 * Resizes a previous symmetric memory allocation starting at "ptr"
 * to "size" bytes.
 *
 * @section Return
 * a pointer to the resized area, or NULL if this is not
 * possible.
 *
 */
void *shmem_align(size_t alignment, size_t size) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief dynamically allocates symmetric memory with hints about
 * memory properties
 *
 * @page shmem_malloc_with_hints
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void *shmem_malloc(size_t size);
 @endcode
 *
 * @param size number of bytes requested
 * @param hints bitwise or of hint values
 *
 * @section Effect Allocates "size" bytes of contiguous memory
 * from the PE's symmetric heap, allowing the implementation to
 * optimize allocation by using "hints".
 *
 * @section Return
 * a pointer to the requested memory location, or NULL if
 * the requested memory is not available.
 *
 */
void *shmem_malloc_with_hints(size_t size, long hints) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief test for symmetric variable to change value
 * @page shmem_long_test
 * @page shmem_long_wait_until
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_long_test(long *ivar, int cmp, long cmp_value);
 @endcode
 *
 * @section Effect
 *
 * ivar may be updated by another PE
 *
 * @section Return
 * 1 if the comparison is true, 0 if not
 *
 */

#define API_DECL_TEST_AND_WAIT_UNTIL(_opname, _rettype, _typename, _type)      \
  _rettype shmem_##_typename##_##_opname(_type *ivar, int cmp, _type cmp_value);

/* Deprecated types (short, ushort) */
API_DECL_TEST_AND_WAIT_UNTIL(test, int, short, short); //  _DEPRECATED;
API_DECL_TEST_AND_WAIT_UNTIL(test, int, ushort, unsigned short); // _DEPRECATED;

/* All other types from SHMEM_STANDARD_AMO_TYPE_TABLE */
#define DECL_TEST(_type, _typename)                                            \
  API_DECL_TEST_AND_WAIT_UNTIL(test, int, _typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_TEST)
#undef DECL_TEST

/* Deprecated types (short, ushort) */
API_DECL_TEST_AND_WAIT_UNTIL(wait_until, void, short, short); // _DEPRECATED;
API_DECL_TEST_AND_WAIT_UNTIL(wait_until, void, ushort,
                             unsigned short); // _DEPRECATED;

/* All other types from SHMEM_STANDARD_AMO_TYPE_TABLE */
#define DECL_WAIT_UNTIL(_type, _typename)                                      \
  API_DECL_TEST_AND_WAIT_UNTIL(wait_until, void, _typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_WAIT_UNTIL)
#undef DECL_WAIT_UNTIL

#undef API_DECL_TEST_AND_WAIT_UNTIL

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Test if all variables in an array meet a specified condition
 * @page shmem_long_test_all
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * int shmem_long_test_all(long *ivars, size_t nelems, const int *status, int
 * cmp, long cmp_value);
 * @endcode
 *
 * @param ivars     Array of symmetric variables to be tested
 * @param nelems    Number of elements in the ivars array
 * @param status    Array indicating which ivars elements to monitor
 * @param cmp       Comparison operator from shmem_cmp_constants
 * @param cmp_value The value to compare against
 *
 * @section Effect
 * Tests if all elements in ivars array meet the condition specified by the
 * comparison operator and comparison value. Only elements with corresponding
 * non-zero status values are tested.
 *
 * @section Return
 * Returns 1 if all tested elements meet the condition, 0 otherwise.
 */
#define API_DECL_TEST_ALL(_typename, _type)                                    \
  int shmem_##_typename##_test_all(_type *ivars, size_t nelems,                \
                                   const int *status, int cmp,                 \
                                   _type cmp_value);

#define DECL_TEST_ALL(_type, _typename) API_DECL_TEST_ALL(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_TEST_ALL)
#undef DECL_TEST_ALL

#undef API_DECL_TEST_ALL

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Test if any variable in an array meets a specified condition
 * @page shmem_long_test_any
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * size_t shmem_long_test_any(long *ivars, size_t nelems, const int *status, int
 * cmp, long cmp_value);
 * @endcode
 *
 * @param ivars     Array of symmetric variables to be tested
 * @param nelems    Number of elements in the ivars array
 * @param status    Array indicating which ivars elements to monitor
 * @param cmp       Comparison operator from shmem_cmp_constants
 * @param cmp_value The value to compare against
 *
 * @section Effect
 * Tests if any element in ivars array meets the condition specified by the
 * comparison operator and comparison value. Only elements with corresponding
 * non-zero status values are tested.
 *
 * @section Return
 * Returns the index of the first element that meets the condition, or nelems if
 * no element meets the condition.
 */
#define API_DECL_TEST_ANY(_typename, _type)                                    \
  size_t shmem_##_typename##_test_any(_type *ivars, size_t nelems,             \
                                      const int *status, int cmp,              \
                                      _type cmp_value);

#define DECL_TEST_ANY(_type, _typename) API_DECL_TEST_ANY(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_TEST_ANY)
#undef DECL_TEST_ANY

#undef API_DECL_TEST_ANY

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Test if some variables in an array meet a specified condition
 * @page shmem_long_test_some
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * size_t shmem_long_test_some(long *ivars, size_t nelems, size_t *indices,
 * const int *status, int cmp, long cmp_value);
 * @endcode
 *
 * @param ivars     Array of symmetric variables to be tested
 * @param nelems    Number of elements in the ivars array
 * @param indices   Array to store indices of elements that meet the condition
 * @param status    Array indicating which ivars elements to monitor
 * @param cmp       Comparison operator from shmem_cmp_constants
 * @param cmp_value The value to compare against
 *
 * @section Effect
 * Tests if any elements in ivars array meet the condition specified by the
 * comparison operator and comparison value. Only elements with corresponding
 * non-zero status values are tested. The indices of elements that meet the
 * condition are stored in the indices array.
 *
 * @section Return
 * Returns the number of elements that meet the condition.
 */
#define API_DECL_TEST_SOME(_typename, _type)                                   \
  size_t shmem_##_typename##_test_some(_type *ivars, size_t nelems,            \
                                       size_t *indices, const int *status,     \
                                       int cmp, _type cmp_value);

#define DECL_TEST_SOME(_type, _typename) API_DECL_TEST_SOME(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_TEST_SOME)
#undef DECL_TEST_SOME

#undef API_DECL_TEST_SOME

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Test if all variables in an array meet specified conditions using a
 * vector of comparison values
 * @page shmem_long_test_all_vector
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * int shmem_long_test_all_vector(long *ivars, size_t nelems, const int *status,
 * int cmp, long *cmp_values);
 * @endcode
 *
 * @param ivars      Array of symmetric variables to be tested
 * @param nelems     Number of elements in the ivars array
 * @param status     Array indicating which ivars elements to monitor
 * @param cmp        Comparison operator from shmem_cmp_constants
 * @param cmp_values Array of values to compare against each element
 *
 * @section Effect
 * Tests if all elements in ivars array meet the conditions specified by the
 * comparison operator and corresponding comparison values in cmp_values array.
 * Only elements with corresponding non-zero status values are tested.
 *
 * @section Return
 * Returns 1 if all elements meet their conditions, 0 otherwise.
 */
#define API_DECL_TEST_ALL_VECTOR(_typename, _type)                             \
  int shmem_##_typename##_test_all_vector(_type *ivars, size_t nelems,         \
                                          const int *status, int cmp,          \
                                          _type *cmp_values);

#define DECL_TEST_ALL_VECTOR(_type, _typename)                                 \
  API_DECL_TEST_ALL_VECTOR(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_TEST_ALL_VECTOR)
#undef DECL_TEST_ALL_VECTOR

#undef API_DECL_TEST_ALL_VECTOR

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Test if any variable in an array meets specified conditions using a
 * vector of comparison values
 * @page shmem_long_test_any_vector
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * size_t shmem_long_test_any_vector(long *ivars, size_t nelems, const int
 * *status, int cmp, long *cmp_values);
 * @endcode
 *
 * @param ivars      Array of symmetric variables to be tested
 * @param nelems     Number of elements in the ivars array
 * @param status     Array indicating which ivars elements to monitor
 * @param cmp        Comparison operator from shmem_cmp_constants
 * @param cmp_values Array of values to compare against each element
 *
 * @section Effect
 * Tests if any element in ivars array meets the condition specified by the
 * comparison operator and corresponding comparison value in cmp_values array.
 * Only elements with corresponding non-zero status values are tested.
 *
 * @section Return
 * Returns the index of the first element that meets its condition, or nelems if
 * no element meets its condition.
 */
#define API_DECL_TEST_ANY_VECTOR(_typename, _type)                             \
  size_t shmem_##_typename##_test_any_vector(_type *ivars, size_t nelems,      \
                                             const int *status, int cmp,       \
                                             _type *cmp_values);

#define DECL_TEST_ANY_VECTOR(_type, _typename)                                 \
  API_DECL_TEST_ANY_VECTOR(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_TEST_ANY_VECTOR)
#undef DECL_TEST_ANY_VECTOR

#undef API_DECL_TEST_ANY_VECTOR

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Test if some variables in an array meet specified conditions using a
 * vector of comparison values
 * @page shmem_long_test_some_vector
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * size_t shmem_long_test_some_vector(long *ivars, size_t nelems, size_t
 * *indices, const int *status, int cmp, long *cmp_values);
 * @endcode
 *
 * @param ivars      Array of symmetric variables to be tested
 * @param nelems     Number of elements in the ivars array
 * @param indices    Array to store indices of elements that meet their
 * conditions
 * @param status     Array indicating which ivars elements to monitor
 * @param cmp        Comparison operator from shmem_cmp_constants
 * @param cmp_values Array of values to compare against each element
 *
 * @section Effect
 * Tests if elements in ivars array meet the conditions specified by the
 * comparison operator and corresponding comparison values in cmp_values array.
 * Only elements with corresponding non-zero status values are tested. Indices
 * of elements that meet their conditions are stored in the indices array.
 *
 * @section Return
 * Returns the number of elements that meet their conditions.
 */
#define API_DECL_TEST_SOME_VECTOR(_typename, _type)                            \
  size_t shmem_##_typename##_test_some_vector(                                 \
      _type *ivars, size_t nelems, size_t *indices, const int *status,         \
      int cmp, _type *cmp_values);

#define DECL_TEST_SOME_VECTOR(_type, _typename)                                \
  API_DECL_TEST_SOME_VECTOR(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_TEST_SOME_VECTOR)
#undef DECL_TEST_SOME_VECTOR

#undef API_DECL_TEST_SOME_VECTOR

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Wait until all variables in an array meet a specified condition
 * @page shmem_long_wait_until_all
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_long_wait_until_all(long *ivars, size_t nelems, const int *status,
 * int cmp, long cmp_value);
 * @endcode
 *
 * @param ivars      Array of symmetric variables to wait on
 * @param nelems     Number of elements in the ivars array
 * @param status     Array indicating which ivars elements to monitor
 * @param cmp        Comparison operator from shmem_cmp_constants
 * @param cmp_value  Value to compare against each element
 *
 * @section Effect
 * Waits until all elements in ivars array meet the condition specified by the
 * comparison operator and comparison value. Only elements with corresponding
 * non-zero status values are monitored.
 *
 * @section Return
 * None.
 */
#define API_DECL_WAIT_UNTIL_ALL(_typename, _type)                              \
  void shmem_##_typename##_wait_until_all(_type *ivars, size_t nelems,         \
                                          const int *status, int cmp,          \
                                          _type cmp_value);

#define DECL_WAIT_UNTIL_ALL(_type, _typename)                                  \
  API_DECL_WAIT_UNTIL_ALL(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_WAIT_UNTIL_ALL)
#undef DECL_WAIT_UNTIL_ALL

#undef API_DECL_WAIT_UNTIL_ALL

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Wait until any variable in an array meets a specified condition
 * @page shmem_long_wait_until_any
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * size_t shmem_long_wait_until_any(long *ivars, size_t nelems, const int
 * *status, int cmp, long cmp_value);
 * @endcode
 *
 * @param ivars      Array of symmetric variables to wait on
 * @param nelems     Number of elements in the ivars array
 * @param status     Array indicating which ivars elements to monitor
 * @param cmp        Comparison operator from shmem_cmp_constants
 * @param cmp_value  Value to compare against each element
 *
 * @section Effect
 * Waits until any element in ivars array meets the condition specified by the
 * comparison operator and comparison value. Only elements with corresponding
 * non-zero status values are monitored.
 *
 * @section Return
 * Index of the first element that meets the condition.
 */
#define API_DECL_WAIT_UNTIL_ANY(_typename, _type)                              \
  size_t shmem_##_typename##_wait_until_any(_type *ivars, size_t nelems,       \
                                            const int *status, int cmp,        \
                                            _type cmp_value);

#define DECL_WAIT_UNTIL_ANY(_type, _typename)                                  \
  API_DECL_WAIT_UNTIL_ANY(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_WAIT_UNTIL_ANY)
#undef DECL_WAIT_UNTIL_ANY

#undef API_DECL_WAIT_UNTIL_ANY

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Wait until some variables in an array meet a specified condition
 * @page shmem_long_wait_until_some
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * size_t shmem_long_wait_until_some(long *ivars, size_t nelems, size_t
 * *indices, const int *status, int cmp, long cmp_value);
 * @endcode
 *
 * @param ivars      Array of symmetric variables to wait on
 * @param nelems     Number of elements in the ivars array
 * @param indices    Array to store indices of elements that meet the condition
 * @param status     Array indicating which ivars elements to monitor
 * @param cmp        Comparison operator from shmem_cmp_constants
 * @param cmp_value  Value to compare against each element
 *
 * @section Effect
 * Waits until at least one element in ivars array meets the condition specified
 * by the comparison operator and comparison value. Only elements with
 * corresponding non-zero status values are monitored. Indices of elements that
 * meet the condition are stored in the indices array.
 *
 * @section Return
 * Number of elements that meet the condition.
 */
#define API_DECL_WAIT_UNTIL_SOME(_typename, _type)                             \
  size_t shmem_##_typename##_wait_until_some(                                  \
      _type *ivars, size_t nelems, size_t *indices, const int *status,         \
      int cmp, _type cmp_value);

#define DECL_WAIT_UNTIL_SOME(_type, _typename)                                 \
  API_DECL_WAIT_UNTIL_SOME(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_WAIT_UNTIL_SOME)
#undef DECL_WAIT_UNTIL_SOME

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Wait until all variables in an array meet specified conditions using a
 * vector of comparison values
 * @page shmem_long_wait_until_all_vector
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_long_wait_until_all_vector(long *ivars, size_t nelems, const int
 * *status, int cmp, long *cmp_values);
 * @endcode
 *
 * @param ivars      Array of symmetric variables to wait on
 * @param nelems     Number of elements in the ivars array
 * @param status     Array indicating which ivars elements to monitor
 * @param cmp        Comparison operator from shmem_cmp_constants
 * @param cmp_values Array of values to compare against each element
 *
 * @section Effect
 * Waits until all elements in ivars array meet the conditions specified by the
 * comparison operator and corresponding comparison values in cmp_values array.
 * Only elements with corresponding non-zero status values are monitored.
 *
 * @section Return
 * None.
 */
#define API_DECL_WAIT_UNTIL_ALL_VECTOR(_typename, _type)                       \
  void shmem_##_typename##_wait_until_all_vector(_type *ivars, size_t nelems,  \
                                                 const int *status, int cmp,   \
                                                 _type *cmp_values);

#define DECL_WAIT_UNTIL_ALL_VECTOR(_type, _typename)                           \
  API_DECL_WAIT_UNTIL_ALL_VECTOR(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_WAIT_UNTIL_ALL_VECTOR)
#undef DECL_WAIT_UNTIL_ALL_VECTOR

#undef API_DECL_WAIT_UNTIL_ALL_VECTOR

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Wait until any variable in an array meets its condition using a vector
 * of comparison values
 * @page shmem_long_wait_until_any_vector
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * size_t shmem_long_wait_until_any_vector(long *ivars, size_t nelems, const int
 * *status, int cmp, long *cmp_values);
 * @endcode
 *
 * @param ivars      Array of symmetric variables to wait on
 * @param nelems     Number of elements in the ivars array
 * @param status     Array indicating which ivars elements to monitor
 * @param cmp        Comparison operator from shmem_cmp_constants
 * @param cmp_values Array of values to compare against each element
 *
 * @section Effect
 * Waits until any element in ivars array meets its condition specified by the
 * comparison operator and corresponding comparison value in cmp_values array.
 * Only elements with corresponding non-zero status values are monitored.
 *
 * @section Return
 * Index of the first element that meets its condition.
 */
#define API_DECL_WAIT_UNTIL_ANY_VECTOR(_typename, _type)                       \
  size_t shmem_##_typename##_wait_until_any_vector(                            \
      _type *ivars, size_t nelems, const int *status, int cmp,                 \
      _type *cmp_values);

#define DECL_WAIT_UNTIL_ANY_VECTOR(_type, _typename)                           \
  API_DECL_WAIT_UNTIL_ANY_VECTOR(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_WAIT_UNTIL_ANY_VECTOR)
#undef DECL_WAIT_UNTIL_ANY_VECTOR

#undef API_DECL_WAIT_UNTIL_ANY_VECTOR

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Wait until some variables in an array meet their conditions using a
 * vector of comparison values
 * @page shmem_long_wait_until_some_vector
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * size_t shmem_long_wait_until_some_vector(long *ivars, size_t nelems, size_t
 * *indices, const int *status, int cmp, long *cmp_values);
 * @endcode
 *
 * @param ivars      Array of symmetric variables to wait on
 * @param nelems     Number of elements in the ivars array
 * @param indices    Array to store indices of elements that meet their
 * conditions
 * @param status     Array indicating which ivars elements to monitor
 * @param cmp        Comparison operator from shmem_cmp_constants
 * @param cmp_values Array of values to compare against each element
 *
 * @section Effect
 * Waits until at least one element in ivars array meets its condition specified
 * by the comparison operator and corresponding comparison value in cmp_values
 * array. Only elements with corresponding non-zero status values are monitored.
 * Indices of elements that meet their conditions are stored in the indices
 * array.
 *
 * @section Return
 * Number of elements that meet their conditions.
 */
#define API_DECL_WAIT_UNTIL_SOME_VECTOR(_typename, _type)                      \
  size_t shmem_##_typename##_wait_until_some_vector(                           \
      _type *ivars, size_t nelems, size_t *indices, const int *status,         \
      int cmp, _type *cmp_values);

#define DECL_WAIT_UNTIL_SOME_VECTOR(_type, _typename)                          \
  API_DECL_WAIT_UNTIL_SOME_VECTOR(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_WAIT_UNTIL_SOME_VECTOR)
#undef DECL_WAIT_UNTIL_SOME_VECTOR

#undef API_DECL_WAIT_UNTIL_SOME_VECTOR

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief wait for symmetric variable to change value
 * @page shmem_long_wait
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_long_wait(long *ivar, long cmp_value);
 @endcode
 *
 * @section Effect
 *
 * ivar updated by another PE, wait for that to happen
 *
 * @section Return
 * None.
 *
 * @section Deprecated
 * by \ref shmem_long_wait_until
 *
 */
#define API_DECL_WAIT(_typename, _type)                                        \
  void shmem_##_typename##_##wait(_type *ivar, _type cmp_value)                \
      _DEPRECATED_BY(                                                          \
          shmem_##_typename##_wait_until with compare operator SHMEM_CMP_NE,   \
          1.4);

#define DECL_WAIT(_type, _typename) API_DECL_WAIT(_typename, _type)
SHMEM_DEPR_PT2PT_SYNC_TYPE_TABLE(DECL_WAIT)
#undef DECL_WAIT

#undef API_DECL_WAIT

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief swap value into symmetric variable, fetch back old value
 * @page shmem_long_atomic_swap
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 long shmem_ctx_long_atomic_swap(shmem_ctx_t ctx, long *target, long value, int
 pe);
 @endcode
 *
 * @section Effect
 *
 * swaps contents of remote variable with supplied value
 *
 * @section Return
 * None.
 *
 */
#define API_DECL_ATOMIC_SWAP(_typename, _type)                                 \
  _type shmem_##_typename##_atomic_swap(_type *dest, _type value, int pe)      \
      _WUR;                                                                    \
  _type shmem_ctx_##_typename##_atomic_swap(shmem_ctx_t ctx, _type *dest,      \
                                            _type value, int pe) _WUR;

#define DECL_ATOMIC_SWAP(_type, _typename)                                     \
  API_DECL_ATOMIC_SWAP(_typename, _type)
SHMEM_EXTENDED_AMO_TYPE_TABLE(DECL_ATOMIC_SWAP)
#undef DECL_ATOMIC_SWAP

#undef API_DECL_ATOMIC_SWAP

////////////////////////////////////////////////////////////////////////////////
#define API_DECL_ATOMIC_SWAP_NBI(_typename, _type)                             \
  void shmem_##_typename##_atomic_swap_nbi(_type *fetch, _type *target,        \
                                           _type value, int pe);               \
  void shmem_ctx_##_typename##_atomic_swap_nbi(                                \
      shmem_ctx_t ctx, _type *fetch, _type *target, _type value, int pe);

#define DECL_ATOMIC_SWAP_NBI(_type, _typename)                                 \
  API_DECL_ATOMIC_SWAP_NBI(_typename, _type)
SHMEM_EXTENDED_AMO_TYPE_TABLE(DECL_ATOMIC_SWAP_NBI)
#undef DECL_ATOMIC_SWAP_NBI

#undef API_DECL_ATOMIC_SWAP_NBI

long shmem_long_swap(long *target, long value, int pe)
    _DEPRECATED_BY(shmem_long_atomic_swap, 1.4) _WUR;
int shmem_int_swap(int *target, int value, int pe)
    _DEPRECATED_BY(shmem_int_atomic_swap, 1.4) _WUR;
long long shmem_longlong_swap(long long *target, long long value, int pe)
    _DEPRECATED_BY(shmem_longlong_atomic_swap, 1.4) _WUR;
float shmem_float_swap(float *target, float value, int pe)
    _DEPRECATED_BY(shmem_float_atomic_swap, 1.4) _WUR;
double shmem_double_swap(double *target, double value, int pe)
    _DEPRECATED_BY(shmem_double_atomic_swap, 1.4) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief conditionally swap value into symmetric variable, fetch
 * back old value
 * @page shmem_long_atomic_compare_swap
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 long shmem_ctx_long_atomic_compare_swap(shmem_ctx_t ctx,
                                         long *target,
                                         long cond, long value,
                                         int pe);
 @endcode
 *
 * @section Effect
 *
 * Performs an atomic swap of @a value but only if @a cond is
 * equal to the old value on the remote PE.
 *
 * @section Return
 * None.
 *
 */
#define API_DECL_ATOMIC_COMPARE_SWAP(_typename, _type)                         \
  _type shmem_##_typename##_atomic_compare_swap(_type *dest, _type cond,       \
                                                _type value, int pe);          \
  _type shmem_ctx_##_typename##_atomic_compare_swap(                           \
      shmem_ctx_t ctx, _type *dest, _type cond, _type value, int pe) _WUR;

#define DECL_ATOMIC_COMPARE_SWAP(_type, _typename)                             \
  API_DECL_ATOMIC_COMPARE_SWAP(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_ATOMIC_COMPARE_SWAP)
#undef DECL_ATOMIC_COMPARE_SWAP

#undef API_DECL_ATOMIC_COMPARE_SWAP

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Non-blocking atomic conditional swap operation
 * @page shmem_atomic_compare_swap_nbi
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_ctx_long_atomic_compare_swap_nbi(shmem_ctx_t ctx, long *fetch,
 *                                            long *dest, long cond, long value,
 *                                            int pe);
 * @endcode
 *
 * @param fetch    Local address where to store the old value from the target PE
 * @param dest     Address of the symmetric data object to be updated on the
 * target PE
 * @param cond     Value to be compared with the value at dest on the target PE
 * @param value    Value to be atomically written if cond equals the value at
 * dest
 * @param pe       PE number of the target PE
 *
 * @section Effect
 * Performs a non-blocking atomic conditional swap operation. If the value at
 * address dest on PE pe equals cond, then value is written to address dest on
 * PE pe. The old value at dest on PE pe is returned in fetch. The operation is
 * non-blocking in that it may return before the operation is completed at the
 * target PE.
 *
 * @section Return
 * None.
 */
#define API_DECL_ATOMIC_COMPARE_SWAP_NBI(_typename, _type)                     \
  void shmem_##_typename##_atomic_compare_swap_nbi(                            \
      _type *fetch, _type *dest, _type cond, _type value, int pe);             \
  void shmem_ctx_##_typename##_atomic_compare_swap_nbi(                        \
      shmem_ctx_t ctx, _type *fetch, _type *dest, _type cond, _type value,     \
      int pe);

#define DECL_ATOMIC_COMPARE_SWAP_NBI(_type, _typename)                         \
  API_DECL_ATOMIC_COMPARE_SWAP_NBI(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_ATOMIC_COMPARE_SWAP_NBI)
#undef DECL_ATOMIC_COMPARE_SWAP_NBI

long shmem_long_cswap(long *target, long cond, long value, int pe)
    _DEPRECATED_BY(shmem_long_atomic_compare_swap, 1.4) _WUR;
int shmem_int_cswap(int *target, int cond, int value, int pe)
    _DEPRECATED_BY(shmem_int_atomic_compare_swap, 1.4) _WUR;
long long shmem_longlong_cswap(long long *target, long long cond,
                               long long value, int pe)
    _DEPRECATED_BY(shmem_longlong_atomic_compare_swap, 1.4) _WUR;

#undef API_DECL_CSWAP_NBI

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief add value to symmetric variable, fetch back old value
 * @page shmem_long_atomic_fetch_add
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 long shmem_long_atomic_fetch_add(long *target, long value, int pe);
 @endcode
 *
 * @section Effect
 *
 * atomic fetch-and-add on another PE
 *
 * @section Return
 * None.
 *
 */
#define API_DECL_ATOMIC_FETCH_ADD(_typename, _type)                            \
  _type shmem_##_typename##_atomic_fetch_add(_type *dest, _type value, int pe) \
      _WUR;                                                                    \
  _type shmem_ctx_##_typename##_atomic_fetch_add(shmem_ctx_t ctx, _type *dest, \
                                                 _type value, int pe) _WUR;

#define DECL_ATOMIC_FETCH_ADD(_type, _typename)                                \
  API_DECL_ATOMIC_FETCH_ADD(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_ATOMIC_FETCH_ADD)
#undef DECL_ATOMIC_FETCH_ADD
#undef API_DECL_ATOMIC_FETCH_ADD

int shmem_int_fadd(int *target, int value, int pe)
    _DEPRECATED_BY(shmem_int_atomic_fetch_add, 1.4) _WUR;
long shmem_long_fadd(long *target, long value, int pe)
    _DEPRECATED_BY(shmem_long_atomic_fetch_add, 1.4) _WUR;
long long shmem_longlong_fadd(long long *target, long long value, int pe)
    _DEPRECATED_BY(shmem_longlong_atomic_fetch_add, 1.4) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Non-blocking atomic fetch-and-add on a remote PE
 * @page shmem_long_atomic_fetch_add_nbi
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_long_atomic_fetch_add_nbi(long *fetch, const long *dest, long
 * value, int pe);
 * @endcode
 *
 * @param fetch    Local address to store fetched value
 * @param dest     Address of symmetric data object on target PE
 * @param value    Value to be atomically added
 * @param pe       PE number of target PE
 *
 * @section Effect
 * Atomically adds value to dest on PE pe and returns the previous contents of
 * dest through fetch without blocking. The operation must be completed without
 * the possibility of another process updating dest between the fetch and add.
 */
#define API_DECL_ATOMIC_FETCH_ADD_NBI(_typename, _type)                        \
  void shmem_##_typename##_atomic_fetch_add_nbi(                               \
      _type *fetch, const _type *dest, _type value, int pe);                   \
  void shmem_ctx_##_typename##_atomic_fetch_add_nbi(                           \
      shmem_ctx_t ctx, _type *fetch, const _type *dest, _type value, int pe);

#define DECL_ATOMIC_FETCH_ADD_NBI(_type, _typename)                            \
  API_DECL_ATOMIC_FETCH_ADD_NBI(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_ATOMIC_FETCH_ADD_NBI)
#undef DECL_ATOMIC_FETCH_ADD_NBI
#undef API_DECL_ATOMIC_FETCH_ADD_NBI

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief increment symmetric variable, fetch back old value
 * @page shmem_long_atomic_fetch_inc
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 long shmem_long_atomic_fetch_inc(long *target, int pe);
 @endcode
 *
 * @section Effect
 *
 * atomic fetch-and-increment on another PE
 *
 * @section Return
 * None.
 *
 */
#define API_DECL_ATOMIC_FETCH_INC(_typename, _type)                            \
  _type shmem_##_typename##_atomic_fetch_inc(_type *dest, int pe) _WUR;        \
  _type shmem_ctx_##_typename##_atomic_fetch_inc(shmem_ctx_t ctx, _type *dest, \
                                                 int pe) _WUR;

#define DECL_ATOMIC_FETCH_INC(_type, _typename)                                \
  API_DECL_ATOMIC_FETCH_INC(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_ATOMIC_FETCH_INC)
#undef DECL_ATOMIC_FETCH_INC
#undef API_DECL_ATOMIC_FETCH_INC

long shmem_long_finc(long *target, int pe)
    _DEPRECATED_BY(shmem_long_atomic_fetch_inc, 1.4) _WUR;
int shmem_int_finc(int *target, int pe)
    _DEPRECATED_BY(shmem_int_atomic_fetch_inc, 1.4) _WUR;
long long shmem_longlong_finc(long long *target, int pe)
    _DEPRECATED_BY(shmem_longlong_atomic_fetch_inc, 1.4) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform a non-blocking atomic fetch-and-increment
 * operation
 * @page shmem_atomic_fetch_inc_nbi
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_atomic_fetch_inc_nbi(TYPE *fetch, TYPE *dest, int pe);
 * void shmem_ctx_atomic_fetch_inc_nbi(shmem_ctx_t ctx, TYPE *fetch, TYPE *dest,
 * int pe);
 * @endcode
 *
 * @param fetch    Local address to store fetched value
 * @param dest     Address of symmetric data object on target PE
 * @param pe       PE number of target PE
 *
 * @section Effect
 * Atomically performs a non-blocking fetch of the old value at dest on PE pe
 * and increments dest. The old value from dest is returned in fetch. The
 * operation must be completed without the possibility of another process
 * updating dest between the fetch and the increment.
 *
 * @section Return
 * None.
 */
#define API_DECL_ATOMIC_FETCH_INC_NBI(_typename, _type)                        \
  void shmem_##_typename##_atomic_fetch_inc_nbi(_type *fetch, _type *dest,     \
                                                int pe);                       \
  void shmem_ctx_##_typename##_atomic_fetch_inc_nbi(                           \
      shmem_ctx_t ctx, _type *fetch, _type *dest, int pe);

#define DECL_ATOMIC_FETCH_INC_NBI(_type, _typename)                            \
  API_DECL_ATOMIC_FETCH_INC_NBI(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_ATOMIC_FETCH_INC_NBI)
#undef DECL_ATOMIC_FETCH_INC_NBI
#undef API_DECL_ATOMIC_FETCH_INC_NBI

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief add value to symmetric variable
 * @page shmem_long_atomic_inc
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_long_atomic_add(long *target, long value, int pe);
 @endcode
 *
 * @section Effect
 *
 * atomic add on another PE
 *
 * @section Return
 * None.
 *
 */
#define API_DECL_ATOMIC_ADD(_typename, _type)                                  \
  void shmem_##_typename##_atomic_add(_type *dest, _type value, int pe);       \
  void shmem_ctx_##_typename##_atomic_add(shmem_ctx_t ctx, _type *dest,        \
                                          _type value, int pe);

#define DECL_ATOMIC_ADD(_type, _typename) API_DECL_ATOMIC_ADD(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_ATOMIC_ADD)
#undef DECL_ATOMIC_ADD
#undef API_DECL_ATOMIC_ADD

void shmem_long_add(long *target, long value, int pe)
    _DEPRECATED_BY(shmem_long_atomic_add, 1.4);
void shmem_int_add(int *target, int value, int pe)
    _DEPRECATED_BY(shmem_int_atomic_add, 1.4);
void shmem_longlong_add(long long *target, long long value, int pe)
    _DEPRECATED_BY(shmem_longlong_atomic_add, 1.4);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform an atomic "or" operation
 * between a data value and the target data object.
 * @page shmem_ulong_atomic_or
 * @section Synopsis
 *
 * - C/C++:
 * @code
 void shmem_ulong_atomic_or(unsigned long *dest, unsigned long value, int pe);
 * @endcode
 *
 * @param dest    Address of the symmetric data object where to save
 *                    the data on the target pe.
 * @param value     The value with which the exclusive-or operation is
 *                    atomically performed with the data at address dest.
 * @param pe        An integer that indicates the PE number upon
 *                which dest is to be updated.
 *
 * @section Constraints
 *      - dest must be the address of a symmetric data object.
 *      - If using C/C++, the type of value must match that implied in
 *        the Synopsis section.
 *      - value must be the same type as the target data object.
 *      - This process must be carried out guaranteeing that it will not
 *          be interrupted by any other atomic operation on the
 *          specified type.
 *
 * @section Effect
 *
 * The atomic or routines perform a bitwise or-operation between
 * value and the data at address dest on PE pe. The operation must
 * be completed without the possibility of another process updating
 * dest at the same time.
 *
 * @section Return
 * None.
 *
 */
#define API_DECL_ATOMIC_OR(_typename, _type)                                   \
  void shmem_##_typename##_atomic_or(_type *dest, _type value, int pe);        \
  void shmem_ctx_##_typename##_atomic_or(shmem_ctx_t ctx, _type *dest,         \
                                         _type value, int pe);

#define DECL_ATOMIC_OR(_type, _typename) API_DECL_ATOMIC_OR(_typename, _type)
SHMEM_BITWISE_AMO_TYPE_TABLE(DECL_ATOMIC_OR)
#undef DECL_ATOMIC_OR
#undef API_DECL_ATOMIC_OR

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform an atomic "or"
 * operation between a data value and the target data object and
 * return the former remote value.
 * @page shmem_ulong_atomic_fetch_or
 * @section Synopsis
 *
 * - C/C++:
 * @code
 long shmem_ulong_atomic_fetch_or(unsigned long *dest, unsigned long value, int
 pe);
 * @endcode
 *
 * @param dest    Address of the symmetric data object where to save
 *                    the data on the target pe.
 * @param value     The value with which the exclusive-or operation is
 *                    atomically performed with the data at address dest.
 * @param pe        An integer that indicates the PE number upon
 *                which dest is to be updated.
 *
 * @section Constraints
 *      - dest must be the address of a symmetric data object.
 *      - If using C/C++, the type of value must match that implied in
 *        the Synopsis section.
 *      - value must be the same type as the target data object.
 *      - This process must be carried out guaranteeing that it will not
 *          be interrupted by any other atomic operation on the
 *          specified type.
 *
 * @section Effect
 *
 * The atomic and routines perform a bitwise and-operation between
 * value and the data at address dest on PE pe. The operation must
 * be completed without the possibility of another process updating
 * dest at the same time.
 *
 * @section Return
 * Value stored previously in remote location.
 *
 */
#define API_DECL_ATOMIC_FETCH_OR(_typename, _type)                             \
  _type shmem_##_typename##_atomic_fetch_or(_type *target, _type value,        \
                                            int pe) _WUR;                      \
  _type shmem_ctx_##_typename##_atomic_fetch_or(                               \
      shmem_ctx_t ctx, _type *target, _type value, int pe) _WUR;

#define DECL_ATOMIC_FETCH_OR(_type, _typename)                                 \
  API_DECL_ATOMIC_FETCH_OR(_typename, _type)
SHMEM_BITWISE_AMO_TYPE_TABLE(DECL_ATOMIC_FETCH_OR)
#undef DECL_ATOMIC_FETCH_OR
#undef API_DECL_ATOMIC_FETCH_OR

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform a non-blocking atomic fetch-or operation
 * @page shmem_atomic_fetch_or_nbi
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_atomic_fetch_or_nbi(TYPE *fetch, TYPE *target, TYPE value, int
 * pe); void shmem_ctx_atomic_fetch_or_nbi(shmem_ctx_t ctx, TYPE *fetch, TYPE
 * *target, TYPE value, int pe);
 * @endcode
 *
 * @param fetch    Local address to store fetched value
 * @param target   Address of symmetric data object on target PE
 * @param value    Value to be combined with target
 * @param pe       PE number of target PE
 *
 * @section Effect
 * Atomically performs a non-blocking fetch of the old value at target on PE pe
 * and performs a bitwise OR with value. The old value from target is returned
 * in fetch. The operation must be completed without the possibility of another
 * process updating target between the fetch and the OR.
 */
#define API_DECL_FETCH_OR_NBI(_typename, _type)                                \
  void shmem_##_typename##_atomic_fetch_or_nbi(_type *fetch, _type *target,    \
                                               _type value, int pe);           \
  void shmem_ctx_##_typename##_atomic_fetch_or_nbi(                            \
      shmem_ctx_t ctx, _type *fetch, _type *target, _type value, int pe);

#define DECL_FETCH_OR_NBI(_type, _typename)                                    \
  API_DECL_FETCH_OR_NBI(_typename, _type)
SHMEM_BITWISE_AMO_TYPE_TABLE(DECL_FETCH_OR_NBI)
#undef DECL_FETCH_OR_NBI
#undef API_DECL_FETCH_OR_NBI

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform an atomic "and" operation
 * between a data value and the target data object.
 * @page shmem_ulong_atomic_and
 * @section Synopsis
 *
 * - C/C++:
 * @code
 void shmem_ulong_atomic_and(unsigned long *dest, unsigned long value, int pe);
 * @endcode
 *
 * @param dest    Address of the symmetric data object where to save
 *                    the data on the target pe.
 * @param value     The value with which the exclusive-or operation is
 *                    atomically performed with the data at address dest.
 * @param pe        An integer that indicates the PE number upon
 *                which dest is to be updated.
 *
 * @section Constraints
 *      - dest must be the address of a symmetric data object.
 *      - If using C/C++, the type of value must match that implied in
 *        the Synopsis section.
 *      - value must be the same type as the target data object.
 *      - This process must be carried out guaranteeing that it will not
 *          be interrupted by any other atomic operation on the
 *          specified type.
 *
 * @section Effect
 *
 * The atomic exclusive-or routines perform an and-operation between
 * value and the data at address dest on PE pe. The operation must
 * be completed without the possibility of another process updating
 * dest between the time of the fetch and the update.
 *
 * @section Return
 * None.
 *
 */

#define API_DECL_ATOMIC_AND(_typename, _type)                                  \
  void shmem_##_typename##_atomic_and(_type *dest, _type value, int pe);       \
  void shmem_ctx_##_typename##_atomic_and(shmem_ctx_t ctx, _type *dest,        \
                                          _type value, int pe);

#define DECL_ATOMIC_AND(_type, _typename) API_DECL_ATOMIC_AND(_typename, _type)
SHMEM_BITWISE_AMO_TYPE_TABLE(DECL_ATOMIC_AND)
#undef DECL_ATOMIC_AND
#undef API_DECL_ATOMIC_AND

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform an atomic "and"
 * operation between a data value and the target data object and
 * return the former remote value.
 * @page shmem_ulong_atomic_fetch_and
 * @section Synopsis
 *
 * - C/C++:
 * @code
 long shmem_ulong_atomic_fetch_and(unsigned long *dest, unsigned long value, int
 pe);
 * @endcode
 *
 * @param dest    Address of the symmetric data object where to save
 *                    the data on the target pe.
 * @param value     The value with which the exclusive-or operation is
 *                    atomically performed with the data at address dest.
 * @param pe        An integer that indicates the PE number upon
 *                which dest is to be updated.
 *
 * @section Constraints
 *      - dest must be the address of a symmetric data object.
 *      - If using C/C++, the type of value must match that implied in
 *        the Synopsis section.
 *      - value must be the same type as the target data object.
 *      - This process must be carried out guaranteeing that it will not
 *          be interrupted by any other atomic operation on the
 *          specified type.
 *
 * @section Effect
 *
 * The atomic exclusive-or routines perform an and-operation between
 * value and the data at address dest on PE pe. The operation must
 * be completed without the possibility of another process updating
 * dest between the time of the fetch and the update.
 *
 * @section Return
 * Value stored previously in remote location.
 *
 */

#define API_DECL_ATOMIC_FETCH_AND(_typename, _type)                            \
  _type shmem_##_typename##_atomic_fetch_and(_type *dest, _type value, int pe) \
      _WUR;                                                                    \
  _type shmem_ctx_##_typename##_atomic_fetch_and(shmem_ctx_t ctx, _type *dest, \
                                                 _type value, int pe) _WUR;

#define DECL_ATOMIC_FETCH_AND(_type, _typename)                                \
  API_DECL_ATOMIC_FETCH_AND(_typename, _type)
SHMEM_BITWISE_AMO_TYPE_TABLE(DECL_ATOMIC_FETCH_AND)
#undef DECL_ATOMIC_FETCH_AND
#undef API_DECL_ATOMIC_FETCH_AND

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Non-blocking atomic fetch-and-and operation
 * @page shmem_atomic_fetch_and_nbi
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_ctx_long_atomic_fetch_and_nbi(shmem_ctx_t ctx, long *fetch,
 *                                         long *dest, long value, int pe);
 * @endcode
 *
 * @param fetch    Local address where to store the old value from the target PE
 * @param dest     Address of the symmetric data object to be updated on the
 * target PE
 * @param value    Value to be atomically ANDed with the value at dest
 * @param pe       PE number of the target PE
 *
 * @section Effect
 * Performs a non-blocking atomic fetch-and-and operation. The old value at dest
 * on PE pe is returned in fetch and atomically ANDed with value. The operation
 * is non-blocking in that it may return before the operation is completed at
 * the target PE.
 *
 * @section Return
 * None.
 */
#define API_DECL_FETCH_AND_NBI(_typename, _type)                               \
  void shmem_##_typename##_atomic_fetch_and_nbi(_type *fetch, _type *dest,     \
                                                _type value, int pe);          \
  void shmem_ctx_##_typename##_atomic_fetch_and_nbi(                           \
      shmem_ctx_t ctx, _type *fetch, _type *dest, _type value, int pe);

#define DECL_FETCH_AND_NBI(_type, _typename)                                   \
  API_DECL_FETCH_AND_NBI(_typename, _type)
SHMEM_BITWISE_AMO_TYPE_TABLE(DECL_FETCH_AND_NBI)
#undef DECL_FETCH_AND_NBI
#undef API_DECL_FETCH_AND_NBI

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform an atomic "xor" operation
 * between a data value and the target data object.
 * @page shmem_ulong_atomic_xor
 * @section Synopsis
 *
 * - C/C++:
 * @code
 void shmem_ulong_atomic_xor(unsigned long *dest, unsigned long value, int pe);
 * @endcode
 *
 * @param dest    Address of the symmetric data object where to save
 *                    the data on the target pe.
 * @param value     The value with which the exclusive-or operation is
 *                    atomically performed with the data at address dest.
 * @param pe        An integer that indicates the PE number upon
 *                which dest is to be updated.
 *
 * @section Constraints
 *      - dest must be the address of a symmetric data object.
 *      - If using C/C++, the type of value must match that implied in
 *        the Synopsis section.
 *      - value must be the same type as the target data object.
 *      - This process must be carried out guaranteeing that it will not
 *          be interrupted by any other atomic operation on the
 *          specified type.
 *
 * @section Effect
 *
 * The atomic exclusive-or routines perform an xor-operation between
 * value and the data at address dest on PE pe. The operation must
 * be completed without the possibility of another process updating
 * dest between the time of the fetch and the update.
 *
 * @section Return
 * None.
 *
 */
#define API_DECL_ATOMIC_XOR(_typename, _type)                                  \
  void shmem_##_typename##_atomic_xor(_type *dest, _type value, int pe);       \
  void shmem_ctx_##_typename##_atomic_xor(shmem_ctx_t ctx, _type *dest,        \
                                          _type value, int pe);

#define DECL_ATOMIC_XOR(_type, _typename) API_DECL_ATOMIC_XOR(_typename, _type)
SHMEM_BITWISE_AMO_TYPE_TABLE(DECL_ATOMIC_XOR)
#undef DECL_ATOMIC_XOR
#undef API_DECL_ATOMIC_XOR

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform a non-blocking atomic xor operation
 * @page shmem_atomic_xor_nbi
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_atomic_xor_nbi(TYPE *fetch, TYPE *target, TYPE value, int pe);
 * void shmem_ctx_atomic_xor_nbi(shmem_ctx_t ctx, TYPE *fetch, TYPE *target,
 *                               TYPE value, int pe);
 * @endcode
 *
 * @param fetch    Local address to store fetched value
 * @param dest     Address of symmetric data object on target PE
 * @param value    Value to be combined with target
 * @param pe       PE number of target PE
 *
 * @section Effect
 * Atomically performs a non-blocking fetch of the old value at dest on PE pe
 * and performs a bitwise XOR with value. The old value from dest is returned
 * in fetch. The operation must be completed without the possibility of another
 * process updating dest between the fetch and the XOR.
 */
#define API_DECL_ATOMIC_XOR_NBI(_typename, _type)                              \
  void shmem_##_typename##_atomic_xor_nbi(_type *fetch, _type *dest,           \
                                          _type value, int pe);                \
  void shmem_ctx_##_typename##_atomic_xor_nbi(                                 \
      shmem_ctx_t ctx, _type *fetch, _type *dest, _type value, int pe);

#define DECL_ATOMIC_XOR_NBI(_type, _typename)                                  \
  API_DECL_ATOMIC_XOR_NBI(_typename, _type)
SHMEM_BITWISE_AMO_TYPE_TABLE(DECL_ATOMIC_XOR_NBI)
#undef DECL_ATOMIC_XOR_NBI
#undef API_DECL_ATOMIC_XOR_NBI

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform an atomic "xor"
 * operation between a data value and the target data object and
 * return the former remote value.
 * @page shmem_ulong_atomic_fetch_xor
 * @section Synopsis
 *
 * - C/C++:
 * @code
 long shmem_ulong_atomic_fetch_xor(unsigned long *dest, unsigned long value, int
 pe);
 * @endcode
 *
 * @param dest    Address of the symmetric data object where to save
 *                    the data on the target pe.
 * @param value     The value with which the exclusive-or operation is
 *                    atomically performed with the data at address dest.
 * @param pe        An integer that indicates the PE number upon
 *                which dest is to be updated.
 *
 * @section Constraints
 *      - dest must be the address of a symmetric data object.
 *      - If using C/C++, the type of value must match that implied in
 *        the Synopsis section.
 *      - value must be the same type as the target data object.
 *      - This process must be carried out guaranteeing that it will not
 *          be interrupted by any other atomic operation on the
 *          specified type.
 *
 * @section Effect
 *
 * The atomic exclusive-or routines perform an xor-operation between
 * value and the data at address dest on PE pe. The operation must
 * be completed without the possibility of another process updating
 * dest between the time of the fetch and the update.
 *
 * @section Return
 *
 * Value stored previously in remote location.
 *
 */
#define API_DECL_ATOMIC_FETCH_XOR(_typename, _type)                            \
  _type shmem_##_typename##_atomic_fetch_xor(_type *dest, _type value, int pe) \
      _WUR;                                                                    \
  _type shmem_ctx_##_typename##_atomic_fetch_xor(shmem_ctx_t ctx, _type *dest, \
                                                 _type value, int pe) _WUR;

#define DECL_ATOMIC_FETCH_XOR(_type, _typename)                                \
  API_DECL_ATOMIC_FETCH_XOR(_typename, _type)
SHMEM_BITWISE_AMO_TYPE_TABLE(DECL_ATOMIC_FETCH_XOR)
#undef DECL_ATOMIC_FETCH_XOR
#undef API_DECL_ATOMIC_FETCH_XOR

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform a non-blocking atomic fetch-xor operation
 * @page shmem_atomic_fetch_xor_nbi
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_atomic_fetch_xor_nbi(TYPE *fetch, TYPE *target, TYPE value, int
 * pe); void shmem_ctx_atomic_fetch_xor_nbi(shmem_ctx_t ctx, TYPE *fetch, TYPE
 * *target, TYPE value, int pe);
 * @endcode
 *
 * @param fetch    Local address to store fetched value
 * @param dest     Address of symmetric data object on target PE
 * @param value    Value to be combined with target
 * @param pe       PE number of target PE
 *
 * @section Effect
 * Atomically performs a non-blocking fetch of the old value at dest on PE pe
 * and performs a bitwise XOR with value. The old value from dest is returned
 * in fetch. The operation must be completed without the possibility of another
 * process updating dest between the fetch and the XOR.
 */
#define API_DECL_ATOMIC_FETCH_XOR_NBI(_typename, _type)                        \
  void shmem_##_typename##_atomic_fetch_xor_nbi(_type *fetch, _type *dest,     \
                                                _type value, int pe);          \
  void shmem_ctx_##_typename##_atomic_fetch_xor_nbi(                           \
      shmem_ctx_t ctx, _type *fetch, _type *dest, _type value, int pe);

#define DECL_ATOMIC_FETCH_XOR_NBI(_type, _typename)                            \
  API_DECL_ATOMIC_FETCH_XOR_NBI(_typename, _type)
SHMEM_BITWISE_AMO_TYPE_TABLE(DECL_ATOMIC_FETCH_XOR_NBI)
#undef DECL_ATOMIC_FETCH_XOR_NBI
#undef API_DECL_ATOMIC_FETCH_XOR_NBI

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief increment symmetric variable
 * @page shmem_long_atomic_inc
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_long_atomic_inc(long *target, int pe);
 @endcode
 *
 * @section Effect
 *
 * atomic increment on another PE
 *
 * @section Return
 * None.
 *
 */
#define API_DECL_ATOMIC_INC(_typename, _type)                                  \
  void shmem_##_typename##_atomic_inc(_type *dest, int pe);                    \
  void shmem_ctx_##_typename##_atomic_inc(shmem_ctx_t ctx, _type *dest, int pe);

#define DECL_ATOMIC_INC(_type, _typename) API_DECL_ATOMIC_INC(_typename, _type)
SHMEM_STANDARD_AMO_TYPE_TABLE(DECL_ATOMIC_INC)
#undef DECL_ATOMIC_INC
#undef API_DECL_ATOMIC_INC

void shmem_long_inc(long *target, int pe)
    _DEPRECATED_BY(shmem_long_atomic_inc, 1.4);
void shmem_int_inc(int *target, int pe)
    _DEPRECATED_BY(shmem_int_atomic_inc, 1.4);
void shmem_longlong_inc(long long *target, int pe)
    _DEPRECATED_BY(shmem_longlong_atomic_inc, 1.4);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform an atomic fetch from a remote PE
 * @page shmem_ctx_long_atomic_fetch
 * @section Synopsis
 *
 * - C/C++:
 * @code
 long shmem_ctx_long_atomic_fetch(shmem_ctx_t ctx, const long *dest, int pe);
 * @endcode
 *
 * @param dest    Address of the symmetric data object in which save the
 *                    data on the target pe.
 * @param pe        An integer that indicates the PE number upon
 *                    which dest is to be updated.
 *
 * @section Constraints
 *      - dest must be the address of a symmetric data object.
 *      - If using C/C++, the type of value must match that implied
 *        in the Synopsis section.
 *      - value must be the same type as the target data object.
 *      - This process must be carried out guaranteeing that it will not
 *          be interrupted by any other atomic operation on the
 *          specified type.
 *
 * @section Effect
 *
 * The atomic fetch routines atomically return the value at address
 * "dest" on PE pe. The operation must
 * be completed without the possibility of another process updating
 * dest on PE pe using the same type.
 *
 * @section Return
 * The value stored at address "dest" on PE pe.
 *
 */
#define API_DECL_ATOMIC_FETCH(_typename, _type)                                \
  _type shmem_##_typename##_atomic_fetch(const _type *source, int pe) _WUR;    \
  _type shmem_ctx_##_typename##_atomic_fetch(                                  \
      shmem_ctx_t ctx, const _type *source, int pe) _WUR;

#define DECL_ATOMIC_FETCH(_type, _typename)                                    \
  API_DECL_ATOMIC_FETCH(_typename, _type)
SHMEM_EXTENDED_AMO_TYPE_TABLE(DECL_ATOMIC_FETCH)
#undef DECL_ATOMIC_FETCH
#undef API_DECL_ATOMIC_FETCH

int shmem_int_fetch(const int *source, int pe)
    _DEPRECATED_BY(shmem_int_atomic_fetch, 1.4) _WUR;
long shmem_long_fetch(const long *source, int pe)
    _DEPRECATED_BY(shmem_long_atomic_fetch, 1.4) _WUR;
long long shmem_longlong_fetch(const long long *source, int pe)
    _DEPRECATED_BY(shmem_longlong_atomic_fetch, 1.4) _WUR;
float shmem_float_fetch(const float *source, int pe)
    _DEPRECATED_BY(shmem_float_atomic_fetch, 1.4) _WUR;
double shmem_double_fetch(const double *source, int pe)
    _DEPRECATED_BY(shmem_double_atomic_fetch, 1.4) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform a non-blocking atomic fetch operation
 * @page shmem_atomic_fetch_nbi
 * @section Synopsis
 *
 * @subsection c C/C++
 * @code
 * void shmem_atomic_fetch_nbi(TYPE *fetch, const TYPE *source, int pe);
 * void shmem_ctx_atomic_fetch_nbi(shmem_ctx_t ctx, TYPE *fetch, const TYPE
 * *source, int pe);
 * @endcode
 *
 * @param fetch    Local address to store fetched value
 * @param source   Address of symmetric data object on target PE
 * @param pe       PE number of target PE
 *
 * @section Effect
 * Atomically performs a non-blocking fetch of the value at source on PE pe. The
 * value from source is returned in fetch. The operation must be completed
 * without the possibility of another process updating source between the fetch.
 *
 * @section Return
 * None.
 */
#define API_DECL_ATOMIC_FETCH_NBI(_typename, _type)                            \
  void shmem_##_typename##_atomic_fetch_nbi(_type *fetch, const _type *source, \
                                            int pe);                           \
  void shmem_ctx_##_typename##_atomic_fetch_nbi(shmem_ctx_t ctx, _type *fetch, \
                                                const _type *source, int pe);

#define DECL_ATOMIC_FETCH_NBI(_type, _typename)                                \
  API_DECL_ATOMIC_FETCH_NBI(_typename, _type)
SHMEM_EXTENDED_AMO_TYPE_TABLE(DECL_ATOMIC_FETCH_NBI)
#undef DECL_ATOMIC_FETCH_NBI
#undef API_DECL_ATOMIC_FETCH_NBI

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief These routines perform an atomic set of a variable on a
 * remote PE
 * @page shmem_ctx_long_atomic_set
 *
 * @section Synopsis
 *
 * - C/C++:
 * @code
 void shmem_ctx_long_atomic_set(shmem_ctx_t ctx, long *dest, long value, int
 pe);
 * @endcode
 *
 * @param dest    Address of the symmetric data object in which save the
 *                    data on the target pe.
 * @param value     The remote dest address is atomically set to
 *                    this value.
 * @param pe        An integer that indicates the PE number upon
 *                    which dest is to be updated.
 *
 * @section Constraints
 *      - dest must be the address of a symmetric data object.
 *      - If using C/C++, the type of value must match that implied in the
 *        Synopsis section.
 *      - value must be the same type as the dest data object.
 *      - This process must be carried out guaranteeing that it will not
 *          be interrupted by any other atomic operation on the
 *          specified type.
 *
 * @section Effect
 *
 * The atomic set routines atomically update an address to be "value" on
 * PE pe. The operation must
 * be completed without the possibility of another process updating
 * dest on PE pe using the same type.
 *
 * @section Return
 * None.
 *
 */
#define API_DECL_ATOMIC_SET(_typename, _type)                                  \
  void shmem_##_typename##_atomic_set(_type *dest, _type value, int pe);       \
  void shmem_ctx_##_typename##_atomic_set(shmem_ctx_t ctx, _type *dest,        \
                                          _type value, int pe);

#define DECL_ATOMIC_SET(_type, _typename) API_DECL_ATOMIC_SET(_typename, _type)
SHMEM_EXTENDED_AMO_TYPE_TABLE(DECL_ATOMIC_SET)
#undef DECL_ATOMIC_SET
#undef API_DECL_ATOMIC_SET

void shmem_int_set(int *dest, int value, int pe)
    _DEPRECATED_BY(shmem_int_atomic_set, 1.4);
void shmem_long_set(long *dest, long value, int pe)
    _DEPRECATED_BY(shmem_long_atomic_set, 1.4);
void shmem_longlong_set(long long *dest, long long value, int pe)
    _DEPRECATED_BY(shmem_longlong_atomic_set, 1.4);
void shmem_float_set(float *dest, float value, int pe)
    _DEPRECATED_BY(shmem_float_atomic_set, 1.4);
void shmem_double_set(double *dest, double value, int pe)
    _DEPRECATED_BY(shmem_double_atomic_set, 1.4);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief claims a distributed lock
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_set_lock(long *lock);
 @endcode
 *
 * @param[in, out] lock a symmetric variable
 *
 * @section Effect
 *
 * The calling PE claims a lock on the symmetric variable.  Blocks
 * until lock acquired.
 *
 * @section Return
 * None.
 *
 */
void shmem_set_lock(long *lock);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief releases a distributed lock
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_clear_lock(long *lock);
 @endcode
 *
 * @param[in, out] lock a symmetric variable
 *
 * @section Effect
 *
 * The calling PE releases a lock on the symmetric variable.
 *
 * @section Return
 * None.
 *
 */
void shmem_clear_lock(long *lock);

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief tests a distributed lock
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_test_lock(long *lock);
 @endcode
 *
 * @param[in, out] lock a symmetric variable
 *
 * @section Effect
 *
 * The calling PE checks to see if lock can be acquired.  If yes,
 * the lock is claimed, otherwise the lock is not claimed and the
 * call returns immediately.  until lock acquired.
 *
 * @section Return
 * non-zero if lock acquired, 0 if not.
 *
 */
int shmem_test_lock(long *lock) _WUR;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Performs a bitwise AND reduction across a team
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_<typename>_and_reduce(shmem_team_t team, _type *dest, const _type
 *source, size_t nreduce);
 @endcode
 *
 * @param[in] team    Team on which to perform the reduction
 * @param[out] dest   Output array on all PEs in the team
 * @param[in] source  Input array on all PEs in the team
 * @param[in] nreduce Number of elements in the input/output arrays
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_AND_REDUCE_TYPE(_type, _typename)                                  \
  int shmem_##_typename##_and_reduce(shmem_team_t team, _type *dest,           \
                                     const _type *source, size_t nreduce);

#define DECL_AND_REDUCE(_type, _typename) API_AND_REDUCE_TYPE(_type, _typename)
SHMEM_REDUCE_BITWISE_TYPE_TABLE(DECL_AND_REDUCE)
#undef DECL_AND_REDUCE
#undef API_AND_REDUCE_TYPE

/**
 * @brief Performs a bitwise OR reduction across a team
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_<typename>_or_reduce(shmem_team_t team, _type *dest, const _type
 *source, size_t nreduce);
 @endcode
 *
 * @param[in] team    Team on which to perform the reduction
 * @param[out] dest   Output array on all PEs in the team
 * @param[in] source  Input array on all PEs in the team
 * @param[in] nreduce Number of elements in the input/output arrays
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_OR_REDUCE_TYPE(_type, _typename)                                   \
  int shmem_##_typename##_or_reduce(shmem_team_t team, _type *dest,            \
                                    const _type *source, size_t nreduce);

#define DECL_OR_REDUCE(_type, _typename) API_OR_REDUCE_TYPE(_type, _typename)
SHMEM_REDUCE_BITWISE_TYPE_TABLE(DECL_OR_REDUCE)
#undef DECL_OR_REDUCE
#undef API_OR_REDUCE_TYPE

/**
 * @brief Performs a bitwise XOR reduction across a team
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_<typename>_xor_reduce(shmem_team_t team, _type *dest, const _type
 *source, size_t nreduce);
 @endcode
 *
 * @param[in] team    Team on which to perform the reduction
 * @param[out] dest   Output array on all PEs in the team
 * @param[in] source  Input array on all PEs in the team
 * @param[in] nreduce Number of elements in the input/output arrays
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_XOR_REDUCE_TYPE(_type, _typename)                                  \
  int shmem_##_typename##_xor_reduce(shmem_team_t team, _type *dest,           \
                                     const _type *source, size_t nreduce);

#define DECL_XOR_REDUCE(_type, _typename) API_XOR_REDUCE_TYPE(_type, _typename)
SHMEM_REDUCE_BITWISE_TYPE_TABLE(DECL_XOR_REDUCE)
#undef DECL_XOR_REDUCE
#undef API_XOR_REDUCE_TYPE

/**
 * @brief Performs a maximum value reduction across a team
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_<typename>_max_reduce(shmem_team_t team, _type *dest, const _type
 *source, size_t nreduce);
 @endcode
 *
 * @param[in] team    Team on which to perform the reduction
 * @param[out] dest   Output array on all PEs in the team
 * @param[in] source  Input array on all PEs in the team
 * @param[in] nreduce Number of elements in the input/output arrays
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_MAX_REDUCE_TYPE(_type, _typename)                                  \
  int shmem_##_typename##_max_reduce(shmem_team_t team, _type *dest,           \
                                     const _type *source, size_t nreduce);

#define DECL_MAX_REDUCE(_type, _typename) API_MAX_REDUCE_TYPE(_type, _typename)
SHMEM_REDUCE_MINMAX_TYPE_TABLE(DECL_MAX_REDUCE)
#undef DECL_MAX_REDUCE
#undef API_MAX_REDUCE_TYPE

/**
 * @brief Performs a minimum value reduction across a team
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_<typename>_min_reduce(shmem_team_t team, _type *dest, const _type
 *source, size_t nreduce);
 @endcode
 *
 * @param[in] team    Team on which to perform the reduction
 * @param[out] dest   Output array on all PEs in the team
 * @param[in] source  Input array on all PEs in the team
 * @param[in] nreduce Number of elements in the input/output arrays
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_MIN_REDUCE_TYPE(_type, _typename)                                  \
  int shmem_##_typename##_min_reduce(shmem_team_t team, _type *dest,           \
                                     const _type *source, size_t nreduce);

#define DECL_MIN_REDUCE(_type, _typename) API_MIN_REDUCE_TYPE(_type, _typename)
SHMEM_REDUCE_MINMAX_TYPE_TABLE(DECL_MIN_REDUCE)
#undef DECL_MIN_REDUCE
#undef API_MIN_REDUCE_TYPE

/**
 * @brief Performs a sum reduction across a team
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_<typename>_sum_reduce(shmem_team_t team, _type *dest, const _type
 *source, size_t nreduce);
 @endcode
 *
 * @param[in] team    Team on which to perform the reduction
 * @param[out] dest   Output array on all PEs in the team
 * @param[in] source  Input array on all PEs in the team
 * @param[in] nreduce Number of elements in the input/output arrays
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_SUM_REDUCE_TYPE(_type, _typename)                                  \
  int shmem_##_typename##_sum_reduce(shmem_team_t team, _type *dest,           \
                                     const _type *source, size_t nreduce);

#define DECL_SUM_REDUCE(_type, _typename) API_SUM_REDUCE_TYPE(_type, _typename)
SHMEM_REDUCE_ARITH_TYPE_TABLE(DECL_SUM_REDUCE)
#undef DECL_SUM_REDUCE
#undef API_SUM_REDUCE_TYPE

/**
 * @brief Performs a product reduction across a team
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_<typename>_prod_reduce(shmem_team_t team, _type *dest, const _type
 *source, size_t nreduce);
 @endcode
 *
 * @param[in] team    Team on which to perform the reduction
 * @param[out] dest   Output array on all PEs in the team
 * @param[in] source  Input array on all PEs in the team
 * @param[in] nreduce Number of elements in the input/output arrays
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_PROD_REDUCE_TYPE(_type, _typename)                                 \
  int shmem_##_typename##_prod_reduce(shmem_team_t team, _type *dest,          \
                                      const _type *source, size_t nreduce);

#define DECL_PROD_REDUCE(_type, _typename)                                     \
  API_PROD_REDUCE_TYPE(_type, _typename)
SHMEM_REDUCE_ARITH_TYPE_TABLE(DECL_PROD_REDUCE)
#undef DECL_PROD_REDUCE
#undef API_PROD_REDUCE_TYPE

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Macro to declare legacy reduction operations (deprecated)
 */
#define API_TO_ALL_TYPE(_type, _typename, _op)                                 \
  void shmem_##_typename##_##_op##_to_all(                                     \
      _type *target, const _type *source, int nreduce, int PE_start,           \
      int logPE_stride, int PE_size, _type *pWrk, long *pSync)                 \
      _DEPRECATED_BY(shmem_##_typename##_##_op##_reduce, 1.5);

/* Declare SUM reductions */
#define DECL_SUM_TO_ALL(_type, _typename) API_TO_ALL_TYPE(_type, _typename, sum)
SHMEM_TO_ALL_ARITH_TYPE_TABLE(DECL_SUM_TO_ALL)
#undef DECL_SUM_TO_ALL

/* Declare PROD reductions */
#define DECL_PROD_TO_ALL(_type, _typename)                                     \
  API_TO_ALL_TYPE(_type, _typename, prod)
SHMEM_TO_ALL_ARITH_TYPE_TABLE(DECL_PROD_TO_ALL)
#undef DECL_PROD_TO_ALL

/* Declare AND reductions */
#define DECL_AND_TO_ALL(_type, _typename) API_TO_ALL_TYPE(_type, _typename, and)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(DECL_AND_TO_ALL)
#undef DECL_AND_TO_ALL

/* Declare OR reductions */
#define DECL_OR_TO_ALL(_type, _typename) API_TO_ALL_TYPE(_type, _typename, or)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(DECL_OR_TO_ALL)
#undef DECL_OR_TO_ALL

/* Declare XOR reductions */
#define DECL_XOR_TO_ALL(_type, _typename) API_TO_ALL_TYPE(_type, _typename, xor)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(DECL_XOR_TO_ALL)
#undef DECL_XOR_TO_ALL

/* Declare MAX reductions */
#define DECL_MAX_TO_ALL(_type, _typename) API_TO_ALL_TYPE(_type, _typename, max)
SHMEM_TO_ALL_MINMAX_TYPE_TABLE(DECL_MAX_TO_ALL)
#undef DECL_MAX_TO_ALL

/* Declare MIN reductions */
#define DECL_MIN_TO_ALL(_type, _typename) API_TO_ALL_TYPE(_type, _typename, min)
SHMEM_TO_ALL_MINMAX_TYPE_TABLE(DECL_MIN_TO_ALL)
#undef DECL_MIN_TO_ALL

#undef API_TO_ALL_TYPE

////////////////////////////////////////////////////////////////////////////////
/*
 * @brief Performs an inclusive sum scan operation
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_<typename>_sum_inscan(shmem_team_t team, _type *dest, const _type
 *source, size_t nelems);
 @endcode
 *
 * @param[in] team    Team on which to perform the sum inscan
 * @param[out] dest   Output array on all PEs in the team
 * @param[in] source  Input array on all PEs in the team
 * @param[in] nelems Number of elements in the input/output arrays
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_INSCAN_TYPE(_type, _typename)                                   \
  int shmem_##_typename##_sum_inscan(shmem_team_t team, _type *dest,        \
                                      const _type *source, size_t nelems);

#define DECL_INSCAN(_type, _typename)                                       \
  API_INSCAN_TYPE(_type, _typename)
SHMEM_SCAN_ARITH_TYPE_TABLE(DECL_INSCAN)
#undef DECL_INSCAN
#undef API_INSCAN_TYPE

/*
 * @brief Performs an exclusive sum scan operation
 *
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_<typename>_sum_exscan(shmem_team_t team, _type *dest, const _type
 *source, size_t nelems);
 @endcode
 *
 * @param[in] team    Team on which to perform the sum_exscan
 * @param[out] dest   Output array on all PEs in the team
 * @param[in] source  Input array on all PEs in the team
 * @param[in] nelems Number of elements in the input/output arrays
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_EXSCAN_TYPE(_type, _typename)                                   \
  int shmem_##_typename##_sum_exscan(shmem_team_t team, _type *dest,        \
                                      const _type *source, size_t nelems);

#define DECL_EXSCAN(_type, _typename)                                       \
  API_EXSCAN_TYPE(_type, _typename)
SHMEM_SCAN_ARITH_TYPE_TABLE(DECL_EXSCAN)
#undef DECL_EXSCAN
#undef API_EXSCAN_TYPE

////////////////////////////////////////////////////////////////////////////////
/**
 * broadcasts
 *
 * These routines perform a broadcast operation across a team. The root PE
 * broadcasts data to all other PEs in the team.
 *
 * @param team    The team over which to broadcast
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array on root PE
 * @param nelems  Number of elements to broadcast
 * @param PE_root PE number of root PE where data originates
 * @return        Zero on success, non-zero on failure
 */
#define API_BROADCAST_TYPE(_type, _typename)                                   \
  int shmem_##_typename##_broadcast(shmem_team_t team, _type *dest,            \
                                    const _type *source, size_t nelems,        \
                                    int PE_root);

#define DECL_BROADCAST(_type, _typename) API_BROADCAST_TYPE(_type, _typename)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_BROADCAST)
#undef DECL_BROADCAST
#undef API_BROADCAST_TYPE

/**
 * Generic memory broadcast routine
 *
 * @param team    The team over which to broadcast
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array on root PE
 * @param nelems  Number of elements to broadcast
 * @param PE_root PE number of root PE where data originates
 * @return        Zero on success, non-zero on failure
 */
int shmem_broadcastmem(shmem_team_t team, void *dest, const void *source,
                       size_t nelems, int PE_root);

/**
 * Legacy broadcast operations (deprecated)
 *
 * @param target       Symmetric destination array on all PEs
 * @param source       Source array on root PE
 * @param nelems       Number of elements to broadcast
 * @param PE_root      PE number of root PE where data originates
 * @param PE_start     First PE number of active set
 * @param logPE_stride Log2 of stride between consecutive PE numbers
 * @param PE_size      Number of PEs in active set
 * @param pSync        Symmetric work array
 */
#define API_BROADCAST_SIZE(_size)                                              \
  void shmem_broadcast##_size(void *target, const void *source, size_t nelems, \
                              int PE_root, int PE_start, int logPE_stride,     \
                              int PE_size, long *pSync)                        \
      _DEPRECATED_BY(shmem_broadcastmem or shmem_<typename> _broadcast, 1.5);

API_BROADCAST_SIZE(32)
API_BROADCAST_SIZE(64)
////////////////////////////////////////////////////////////////////////////////

/**
 * Collects
 */

/**
 * @brief Collective operation to concatenate data from multiple PEs into a
 * single array
 *
 * The collect routines concatenate blocks of data from multiple PEs in the team
 * into a single array on each PE in order of ascending PE number. The source
 * array may contain a different amount of data from each PE.
 *
 * @note The total size of the received data must be known by all PEs in the
 * team.
 */

/**
 * @brief Type-specific collect routines
 *
 * @param team    Team on which to perform the collect
 * @param dest    Symmetric destination array on all PEs
 * @param source  Local array containing data to be concatenated
 * @param nelems  Number of elements in source array
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_COLLECT_TYPE(_type, _typename)                                     \
  int shmem_##_typename##_collect(shmem_team_t team, _type *dest,              \
                                  const _type *source, size_t nelems);

#define DECL_COLLECT(_type, _typename) API_COLLECT_TYPE(_type, _typename)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_COLLECT)
#undef DECL_COLLECT
#undef API_COLLECT_TYPE

/**
 * @brief Generic memory collect routine
 *
 * @param team    Team on which to perform the collect
 * @param dest    Symmetric destination array on all PEs
 * @param source  Local array containing data to be concatenated
 * @param nelems  Number of elements in source array
 *
 * @return Zero on success, non-zero otherwise
 */
int shmem_collectmem(shmem_team_t team, void *dest, const void *source,
                     size_t nelems);

/**
 * @brief Legacy collect operations (deprecated)
 *
 * @param target       Symmetric destination array on all PEs
 * @param source       Local array containing data to be concatenated
 * @param nelems       Number of elements in source array
 * @param PE_start     First PE number of active set
 * @param logPE_stride Log2 of stride between consecutive PE numbers
 * @param PE_size      Number of PEs in active set
 * @param pSync        Symmetric work array
 */
#define API_COLLECT_SIZE(_opname, _size)                                       \
  void shmem_##_opname##_size(void *target, const void *source, size_t nelems, \
                              int PE_start, int logPE_stride, int PE_size,     \
                              long *pSync)                                     \
      _DEPRECATED_BY(shmem_collectmem or shmem_<typename> _collect, 1.5);

API_COLLECT_SIZE(collect, 32)
API_COLLECT_SIZE(collect, 64)

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Type-specific fcollect routines
 *
 * @param team    Team on which to perform the fcollect
 * @param dest    Symmetric destination array on all PEs
 * @param source  Local array containing data to be concatenated
 * @param nelems  Number of elements in source array
 *
 * @return Zero on success, non-zero otherwise
 */
#define API_FCOLLECT_TYPE(_type, _typename)                                    \
  int shmem_##_typename##_fcollect(shmem_team_t team, _type *dest,             \
                                   const _type *source, size_t nelems);

#define DECL_FCOLLECT(_type, _typename) API_FCOLLECT_TYPE(_type, _typename)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_FCOLLECT)
#undef DECL_FCOLLECT
#undef API_FCOLLECT_TYPE

/**
 * @brief Generic memory fcollect routine
 *
 * @param team    Team on which to perform the fcollect
 * @param dest    Symmetric destination array on all PEs
 * @param source  Local array containing data to be concatenated
 * @param nelems  Number of elements in source array
 *
 * @return Zero on success, non-zero otherwise
 */
int shmem_fcollectmem(shmem_team_t team, void *dest, const void *source,
                      size_t nelems);

/**
 * @brief Legacy fcollect operations (deprecated)
 *
 * @param target       Symmetric destination array on all PEs
 * @param source       Local array containing data to be concatenated
 * @param nelems       Number of elements in source array
 * @param PE_start     First PE number of active set
 * @param logPE_stride Log2 of stride between consecutive PE numbers
 * @param PE_size      Number of PEs in active set
 * @param pSync        Symmetric work array
 */
#define API_FCOLLECT_SIZE(_opname, _size)                                      \
  void shmem_##_opname##_size(void *target, const void *source, size_t nelems, \
                              int PE_start, int logPE_stride, int PE_size,     \
                              long *pSync)                                     \
      _DEPRECATED_BY(shmem_fcollectmem or shmem_<typename> _fcollect, 1.5);

API_FCOLLECT_SIZE(fcollect, 32)
API_FCOLLECT_SIZE(fcollect, 64)

//////////////////////////////////////////////////////////////////////////////////
/**
 * all-to-all collectives
 */
/**
 * @brief All-to-all collective routines
 *
 * These routines perform an all-to-all collective operation across a team. Each
 * PE contributes distinct data to every other PE in the team.
 *
 * The all-to-all routines require that:
 * - The amount of data sent from each PE must be equal to the amount of data
 * received by each PE
 * - The data type and count must be identical across all PEs
 * - The target and source arrays must be symmetric
 *
 * @param team    The team over which to perform the all-to-all operation
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array containing data to be sent to all PEs
 * @param nelems  Number of elements contributed by and received from each PE
 * @return        Zero on success, non-zero on failure
 */
#define API_ALLTOALL_TYPE(_type, _typename)                                    \
  int shmem_##_typename##_alltoall(shmem_team_t team, _type *dest,             \
                                   const _type *source, size_t nelems);

#define DECL_ALLTOALL(_type, _typename) API_ALLTOALL_TYPE(_type, _typename)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_ALLTOALL)
#undef DECL_ALLTOALL
#undef API_ALLTOALL_TYPE

/**
 * Generic memory alltoall routine
 *
 * @param team    The team over which to alltoall
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array on root PE
 * @param nelems  Number of elements to alltoall
 * @return        Zero on success, non-zero on failure
 */
int shmem_alltoallmem(shmem_team_t team, void *dest, const void *source,
                      size_t nelems);

/**
 * @brief Sized alltoall routine (deprecated)
 *
 * @param target       Symmetric destination array on all PEs
 * @param source       Source array containing data to be sent to all PEs
 * @param nelems       Number of elements contributed by and received from each
 * PE
 * @param PE_start     First PE number of the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PE numbers in
 * active set
 * @param PE_size      Number of PEs in the active set
 * @param pSync        Symmetric work array of size SHMEM_ALLTOALL_SYNC_SIZE
 */
#define API_ALLTOALL_SIZE(_size)                                               \
  void shmem_alltoall##_size(void *target, const void *source, size_t nelems,  \
                             int PE_start, int logPE_stride, int PE_size,      \
                             long *pSync)                                      \
      _DEPRECATED_BY(shmem_alltoallmem or shmem_<typename> _alltoall, 1.5);

API_ALLTOALL_SIZE(32)
API_ALLTOALL_SIZE(64)

//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Sized alltoall routines (deprecated)
 *
 * @param team    The team over which to alltoall
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array on root PE
 * @param nelems  Number of elements to alltoall
 * @return        Zero on success, non-zero on failure
 */
#define API_ALLTOALLS_TYPE(_type, _typename)                                   \
  int shmem_##_typename##_alltoalls(shmem_team_t team, _type *dest,            \
                                    const _type *source, ptrdiff_t dst,        \
                                    ptrdiff_t sst, size_t nelems);

#define DECL_ALLTOALLS(_type, _typename) API_ALLTOALLS_TYPE(_type, _typename)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_ALLTOALLS)
#undef DECL_ALLTOALLS
#undef API_ALLTOALLS_TYPE

/**
 * @brief Generic memory alltoall routine (deprecated)
 *
 * @param team    The team over which to alltoall
 * @param dest    Symmetric destination array on all PEs
 * @param source  Source array on root PE
 * @param dst     Destination array on root PE
 * @param sst     Source array on root PE
 * @param nelems  Number of elements to alltoall
 * @return        Zero on success, non-zero on failure
 */
int shmem_alltoallsmem(shmem_team_t team, void *dest, const void *source,
                       ptrdiff_t dst, ptrdiff_t sst, size_t nelems);

/**
 * @brief Sized alltoall routine (deprecated)
 *
 * @param target       Symmetric destination array on all PEs
 * @param source       Source array containing data to be sent to all PEs
 * @param nelems       Number of elements contributed by and received from each
 * PE
 * @param PE_start     First PE number of the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PE numbers in
 * active set
 * @param PE_size      Number of PEs in the active set
 * @param pSync        Symmetric work array of size SHMEM_ALLTOALL_SYNC_SIZE
 */
#define API_ALLTOALLS_SIZE(_size)                                              \
  void shmem_alltoalls##_size(void *target, const void *source, ptrdiff_t dst, \
                              ptrdiff_t sst, size_t nelems, int PE_start,      \
                              int logPE_stride, int PE_size, long *pSync)      \
      _DEPRECATED_BY(shmem_alltoallsmem or shmem_<typename> _alltoalls, 1.5);

API_ALLTOALLS_SIZE(32)
API_ALLTOALLS_SIZE(64)

//////////////////////////////////////////////////////////////////////////////////
/**
 * Contexts-based API
 *
 */

/**
 * @brief creates a new context
 * @page shmem_ctx_create
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 int shmem_ctx_create(long options, shmem_ctx_t *ctxp);
 @endcode
 *
 * @param[in] options how to create this context
 * @param[out] ctxp pointer to the newly created context
 *
 * @section Effect
 *
 * creates a new communication context and returns
 * its handle through the ctx argument.
 *
 * @section Options
 *
 * bitwise-or of SHMEM_CTX_SERIALIZED, SHMEM_CTX_PRIVATE,
 * SHMEM_CTX_NOSTORE, or zero for no special options.
 *
 * @section Return
 *
 * Zero on success, non-zero otherwise
 *
 */
int shmem_ctx_create(long options, shmem_ctx_t *ctxp);

//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief creates a new context
 * @page shmem_ctx_destroy
 * @section Synopsis
 *
 * @subsection c C/C++
 @code
 void shmem_ctx_destroy(shmem_ctx_t ctx);
 @endcode
 *
 * @param[in] ctx an existing context
 *
 * @section Effect
 *
 * destroys a context that was created by a call to \ref shmem_ctx_create.
 *
 * @section Notes
 *
 * it is illegal to pass the default context SHMEM_CTX_DEFAULT to
 * this routine.
 *
 * @section Return
 * None.
 *
 */
void shmem_ctx_destroy(shmem_ctx_t ctx);

//////////////////////////////////////////////////////////////////////////////////
/*
 * Teams
 *
 */
#include <shmem/teams.h>

/*
 * C11 Generics
 *
 */
#include <shmem/generics.h>

/*
 * deprecated cache routines
 */
#include <shmem/cache.h>

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*
 * tidy-up complex "I" macro detection.
 *
 */
#ifdef shmemi_h_I_already_defined__
#undef shmemi_h_I_already_defined__
#else
#undef I
#endif /* shmemi_h_I_already_defined__ */

#ifdef shmem_sync
#undef shmem_sync
#endif

/* --- Begin new variadic macro override for shmem_sync --- */
/* Define helper functions for the two cases */
#define _SHMEM_SYNC_4(PE_start, logPE_stride, PE_size, pSync)                  \
  shmem_sync_deprecated(PE_start, logPE_stride, PE_size, pSync)
#define _SHMEM_SYNC_1(team) shmem_team_sync(team)

/* Helper macro to select the 5th argument */
#define _GET_5TH_ARG(a, b, c, d, e, ...) e

/* Define the variadic shmem_sync macro.
   - If one argument is provided, it expands to _SHMEM_SYNC_1(team)
   - If four arguments are provided, it expands to _SHMEM_SYNC_4(...)
*/

#define shmem_sync(...)                                                        \
  _GET_5TH_ARG(__VA_ARGS__, _SHMEM_SYNC_4, _SHMEM_SYNC_4, _SHMEM_SYNC_4,       \
               _SHMEM_SYNC_1)                                                  \
  (__VA_ARGS__)
/* --- End new variadic macro override for shmem_sync --- */

#endif /* ! _SHMEM_API_H */
