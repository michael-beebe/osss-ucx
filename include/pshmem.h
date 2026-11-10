/**
 * @file pshmem.h
 * @brief OpenSHMEM prefixed API header file
 *
 * For license: see LICENSE file at top-level
 */

#ifndef _PSHMEM_H
#define _PSHMEM_H 1

#include "shmem.h"
#include <shmem/api_types.h>

#include <stdint.h> /* uint*_t */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * init & query
 */

/**
 * @brief Initialize the OpenSHMEM environment with a specified number of PEs
 * @param npes Number of PEs to initialize
 * @deprecated Use pshmem_init() instead
 */
void pstart_pes(int npes);

/**
 * @brief Initialize the OpenSHMEM environment
 */
void pshmem_init(void);

/**
 * @brief Reports initialization status through the profiling interface.
 * @param[out] initialized Nonzero while initialized, zero otherwise.
 */
void pshmem_query_initialized(int *initialized);

/**
 * @brief Clean up the OpenSHMEM environment
 */
void pshmem_finalize(void);

/**
 * @brief Get the OpenSHMEM version information
 * @param major Major version number
 * @param minor Minor version number
 */
void pshmem_info_get_version(int *major, int *minor);

/**
 * @brief Get the OpenSHMEM implementation name
 * @param name Buffer to store the implementation name
 */
void pshmem_info_get_name(char *name);

/**
 * @brief Initialize OpenSHMEM with thread support
 * @param req Requested thread support level
 * @param prov Provided thread support level
 */
int pshmem_init_thread(int req, int *prov);

/**
 * @brief Query the thread support level
 * @param[out] prov Current thread support level
 */
void pshmem_query_thread(int *prov);

/**
 * @brief Terminate all PEs with an exit code
 * @param status Exit status
 */
void pshmem_global_exit(int status);

/**
 * @brief Get the PE number of the calling PE
 * @return PE number
 * @deprecated Use pshmem_my_pe() instead
 */
int p_my_pe(void) _WUR;

/**
 * @brief Get the PE number of the calling PE
 * @return PE number
 */
int pshmem_my_pe(void) _WUR;

/**
 * @brief Get the total number of PEs
 * @return Number of PEs
 * @deprecated Use pshmem_n_pes() instead
 */
int p_num_pes(void) _WUR;

/**
 * @brief Get the total number of PEs
 * @return Number of PEs
 */
int pshmem_n_pes(void) _WUR;

/*
 * Teams API
 */

/**
 * @brief Get the PE number of the calling PE in the given team
 * @param team The team to query
 * @return PE number in the team
 */
int pshmem_team_my_pe(shmem_team_t team) _WUR;

/**
 * @brief Get the total number of PEs in the given team
 * @param team The team to query
 * @return Number of PEs in the team
 */
int pshmem_team_n_pes(shmem_team_t team) _WUR;

/**
 * @brief Get the configuration of a team
 * @param team The team to query
 * @param config_mask Mask indicating which config values to retrieve
 * @param config Configuration structure to fill
 * @return 0 on success, nonzero on failure
 */
int pshmem_team_get_config(shmem_team_t team, long config_mask,
                           shmem_team_config_t *config) _WUR;

/**
 * @brief Translate a PE number from one team to another
 * @param src_team Source team
 * @param src_pe PE number in source team
 * @param dest_team Destination team
 * @return Translated PE number in destination team
 */
int pshmem_team_translate_pe(shmem_team_t src_team, int src_pe,
                             shmem_team_t dest_team) _WUR;

/**
 * @brief Create a new team by splitting parent team with strided PE selection
 * @param parent_team Team to split
 * @param start Starting PE number
 * @param stride Stride between PEs
 * @param size Number of PEs in new team
 * @param config Team configuration
 * @param config_mask Configuration options mask
 * @param new_team Handle for new team
 * @return 0 on success, nonzero on failure
 */
int pshmem_team_split_strided(shmem_team_t parent_team, int start, int stride,
                              int size, const shmem_team_config_t *config,
                              long config_mask, shmem_team_t *new_team) _WUR;

/**
 * @brief Split a team into a 2D grid
 * @param parent_team Team to split
 * @param xrange Size of x dimension
 * @param xaxis_config Configuration for x-axis team
 * @param xaxis_mask Configuration mask for x-axis
 * @param xaxis_team Handle for x-axis team
 * @param yaxis_config Configuration for y-axis team
 * @param yaxis_mask Configuration mask for y-axis
 * @param yaxis_team Handle for y-axis team
 * @return 0 on success, nonzero on failure
 */
int pshmem_team_split_2d(shmem_team_t parent_team, int xrange,
                         const shmem_team_config_t *xaxis_config,
                         long xaxis_mask, shmem_team_t *xaxis_team,
                         const shmem_team_config_t *yaxis_config,
                         long yaxis_mask, shmem_team_t *yaxis_team) _WUR;

/**
 * @brief Destroy a team
 * @param team Team to destroy
 */
void pshmem_team_destroy(shmem_team_t team);

/**
 * @brief Create a communication context from a team
 * @param team Team to create context from
 * @param options Context creation options
 * @param ctxp Handle for new context
 * @return 0 on success, nonzero on failure
 */
int pshmem_team_create_ctx(shmem_team_t team, long options,
                           shmem_ctx_t *ctxp) _WUR;

/**
 * @brief Get the team associated with a context
 * @param ctx Context to query
 * @param team Handle to store associated team
 * @return 0 on success, nonzero on failure
 */
int pshmem_ctx_get_team(shmem_ctx_t ctx, shmem_team_t *team) _WUR;

/**
 * @brief Synchronize all PEs in a team
 * @param team Team to synchronize
 * @return 0 on success, nonzero on failure
 */
int pshmem_team_sync(shmem_team_t team) _WUR;

/**
 * @brief Get a pointer to a symmetric data object on a remote PE in a team
 * @param team Team containing the PE
 * @param dest Symmetric data object on remote PE
 * @param pe PE number in the team
 * @return Pointer to remote data object
 */
void *pshmem_team_ptr(shmem_team_t team, const void *dest, int pe) _WUR;

/*
 * I/O
 */
/**
 * @brief I/O operations for OpenSHMEM
 */

/**
 * @brief Macro to declare typed put/get operations
 * @param _opname Operation name (put/get)
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_PUTGET(_opname, _name, _type)                              \
  void pshmem_##_name##_##_opname(_type *dest, const _type *src,               \
                                  size_t nelems, int pe);

#define DECL_PSHMEM_PUT(_type, _typename)                                      \
  PSHMEM_DECL_PUTGET(put, _typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_PSHMEM_PUT)
#undef DECL_PSHMEM_PUT

#define DECL_PSHMEM_GET(_type, _typename)                                      \
  PSHMEM_DECL_PUTGET(get, _typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_PSHMEM_GET)
#undef DECL_PSHMEM_GET

#undef PSHMEM_DECL_PUTGET

/**
 * @brief Macro to declare sized put/get operations
 * @param _opname Operation name (put/get)
 * @param _size Size in bits
 */
#define PSHMEM_DECL_PUTGET_SIZE(_opname, _size)                                \
  void pshmem_##_opname##_size(void *dest, const void *src, size_t nelems,     \
                               int pe);

PSHMEM_DECL_PUTGET_SIZE(put, 8)
PSHMEM_DECL_PUTGET_SIZE(put, 16)
PSHMEM_DECL_PUTGET_SIZE(put, 32)
PSHMEM_DECL_PUTGET_SIZE(put, 64)
PSHMEM_DECL_PUTGET_SIZE(put, 128)

PSHMEM_DECL_PUTGET_SIZE(get, 8)
PSHMEM_DECL_PUTGET_SIZE(get, 16)
PSHMEM_DECL_PUTGET_SIZE(get, 32)
PSHMEM_DECL_PUTGET_SIZE(get, 64)
PSHMEM_DECL_PUTGET_SIZE(get, 128)

#undef PSHMEM_DECL_PUTGET_SIZE

/**
 * @brief Declares typed interleaved block transfers and their context forms.
 * @param _type Element type.
 * @param _name OpenSHMEM type name.
 */
#define PSHMEM_DECL_IB_TRANSFER(_type, _name)                                  \
  void pshmem_##_name##_ibput(_type *dest, const _type *source, ptrdiff_t dst, \
                              ptrdiff_t sst, size_t bsize, size_t nblocks,     \
                              int pe);                                         \
  void pshmem_##_name##_ibget(_type *dest, const _type *source, ptrdiff_t dst, \
                              ptrdiff_t sst, size_t bsize, size_t nblocks,     \
                              int pe);                                         \
  void pshmem_ctx_##_name##_ibput(                                             \
      shmem_ctx_t ctx, _type *dest, const _type *source, ptrdiff_t dst,        \
      ptrdiff_t sst, size_t bsize, size_t nblocks, int pe);                    \
  void pshmem_ctx_##_name##_ibget(                                             \
      shmem_ctx_t ctx, _type *dest, const _type *source, ptrdiff_t dst,        \
      ptrdiff_t sst, size_t bsize, size_t nblocks, int pe);

SHMEM_STANDARD_RMA_TYPE_TABLE(PSHMEM_DECL_IB_TRANSFER)
#undef PSHMEM_DECL_IB_TRANSFER

/**
 * @brief Declares sized interleaved block transfers and their context forms.
 * @param _size Element size in bits.
 */
#define PSHMEM_DECL_IB_TRANSFER_SIZE(_size)                                    \
  void pshmem_ibput##_size(void *dest, const void *source, ptrdiff_t dst,      \
                           ptrdiff_t sst, size_t bsize, size_t nblocks,        \
                           int pe);                                            \
  void pshmem_ibget##_size(void *dest, const void *source, ptrdiff_t dst,      \
                           ptrdiff_t sst, size_t bsize, size_t nblocks,        \
                           int pe);                                            \
  void pshmem_ctx_ibput##_size(                                                \
      shmem_ctx_t ctx, void *dest, const void *source, ptrdiff_t dst,          \
      ptrdiff_t sst, size_t bsize, size_t nblocks, int pe);                    \
  void pshmem_ctx_ibget##_size(                                                \
      shmem_ctx_t ctx, void *dest, const void *source, ptrdiff_t dst,          \
      ptrdiff_t sst, size_t bsize, size_t nblocks, int pe);

PSHMEM_DECL_IB_TRANSFER_SIZE(8)
PSHMEM_DECL_IB_TRANSFER_SIZE(16)
PSHMEM_DECL_IB_TRANSFER_SIZE(32)
PSHMEM_DECL_IB_TRANSFER_SIZE(64)
PSHMEM_DECL_IB_TRANSFER_SIZE(128)
#undef PSHMEM_DECL_IB_TRANSFER_SIZE

/**
 * @brief Put contiguous data to remote PE
 * @param dest Destination address on remote PE
 * @param src Source address
 * @param nelems Number of elements
 * @param pe Remote PE number
 */
void pshmem_putmem(void *dest, const void *src, size_t nelems, int pe);

/**
 * @brief Get contiguous data from remote PE
 * @param dest Destination address
 * @param src Source address on remote PE
 * @param nelems Number of elements
 * @param pe Remote PE number
 */
void pshmem_getmem(void *dest, const void *src, size_t nelems, int pe);

/**
 * @brief Macro to declare put operations for single elements
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_P(_name, _type)                                            \
  void pshmem_##_name##_p(_type *dest, _type src, int pe);

#define DECL_PSHMEM_P(_type, _typename) PSHMEM_DECL_P(_typename, _type)
SHMEM_STANDARD_RMA_TYPE_TABLE(DECL_PSHMEM_P)
#undef DECL_PSHMEM_P

#undef PSHMEM_DECL_P

/**
 * @brief Macro to declare get operations for single elements
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_G(_name, _type)                                            \
  _type pshmem_##_name##_g(const _type *dest, int pe);

PSHMEM_DECL_G(float, float)
PSHMEM_DECL_G(double, double)
PSHMEM_DECL_G(longdouble, long double)
PSHMEM_DECL_G(schar, signed char)
PSHMEM_DECL_G(char, char)
PSHMEM_DECL_G(short, short)
PSHMEM_DECL_G(int, int)
PSHMEM_DECL_G(long, long)
PSHMEM_DECL_G(longlong, long long)
PSHMEM_DECL_G(uchar, unsigned char)
PSHMEM_DECL_G(ushort, unsigned short)
PSHMEM_DECL_G(uint, unsigned int)
PSHMEM_DECL_G(ulong, unsigned long)
PSHMEM_DECL_G(ulonglong, unsigned long long)
PSHMEM_DECL_G(int8, int8_t)
PSHMEM_DECL_G(int16, int16_t)
PSHMEM_DECL_G(int32, int32_t)
PSHMEM_DECL_G(int64, int64_t)
PSHMEM_DECL_G(uint8, uint8_t)
PSHMEM_DECL_G(uint16, uint16_t)
PSHMEM_DECL_G(uint32, uint32_t)
PSHMEM_DECL_G(uint64, uint64_t)
PSHMEM_DECL_G(size, size_t)
PSHMEM_DECL_G(ptrdiff, ptrdiff_t)

#undef PSHMEM_DECL_G

/*
 * strided I/O
 */

/**
 * @brief Macro to declare strided put/get operations
 * @param _opname Operation name (put/get)
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_STRIDED_OP(_opname, _name, _type)                          \
  void pshmem_##_name##_i##_opname(_type *dest, const _type *src,              \
                                   ptrdiff_t tst, ptrdiff_t sst,               \
                                   size_t nelems, int pe);

PSHMEM_DECL_STRIDED_OP(put, float, float)
PSHMEM_DECL_STRIDED_OP(put, double, double)
PSHMEM_DECL_STRIDED_OP(put, longdouble, long double)
PSHMEM_DECL_STRIDED_OP(put, schar, signed char)
PSHMEM_DECL_STRIDED_OP(put, char, char)
PSHMEM_DECL_STRIDED_OP(put, short, short)
PSHMEM_DECL_STRIDED_OP(put, int, int)
PSHMEM_DECL_STRIDED_OP(put, long, long)
PSHMEM_DECL_STRIDED_OP(put, longlong, long long)
PSHMEM_DECL_STRIDED_OP(put, uchar, unsigned char)
PSHMEM_DECL_STRIDED_OP(put, ushort, unsigned short)
PSHMEM_DECL_STRIDED_OP(put, uint, unsigned int)
PSHMEM_DECL_STRIDED_OP(put, ulong, unsigned long)
PSHMEM_DECL_STRIDED_OP(put, ulonglong, unsigned long long)
PSHMEM_DECL_STRIDED_OP(put, int8, int8_t)
PSHMEM_DECL_STRIDED_OP(put, int16, int16_t)
PSHMEM_DECL_STRIDED_OP(put, int32, int32_t)
PSHMEM_DECL_STRIDED_OP(put, int64, int64_t)
PSHMEM_DECL_STRIDED_OP(put, uint8, uint8_t)
PSHMEM_DECL_STRIDED_OP(put, uint16, uint16_t)
PSHMEM_DECL_STRIDED_OP(put, uint32, uint32_t)
PSHMEM_DECL_STRIDED_OP(put, uint64, uint64_t)
PSHMEM_DECL_STRIDED_OP(put, size, size_t)
PSHMEM_DECL_STRIDED_OP(put, ptrdiff, ptrdiff_t)

PSHMEM_DECL_STRIDED_OP(get, float, float)
PSHMEM_DECL_STRIDED_OP(get, double, double)
PSHMEM_DECL_STRIDED_OP(get, longdouble, long double)
PSHMEM_DECL_STRIDED_OP(get, schar, signed char)
PSHMEM_DECL_STRIDED_OP(get, char, char)
PSHMEM_DECL_STRIDED_OP(get, short, short)
PSHMEM_DECL_STRIDED_OP(get, int, int)
PSHMEM_DECL_STRIDED_OP(get, long, long)
PSHMEM_DECL_STRIDED_OP(get, longlong, long long)
PSHMEM_DECL_STRIDED_OP(get, uchar, unsigned char)
PSHMEM_DECL_STRIDED_OP(get, ushort, unsigned short)
PSHMEM_DECL_STRIDED_OP(get, uint, unsigned int)
PSHMEM_DECL_STRIDED_OP(get, ulong, unsigned long)
PSHMEM_DECL_STRIDED_OP(get, ulonglong, unsigned long long)
PSHMEM_DECL_STRIDED_OP(get, int8, int8_t)
PSHMEM_DECL_STRIDED_OP(get, int16, int16_t)
PSHMEM_DECL_STRIDED_OP(get, int32, int32_t)
PSHMEM_DECL_STRIDED_OP(get, int64, int64_t)
PSHMEM_DECL_STRIDED_OP(get, uint8, uint8_t)
PSHMEM_DECL_STRIDED_OP(get, uint16, uint16_t)
PSHMEM_DECL_STRIDED_OP(get, uint32, uint32_t)
PSHMEM_DECL_STRIDED_OP(get, uint64, uint64_t)
PSHMEM_DECL_STRIDED_OP(get, size, size_t)
PSHMEM_DECL_STRIDED_OP(get, ptrdiff, ptrdiff_t)

#undef PSHMEM_DECL_STRIDED_OP

/**
 * @brief Macro to declare sized strided put/get operations
 * @param _opname Operation name (put/get)
 * @param _size Size in bits
 */
#define PSHMEM_DECL_STRIDED_OPSIZE(_opname, _size)                             \
  void pshmem_i##_opname##_size(void *dest, const void *src, ptrdiff_t tst,    \
                                ptrdiff_t sst, size_t nelems, int pe);

PSHMEM_DECL_STRIDED_OPSIZE(put, 8)
PSHMEM_DECL_STRIDED_OPSIZE(put, 16)
PSHMEM_DECL_STRIDED_OPSIZE(put, 32)
PSHMEM_DECL_STRIDED_OPSIZE(put, 64)
PSHMEM_DECL_STRIDED_OPSIZE(put, 128)

PSHMEM_DECL_STRIDED_OPSIZE(get, 8)
PSHMEM_DECL_STRIDED_OPSIZE(get, 16)
PSHMEM_DECL_STRIDED_OPSIZE(get, 32)
PSHMEM_DECL_STRIDED_OPSIZE(get, 64)
PSHMEM_DECL_STRIDED_OPSIZE(get, 128)

#undef PSHMEM_DECL_STRIDED_OPSIZE

/*
 * non-blocking implicit put/get
 *
 */

/**
 * @brief Macro to declare non-blocking put/get operations
 * @param _opname Operation name (put/get)
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_PUTGET_NBI(_opname, _name, _type)                          \
  void pshmem_##_name##_##_opname##_nbi(_type *dest, const _type *src,         \
                                        size_t nelems, int pe);

PSHMEM_DECL_PUTGET_NBI(put, float, float)
PSHMEM_DECL_PUTGET_NBI(put, double, double)
PSHMEM_DECL_PUTGET_NBI(put, longdouble, long double)
PSHMEM_DECL_PUTGET_NBI(put, schar, signed char)
PSHMEM_DECL_PUTGET_NBI(put, char, char)
PSHMEM_DECL_PUTGET_NBI(put, short, short)
PSHMEM_DECL_PUTGET_NBI(put, int, int)
PSHMEM_DECL_PUTGET_NBI(put, long, long)
PSHMEM_DECL_PUTGET_NBI(put, longlong, long long)
PSHMEM_DECL_PUTGET_NBI(put, uchar, unsigned char)
PSHMEM_DECL_PUTGET_NBI(put, ushort, unsigned short)
PSHMEM_DECL_PUTGET_NBI(put, uint, unsigned int)
PSHMEM_DECL_PUTGET_NBI(put, ulong, unsigned long)
PSHMEM_DECL_PUTGET_NBI(put, ulonglong, unsigned long long)
PSHMEM_DECL_PUTGET_NBI(put, int8, int8_t)
PSHMEM_DECL_PUTGET_NBI(put, int16, int16_t)
PSHMEM_DECL_PUTGET_NBI(put, int32, int32_t)
PSHMEM_DECL_PUTGET_NBI(put, int64, int64_t)
PSHMEM_DECL_PUTGET_NBI(put, uint8, uint8_t)
PSHMEM_DECL_PUTGET_NBI(put, uint16, uint16_t)
PSHMEM_DECL_PUTGET_NBI(put, uint32, uint32_t)
PSHMEM_DECL_PUTGET_NBI(put, uint64, uint64_t)
PSHMEM_DECL_PUTGET_NBI(put, size, size_t)
PSHMEM_DECL_PUTGET_NBI(put, ptrdiff, ptrdiff_t)

PSHMEM_DECL_PUTGET_NBI(get, float, float)
PSHMEM_DECL_PUTGET_NBI(get, double, double)
PSHMEM_DECL_PUTGET_NBI(get, longdouble, long double)
PSHMEM_DECL_PUTGET_NBI(get, schar, signed char)
PSHMEM_DECL_PUTGET_NBI(get, char, char)
PSHMEM_DECL_PUTGET_NBI(get, short, short)
PSHMEM_DECL_PUTGET_NBI(get, int, int)
PSHMEM_DECL_PUTGET_NBI(get, long, long)
PSHMEM_DECL_PUTGET_NBI(get, longlong, long long)
PSHMEM_DECL_PUTGET_NBI(get, uchar, unsigned char)
PSHMEM_DECL_PUTGET_NBI(get, ushort, unsigned short)
PSHMEM_DECL_PUTGET_NBI(get, uint, unsigned int)
PSHMEM_DECL_PUTGET_NBI(get, ulong, unsigned long)
PSHMEM_DECL_PUTGET_NBI(get, ulonglong, unsigned long long)
PSHMEM_DECL_PUTGET_NBI(get, int8, int8_t)
PSHMEM_DECL_PUTGET_NBI(get, int16, int16_t)
PSHMEM_DECL_PUTGET_NBI(get, int32, int32_t)
PSHMEM_DECL_PUTGET_NBI(get, int64, int64_t)
PSHMEM_DECL_PUTGET_NBI(get, uint8, uint8_t)
PSHMEM_DECL_PUTGET_NBI(get, uint16, uint16_t)
PSHMEM_DECL_PUTGET_NBI(get, uint32, uint32_t)
PSHMEM_DECL_PUTGET_NBI(get, uint64, uint64_t)
PSHMEM_DECL_PUTGET_NBI(get, size, size_t)
PSHMEM_DECL_PUTGET_NBI(get, ptrdiff, ptrdiff_t)

#undef PSHMEM_DECL_PUTGET_NBI

/**
 * @brief Macro to declare sized non-blocking put/get operations
 * @param _opname Operation name (put/get)
 * @param _size Size in bits
 */
#define PSHMEM_DECL_PUTGET_SIZE_NBI(_opname, _size)                            \
  void pshmem_##_opname##_size##_nbi(void *dest, const void *src,              \
                                     size_t nelems, int pe);

PSHMEM_DECL_PUTGET_SIZE_NBI(put, 8)
PSHMEM_DECL_PUTGET_SIZE_NBI(put, 16)
PSHMEM_DECL_PUTGET_SIZE_NBI(put, 32)
PSHMEM_DECL_PUTGET_SIZE_NBI(put, 64)
PSHMEM_DECL_PUTGET_SIZE_NBI(put, 128)

PSHMEM_DECL_PUTGET_SIZE_NBI(get, 8)
PSHMEM_DECL_PUTGET_SIZE_NBI(get, 16)
PSHMEM_DECL_PUTGET_SIZE_NBI(get, 32)
PSHMEM_DECL_PUTGET_SIZE_NBI(get, 64)
PSHMEM_DECL_PUTGET_SIZE_NBI(get, 128)

#undef PSHMEM_DECL_PUTGET_SIZE_NBI

/**
 * @brief Non-blocking put operation for contiguous data
 * @param dest Destination address on remote PE
 * @param source Source address
 * @param nelems Number of elements
 * @param pe Remote PE number
 */
void pshmem_putmem_nbi(void *dest, const void *source, size_t nelems, int pe);

/**
 * @brief Non-blocking get operation for contiguous data
 * @param dest Destination address
 * @param source Source address on remote PE
 * @param nelems Number of elements
 * @param pe Remote PE number
 */
void pshmem_getmem_nbi(void *dest, const void *source, size_t nelems, int pe);

#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 201112L

/**
 * @defgroup pshmem_c11_generics OpenSHMEM C11 Generic Operations
 * @brief C11 generic selection macros for OpenSHMEM operations
 * @{
 *
 * These macros use C11 _Generic to select the appropriate typed function based
 * on argument types. For example, pshmem_put() will map to pshmem_float_put()
 * for float arguments, pshmem_long_put() for long arguments, etc.
 */

/* see \ref shmem_long_put() */
#define pshmem_put(dest, source, nelems, pe)                                   \
  _Generic(*(dest),                                                            \
      float: pshmem_float_put,                                                 \
      double: pshmem_double_put,                                               \
      long double: pshmem_longdouble_put,                                      \
      char: pshmem_char_put,                                                   \
      short: pshmem_short_put,                                                 \
      int: pshmem_int_put,                                                     \
      long: pshmem_long_put,                                                   \
      long long: pshmem_longlong_put)(dest, source, nelems, pe)

/* see \ref shmem_long_get() */
#define pshmem_get(dest, source, nelems, pe)                                   \
  _Generic(*(dest),                                                            \
      float: pshmem_float_get,                                                 \
      double: pshmem_double_get,                                               \
      long double: pshmem_longdouble_get,                                      \
      char: pshmem_char_get,                                                   \
      short: pshmem_short_get,                                                 \
      int: pshmem_int_get,                                                     \
      long: pshmem_long_get,                                                   \
      long long: pshmem_longlong_get)(dest, source, nelems, pe)

/* see \ref shmem_long_p() */
#define pshmem_p(dest, value, pe)                                              \
  _Generic(*(dest),                                                            \
      float: pshmem_float_p,                                                   \
      double: pshmem_double_p,                                                 \
      long double: pshmem_longdouble_p,                                        \
      char: pshmem_char_p,                                                     \
      short: pshmem_short_p,                                                   \
      int: pshmem_int_p,                                                       \
      long: pshmem_long_p,                                                     \
      long long: pshmem_longlong_p)(dest, value, pe)

/* see \ref shmem_long_g() */
#define pshmem_g(addr, pe)                                                     \
  _Generic((addr),                                                             \
      float: pshmem_float_g,                                                   \
      double: pshmem_double_g,                                                 \
      long double: pshmem_longdouble_g,                                        \
      char: pshmem_char_g,                                                     \
      short: pshmem_short_g,                                                   \
      int: pshmem_int_g,                                                       \
      long: pshmem_long_g,                                                     \
      long long: pshmem_longlong_g)(addr, pe)

/* see \ref shmem_long_iput() */
#define pshmem_iput(dest, source, dst, sst, nelems, pe)                        \
  _Generic(*(dest),                                                            \
      float: pshmem_float_iput,                                                \
      double: pshmem_double_iput,                                              \
      long double: pshmem_longdouble_iput,                                     \
      char: pshmem_char_iput,                                                  \
      short: pshmem_short_iput,                                                \
      int: pshmem_int_iput,                                                    \
      long: pshmem_long_iput,                                                  \
      long long: pshmem_longlong_iput)(dest, source, dst, sst, nelems, pe)

/* see \ref shmem_long_iput() */
#define pshmem_iput(dest, source, dst, sst, nelems, pe)                        \
  _Generic(*(dest),                                                            \
      float: pshmem_float_iput,                                                \
      double: pshmem_double_iput,                                              \
      long double: pshmem_longdouble_iput,                                     \
      char: pshmem_char_iput,                                                  \
      short: pshmem_short_iput,                                                \
      int: pshmem_int_iput,                                                    \
      long: pshmem_long_iput,                                                  \
      long long: pshmem_longlong_iput)(dest, source, dst, sst, nelems, pe)

/* see \ref shmem_long_swap() */
#define pshmem_swap(dest, value, pe)                                           \
  _Generic(*(dest),                                                            \
      int: pshmem_int_swap,                                                    \
      long: pshmem_long_swap,                                                  \
      long long: pshmem_longlong_swap,                                         \
      float: pshmem_float_swap,                                                \
      double: pshmem_double_swap)(dest, value, pe)

/* see \ref shmem_long_cswap() */
#define pshmem_cswap(dest, cond, value, pe)                                    \
  _Generic(*(dest),                                                            \
      int: pshmem_int_cswap,                                                   \
      long: pshmem_long_cswap,                                                 \
      long long: pshmem_longlong_cswap)(dest, cond, value, pe)

/* see \ref shmem_long_fadd() */
#define pshmem_fadd(dest, value, pe)                                           \
  _Generic(*(dest),                                                            \
      int: pshmem_int_fadd,                                                    \
      long: pshmem_long_fadd,                                                  \
      long long: pshmem_longlong_fadd)(dest, value, pe)

/* see \ref shmem_long_finc() */
#define pshmem_finc(dest, pe)                                                  \
  _Generic(*(dest),                                                            \
      int: pshmem_int_finc,                                                    \
      long: pshmem_long_finc,                                                  \
      long long: pshmem_longlong_finc)(dest, pe)

/* see \ref shmem_long_add() */
#define pshmem_add(dest, value, pe)                                            \
  _Generic(*(dest),                                                            \
      int: pshmem_int_add,                                                     \
      long: pshmem_long_add,                                                   \
      long long: pshmem_longlong_add)(dest, value, pe)

/* see \ref shmem_long_add() */
#define pshmem_add(dest, value, pe)                                            \
  _Generic(*(dest),                                                            \
      int: pshmem_int_add,                                                     \
      long: pshmem_long_add,                                                   \
      long long: pshmem_longlong_add)(dest, value, pe)

/* see \ref shmem_long_inc() */
#define pshmem_inc(dest, pe)                                                   \
  _Generic(*(dest),                                                            \
      int: pshmem_int_inc,                                                     \
      long: pshmem_long_inc,                                                   \
      long long: pshmem_longlong_inc)(dest, pe)

/* see \ref shmem_long_fetch() */
#define pshmem_fetch(dest, pe)                                                 \
  _Generic(*(dest),                                                            \
      int: pshmem_int_fetch,                                                   \
      const int: pshmem_int_fetch,                                             \
      long: pshmem_long_fetch,                                                 \
      const long: pshmem_long_fetch,                                           \
      long long: pshmem_longlong_fetch,                                        \
      const long long: pshmem_longlong_fetch,                                  \
      float: pshmem_float_fetch,                                               \
      const float: pshmem_float_fetch,                                         \
      double: pshmem_double_fetch,                                             \
      const double: pshmem_double_fetch)(dest, pe)

/* see \ref shmem_long_set() */
#define pshmem_set(dest, value, pe)                                            \
  _Generic(*(dest),                                                            \
      int: pshmem_int_set,                                                     \
      long: pshmem_long_set,                                                   \
      long long: pshmem_longlong_set,                                          \
      float: pshmem_float_set,                                                 \
      double: pshmem_double_set)(dest, value, pe)

/**
 * @brief Generic wait operation
 * @param ivar Local address to wait on
 * @param cmp_value Value to compare against
 */
#define pshmem_wait(ivar, cmp_value)                                           \
  _Generic(*(ivar),                                                            \
      short: pshmem_short_wait,                                                \
      int: pshmem_int_wait,                                                    \
      long: pshmem_long_wait,                                                  \
      long long: pshmem_longlong_wait)(ivar, cmp_value)

/**
 * @brief Generic wait until operation
 * @param ivar Local address to wait on
 * @param cmp Comparison operation
 * @param cmp_value Value to compare against
 */
#define pshmem_wait_until(ivar, cmp, cmp_value)                                \
  _Generic(*(ivar),                                                            \
      short: pshmem_short_wait_until,                                          \
      int: pshmem_int_wait_until,                                              \
      long: pshmem_long_wait_until,                                            \
      long long: pshmem_longlong_wait_until)(ivar, cmp, cmp_value)

/**
 * @brief Generic team broadcast operation
 */
#define pshmem_broadcast(team, dest, source, nelems, PE_root)                  \
  _Generic(*(dest),                                                            \
      float: pshmem_float_broadcast,                                           \
      double: pshmem_double_broadcast,                                         \
      long double: pshmem_longdouble_broadcast,                                \
      char: pshmem_char_broadcast,                                             \
      signed char: pshmem_schar_broadcast,                                     \
      short: pshmem_short_broadcast,                                           \
      int: pshmem_int_broadcast,                                               \
      long: pshmem_long_broadcast,                                             \
      long long: pshmem_longlong_broadcast,                                    \
      unsigned char: pshmem_uchar_broadcast,                                   \
      unsigned short: pshmem_ushort_broadcast,                                 \
      unsigned int: pshmem_uint_broadcast,                                     \
      unsigned long: pshmem_ulong_broadcast,                                   \
      unsigned long long: pshmem_ulonglong_broadcast)(team, dest, source,      \
                                                      nelems, PE_root)

/**
 * @brief Generic team collect operation
 */
#define pshmem_collect(team, dest, source, nelems)                             \
  _Generic(*(dest),                                                            \
      float: pshmem_float_collect,                                             \
      double: pshmem_double_collect,                                           \
      long double: pshmem_longdouble_collect,                                  \
      char: pshmem_char_collect,                                               \
      signed char: pshmem_schar_collect,                                       \
      short: pshmem_short_collect,                                             \
      int: pshmem_int_collect,                                                 \
      long: pshmem_long_collect,                                               \
      long long: pshmem_longlong_collect)(team, dest, source, nelems)

/**
 * @brief Generic team fcollect operation
 */
#define pshmem_fcollect(team, dest, source, nelems)                            \
  _Generic(*(dest),                                                            \
      float: pshmem_float_fcollect,                                            \
      double: pshmem_double_fcollect,                                          \
      long double: pshmem_longdouble_fcollect,                                 \
      char: pshmem_char_fcollect,                                              \
      signed char: pshmem_schar_fcollect,                                      \
      short: pshmem_short_fcollect,                                            \
      int: pshmem_int_fcollect,                                                \
      long: pshmem_long_fcollect,                                              \
      long long: pshmem_longlong_fcollect)(team, dest, source, nelems)

/**
 * @brief Generic team alltoall operation
 */
#define pshmem_alltoall(team, dest, source, nelems)                            \
  _Generic(*(dest),                                                            \
      float: pshmem_float_alltoall,                                            \
      double: pshmem_double_alltoall,                                          \
      long double: pshmem_longdouble_alltoall,                                 \
      char: pshmem_char_alltoall,                                              \
      signed char: pshmem_schar_alltoall,                                      \
      short: pshmem_short_alltoall,                                            \
      int: pshmem_int_alltoall,                                                \
      long: pshmem_long_alltoall,                                              \
      long long: pshmem_longlong_alltoall)(team, dest, source, nelems)

/**
 * @brief Generic team sum reduction operation
 */
#define pshmem_sum_reduce(team, dest, source, nreduce)                         \
  _Generic(*(dest),                                                            \
      char: pshmem_char_sum_reduce,                                            \
      signed char: pshmem_schar_sum_reduce,                                    \
      short: pshmem_short_sum_reduce,                                          \
      int: pshmem_int_sum_reduce,                                              \
      long: pshmem_long_sum_reduce,                                            \
      long long: pshmem_longlong_sum_reduce,                                   \
      float: pshmem_float_sum_reduce,                                          \
      double: pshmem_double_sum_reduce,                                        \
      long double: pshmem_longdouble_sum_reduce)(team, dest, source, nreduce)

/**
 * @brief Generic team maximum reduction operation
 */
#define pshmem_max_reduce(team, dest, source, nreduce)                         \
  _Generic(*(dest),                                                            \
      char: pshmem_char_max_reduce,                                            \
      signed char: pshmem_schar_max_reduce,                                    \
      short: pshmem_short_max_reduce,                                          \
      int: pshmem_int_max_reduce,                                              \
      long: pshmem_long_max_reduce,                                            \
      long long: pshmem_longlong_max_reduce,                                   \
      float: pshmem_float_max_reduce,                                          \
      double: pshmem_double_max_reduce,                                        \
      long double: pshmem_longdouble_max_reduce)(team, dest, source, nreduce)

/**
 * @brief Generic team minimum reduction operation
 */
#define pshmem_min_reduce(team, dest, source, nreduce)                         \
  _Generic(*(dest),                                                            \
      char: pshmem_char_min_reduce,                                            \
      signed char: pshmem_schar_min_reduce,                                    \
      short: pshmem_short_min_reduce,                                          \
      int: pshmem_int_min_reduce,                                              \
      long: pshmem_long_min_reduce,                                            \
      long long: pshmem_longlong_min_reduce,                                   \
      float: pshmem_float_min_reduce,                                          \
      double: pshmem_double_min_reduce,                                        \
      long double: pshmem_longdouble_min_reduce)(team, dest, source, nreduce)

/**
 * @brief Generic team product reduction operation
 */
#define pshmem_prod_reduce(team, dest, source, nreduce)                        \
  _Generic(*(dest),                                                            \
      char: pshmem_char_prod_reduce,                                           \
      signed char: pshmem_schar_prod_reduce,                                   \
      short: pshmem_short_prod_reduce,                                         \
      int: pshmem_int_prod_reduce,                                             \
      long: pshmem_long_prod_reduce,                                           \
      long long: pshmem_longlong_prod_reduce,                                  \
      float: pshmem_float_prod_reduce,                                         \
      double: pshmem_double_prod_reduce,                                       \
      long double: pshmem_longdouble_prod_reduce)(team, dest, source, nreduce)

/**
 * @brief Generic team alltoalls operation
 */
#define pshmem_alltoalls(team, dest, source, dst, sst, nelems)                 \
  _Generic(*(dest),                                                            \
      float: pshmem_float_alltoalls,                                           \
      double: pshmem_double_alltoalls,                                         \
      long double: pshmem_longdouble_alltoalls,                                \
      char: pshmem_char_alltoalls,                                             \
      signed char: pshmem_schar_alltoalls,                                     \
      short: pshmem_short_alltoalls,                                           \
      int: pshmem_int_alltoalls,                                               \
      long: pshmem_long_alltoalls,                                             \
      long long: pshmem_longlong_alltoalls)(team, dest, source, dst, sst,      \
                                            nelems)

/**
 * @brief Generic team bitwise AND reduction operation
 */
#define pshmem_and_reduce(team, dest, source, nreduce)                         \
  _Generic(*(dest),                                                            \
      unsigned char: pshmem_uchar_and_reduce,                                  \
      unsigned short: pshmem_ushort_and_reduce,                                \
      unsigned int: pshmem_uint_and_reduce,                                    \
      unsigned long: pshmem_ulong_and_reduce,                                  \
      unsigned long long: pshmem_ulonglong_and_reduce)(team, dest, source,     \
                                                       nreduce)

/**
 * @brief Generic team bitwise OR reduction operation
 */
#define pshmem_or_reduce(team, dest, source, nreduce)                          \
  _Generic(*(dest),                                                            \
      unsigned char: pshmem_uchar_or_reduce,                                   \
      unsigned short: pshmem_ushort_or_reduce,                                 \
      unsigned int: pshmem_uint_or_reduce,                                     \
      unsigned long: pshmem_ulong_or_reduce,                                   \
      unsigned long long: pshmem_ulonglong_or_reduce)(team, dest, source,      \
                                                      nreduce)

/**
 * @brief Generic team bitwise XOR reduction operation
 */
#define pshmem_xor_reduce(team, dest, source, nreduce)                         \
  _Generic(*(dest),                                                            \
      unsigned char: pshmem_uchar_xor_reduce,                                  \
      unsigned short: pshmem_ushort_xor_reduce,                                \
      unsigned int: pshmem_uint_xor_reduce,                                    \
      unsigned long: pshmem_ulong_xor_reduce,                                  \
      unsigned long long: pshmem_ulonglong_xor_reduce)(team, dest, source,     \
                                                       nreduce)

/** @} */

#endif /* __STDC_VERSION__ >= 201112L test */
#endif /* __STDC_VERSION__ defined test */

/*
 * barriers & syncs
 */

void pshmem_barrier_all(void);
void pshmem_barrier(int PE_start, int logPE_stride, int PE_size, long *pSync);
void pshmem_sync_all(void);
// void pshmem_sync_deprecated(int PE_start, int logPE_stride, int PE_size, long
// *pSync);
void pshmem_fence(void);
void pshmem_quiet(void);

/**
 * @brief Completes communication to the listed PEs on the default context.
 * @param target_pes Array of world PE numbers, ignored when npes is zero.
 * @param npes Number of target PEs.
 */
void pshmem_pe_quiet(const int *target_pes, size_t npes);

/*
 * accessibility
 */

int pshmem_pe_accessible(int pe) _WUR;
int pshmem_addr_accessible(const void *addr, int pe) _WUR;

/*
 * symmetric memory management
 */

void *pshmalloc(size_t size) _WUR;
void pshfree(void *ptr);
void *pshrealloc(void *ptr, size_t size) _WUR;
void *pshmemalign(size_t alignment, size_t size) _WUR;

void *pshmem_malloc(size_t size) _WUR;
void *pshmem_calloc(size_t count, size_t size) _WUR;
void pshmem_free(void *ptr);
void *pshmem_realloc(void *ptr, size_t size) _WUR;
void *pshmem_memalign(size_t alignment, size_t size) _WUR;

/*
 * wait operations
 */

int pshmem_short_test(short *ivar, int cmp, short cmp_value);
int pshmem_int_test(int *ivar, int cmp, int cmp_value);
int pshmem_long_test(long *ivar, int cmp, long cmp_value);
int pshmem_longlong_test(long long *ivar, int cmp, long long cmp_value);
int pshmem_ushort_test(unsigned short *ivar, int cmp, unsigned short cmp_value);
int pshmem_uint_test(unsigned int *ivar, int cmp, unsigned int cmp_value);
int pshmem_ulong_test(unsigned long *ivar, int cmp, unsigned long cmp_value);
int pshmem_ulonglong_test(unsigned long long *ivar, int cmp,
                          unsigned long long cmp_value);
int pshmem_int32_test(int32_t *ivar, int cmp, int32_t cmp_value);
int pshmem_int64_test(int64_t *ivar, int cmp, int64_t cmp_value);
int pshmem_uint32_test(uint32_t *ivar, int cmp, uint32_t cmp_value);
int pshmem_uint64_test(uint64_t *ivar, int cmp, uint64_t cmp_value);
int pshmem_size_test(size_t *ivar, int cmp, size_t cmp_value);
int pshmem_ptrdiff_test(ptrdiff_t *ivar, int cmp, ptrdiff_t cmp_value);

/**
 * @brief Declares profiling entry points for vector comparisons.
 * @param _type Comparison element type.
 * @param _name OpenSHMEM type name.
 */
#define PSHMEM_DECL_VECTOR_SYNC(_type, _name)                                  \
  int pshmem_##_name##_test_all_vector(_type *ivars, size_t nelems,            \
                                       const int *status, int cmp,             \
                                       const _type *cmp_values);               \
  size_t pshmem_##_name##_test_any_vector(_type *ivars, size_t nelems,         \
                                          const int *status, int cmp,          \
                                          const _type *cmp_values);            \
  size_t pshmem_##_name##_test_some_vector(_type *ivars, size_t nelems,        \
                                           size_t *indices, const int *status, \
                                           int cmp, const _type *cmp_values);  \
  void pshmem_##_name##_wait_until_all_vector(_type *ivars, size_t nelems,     \
                                              const int *status, int cmp,      \
                                              const _type *cmp_values);        \
  size_t pshmem_##_name##_wait_until_any_vector(_type *ivars, size_t nelems,   \
                                                const int *status, int cmp,    \
                                                const _type *cmp_values);      \
  size_t pshmem_##_name##_wait_until_some_vector(                              \
      _type *ivars, size_t nelems, size_t *indices, const int *status,         \
      int cmp, const _type *cmp_values);

SHMEM_STANDARD_AMO_TYPE_TABLE(PSHMEM_DECL_VECTOR_SYNC)
#undef PSHMEM_DECL_VECTOR_SYNC

void pshmem_short_wait_until(short *ivar, int cmp, short cmp_value);
void pshmem_int_wait_until(int *ivar, int cmp, int cmp_value);
void pshmem_long_wait_until(long *ivar, int cmp, long cmp_value);
void pshmem_longlong_wait_until(long long *ivar, int cmp, long long cmp_value);
void pshmem_ushort_wait_until(unsigned short *ivar, int cmp,
                              unsigned short cmp_value);
void pshmem_uint_wait_until(unsigned int *ivar, int cmp,
                            unsigned int cmp_value);
void pshmem_ulong_wait_until(unsigned long *ivar, int cmp,
                             unsigned long cmp_value);
void pshmem_ulonglong_wait_until(unsigned long long *ivar, int cmp,
                                 unsigned long long cmp_value);
void pshmem_int32_wait_until(int32_t *ivar, int cmp, int32_t cmp_value);
void pshmem_int64_wait_until(int64_t *ivar, int cmp, int64_t cmp_value);
void pshmem_uint32_wait_until(uint32_t *ivar, int cmp, uint32_t cmp_value);
void pshmem_uint64_wait_until(uint64_t *ivar, int cmp, uint64_t cmp_value);
void pshmem_size_wait_until(size_t *ivar, int cmp, size_t cmp_value);
void pshmem_ptrdiff_wait_until(ptrdiff_t *ivar, int cmp, ptrdiff_t cmp_value);

/* deprecations */
void pshmem_short_wait(short *ivar, short cmp_value);
void pshmem_int_wait(int *ivar, int cmp_value);
void pshmem_long_wait(long *ivar, long cmp_value);
void pshmem_longlong_wait(long long *ivar, long long cmp_value);
void pshmem_ushort_wait(unsigned short *ivar, unsigned short cmp_value);
void pshmem_uint_wait(unsigned int *ivar, unsigned int cmp_value);
void pshmem_ulong_wait(unsigned long *ivar, unsigned long cmp_value);
void pshmem_ulonglong_wait(unsigned long long *ivar,
                           unsigned long long cmp_value);
void pshmem_int32_wait(int32_t *ivar, int32_t cmp_value);
void pshmem_int64_wait(int64_t *ivar, int64_t cmp_value);
void pshmem_uint32_wait(uint32_t *ivar, uint32_t cmp_value);
void pshmem_uint64_wait(uint64_t *ivar, uint64_t cmp_value);
void pshmem_size_wait(size_t *ivar, size_t cmp_value);
void pshmem_ptrdiff_wait(ptrdiff_t *ivar, ptrdiff_t cmp_value);

/*
 * atomic swaps
 */

float pshmem_float_atomic_swap(float *target, float value, int pe) _WUR;
double pshmem_double_atomic_swap(double *target, double value, int pe) _WUR;
short pshmem_short_atomic_swap(short *target, short value, int pe) _WUR;
int pshmem_int_atomic_swap(int *target, int value, int pe) _WUR;
long pshmem_long_atomic_swap(long *target, long value, int pe) _WUR;
long long pshmem_longlong_atomic_swap(long long *target, long long value,
                                      int pe) _WUR;
unsigned short pshmem_ushort_atomic_swap(unsigned short *target,
                                         unsigned short value, int pe) _WUR;
unsigned int pshmem_uint_atomic_swap(unsigned int *target, unsigned int value,
                                     int pe) _WUR;
unsigned long pshmem_ulong_atomic_swap(unsigned long *target,
                                       unsigned long value, int pe) _WUR;
unsigned long long pshmem_ulonglong_atomic_swap(unsigned long long *target,
                                                unsigned long long value,
                                                int pe) _WUR;
int32_t pshmem_int32_atomic_swap(int32_t *target, int32_t value, int pe) _WUR;
int64_t pshmem_int64_atomic_swap(int64_t *target, int64_t value, int pe) _WUR;
uint32_t pshmem_uint32_atomic_swap(uint32_t *target, uint32_t value,
                                   int pe) _WUR;
uint64_t pshmem_uint64_atomic_swap(uint64_t *target, uint64_t value,
                                   int pe) _WUR;
size_t pshmem_size_atomic_swap(size_t *target, size_t value, int pe) _WUR;
ptrdiff_t pshmem_ptrdiff_atomic_swap(ptrdiff_t *target, ptrdiff_t value,
                                     int pe) _WUR;

short pshmem_short_atomic_compare_swap(short *target, short value, int pe) _WUR;
int pshmem_int_atomic_compare_swap(int *target, int value, int pe) _WUR;
long pshmem_long_atomic_compare_swap(long *target, long value, int pe) _WUR;
long long pshmem_longlong_atomic_compare_swap(long long *target,
                                              long long value, int pe) _WUR;
unsigned short pshmem_ushort_atomic_compare_swap(unsigned short *target,
                                                 unsigned short value,
                                                 int pe) _WUR;
unsigned int pshmem_uint_atomic_compare_swap(unsigned int *target,
                                             unsigned int value, int pe) _WUR;
unsigned long pshmem_ulong_atomic_compare_swap(unsigned long *target,
                                               unsigned long value,
                                               int pe) _WUR;
unsigned long long
pshmem_ulonglong_atomic_compare_swap(unsigned long long *target,
                                     unsigned long long value, int pe) _WUR;
int32_t pshmem_int32_atomic_compare_swap(int32_t *target, int32_t value,
                                         int pe) _WUR;
int64_t pshmem_int64_atomic_compare_swap(int64_t *target, int64_t value,
                                         int pe) _WUR;
uint32_t pshmem_uint32_atomic_compare_swap(uint32_t *target, uint32_t value,
                                           int pe) _WUR;
uint64_t pshmem_uint64_atomic_compare_swap(uint64_t *target, uint64_t value,
                                           int pe) _WUR;
size_t pshmem_size_atomic_compare_swap(size_t *target, size_t value,
                                       int pe) _WUR;
ptrdiff_t pshmem_ptrdiff_atomic_compare_swap(ptrdiff_t *target, ptrdiff_t value,
                                             int pe) _WUR;

/*
 * atomic fetch-{add,inc} & add,inc
 */

int pshmem_int_fadd(int *target, int value, int pe) _WUR;
long pshmem_long_fadd(long *target, long value, int pe) _WUR;
long long pshmem_longlong_fadd(long long *target, long long value, int pe) _WUR;
int pshmem_int_finc(int *target, int pe) _WUR;
long pshmem_long_finc(long *target, int pe) _WUR;
long long pshmem_longlong_finc(long long *target, int pe) _WUR;

void pshmem_int_add(int *target, int value, int pe);
void pshmem_long_add(long *target, long value, int pe);
void pshmem_longlong_add(long long *target, long long value, int pe);
void pshmem_int_inc(int *target, int pe);
void pshmem_long_inc(long *target, int pe);
void pshmem_longlong_inc(long long *target, int pe);

/*
 * cache flushing
 */

void pshmem_clear_cache_inv(void);
void pshmem_set_cache_inv(void);
void pshmem_clear_cache_line_inv(void *target);
void pshmem_set_cache_line_inv(void *target);
void pshmem_udcflush(void);
void pshmem_udcflush_line(void *target);

/*
 * reductions
 */

void pshmem_complexd_sum_to_all(COMPLEXIFY(double) * target,
                                const COMPLEXIFY(double) * source, int nreduce,
                                int PE_start, int logPE_stride, int PE_size,
                                COMPLEXIFY(double) * pWrk, long *pSync);
void pshmem_complexf_sum_to_all(COMPLEXIFY(float) * target,
                                const COMPLEXIFY(float) * source, int nreduce,
                                int PE_start, int logPE_stride, int PE_size,
                                COMPLEXIFY(float) * pWrk, long *pSync);
void pshmem_double_sum_to_all(double *target, const double *source, int nreduce,
                              int PE_start, int logPE_stride, int PE_size,
                              double *pWrk, long *pSync);
void pshmem_float_sum_to_all(float *target, const float *source, int nreduce,
                             int PE_start, int logPE_stride, int PE_size,
                             float *pWrk, long *pSync);
void pshmem_int_sum_to_all(int *target, const int *source, int nreduce,
                           int PE_start, int logPE_stride, int PE_size,
                           int *pWrk, long *pSync);
void pshmem_long_sum_to_all(long *target, const long *source, int nreduce,
                            int PE_start, int logPE_stride, int PE_size,
                            long *pWrk, long *pSync);
void pshmem_longdouble_sum_to_all(long double *target,
                                  const long double *source, int nreduce,
                                  int PE_start, int logPE_stride, int PE_size,
                                  long double *pWrk, long *pSync);
void pshmem_longlong_sum_to_all(long long *target, const long long *source,
                                int nreduce, int PE_start, int logPE_stride,
                                int PE_size, long long *pWrk, long *pSync);
void pshmem_short_sum_to_all(short *target, const short *source, int nreduce,
                             int PE_start, int logPE_stride, int PE_size,
                             short *pWrk, long *pSync);

void pshmem_double_prod_to_all(double *target, const double *source,
                               int nreduce, int PE_start, int logPE_stride,
                               int PE_size, double *pWrk, long *pSync);
void pshmem_float_prod_to_all(float *target, const float *source, int nreduce,
                              int PE_start, int logPE_stride, int PE_size,
                              float *pWrk, long *pSync);
void pshmem_int_prod_to_all(int *target, const int *source, int nreduce,
                            int PE_start, int logPE_stride, int PE_size,
                            int *pWrk, long *pSync);
void pshmem_long_prod_to_all(long *target, const long *source, int nreduce,
                             int PE_start, int logPE_stride, int PE_size,
                             long *pWrk, long *pSync);
void pshmem_longdouble_prod_to_all(long double *target,
                                   const long double *source, int nreduce,
                                   int PE_start, int logPE_stride, int PE_size,
                                   long double *pWrk, long *pSync);
void pshmem_longlong_prod_to_all(long long *target, const long long *source,
                                 int nreduce, int PE_start, int logPE_stride,
                                 int PE_size, long long *pWrk, long *pSync);
void pshmem_short_prod_to_all(short *target, const short *source, int nreduce,
                              int PE_start, int logPE_stride, int PE_size,
                              short *pWrk, long *pSync);
void pshmem_complexd_prod_to_all(COMPLEXIFY(double) * target,
                                 const COMPLEXIFY(double) * source, int nreduce,
                                 int PE_start, int logPE_stride, int PE_size,
                                 COMPLEXIFY(double) * pWrk, long *pSync);
void pshmem_complexf_prod_to_all(COMPLEXIFY(float) * target,
                                 const COMPLEXIFY(float) * source, int nreduce,
                                 int PE_start, int logPE_stride, int PE_size,
                                 COMPLEXIFY(float) * pWrk, long *pSync);

void pshmem_int_and_to_all(int *target, const int *source, int nreduce,
                           int PE_start, int logPE_stride, int PE_size,
                           int *pWrk, long *pSync);
void pshmem_long_and_to_all(long *target, const long *source, int nreduce,
                            int PE_start, int logPE_stride, int PE_size,
                            long *pWrk, long *pSync);
void pshmem_longlong_and_to_all(long long *target, const long long *source,
                                int nreduce, int PE_start, int logPE_stride,
                                int PE_size, long long *pWrk, long *pSync);
void pshmem_short_and_to_all(short *target, const short *source, int nreduce,
                             int PE_start, int logPE_stride, int PE_size,
                             short *pWrk, long *pSync);

void pshmem_int_or_to_all(int *target, const int *source, int nreduce,
                          int PE_start, int logPE_stride, int PE_size,
                          int *pWrk, long *pSync);
void pshmem_long_or_to_all(long *target, const long *source, int nreduce,
                           int PE_start, int logPE_stride, int PE_size,
                           long *pWrk, long *pSync);
void pshmem_longlong_or_to_all(long long *target, const long long *source,
                               int nreduce, int PE_start, int logPE_stride,
                               int PE_size, long long *pWrk, long *pSync);
void pshmem_short_or_to_all(short *target, const short *source, int nreduce,
                            int PE_start, int logPE_stride, int PE_size,
                            short *pWrk, long *pSync);

void pshmem_int_xor_to_all(int *target, const int *source, int nreduce,
                           int PE_start, int logPE_stride, int PE_size,
                           int *pWrk, long *pSync);
void pshmem_long_xor_to_all(long *target, const long *source, int nreduce,
                            int PE_start, int logPE_stride, int PE_size,
                            long *pWrk, long *pSync);
void pshmem_longlong_xor_to_all(long long *target, const long long *source,
                                int nreduce, int PE_start, int logPE_stride,
                                int PE_size, long long *pWrk, long *pSync);
void pshmem_short_xor_to_all(short *target, const short *source, int nreduce,
                             int PE_start, int logPE_stride, int PE_size,
                             short *pWrk, long *pSync);

void pshmem_int_max_to_all(int *target, const int *source, int nreduce,
                           int PE_start, int logPE_stride, int PE_size,
                           int *pWrk, long *pSync);
void pshmem_long_max_to_all(long *target, const long *source, int nreduce,
                            int PE_start, int logPE_stride, int PE_size,
                            long *pWrk, long *pSync);
void pshmem_longlong_max_to_all(long long *target, const long long *source,
                                int nreduce, int PE_start, int logPE_stride,
                                int PE_size, long long *pWrk, long *pSync);
void pshmem_short_max_to_all(short *target, const short *source, int nreduce,
                             int PE_start, int logPE_stride, int PE_size,
                             short *pWrk, long *pSync);
void pshmem_longdouble_max_to_all(long double *target,
                                  const long double *source, int nreduce,
                                  int PE_start, int logPE_stride, int PE_size,
                                  long double *pWrk, long *pSync);
void pshmem_float_max_to_all(float *target, const float *source, int nreduce,
                             int PE_start, int logPE_stride, int PE_size,
                             float *pWrk, long *pSync);
void pshmem_double_max_to_all(double *target, const double *source, int nreduce,
                              int PE_start, int logPE_stride, int PE_size,
                              double *pWrk, long *pSync);

void pshmem_int_min_to_all(int *target, const int *source, int nreduce,
                           int PE_start, int logPE_stride, int PE_size,
                           int *pWrk, long *pSync);
void pshmem_long_min_to_all(long *target, const long *source, int nreduce,
                            int PE_start, int logPE_stride, int PE_size,
                            long *pWrk, long *pSync);
void pshmem_longlong_min_to_all(long long *target, const long long *source,
                                int nreduce, int PE_start, int logPE_stride,
                                int PE_size, long long *pWrk, long *pSync);
void pshmem_short_min_to_all(short *target, const short *source, int nreduce,
                             int PE_start, int logPE_stride, int PE_size,
                             short *pWrk, long *pSync);
void pshmem_longdouble_min_to_all(long double *target,
                                  const long double *source, int nreduce,
                                  int PE_start, int logPE_stride, int PE_size,
                                  long double *pWrk, long *pSync);
void pshmem_float_min_to_all(float *target, const float *source, int nreduce,
                             int PE_start, int logPE_stride, int PE_size,
                             float *pWrk, long *pSync);
void pshmem_double_min_to_all(double *target, const double *source, int nreduce,
                              int PE_start, int logPE_stride, int PE_size,
                              double *pWrk, long *pSync);

/*
 * broadcasts
 */

void pshmem_broadcast32(void *target, const void *source, size_t nelems,
                        int PE_root, int PE_start, int logPE_stride,
                        int PE_size, long *pSync);

void pshmem_broadcast64(void *target, const void *source, size_t nelems,
                        int PE_root, int PE_start, int logPE_stride,
                        int PE_size, long *pSync);

/*
 * collects
 */

void pshmem_fcollect32(void *target, const void *source, size_t nelems,
                       int PE_start, int logPE_stride, int PE_size,
                       long *pSync);
void pshmem_fcollect64(void *target, const void *source, size_t nelems,
                       int PE_start, int logPE_stride, int PE_size,
                       long *pSync);

void pshmem_collect32(void *target, const void *source, size_t nelems,
                      int PE_start, int logPE_stride, int PE_size, long *pSync);
void pshmem_collect64(void *target, const void *source, size_t nelems,
                      int PE_start, int logPE_stride, int PE_size, long *pSync);

/*
 * team-based collectives
 */

/**
 * @brief Macro to declare typed team broadcast operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_BROADCAST(_name, _type)                               \
  int pshmem_##_name##_broadcast(shmem_team_t team, _type *dest,               \
                                 const _type *source, size_t nelems,           \
                                 int PE_root);

PSHMEM_DECL_TEAM_BROADCAST(float, float)
PSHMEM_DECL_TEAM_BROADCAST(double, double)
PSHMEM_DECL_TEAM_BROADCAST(longdouble, long double)
PSHMEM_DECL_TEAM_BROADCAST(char, char)
PSHMEM_DECL_TEAM_BROADCAST(schar, signed char)
PSHMEM_DECL_TEAM_BROADCAST(short, short)
PSHMEM_DECL_TEAM_BROADCAST(int, int)
PSHMEM_DECL_TEAM_BROADCAST(long, long)
PSHMEM_DECL_TEAM_BROADCAST(longlong, long long)
PSHMEM_DECL_TEAM_BROADCAST(uchar, unsigned char)
PSHMEM_DECL_TEAM_BROADCAST(ushort, unsigned short)
PSHMEM_DECL_TEAM_BROADCAST(uint, unsigned int)
PSHMEM_DECL_TEAM_BROADCAST(ulong, unsigned long)
PSHMEM_DECL_TEAM_BROADCAST(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_BROADCAST(int8, int8_t)
PSHMEM_DECL_TEAM_BROADCAST(int16, int16_t)
PSHMEM_DECL_TEAM_BROADCAST(int32, int32_t)
PSHMEM_DECL_TEAM_BROADCAST(int64, int64_t)
PSHMEM_DECL_TEAM_BROADCAST(uint8, uint8_t)
PSHMEM_DECL_TEAM_BROADCAST(uint16, uint16_t)
PSHMEM_DECL_TEAM_BROADCAST(uint32, uint32_t)
PSHMEM_DECL_TEAM_BROADCAST(uint64, uint64_t)
PSHMEM_DECL_TEAM_BROADCAST(size, size_t)
PSHMEM_DECL_TEAM_BROADCAST(ptrdiff, ptrdiff_t)

#undef PSHMEM_DECL_TEAM_BROADCAST

/**
 * @brief Generic memory broadcast routine
 * @param team The team over which to broadcast
 * @param dest Symmetric destination array on all PEs
 * @param source Source array on root PE
 * @param nelems Number of elements to broadcast
 * @param PE_root PE number of root PE where data originates
 * @return Zero on success, non-zero on failure
 */
int pshmem_broadcastmem(shmem_team_t team, void *dest, const void *source,
                        size_t nelems, int PE_root) _WUR;

/**
 * @brief Macro to declare typed team collect operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_COLLECT(_name, _type)                                 \
  int pshmem_##_name##_collect(shmem_team_t team, _type *dest,                 \
                               const _type *source, size_t nelems);

PSHMEM_DECL_TEAM_COLLECT(float, float)
PSHMEM_DECL_TEAM_COLLECT(double, double)
PSHMEM_DECL_TEAM_COLLECT(longdouble, long double)
PSHMEM_DECL_TEAM_COLLECT(char, char)
PSHMEM_DECL_TEAM_COLLECT(schar, signed char)
PSHMEM_DECL_TEAM_COLLECT(short, short)
PSHMEM_DECL_TEAM_COLLECT(int, int)
PSHMEM_DECL_TEAM_COLLECT(long, long)
PSHMEM_DECL_TEAM_COLLECT(longlong, long long)
PSHMEM_DECL_TEAM_COLLECT(uchar, unsigned char)
PSHMEM_DECL_TEAM_COLLECT(ushort, unsigned short)
PSHMEM_DECL_TEAM_COLLECT(uint, unsigned int)
PSHMEM_DECL_TEAM_COLLECT(ulong, unsigned long)
PSHMEM_DECL_TEAM_COLLECT(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_COLLECT(int8, int8_t)
PSHMEM_DECL_TEAM_COLLECT(int16, int16_t)
PSHMEM_DECL_TEAM_COLLECT(int32, int32_t)
PSHMEM_DECL_TEAM_COLLECT(int64, int64_t)
PSHMEM_DECL_TEAM_COLLECT(uint8, uint8_t)
PSHMEM_DECL_TEAM_COLLECT(uint16, uint16_t)
PSHMEM_DECL_TEAM_COLLECT(uint32, uint32_t)
PSHMEM_DECL_TEAM_COLLECT(uint64, uint64_t)
PSHMEM_DECL_TEAM_COLLECT(size, size_t)
PSHMEM_DECL_TEAM_COLLECT(ptrdiff, ptrdiff_t)

#undef PSHMEM_DECL_TEAM_COLLECT

/**
 * @brief Generic memory collect routine
 * @param team Team on which to perform the collect
 * @param dest Symmetric destination array on all PEs
 * @param source Local array containing data to be concatenated
 * @param nelems Number of elements in source array
 * @return Zero on success, non-zero otherwise
 */
int pshmem_collectmem(shmem_team_t team, void *dest, const void *source,
                      size_t nelems) _WUR;

/**
 * @brief Macro to declare typed team fcollect operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_FCOLLECT(_name, _type)                                \
  int pshmem_##_name##_fcollect(shmem_team_t team, _type *dest,                \
                                const _type *source, size_t nelems);

PSHMEM_DECL_TEAM_FCOLLECT(float, float)
PSHMEM_DECL_TEAM_FCOLLECT(double, double)
PSHMEM_DECL_TEAM_FCOLLECT(longdouble, long double)
PSHMEM_DECL_TEAM_FCOLLECT(char, char)
PSHMEM_DECL_TEAM_FCOLLECT(schar, signed char)
PSHMEM_DECL_TEAM_FCOLLECT(short, short)
PSHMEM_DECL_TEAM_FCOLLECT(int, int)
PSHMEM_DECL_TEAM_FCOLLECT(long, long)
PSHMEM_DECL_TEAM_FCOLLECT(longlong, long long)
PSHMEM_DECL_TEAM_FCOLLECT(uchar, unsigned char)
PSHMEM_DECL_TEAM_FCOLLECT(ushort, unsigned short)
PSHMEM_DECL_TEAM_FCOLLECT(uint, unsigned int)
PSHMEM_DECL_TEAM_FCOLLECT(ulong, unsigned long)
PSHMEM_DECL_TEAM_FCOLLECT(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_FCOLLECT(int8, int8_t)
PSHMEM_DECL_TEAM_FCOLLECT(int16, int16_t)
PSHMEM_DECL_TEAM_FCOLLECT(int32, int32_t)
PSHMEM_DECL_TEAM_FCOLLECT(int64, int64_t)
PSHMEM_DECL_TEAM_FCOLLECT(uint8, uint8_t)
PSHMEM_DECL_TEAM_FCOLLECT(uint16, uint16_t)
PSHMEM_DECL_TEAM_FCOLLECT(uint32, uint32_t)
PSHMEM_DECL_TEAM_FCOLLECT(uint64, uint64_t)
PSHMEM_DECL_TEAM_FCOLLECT(size, size_t)
PSHMEM_DECL_TEAM_FCOLLECT(ptrdiff, ptrdiff_t)

#undef PSHMEM_DECL_TEAM_FCOLLECT

/**
 * @brief Generic memory fcollect routine
 * @param team Team on which to perform the fcollect
 * @param dest Symmetric destination array on all PEs
 * @param source Local array containing data to be concatenated
 * @param nelems Number of elements in source array
 * @return Zero on success, non-zero otherwise
 */
int pshmem_fcollectmem(shmem_team_t team, void *dest, const void *source,
                       size_t nelems) _WUR;

/**
 * @brief Macro to declare typed team alltoall operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_ALLTOALL(_name, _type)                                \
  int pshmem_##_name##_alltoall(shmem_team_t team, _type *dest,                \
                                const _type *source, size_t nelems);

PSHMEM_DECL_TEAM_ALLTOALL(float, float)
PSHMEM_DECL_TEAM_ALLTOALL(double, double)
PSHMEM_DECL_TEAM_ALLTOALL(longdouble, long double)
PSHMEM_DECL_TEAM_ALLTOALL(char, char)
PSHMEM_DECL_TEAM_ALLTOALL(schar, signed char)
PSHMEM_DECL_TEAM_ALLTOALL(short, short)
PSHMEM_DECL_TEAM_ALLTOALL(int, int)
PSHMEM_DECL_TEAM_ALLTOALL(long, long)
PSHMEM_DECL_TEAM_ALLTOALL(longlong, long long)
PSHMEM_DECL_TEAM_ALLTOALL(uchar, unsigned char)
PSHMEM_DECL_TEAM_ALLTOALL(ushort, unsigned short)
PSHMEM_DECL_TEAM_ALLTOALL(uint, unsigned int)
PSHMEM_DECL_TEAM_ALLTOALL(ulong, unsigned long)
PSHMEM_DECL_TEAM_ALLTOALL(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_ALLTOALL(int8, int8_t)
PSHMEM_DECL_TEAM_ALLTOALL(int16, int16_t)
PSHMEM_DECL_TEAM_ALLTOALL(int32, int32_t)
PSHMEM_DECL_TEAM_ALLTOALL(int64, int64_t)
PSHMEM_DECL_TEAM_ALLTOALL(uint8, uint8_t)
PSHMEM_DECL_TEAM_ALLTOALL(uint16, uint16_t)
PSHMEM_DECL_TEAM_ALLTOALL(uint32, uint32_t)
PSHMEM_DECL_TEAM_ALLTOALL(uint64, uint64_t)
PSHMEM_DECL_TEAM_ALLTOALL(size, size_t)
PSHMEM_DECL_TEAM_ALLTOALL(ptrdiff, ptrdiff_t)

#undef PSHMEM_DECL_TEAM_ALLTOALL

/**
 * @brief Generic memory alltoall routine
 * @param team The team over which to alltoall
 * @param dest Symmetric destination array on all PEs
 * @param source Source array on root PE
 * @param nelems Number of elements to alltoall
 * @return Zero on success, non-zero on failure
 */
int pshmem_alltoallmem(shmem_team_t team, void *dest, const void *source,
                       size_t nelems) _WUR;

/**
 * @brief Macro to declare typed team alltoalls operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_ALLTOALLS(_name, _type)                               \
  int pshmem_##_name##_alltoalls(shmem_team_t team, _type *dest,               \
                                 const _type *source, ptrdiff_t dst,           \
                                 ptrdiff_t sst, size_t nelems);

PSHMEM_DECL_TEAM_ALLTOALLS(float, float)
PSHMEM_DECL_TEAM_ALLTOALLS(double, double)
PSHMEM_DECL_TEAM_ALLTOALLS(longdouble, long double)
PSHMEM_DECL_TEAM_ALLTOALLS(char, char)
PSHMEM_DECL_TEAM_ALLTOALLS(schar, signed char)
PSHMEM_DECL_TEAM_ALLTOALLS(short, short)
PSHMEM_DECL_TEAM_ALLTOALLS(int, int)
PSHMEM_DECL_TEAM_ALLTOALLS(long, long)
PSHMEM_DECL_TEAM_ALLTOALLS(longlong, long long)
PSHMEM_DECL_TEAM_ALLTOALLS(uchar, unsigned char)
PSHMEM_DECL_TEAM_ALLTOALLS(ushort, unsigned short)
PSHMEM_DECL_TEAM_ALLTOALLS(uint, unsigned int)
PSHMEM_DECL_TEAM_ALLTOALLS(ulong, unsigned long)
PSHMEM_DECL_TEAM_ALLTOALLS(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_ALLTOALLS(int8, int8_t)
PSHMEM_DECL_TEAM_ALLTOALLS(int16, int16_t)
PSHMEM_DECL_TEAM_ALLTOALLS(int32, int32_t)
PSHMEM_DECL_TEAM_ALLTOALLS(int64, int64_t)
PSHMEM_DECL_TEAM_ALLTOALLS(uint8, uint8_t)
PSHMEM_DECL_TEAM_ALLTOALLS(uint16, uint16_t)
PSHMEM_DECL_TEAM_ALLTOALLS(uint32, uint32_t)
PSHMEM_DECL_TEAM_ALLTOALLS(uint64, uint64_t)
PSHMEM_DECL_TEAM_ALLTOALLS(size, size_t)
PSHMEM_DECL_TEAM_ALLTOALLS(ptrdiff, ptrdiff_t)

#undef PSHMEM_DECL_TEAM_ALLTOALLS

/**
 * @brief Generic memory alltoalls routine
 * @param team The team over which to alltoalls
 * @param dest Symmetric destination array on all PEs
 * @param source Source array on root PE
 * @param dst Destination stride
 * @param sst Source stride
 * @param nelems Number of elements to alltoalls
 * @return Zero on success, non-zero on failure
 */
int pshmem_alltoallsmem(shmem_team_t team, void *dest, const void *source,
                        ptrdiff_t dst, ptrdiff_t sst, size_t nelems) _WUR;

/*
 * team-based reductions
 */

/**
 * @brief Macro to declare typed team sum reduction operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_SUM_REDUCE(_name, _type)                              \
  int pshmem_##_name##_sum_reduce(shmem_team_t team, _type *dest,              \
                                  const _type *source, size_t nreduce);

PSHMEM_DECL_TEAM_SUM_REDUCE(char, char)
PSHMEM_DECL_TEAM_SUM_REDUCE(schar, signed char)
PSHMEM_DECL_TEAM_SUM_REDUCE(short, short)
PSHMEM_DECL_TEAM_SUM_REDUCE(int, int)
PSHMEM_DECL_TEAM_SUM_REDUCE(long, long)
PSHMEM_DECL_TEAM_SUM_REDUCE(longlong, long long)
PSHMEM_DECL_TEAM_SUM_REDUCE(ptrdiff, ptrdiff_t)
PSHMEM_DECL_TEAM_SUM_REDUCE(uchar, unsigned char)
PSHMEM_DECL_TEAM_SUM_REDUCE(ushort, unsigned short)
PSHMEM_DECL_TEAM_SUM_REDUCE(uint, unsigned int)
PSHMEM_DECL_TEAM_SUM_REDUCE(ulong, unsigned long)
PSHMEM_DECL_TEAM_SUM_REDUCE(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_SUM_REDUCE(int8, int8_t)
PSHMEM_DECL_TEAM_SUM_REDUCE(int16, int16_t)
PSHMEM_DECL_TEAM_SUM_REDUCE(int32, int32_t)
PSHMEM_DECL_TEAM_SUM_REDUCE(int64, int64_t)
PSHMEM_DECL_TEAM_SUM_REDUCE(uint8, uint8_t)
PSHMEM_DECL_TEAM_SUM_REDUCE(uint16, uint16_t)
PSHMEM_DECL_TEAM_SUM_REDUCE(uint32, uint32_t)
PSHMEM_DECL_TEAM_SUM_REDUCE(uint64, uint64_t)
PSHMEM_DECL_TEAM_SUM_REDUCE(size, size_t)
PSHMEM_DECL_TEAM_SUM_REDUCE(float, float)
PSHMEM_DECL_TEAM_SUM_REDUCE(double, double)
PSHMEM_DECL_TEAM_SUM_REDUCE(longdouble, long double)
PSHMEM_DECL_TEAM_SUM_REDUCE(complexd, COMPLEXIFY(double))
PSHMEM_DECL_TEAM_SUM_REDUCE(complexf, COMPLEXIFY(float))

#undef PSHMEM_DECL_TEAM_SUM_REDUCE

/**
 * @brief Macro to declare typed team product reduction operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_PROD_REDUCE(_name, _type)                             \
  int pshmem_##_name##_prod_reduce(shmem_team_t team, _type *dest,             \
                                   const _type *source, size_t nreduce);

PSHMEM_DECL_TEAM_PROD_REDUCE(char, char)
PSHMEM_DECL_TEAM_PROD_REDUCE(schar, signed char)
PSHMEM_DECL_TEAM_PROD_REDUCE(short, short)
PSHMEM_DECL_TEAM_PROD_REDUCE(int, int)
PSHMEM_DECL_TEAM_PROD_REDUCE(long, long)
PSHMEM_DECL_TEAM_PROD_REDUCE(longlong, long long)
PSHMEM_DECL_TEAM_PROD_REDUCE(ptrdiff, ptrdiff_t)
PSHMEM_DECL_TEAM_PROD_REDUCE(uchar, unsigned char)
PSHMEM_DECL_TEAM_PROD_REDUCE(ushort, unsigned short)
PSHMEM_DECL_TEAM_PROD_REDUCE(uint, unsigned int)
PSHMEM_DECL_TEAM_PROD_REDUCE(ulong, unsigned long)
PSHMEM_DECL_TEAM_PROD_REDUCE(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_PROD_REDUCE(int8, int8_t)
PSHMEM_DECL_TEAM_PROD_REDUCE(int16, int16_t)
PSHMEM_DECL_TEAM_PROD_REDUCE(int32, int32_t)
PSHMEM_DECL_TEAM_PROD_REDUCE(int64, int64_t)
PSHMEM_DECL_TEAM_PROD_REDUCE(uint8, uint8_t)
PSHMEM_DECL_TEAM_PROD_REDUCE(uint16, uint16_t)
PSHMEM_DECL_TEAM_PROD_REDUCE(uint32, uint32_t)
PSHMEM_DECL_TEAM_PROD_REDUCE(uint64, uint64_t)
PSHMEM_DECL_TEAM_PROD_REDUCE(size, size_t)
PSHMEM_DECL_TEAM_PROD_REDUCE(float, float)
PSHMEM_DECL_TEAM_PROD_REDUCE(double, double)
PSHMEM_DECL_TEAM_PROD_REDUCE(longdouble, long double)
PSHMEM_DECL_TEAM_PROD_REDUCE(complexd, COMPLEXIFY(double))
PSHMEM_DECL_TEAM_PROD_REDUCE(complexf, COMPLEXIFY(float))

#undef PSHMEM_DECL_TEAM_PROD_REDUCE

/**
 * @brief Macro to declare typed team maximum reduction operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_MAX_REDUCE(_name, _type)                              \
  int pshmem_##_name##_max_reduce(shmem_team_t team, _type *dest,              \
                                  const _type *source, size_t nreduce);

PSHMEM_DECL_TEAM_MAX_REDUCE(char, char)
PSHMEM_DECL_TEAM_MAX_REDUCE(schar, signed char)
PSHMEM_DECL_TEAM_MAX_REDUCE(short, short)
PSHMEM_DECL_TEAM_MAX_REDUCE(int, int)
PSHMEM_DECL_TEAM_MAX_REDUCE(long, long)
PSHMEM_DECL_TEAM_MAX_REDUCE(longlong, long long)
PSHMEM_DECL_TEAM_MAX_REDUCE(ptrdiff, ptrdiff_t)
PSHMEM_DECL_TEAM_MAX_REDUCE(uchar, unsigned char)
PSHMEM_DECL_TEAM_MAX_REDUCE(ushort, unsigned short)
PSHMEM_DECL_TEAM_MAX_REDUCE(uint, unsigned int)
PSHMEM_DECL_TEAM_MAX_REDUCE(ulong, unsigned long)
PSHMEM_DECL_TEAM_MAX_REDUCE(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_MAX_REDUCE(int8, int8_t)
PSHMEM_DECL_TEAM_MAX_REDUCE(int16, int16_t)
PSHMEM_DECL_TEAM_MAX_REDUCE(int32, int32_t)
PSHMEM_DECL_TEAM_MAX_REDUCE(int64, int64_t)
PSHMEM_DECL_TEAM_MAX_REDUCE(uint8, uint8_t)
PSHMEM_DECL_TEAM_MAX_REDUCE(uint16, uint16_t)
PSHMEM_DECL_TEAM_MAX_REDUCE(uint32, uint32_t)
PSHMEM_DECL_TEAM_MAX_REDUCE(uint64, uint64_t)
PSHMEM_DECL_TEAM_MAX_REDUCE(size, size_t)
PSHMEM_DECL_TEAM_MAX_REDUCE(float, float)
PSHMEM_DECL_TEAM_MAX_REDUCE(double, double)
PSHMEM_DECL_TEAM_MAX_REDUCE(longdouble, long double)

#undef PSHMEM_DECL_TEAM_MAX_REDUCE

/**
 * @brief Macro to declare typed team minimum reduction operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_MIN_REDUCE(_name, _type)                              \
  int pshmem_##_name##_min_reduce(shmem_team_t team, _type *dest,              \
                                  const _type *source, size_t nreduce);

PSHMEM_DECL_TEAM_MIN_REDUCE(char, char)
PSHMEM_DECL_TEAM_MIN_REDUCE(schar, signed char)
PSHMEM_DECL_TEAM_MIN_REDUCE(short, short)
PSHMEM_DECL_TEAM_MIN_REDUCE(int, int)
PSHMEM_DECL_TEAM_MIN_REDUCE(long, long)
PSHMEM_DECL_TEAM_MIN_REDUCE(longlong, long long)
PSHMEM_DECL_TEAM_MIN_REDUCE(ptrdiff, ptrdiff_t)
PSHMEM_DECL_TEAM_MIN_REDUCE(uchar, unsigned char)
PSHMEM_DECL_TEAM_MIN_REDUCE(ushort, unsigned short)
PSHMEM_DECL_TEAM_MIN_REDUCE(uint, unsigned int)
PSHMEM_DECL_TEAM_MIN_REDUCE(ulong, unsigned long)
PSHMEM_DECL_TEAM_MIN_REDUCE(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_MIN_REDUCE(int8, int8_t)
PSHMEM_DECL_TEAM_MIN_REDUCE(int16, int16_t)
PSHMEM_DECL_TEAM_MIN_REDUCE(int32, int32_t)
PSHMEM_DECL_TEAM_MIN_REDUCE(int64, int64_t)
PSHMEM_DECL_TEAM_MIN_REDUCE(uint8, uint8_t)
PSHMEM_DECL_TEAM_MIN_REDUCE(uint16, uint16_t)
PSHMEM_DECL_TEAM_MIN_REDUCE(uint32, uint32_t)
PSHMEM_DECL_TEAM_MIN_REDUCE(uint64, uint64_t)
PSHMEM_DECL_TEAM_MIN_REDUCE(size, size_t)
PSHMEM_DECL_TEAM_MIN_REDUCE(float, float)
PSHMEM_DECL_TEAM_MIN_REDUCE(double, double)
PSHMEM_DECL_TEAM_MIN_REDUCE(longdouble, long double)

#undef PSHMEM_DECL_TEAM_MIN_REDUCE

/**
 * @brief Macro to declare typed team bitwise AND reduction operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_AND_REDUCE(_name, _type)                              \
  int pshmem_##_name##_and_reduce(shmem_team_t team, _type *dest,              \
                                  const _type *source, size_t nreduce);

PSHMEM_DECL_TEAM_AND_REDUCE(uchar, unsigned char)
PSHMEM_DECL_TEAM_AND_REDUCE(ushort, unsigned short)
PSHMEM_DECL_TEAM_AND_REDUCE(uint, unsigned int)
PSHMEM_DECL_TEAM_AND_REDUCE(ulong, unsigned long)
PSHMEM_DECL_TEAM_AND_REDUCE(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_AND_REDUCE(int8, int8_t)
PSHMEM_DECL_TEAM_AND_REDUCE(int16, int16_t)
PSHMEM_DECL_TEAM_AND_REDUCE(int32, int32_t)
PSHMEM_DECL_TEAM_AND_REDUCE(int64, int64_t)
PSHMEM_DECL_TEAM_AND_REDUCE(uint8, uint8_t)
PSHMEM_DECL_TEAM_AND_REDUCE(uint16, uint16_t)
PSHMEM_DECL_TEAM_AND_REDUCE(uint32, uint32_t)
PSHMEM_DECL_TEAM_AND_REDUCE(uint64, uint64_t)
PSHMEM_DECL_TEAM_AND_REDUCE(size, size_t)

#undef PSHMEM_DECL_TEAM_AND_REDUCE

/**
 * @brief Macro to declare typed team bitwise OR reduction operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_OR_REDUCE(_name, _type)                               \
  int pshmem_##_name##_or_reduce(shmem_team_t team, _type *dest,               \
                                 const _type *source, size_t nreduce);

PSHMEM_DECL_TEAM_OR_REDUCE(uchar, unsigned char)
PSHMEM_DECL_TEAM_OR_REDUCE(ushort, unsigned short)
PSHMEM_DECL_TEAM_OR_REDUCE(uint, unsigned int)
PSHMEM_DECL_TEAM_OR_REDUCE(ulong, unsigned long)
PSHMEM_DECL_TEAM_OR_REDUCE(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_OR_REDUCE(int8, int8_t)
PSHMEM_DECL_TEAM_OR_REDUCE(int16, int16_t)
PSHMEM_DECL_TEAM_OR_REDUCE(int32, int32_t)
PSHMEM_DECL_TEAM_OR_REDUCE(int64, int64_t)
PSHMEM_DECL_TEAM_OR_REDUCE(uint8, uint8_t)
PSHMEM_DECL_TEAM_OR_REDUCE(uint16, uint16_t)
PSHMEM_DECL_TEAM_OR_REDUCE(uint32, uint32_t)
PSHMEM_DECL_TEAM_OR_REDUCE(uint64, uint64_t)
PSHMEM_DECL_TEAM_OR_REDUCE(size, size_t)

#undef PSHMEM_DECL_TEAM_OR_REDUCE

/**
 * @brief Macro to declare typed team bitwise XOR reduction operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_TEAM_XOR_REDUCE(_name, _type)                              \
  int pshmem_##_name##_xor_reduce(shmem_team_t team, _type *dest,              \
                                  const _type *source, size_t nreduce);

PSHMEM_DECL_TEAM_XOR_REDUCE(uchar, unsigned char)
PSHMEM_DECL_TEAM_XOR_REDUCE(ushort, unsigned short)
PSHMEM_DECL_TEAM_XOR_REDUCE(uint, unsigned int)
PSHMEM_DECL_TEAM_XOR_REDUCE(ulong, unsigned long)
PSHMEM_DECL_TEAM_XOR_REDUCE(ulonglong, unsigned long long)
PSHMEM_DECL_TEAM_XOR_REDUCE(int8, int8_t)
PSHMEM_DECL_TEAM_XOR_REDUCE(int16, int16_t)
PSHMEM_DECL_TEAM_XOR_REDUCE(int32, int32_t)
PSHMEM_DECL_TEAM_XOR_REDUCE(int64, int64_t)
PSHMEM_DECL_TEAM_XOR_REDUCE(uint8, uint8_t)
PSHMEM_DECL_TEAM_XOR_REDUCE(uint16, uint16_t)
PSHMEM_DECL_TEAM_XOR_REDUCE(uint32, uint32_t)
PSHMEM_DECL_TEAM_XOR_REDUCE(uint64, uint64_t)
PSHMEM_DECL_TEAM_XOR_REDUCE(size, size_t)

#undef PSHMEM_DECL_TEAM_XOR_REDUCE

/*
 * Context operations
 */

/**
 * @brief Create a communication context
 * @param options Context creation options (bitwise OR of context attributes)
 * @param ctxp Handle for new context
 * @return 0 on success, nonzero on failure
 */
int pshmem_ctx_create(long options, shmem_ctx_t *ctxp) _WUR;

/**
 * @brief Destroy a communication context
 * @param ctx Context to destroy
 */
void pshmem_ctx_destroy(shmem_ctx_t ctx);

/**
 * @brief Start a communication session on a context
 * @param ctx Context handle
 * @param options Session hint options
 * @param config Pointer to session configuration parameters
 * @param config_mask Bitwise mask of configuration parameters
 */
void pshmem_ctx_session_start(shmem_ctx_t ctx, long options,
                              const shmem_ctx_session_config_t *config,
                              long config_mask);

/**
 * @brief Stop a communication session on a context
 * @param ctx Context handle
 */
void pshmem_ctx_session_stop(shmem_ctx_t ctx);

/*
 * Signal operations
 */

/**
 * @brief Fetch the value of a signal object
 * @param sig_addr Address of the remotely accessible signal object
 * @return The signal value
 */
uint64_t pshmem_signal_fetch(const uint64_t *sig_addr) _WUR;

/**
 * @brief Atomically set a signal value on default context
 */
void pshmem_signal_set(uint64_t *sig_addr, uint64_t signal, int pe);

/**
 * @brief Atomically set a signal value on a context
 */
void pshmem_ctx_signal_set(shmem_ctx_t ctx, uint64_t *sig_addr, uint64_t signal,
                           int pe);

/**
 * @brief Atomically add to a signal value on default context
 */
void pshmem_signal_add(uint64_t *sig_addr, uint64_t signal, int pe);

/**
 * @brief Atomically add to a signal value on a context
 */
void pshmem_ctx_signal_add(shmem_ctx_t ctx, uint64_t *sig_addr, uint64_t signal,
                           int pe);

/**
 * @brief Wait for signal object to change value
 * @param sig_addr Address of the remotely accessible signal object
 * @param cmp Comparison operator (SHMEM_CMP_EQ, etc.)
 * @param cmp_value Value to compare against
 * @return The signal value when condition is met
 */
uint64_t pshmem_signal_wait_until(uint64_t *sig_addr, int cmp,
                                  uint64_t cmp_value) _WUR;

/*
 * Put with signal operations
 */

/**
 * @brief Macro to declare typed put with signal operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_PUT_SIGNAL(_name, _type)                                   \
  void pshmem_##_name##_put_signal(_type *dest, const _type *src,              \
                                   size_t nelems, uint64_t *sig_addr,          \
                                   uint64_t signal, int sig_op, int pe);

PSHMEM_DECL_PUT_SIGNAL(float, float)
PSHMEM_DECL_PUT_SIGNAL(double, double)
PSHMEM_DECL_PUT_SIGNAL(longdouble, long double)
PSHMEM_DECL_PUT_SIGNAL(char, char)
PSHMEM_DECL_PUT_SIGNAL(schar, signed char)
PSHMEM_DECL_PUT_SIGNAL(short, short)
PSHMEM_DECL_PUT_SIGNAL(int, int)
PSHMEM_DECL_PUT_SIGNAL(long, long)
PSHMEM_DECL_PUT_SIGNAL(longlong, long long)
PSHMEM_DECL_PUT_SIGNAL(uchar, unsigned char)
PSHMEM_DECL_PUT_SIGNAL(ushort, unsigned short)
PSHMEM_DECL_PUT_SIGNAL(uint, unsigned int)
PSHMEM_DECL_PUT_SIGNAL(ulong, unsigned long)
PSHMEM_DECL_PUT_SIGNAL(ulonglong, unsigned long long)
PSHMEM_DECL_PUT_SIGNAL(int8, int8_t)
PSHMEM_DECL_PUT_SIGNAL(int16, int16_t)
PSHMEM_DECL_PUT_SIGNAL(int32, int32_t)
PSHMEM_DECL_PUT_SIGNAL(int64, int64_t)
PSHMEM_DECL_PUT_SIGNAL(uint8, uint8_t)
PSHMEM_DECL_PUT_SIGNAL(uint16, uint16_t)
PSHMEM_DECL_PUT_SIGNAL(uint32, uint32_t)
PSHMEM_DECL_PUT_SIGNAL(uint64, uint64_t)
PSHMEM_DECL_PUT_SIGNAL(size, size_t)
PSHMEM_DECL_PUT_SIGNAL(ptrdiff, ptrdiff_t)

#undef PSHMEM_DECL_PUT_SIGNAL

/**
 * @brief Macro to declare typed non-blocking put with signal operations
 * @param _name Type name
 * @param _type C type
 */
#define PSHMEM_DECL_PUT_SIGNAL_NBI(_name, _type)                               \
  void pshmem_##_name##_put_signal_nbi(_type *dest, const _type *src,          \
                                       size_t nelems, uint64_t *sig_addr,      \
                                       uint64_t signal, int sig_op, int pe);

PSHMEM_DECL_PUT_SIGNAL_NBI(float, float)
PSHMEM_DECL_PUT_SIGNAL_NBI(double, double)
PSHMEM_DECL_PUT_SIGNAL_NBI(longdouble, long double)
PSHMEM_DECL_PUT_SIGNAL_NBI(char, char)
PSHMEM_DECL_PUT_SIGNAL_NBI(schar, signed char)
PSHMEM_DECL_PUT_SIGNAL_NBI(short, short)
PSHMEM_DECL_PUT_SIGNAL_NBI(int, int)
PSHMEM_DECL_PUT_SIGNAL_NBI(long, long)
PSHMEM_DECL_PUT_SIGNAL_NBI(longlong, long long)
PSHMEM_DECL_PUT_SIGNAL_NBI(uchar, unsigned char)
PSHMEM_DECL_PUT_SIGNAL_NBI(ushort, unsigned short)
PSHMEM_DECL_PUT_SIGNAL_NBI(uint, unsigned int)
PSHMEM_DECL_PUT_SIGNAL_NBI(ulong, unsigned long)
PSHMEM_DECL_PUT_SIGNAL_NBI(ulonglong, unsigned long long)
PSHMEM_DECL_PUT_SIGNAL_NBI(int8, int8_t)
PSHMEM_DECL_PUT_SIGNAL_NBI(int16, int16_t)
PSHMEM_DECL_PUT_SIGNAL_NBI(int32, int32_t)
PSHMEM_DECL_PUT_SIGNAL_NBI(int64, int64_t)
PSHMEM_DECL_PUT_SIGNAL_NBI(uint8, uint8_t)
PSHMEM_DECL_PUT_SIGNAL_NBI(uint16, uint16_t)
PSHMEM_DECL_PUT_SIGNAL_NBI(uint32, uint32_t)
PSHMEM_DECL_PUT_SIGNAL_NBI(uint64, uint64_t)
PSHMEM_DECL_PUT_SIGNAL_NBI(size, size_t)
PSHMEM_DECL_PUT_SIGNAL_NBI(ptrdiff, ptrdiff_t)

#undef PSHMEM_DECL_PUT_SIGNAL_NBI

/**
 * @brief Macro to declare sized put with signal operations
 * @param _size Size in bits
 */
#define PSHMEM_DECL_PUT_SIGNAL_SIZE(_size)                                     \
  void pshmem_put##_size##_signal(void *dest, const void *src, size_t nelems,  \
                                  uint64_t *sig_addr, uint64_t signal,         \
                                  int sig_op, int pe);                         \
  void pshmem_put##_size##_signal_nbi(void *dest, const void *src,             \
                                      size_t nelems, uint64_t *sig_addr,       \
                                      uint64_t signal, int sig_op, int pe);

PSHMEM_DECL_PUT_SIGNAL_SIZE(8)
PSHMEM_DECL_PUT_SIGNAL_SIZE(16)
PSHMEM_DECL_PUT_SIGNAL_SIZE(32)
PSHMEM_DECL_PUT_SIGNAL_SIZE(64)
PSHMEM_DECL_PUT_SIGNAL_SIZE(128)

#undef PSHMEM_DECL_PUT_SIGNAL_SIZE

/**
 * @brief Memory put with signal operation
 * @param dest Destination address on remote PE
 * @param src Source address
 * @param nelems Number of bytes to transfer
 * @param sig_addr Address of signal object on remote PE
 * @param signal Signal value to write
 * @param sig_op Signal operation (SHMEM_SIGNAL_SET or SHMEM_SIGNAL_ADD)
 * @param pe Remote PE number
 */
void pshmem_putmem_signal(void *dest, const void *src, size_t nelems,
                          uint64_t *sig_addr, uint64_t signal, int sig_op,
                          int pe);

/**
 * @brief Non-blocking memory put with signal operation
 * @param dest Destination address on remote PE
 * @param src Source address
 * @param nelems Number of bytes to transfer
 * @param sig_addr Address of signal object on remote PE
 * @param signal Signal value to write
 * @param sig_op Signal operation (SHMEM_SIGNAL_SET or SHMEM_SIGNAL_ADD)
 * @param pe Remote PE number
 */
void pshmem_putmem_signal_nbi(void *dest, const void *src, size_t nelems,
                              uint64_t *sig_addr, uint64_t signal, int sig_op,
                              int pe);

/*
 * locks/critical section
 */

void pshmem_set_lock(long *lock);
void pshmem_clear_lock(long *lock);
int pshmem_test_lock(long *lock) _WUR;

/*
 * atomic fetch and set
 */

int pshmem_int_fetch(const int *target, int pe);
long pshmem_long_fetch(const long *target, int pe);
long long pshmem_longlong_fetch(const long long *target, int pe);
float pshmem_float_fetch(const float *target, int pe);
double pshmem_double_fetch(const double *target, int pe);

void pshmem_int_set(int *target, int value, int pe);
void pshmem_long_set(long *target, long value, int pe);
void pshmem_longlong_set(long long *target, long long value, int pe);
void pshmem_float_set(float *target, float value, int pe);
void pshmem_double_set(double *target, double value, int pe);

#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 201112L

/*
 * C11 Generic variants
 *
 */

/**
 * @brief Generic put operation
 * @param dest Destination address on remote PE
 * @param source Source address
 * @param nelems Number of elements
 * @param pe Remote PE number
 */
#define pshmem_put(dest, source, nelems, pe)                                   \
  _Generic(*(dest),                                                            \
      float: pshmem_float_put,                                                 \
      double: pshmem_double_put,                                               \
      long double: pshmem_longdouble_put,                                      \
      char: pshmem_char_put,                                                   \
      short: pshmem_short_put,                                                 \
      int: pshmem_int_put,                                                     \
      long: pshmem_long_put,                                                   \
      long long: pshmem_longlong_put)(dest, source, nelems, pe)

/**
 * @brief Generic get operation
 * @param dest Destination address
 * @param source Source address on remote PE
 * @param nelems Number of elements
 * @param pe Remote PE number
 */
#define pshmem_get(dest, source, nelems, pe)                                   \
  _Generic(*(dest),                                                            \
      float: pshmem_float_get,                                                 \
      double: pshmem_double_get,                                               \
      long double: pshmem_longdouble_get,                                      \
      char: pshmem_char_get,                                                   \
      short: pshmem_short_get,                                                 \
      int: pshmem_int_get,                                                     \
      long: pshmem_long_get,                                                   \
      long long: pshmem_longlong_get)(dest, source, nelems, pe)

/**
 * @brief Generic put single value operation
 * @param dest Destination address on remote PE
 * @param value Value to put
 * @param pe Remote PE number
 */
#define pshmem_p(dest, value, pe)                                              \
  _Generic(*(dest),                                                            \
      float: pshmem_float_p,                                                   \
      double: pshmem_double_p,                                                 \
      long double: pshmem_longdouble_p,                                        \
      char: pshmem_char_p,                                                     \
      short: pshmem_short_p,                                                   \
      int: pshmem_int_p,                                                       \
      long: pshmem_long_p,                                                     \
      long long: pshmem_longlong_p)(dest, value, pe)

/**
 * @brief Generic get single value operation
 * @param addr Source address on remote PE
 * @param pe Remote PE number
 * @return Value from remote PE
 */
#define pshmem_g(addr, pe)                                                     \
  _Generic((addr),                                                             \
      float: pshmem_float_g,                                                   \
      double: pshmem_double_g,                                                 \
      long double: pshmem_longdouble_g,                                        \
      char: pshmem_char_g,                                                     \
      short: pshmem_short_g,                                                   \
      int: pshmem_int_g,                                                       \
      long: pshmem_long_g,                                                     \
      long long: pshmem_longlong_g)(addr, pe)

/**
 * @brief Generic strided put operation
 * @param dest Destination address on remote PE
 * @param source Source address
 * @param dst Destination stride
 * @param sst Source stride
 * @param nelems Number of elements
 * @param pe Remote PE number
 */
#define pshmem_iput(dest, source, dst, sst, nelems, pe)                        \
  _Generic(*(dest),                                                            \
      float: pshmem_float_iput,                                                \
      double: pshmem_double_iput,                                              \
      long double: pshmem_longdouble_iput,                                     \
      char: pshmem_char_iput,                                                  \
      short: pshmem_short_iput,                                                \
      int: pshmem_int_iput,                                                    \
      long: pshmem_long_iput,                                                  \
      long long: pshmem_longlong_iput)(dest, source, dst, sst, nelems, pe)

/**
 * @brief Generic strided get operation
 * @param dest Destination address
 * @param source Source address on remote PE
 * @param dst Destination stride
 * @param sst Source stride
 * @param nelems Number of elements
 * @param pe Remote PE number
 */
#define pshmem_iget(dest, source, dst, sst, nelems, pe)                        \
  _Generic(*(dest),                                                            \
      float: pshmem_float_iget,                                                \
      double: pshmem_double_iget,                                              \
      long double: pshmem_longdouble_iget,                                     \
      char: pshmem_char_iget,                                                  \
      short: pshmem_short_iget,                                                \
      int: pshmem_int_iget,                                                    \
      long: pshmem_long_iget,                                                  \
      long long: pshmem_longlong_iget)(dest, source, dst, sst, nelems, pe)

/**
 * @brief Generic atomic swap operation
 * @param dest Destination address on remote PE
 * @param value Value to swap
 * @param pe Remote PE number
 * @return Previous value at remote PE
 */
#define pshmem_swap(dest, value, pe)                                           \
  _Generic(*(dest),                                                            \
      int: pshmem_int_swap,                                                    \
      long: pshmem_long_swap,                                                  \
      long long: pshmem_longlong_swap,                                         \
      float: pshmem_float_swap,                                                \
      double: pshmem_double_swap)(dest, value, pe)

/**
 * @brief Generic atomic compare and swap operation
 * @param dest Destination address on remote PE
 * @param cond Comparison value
 * @param value Value to swap if comparison matches
 * @param pe Remote PE number
 * @return Previous value at remote PE
 */
#define pshmem_cswap(dest, cond, value, pe)                                    \
  _Generic(*(dest),                                                            \
      int: pshmem_int_cswap,                                                   \
      long: pshmem_long_cswap,                                                 \
      long long: pshmem_longlong_cswap)(dest, cond, value, pe)

/**
 * @brief Generic atomic fetch and add operation
 * @param dest Destination address on remote PE
 * @param value Value to add
 * @param pe Remote PE number
 * @return Previous value at remote PE
 */
#define pshmem_fadd(dest, value, pe)                                           \
  _Generic(*(dest),                                                            \
      int: pshmem_int_fadd,                                                    \
      long: pshmem_long_fadd,                                                  \
      long long: pshmem_longlong_fadd)(dest, value, pe)

/**
 * @brief Generic atomic fetch and increment operation
 * @param dest Destination address on remote PE
 * @param pe Remote PE number
 * @return Previous value at remote PE
 */
#define pshmem_finc(dest, pe)                                                  \
  _Generic(*(dest),                                                            \
      int: pshmem_int_finc,                                                    \
      long: pshmem_long_finc,                                                  \
      long long: pshmem_longlong_finc)(dest, pe)

/**
 * @brief Generic atomic add operation
 * @param dest Destination address on remote PE
 * @param value Value to add
 * @param pe Remote PE number
 */
#define pshmem_add(dest, value, pe)                                            \
  _Generic(*(dest),                                                            \
      int: pshmem_int_add,                                                     \
      long: pshmem_long_add,                                                   \
      long long: pshmem_longlong_add)(dest, value, pe)

/**
 * @brief Generic atomic increment operation
 * @param dest Destination address on remote PE
 * @param pe Remote PE number
 */
#define pshmem_inc(dest, pe)                                                   \
  _Generic(*(dest),                                                            \
      int: pshmem_int_inc,                                                     \
      long: pshmem_long_inc,                                                   \
      long long: pshmem_longlong_inc)(dest, pe)

/**
 * @brief Generic atomic fetch operation
 * @param dest Source address on remote PE
 * @param pe Remote PE number
 * @return Value from remote PE
 */
#define pshmem_fetch(dest, pe)                                                 \
  _Generic(*(dest),                                                            \
      int: pshmem_int_fetch,                                                   \
      const int: pshmem_int_fetch,                                             \
      long: pshmem_long_fetch,                                                 \
      const long: pshmem_long_fetch,                                           \
      long long: pshmem_longlong_fetch,                                        \
      const long long: pshmem_longlong_fetch,                                  \
      float: pshmem_float_fetch,                                               \
      const float: pshmem_float_fetch,                                         \
      double: pshmem_double_fetch,                                             \
      const double: pshmem_double_fetch)(dest, pe)

/**
 * @brief Generic atomic set operation
 * @param dest Destination address on remote PE
 * @param value Value to set
 * @param pe Remote PE number
 */
#define pshmem_set(dest, value, pe)                                            \
  _Generic(*(dest),                                                            \
      int: pshmem_int_set,                                                     \
      long: pshmem_long_set,                                                   \
      long long: pshmem_longlong_set,                                          \
      float: pshmem_float_set,                                                 \
      double: pshmem_double_set)(dest, value, pe)

/**
 * @brief Generic wait operation
 * @param ivar Local address to wait on
 * @param cmp_value Value to compare against
 */
#define pshmem_wait(ivar, cmp_value)                                           \
  _Generic(*(ivar),                                                            \
      short: pshmem_short_wait,                                                \
      int: pshmem_int_wait,                                                    \
      long: pshmem_long_wait,                                                  \
      long long: pshmem_longlong_wait)(ivar, cmp_value)

/**
 * @brief Generic wait until operation
 * @param ivar Local address to wait on
 * @param cmp Comparison operation
 * @param cmp_value Value to compare against
 */
#define pshmem_wait_until(ivar, cmp, cmp_value)                                \
  _Generic(*(ivar),                                                            \
      short: pshmem_short_wait_until,                                          \
      int: pshmem_int_wait_until,                                              \
      long: pshmem_long_wait_until,                                            \
      long long: pshmem_longlong_wait_until)(ivar, cmp, cmp_value)

/**
 * @brief Generic team broadcast operation
 */
#define pshmem_broadcast(team, dest, source, nelems, PE_root)                  \
  _Generic(*(dest),                                                            \
      float: pshmem_float_broadcast,                                           \
      double: pshmem_double_broadcast,                                         \
      long double: pshmem_longdouble_broadcast,                                \
      char: pshmem_char_broadcast,                                             \
      signed char: pshmem_schar_broadcast,                                     \
      short: pshmem_short_broadcast,                                           \
      int: pshmem_int_broadcast,                                               \
      long: pshmem_long_broadcast,                                             \
      long long: pshmem_longlong_broadcast,                                    \
      unsigned char: pshmem_uchar_broadcast,                                   \
      unsigned short: pshmem_ushort_broadcast,                                 \
      unsigned int: pshmem_uint_broadcast,                                     \
      unsigned long: pshmem_ulong_broadcast,                                   \
      unsigned long long: pshmem_ulonglong_broadcast)(team, dest, source,      \
                                                      nelems, PE_root)

/**
 * @brief Generic team collect operation
 */
#define pshmem_collect(team, dest, source, nelems)                             \
  _Generic(*(dest),                                                            \
      float: pshmem_float_collect,                                             \
      double: pshmem_double_collect,                                           \
      long double: pshmem_longdouble_collect,                                  \
      char: pshmem_char_collect,                                               \
      signed char: pshmem_schar_collect,                                       \
      short: pshmem_short_collect,                                             \
      int: pshmem_int_collect,                                                 \
      long: pshmem_long_collect,                                               \
      long long: pshmem_longlong_collect)(team, dest, source, nelems)

/**
 * @brief Generic team fcollect operation
 */
#define pshmem_fcollect(team, dest, source, nelems)                            \
  _Generic(*(dest),                                                            \
      float: pshmem_float_fcollect,                                            \
      double: pshmem_double_fcollect,                                          \
      long double: pshmem_longdouble_fcollect,                                 \
      char: pshmem_char_fcollect,                                              \
      signed char: pshmem_schar_fcollect,                                      \
      short: pshmem_short_fcollect,                                            \
      int: pshmem_int_fcollect,                                                \
      long: pshmem_long_fcollect,                                              \
      long long: pshmem_longlong_fcollect)(team, dest, source, nelems)

/**
 * @brief Generic team alltoall operation
 */
#define pshmem_alltoall(team, dest, source, nelems)                            \
  _Generic(*(dest),                                                            \
      float: pshmem_float_alltoall,                                            \
      double: pshmem_double_alltoall,                                          \
      long double: pshmem_longdouble_alltoall,                                 \
      char: pshmem_char_alltoall,                                              \
      signed char: pshmem_schar_alltoall,                                      \
      short: pshmem_short_alltoall,                                            \
      int: pshmem_int_alltoall,                                                \
      long: pshmem_long_alltoall,                                              \
      long long: pshmem_longlong_alltoall)(team, dest, source, nelems)

/**
 * @brief Generic team sum reduction operation
 */
#define pshmem_sum_reduce(team, dest, source, nreduce)                         \
  _Generic(*(dest),                                                            \
      char: pshmem_char_sum_reduce,                                            \
      signed char: pshmem_schar_sum_reduce,                                    \
      short: pshmem_short_sum_reduce,                                          \
      int: pshmem_int_sum_reduce,                                              \
      long: pshmem_long_sum_reduce,                                            \
      long long: pshmem_longlong_sum_reduce,                                   \
      float: pshmem_float_sum_reduce,                                          \
      double: pshmem_double_sum_reduce,                                        \
      long double: pshmem_longdouble_sum_reduce)(team, dest, source, nreduce)

/**
 * @brief Generic team maximum reduction operation
 */
#define pshmem_max_reduce(team, dest, source, nreduce)                         \
  _Generic(*(dest),                                                            \
      char: pshmem_char_max_reduce,                                            \
      signed char: pshmem_schar_max_reduce,                                    \
      short: pshmem_short_max_reduce,                                          \
      int: pshmem_int_max_reduce,                                              \
      long: pshmem_long_max_reduce,                                            \
      long long: pshmem_longlong_max_reduce,                                   \
      float: pshmem_float_max_reduce,                                          \
      double: pshmem_double_max_reduce,                                        \
      long double: pshmem_longdouble_max_reduce)(team, dest, source, nreduce)

/**
 * @brief Generic team minimum reduction operation
 */
#define pshmem_min_reduce(team, dest, source, nreduce)                         \
  _Generic(*(dest),                                                            \
      char: pshmem_char_min_reduce,                                            \
      signed char: pshmem_schar_min_reduce,                                    \
      short: pshmem_short_min_reduce,                                          \
      int: pshmem_int_min_reduce,                                              \
      long: pshmem_long_min_reduce,                                            \
      long long: pshmem_longlong_min_reduce,                                   \
      float: pshmem_float_min_reduce,                                          \
      double: pshmem_double_min_reduce,                                        \
      long double: pshmem_longdouble_min_reduce)(team, dest, source, nreduce)

/**
 * @brief Generic team product reduction operation
 */
#define pshmem_prod_reduce(team, dest, source, nreduce)                        \
  _Generic(*(dest),                                                            \
      char: pshmem_char_prod_reduce,                                           \
      signed char: pshmem_schar_prod_reduce,                                   \
      short: pshmem_short_prod_reduce,                                         \
      int: pshmem_int_prod_reduce,                                             \
      long: pshmem_long_prod_reduce,                                           \
      long long: pshmem_longlong_prod_reduce,                                  \
      float: pshmem_float_prod_reduce,                                         \
      double: pshmem_double_prod_reduce,                                       \
      long double: pshmem_longdouble_prod_reduce)(team, dest, source, nreduce)

/**
 * @brief Generic team alltoalls operation
 */
#define pshmem_alltoalls(team, dest, source, dst, sst, nelems)                 \
  _Generic(*(dest),                                                            \
      float: pshmem_float_alltoalls,                                           \
      double: pshmem_double_alltoalls,                                         \
      long double: pshmem_longdouble_alltoalls,                                \
      char: pshmem_char_alltoalls,                                             \
      signed char: pshmem_schar_alltoalls,                                     \
      short: pshmem_short_alltoalls,                                           \
      int: pshmem_int_alltoalls,                                               \
      long: pshmem_long_alltoalls,                                             \
      long long: pshmem_longlong_alltoalls)(team, dest, source, dst, sst,      \
                                            nelems)

/**
 * @brief Generic team bitwise AND reduction operation
 */
#define pshmem_and_reduce(team, dest, source, nreduce)                         \
  _Generic(*(dest),                                                            \
      unsigned char: pshmem_uchar_and_reduce,                                  \
      unsigned short: pshmem_ushort_and_reduce,                                \
      unsigned int: pshmem_uint_and_reduce,                                    \
      unsigned long: pshmem_ulong_and_reduce,                                  \
      unsigned long long: pshmem_ulonglong_and_reduce)(team, dest, source,     \
                                                       nreduce)

/**
 * @brief Generic team bitwise OR reduction operation
 */
#define pshmem_or_reduce(team, dest, source, nreduce)                          \
  _Generic(*(dest),                                                            \
      unsigned char: pshmem_uchar_or_reduce,                                   \
      unsigned short: pshmem_ushort_or_reduce,                                 \
      unsigned int: pshmem_uint_or_reduce,                                     \
      unsigned long: pshmem_ulong_or_reduce,                                   \
      unsigned long long: pshmem_ulonglong_or_reduce)(team, dest, source,      \
                                                      nreduce)

/**
 * @brief Generic team bitwise XOR reduction operation
 */
#define pshmem_xor_reduce(team, dest, source, nreduce)                         \
  _Generic(*(dest),                                                            \
      unsigned char: pshmem_uchar_xor_reduce,                                  \
      unsigned short: pshmem_ushort_xor_reduce,                                \
      unsigned int: pshmem_uint_xor_reduce,                                    \
      unsigned long: pshmem_ulong_xor_reduce,                                  \
      unsigned long long: pshmem_ulonglong_xor_reduce)(team, dest, source,     \
                                                       nreduce)

/** @} */

#endif /* __STDC_VERSION__ >= 201112L test */
#endif /* __STDC_VERSION__ defined test */

/*
 * Context-aware operations
 */

void pshmem_ctx_fence(shmem_ctx_t ctx);
void pshmem_ctx_quiet(shmem_ctx_t ctx);

/**
 * @brief Completes communication to the listed PEs on a context.
 * @param ctx Communication context.
 * @param target_pes Array of team-relative PE numbers, ignored when npes is
 * zero.
 * @param npes Number of target PEs.
 */
void pshmem_ctx_pe_quiet(shmem_ctx_t ctx, const int *target_pes, size_t npes);
void pshmem_ctx_putmem(shmem_ctx_t ctx, void *dest, const void *src,
                       size_t nelems, int pe);
void pshmem_ctx_getmem(shmem_ctx_t ctx, void *dest, const void *src,
                       size_t nelems, int pe);
void pshmem_ctx_putmem_nbi(shmem_ctx_t ctx, void *dest, const void *src,
                           size_t nelems, int pe);
void pshmem_ctx_getmem_nbi(shmem_ctx_t ctx, void *dest, const void *src,
                           size_t nelems, int pe);

/* Context-aware typed put/get operations */

/* Context-aware put operations */
void pshmem_ctx_float_put(shmem_ctx_t ctx, float *dest, const float *src,
                          size_t nelems, int pe);
void pshmem_ctx_double_put(shmem_ctx_t ctx, double *dest, const double *src,
                           size_t nelems, int pe);
void pshmem_ctx_longdouble_put(shmem_ctx_t ctx, long double *dest,
                               const long double *src, size_t nelems, int pe);
void pshmem_ctx_char_put(shmem_ctx_t ctx, char *dest, const char *src,
                         size_t nelems, int pe);
void pshmem_ctx_schar_put(shmem_ctx_t ctx, signed char *dest,
                          const signed char *src, size_t nelems, int pe);
void pshmem_ctx_short_put(shmem_ctx_t ctx, short *dest, const short *src,
                          size_t nelems, int pe);
void pshmem_ctx_int_put(shmem_ctx_t ctx, int *dest, const int *src,
                        size_t nelems, int pe);
void pshmem_ctx_long_put(shmem_ctx_t ctx, long *dest, const long *src,
                         size_t nelems, int pe);
void pshmem_ctx_longlong_put(shmem_ctx_t ctx, long long *dest,
                             const long long *src, size_t nelems, int pe);
void pshmem_ctx_uchar_put(shmem_ctx_t ctx, unsigned char *dest,
                          const unsigned char *src, size_t nelems, int pe);
void pshmem_ctx_ushort_put(shmem_ctx_t ctx, unsigned short *dest,
                           const unsigned short *src, size_t nelems, int pe);
void pshmem_ctx_uint_put(shmem_ctx_t ctx, unsigned int *dest,
                         const unsigned int *src, size_t nelems, int pe);
void pshmem_ctx_ulong_put(shmem_ctx_t ctx, unsigned long *dest,
                          const unsigned long *src, size_t nelems, int pe);
void pshmem_ctx_ulonglong_put(shmem_ctx_t ctx, unsigned long long *dest,
                              const unsigned long long *src, size_t nelems,
                              int pe);
void pshmem_ctx_int8_put(shmem_ctx_t ctx, int8_t *dest, const int8_t *src,
                         size_t nelems, int pe);
void pshmem_ctx_int16_put(shmem_ctx_t ctx, int16_t *dest, const int16_t *src,
                          size_t nelems, int pe);
void pshmem_ctx_int32_put(shmem_ctx_t ctx, int32_t *dest, const int32_t *src,
                          size_t nelems, int pe);
void pshmem_ctx_int64_put(shmem_ctx_t ctx, int64_t *dest, const int64_t *src,
                          size_t nelems, int pe);
void pshmem_ctx_uint8_put(shmem_ctx_t ctx, uint8_t *dest, const uint8_t *src,
                          size_t nelems, int pe);
void pshmem_ctx_uint16_put(shmem_ctx_t ctx, uint16_t *dest, const uint16_t *src,
                           size_t nelems, int pe);
void pshmem_ctx_uint32_put(shmem_ctx_t ctx, uint32_t *dest, const uint32_t *src,
                           size_t nelems, int pe);
void pshmem_ctx_uint64_put(shmem_ctx_t ctx, uint64_t *dest, const uint64_t *src,
                           size_t nelems, int pe);
void pshmem_ctx_size_put(shmem_ctx_t ctx, size_t *dest, const size_t *src,
                         size_t nelems, int pe);
void pshmem_ctx_ptrdiff_put(shmem_ctx_t ctx, ptrdiff_t *dest,
                            const ptrdiff_t *src, size_t nelems, int pe);

/* Context-aware get operations */
void pshmem_ctx_float_get(shmem_ctx_t ctx, float *dest, const float *src,
                          size_t nelems, int pe);
void pshmem_ctx_double_get(shmem_ctx_t ctx, double *dest, const double *src,
                           size_t nelems, int pe);
void pshmem_ctx_longdouble_get(shmem_ctx_t ctx, long double *dest,
                               const long double *src, size_t nelems, int pe);
void pshmem_ctx_char_get(shmem_ctx_t ctx, char *dest, const char *src,
                         size_t nelems, int pe);
void pshmem_ctx_schar_get(shmem_ctx_t ctx, signed char *dest,
                          const signed char *src, size_t nelems, int pe);
void pshmem_ctx_short_get(shmem_ctx_t ctx, short *dest, const short *src,
                          size_t nelems, int pe);
void pshmem_ctx_int_get(shmem_ctx_t ctx, int *dest, const int *src,
                        size_t nelems, int pe);
void pshmem_ctx_long_get(shmem_ctx_t ctx, long *dest, const long *src,
                         size_t nelems, int pe);
void pshmem_ctx_longlong_get(shmem_ctx_t ctx, long long *dest,
                             const long long *src, size_t nelems, int pe);
void pshmem_ctx_uchar_get(shmem_ctx_t ctx, unsigned char *dest,
                          const unsigned char *src, size_t nelems, int pe);
void pshmem_ctx_ushort_get(shmem_ctx_t ctx, unsigned short *dest,
                           const unsigned short *src, size_t nelems, int pe);
void pshmem_ctx_uint_get(shmem_ctx_t ctx, unsigned int *dest,
                         const unsigned int *src, size_t nelems, int pe);
void pshmem_ctx_ulong_get(shmem_ctx_t ctx, unsigned long *dest,
                          const unsigned long *src, size_t nelems, int pe);
void pshmem_ctx_ulonglong_get(shmem_ctx_t ctx, unsigned long long *dest,
                              const unsigned long long *src, size_t nelems,
                              int pe);
void pshmem_ctx_int8_get(shmem_ctx_t ctx, int8_t *dest, const int8_t *src,
                         size_t nelems, int pe);
void pshmem_ctx_int16_get(shmem_ctx_t ctx, int16_t *dest, const int16_t *src,
                          size_t nelems, int pe);
void pshmem_ctx_int32_get(shmem_ctx_t ctx, int32_t *dest, const int32_t *src,
                          size_t nelems, int pe);
void pshmem_ctx_int64_get(shmem_ctx_t ctx, int64_t *dest, const int64_t *src,
                          size_t nelems, int pe);
void pshmem_ctx_uint8_get(shmem_ctx_t ctx, uint8_t *dest, const uint8_t *src,
                          size_t nelems, int pe);
void pshmem_ctx_uint16_get(shmem_ctx_t ctx, uint16_t *dest, const uint16_t *src,
                           size_t nelems, int pe);
void pshmem_ctx_uint32_get(shmem_ctx_t ctx, uint32_t *dest, const uint32_t *src,
                           size_t nelems, int pe);
void pshmem_ctx_uint64_get(shmem_ctx_t ctx, uint64_t *dest, const uint64_t *src,
                           size_t nelems, int pe);
void pshmem_ctx_size_get(shmem_ctx_t ctx, size_t *dest, const size_t *src,
                         size_t nelems, int pe);
void pshmem_ctx_ptrdiff_get(shmem_ctx_t ctx, ptrdiff_t *dest,
                            const ptrdiff_t *src, size_t nelems, int pe);

/* Context-aware single-element operations */
void pshmem_ctx_float_p(shmem_ctx_t ctx, float *dest, float src, int pe);
void pshmem_ctx_double_p(shmem_ctx_t ctx, double *dest, double src, int pe);
void pshmem_ctx_longdouble_p(shmem_ctx_t ctx, long double *dest,
                             long double src, int pe);
void pshmem_ctx_char_p(shmem_ctx_t ctx, char *dest, char src, int pe);
void pshmem_ctx_schar_p(shmem_ctx_t ctx, signed char *dest, signed char src,
                        int pe);
void pshmem_ctx_short_p(shmem_ctx_t ctx, short *dest, short src, int pe);
void pshmem_ctx_int_p(shmem_ctx_t ctx, int *dest, int src, int pe);
void pshmem_ctx_long_p(shmem_ctx_t ctx, long *dest, long src, int pe);
void pshmem_ctx_longlong_p(shmem_ctx_t ctx, long long *dest, long long src,
                           int pe);

float pshmem_ctx_float_g(shmem_ctx_t ctx, const float *src, int pe);
double pshmem_ctx_double_g(shmem_ctx_t ctx, const double *src, int pe);
long double pshmem_ctx_longdouble_g(shmem_ctx_t ctx, const long double *src,
                                    int pe);
char pshmem_ctx_char_g(shmem_ctx_t ctx, const char *src, int pe);
signed char pshmem_ctx_schar_g(shmem_ctx_t ctx, const signed char *src, int pe);
short pshmem_ctx_short_g(shmem_ctx_t ctx, const short *src, int pe);
int pshmem_ctx_int_g(shmem_ctx_t ctx, const int *src, int pe);
long pshmem_ctx_long_g(shmem_ctx_t ctx, const long *src, int pe);
long long pshmem_ctx_longlong_g(shmem_ctx_t ctx, const long long *src, int pe);

/* Context-aware non-blocking put operations */
void pshmem_ctx_float_put_nbi(shmem_ctx_t ctx, float *dest, const float *src,
                              size_t nelems, int pe);
void pshmem_ctx_double_put_nbi(shmem_ctx_t ctx, double *dest, const double *src,
                               size_t nelems, int pe);
void pshmem_ctx_longdouble_put_nbi(shmem_ctx_t ctx, long double *dest,
                                   const long double *src, size_t nelems,
                                   int pe);
void pshmem_ctx_char_put_nbi(shmem_ctx_t ctx, char *dest, const char *src,
                             size_t nelems, int pe);
void pshmem_ctx_schar_put_nbi(shmem_ctx_t ctx, signed char *dest,
                              const signed char *src, size_t nelems, int pe);
void pshmem_ctx_short_put_nbi(shmem_ctx_t ctx, short *dest, const short *src,
                              size_t nelems, int pe);
void pshmem_ctx_int_put_nbi(shmem_ctx_t ctx, int *dest, const int *src,
                            size_t nelems, int pe);
void pshmem_ctx_long_put_nbi(shmem_ctx_t ctx, long *dest, const long *src,
                             size_t nelems, int pe);
void pshmem_ctx_longlong_put_nbi(shmem_ctx_t ctx, long long *dest,
                                 const long long *src, size_t nelems, int pe);

/* Context-aware non-blocking get operations */
void pshmem_ctx_float_get_nbi(shmem_ctx_t ctx, float *dest, const float *src,
                              size_t nelems, int pe);
void pshmem_ctx_double_get_nbi(shmem_ctx_t ctx, double *dest, const double *src,
                               size_t nelems, int pe);
void pshmem_ctx_longdouble_get_nbi(shmem_ctx_t ctx, long double *dest,
                                   const long double *src, size_t nelems,
                                   int pe);
void pshmem_ctx_char_get_nbi(shmem_ctx_t ctx, char *dest, const char *src,
                             size_t nelems, int pe);
void pshmem_ctx_schar_get_nbi(shmem_ctx_t ctx, signed char *dest,
                              const signed char *src, size_t nelems, int pe);
void pshmem_ctx_short_get_nbi(shmem_ctx_t ctx, short *dest, const short *src,
                              size_t nelems, int pe);
void pshmem_ctx_int_get_nbi(shmem_ctx_t ctx, int *dest, const int *src,
                            size_t nelems, int pe);
void pshmem_ctx_long_get_nbi(shmem_ctx_t ctx, long *dest, const long *src,
                             size_t nelems, int pe);
void pshmem_ctx_longlong_get_nbi(shmem_ctx_t ctx, long long *dest,
                                 const long long *src, size_t nelems, int pe);

/* Context-aware atomic operations */
float pshmem_ctx_float_atomic_swap(shmem_ctx_t ctx, float *target, float value,
                                   int pe);
double pshmem_ctx_double_atomic_swap(shmem_ctx_t ctx, double *target,
                                     double value, int pe);
int pshmem_ctx_int_atomic_swap(shmem_ctx_t ctx, int *target, int value, int pe);
long pshmem_ctx_long_atomic_swap(shmem_ctx_t ctx, long *target, long value,
                                 int pe);
long long pshmem_ctx_longlong_atomic_swap(shmem_ctx_t ctx, long long *target,
                                          long long value, int pe);

int pshmem_ctx_int_atomic_compare_swap(shmem_ctx_t ctx, int *target, int cond,
                                       int value, int pe);
long pshmem_ctx_long_atomic_compare_swap(shmem_ctx_t ctx, long *target,
                                         long cond, long value, int pe);
long long pshmem_ctx_longlong_atomic_compare_swap(shmem_ctx_t ctx,
                                                  long long *target,
                                                  long long cond,
                                                  long long value, int pe);

int pshmem_ctx_int_fadd(shmem_ctx_t ctx, int *target, int value, int pe);
long pshmem_ctx_long_fadd(shmem_ctx_t ctx, long *target, long value, int pe);
long long pshmem_ctx_longlong_fadd(shmem_ctx_t ctx, long long *target,
                                   long long value, int pe);

int pshmem_ctx_int_finc(shmem_ctx_t ctx, int *target, int pe);
long pshmem_ctx_long_finc(shmem_ctx_t ctx, long *target, int pe);
long long pshmem_ctx_longlong_finc(shmem_ctx_t ctx, long long *target, int pe);

void pshmem_ctx_int_add(shmem_ctx_t ctx, int *target, int value, int pe);
void pshmem_ctx_long_add(shmem_ctx_t ctx, long *target, long value, int pe);
void pshmem_ctx_longlong_add(shmem_ctx_t ctx, long long *target,
                             long long value, int pe);

void pshmem_ctx_int_inc(shmem_ctx_t ctx, int *target, int pe);
void pshmem_ctx_long_inc(shmem_ctx_t ctx, long *target, int pe);
void pshmem_ctx_longlong_inc(shmem_ctx_t ctx, long long *target, int pe);

int pshmem_ctx_int_fetch(shmem_ctx_t ctx, const int *target, int pe);
long pshmem_ctx_long_fetch(shmem_ctx_t ctx, const long *target, int pe);
long long pshmem_ctx_longlong_fetch(shmem_ctx_t ctx, const long long *target,
                                    int pe);
float pshmem_ctx_float_fetch(shmem_ctx_t ctx, const float *target, int pe);
double pshmem_ctx_double_fetch(shmem_ctx_t ctx, const double *target, int pe);

void pshmem_ctx_int_set(shmem_ctx_t ctx, int *target, int value, int pe);
void pshmem_ctx_long_set(shmem_ctx_t ctx, long *target, long value, int pe);
void pshmem_ctx_longlong_set(shmem_ctx_t ctx, long long *target,
                             long long value, int pe);
void pshmem_ctx_float_set(shmem_ctx_t ctx, float *target, float value, int pe);
void pshmem_ctx_double_set(shmem_ctx_t ctx, double *target, double value,
                           int pe);

/* Scan operations */
#define PSHMEM_DECL_SCAN_OP(_name, _type)                                      \
  int pshmem_##_name##_sum_inscan(shmem_team_t team, _type *dest,             \
                                  const _type *source, size_t nelems);         \
  int pshmem_##_name##_sum_exscan(shmem_team_t team, _type *dest,             \
                                  const _type *source, size_t nelems);

PSHMEM_DECL_SCAN_OP(char, char)
PSHMEM_DECL_SCAN_OP(schar, signed char)
PSHMEM_DECL_SCAN_OP(short, short)
PSHMEM_DECL_SCAN_OP(int, int)
PSHMEM_DECL_SCAN_OP(long, long)
PSHMEM_DECL_SCAN_OP(longlong, long long)
PSHMEM_DECL_SCAN_OP(ptrdiff, ptrdiff_t)
PSHMEM_DECL_SCAN_OP(uchar, unsigned char)
PSHMEM_DECL_SCAN_OP(ushort, unsigned short)
PSHMEM_DECL_SCAN_OP(uint, unsigned int)
PSHMEM_DECL_SCAN_OP(ulong, unsigned long)
PSHMEM_DECL_SCAN_OP(ulonglong, unsigned long long)
PSHMEM_DECL_SCAN_OP(int8, int8_t)
PSHMEM_DECL_SCAN_OP(int16, int16_t)
PSHMEM_DECL_SCAN_OP(int32, int32_t)
PSHMEM_DECL_SCAN_OP(int64, int64_t)
PSHMEM_DECL_SCAN_OP(uint8, uint8_t)
PSHMEM_DECL_SCAN_OP(uint16, uint16_t)
PSHMEM_DECL_SCAN_OP(uint32, uint32_t)
PSHMEM_DECL_SCAN_OP(uint64, uint64_t)
PSHMEM_DECL_SCAN_OP(size, size_t)
PSHMEM_DECL_SCAN_OP(float, float)
PSHMEM_DECL_SCAN_OP(double, double)
PSHMEM_DECL_SCAN_OP(longdouble, long double)
PSHMEM_DECL_SCAN_OP(complexd, double _Complex)
PSHMEM_DECL_SCAN_OP(complexf, float _Complex)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PSHMEM_H */
