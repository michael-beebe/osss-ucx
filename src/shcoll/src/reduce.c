/*
 * @file reduction.c
 * @brief Implementation of collective reduction operations
 *
 * This file contains implementations of various reduction algorithms including:
 * - Linear reduction
 * - Binomial tree reduction
 * - Recursive doubling reduction
 * - Rabenseifner's algorithm
 *
 * Each algorithm is implemented as a macro that generates type-specific
 * implementations for different reduction operations (AND, OR, XOR, MIN, MAX,
 * SUM, PROD) and data types (int, long, float, double etc).
 */

#include "shcoll.h"
#include <shmem/api_types.h>
#include "util/bithacks.h"
#include "../tests/util/debug.h"

#include "shmem.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

/*
 * @brief Helper macro to define local reduction operations
 *
 * @param _name Name of the reduction operation (e.g. sum, prod)
 * @param _type Data type to operate on
 * @param _op Binary operator to apply
 */
#define REDUCE_HELPER_LOCAL(_name, _type, _op)                                 \
  inline static void local_##_name##_reduce(                                   \
      _type *dest, const _type *src1, const _type *src2, size_t nreduce) {     \
    size_t i;                                                                  \
                                                                               \
    for (i = 0; i < nreduce; i++) {                                            \
      dest[i] = _op(src1[i], src2[i]);                                         \
    }                                                                          \
  }

/*
 * @brief Helper macro to define linear reduction operations
 *
 * Implements a linear reduction algorithm where PE 0 sequentially reduces
 * values from all other PEs.
 *
 * @param _name Name of the reduction operation
 * @param _type Data type to operate on
 * @param _op Binary operator to apply
 */
#define REDUCE_HELPER_LINEAR(_name, _type, _op)                                \
  void reduce_helper_##_name##_linear(                                         \
      _type *dest, const _type *source, int nreduce, int PE_start,             \
      int logPE_stride, int PE_size, _type *pWrk, long *pSync) {               \
    const int stride = 1 << logPE_stride;                                      \
    const int me = shmem_my_pe();                                              \
    const int me_as = (me - PE_start) / stride;                                \
    const size_t nbytes = sizeof(_type) * nreduce;                             \
                                                                               \
    _type *tmp_array;                                                          \
    int i;                                                                     \
                                                                               \
    shcoll_barrier_linear(PE_start, logPE_stride, PE_size, pSync);             \
                                                                               \
    if (me_as == 0) {                                                          \
      tmp_array = malloc(nbytes);                                              \
      if (!tmp_array) {                                                        \
        /* TODO: raise error */                                                \
        exit(-1);                                                              \
      }                                                                        \
                                                                               \
      memcpy(tmp_array, source, nbytes);                                       \
                                                                               \
      for (i = 1; i < PE_size; i++) {                                          \
        shmem_getmem(dest, source, nbytes, PE_start + i * stride);             \
        local_##_name##_reduce(tmp_array, tmp_array, dest, nreduce);           \
      }                                                                        \
                                                                               \
      memcpy(dest, tmp_array, nbytes);                                         \
      free(tmp_array);                                                         \
    }                                                                          \
                                                                               \
    shcoll_barrier_linear(PE_start, logPE_stride, PE_size, pSync);             \
                                                                               \
    shcoll_broadcast8_linear(dest, dest, nreduce * sizeof(_type), PE_start,    \
                             PE_start, logPE_stride, PE_size, pSync + 1);      \
  }

/*
 * @brief Helper macro to define binomial tree reduction operations
 *
 * Implements a binomial tree reduction algorithm for better scalability.
 *
 * @param _name Name of the reduction operation
 * @param _type Data type to operate on
 * @param _op Binary operator to apply
 */
#define REDUCE_HELPER_BINOMIAL(_name, _type, _op)                              \
  void reduce_helper_##_name##_binomial(                                       \
      _type *dest, const _type *source, int nreduce, int PE_start,             \
      int logPE_stride, int PE_size, _type *pWrk, long *pSync) {               \
    const int stride = 1 << logPE_stride;                                      \
    const int me = shmem_my_pe();                                              \
    int me_as = (me - PE_start) / stride;                                      \
    int target_as;                                                             \
    size_t nbytes = sizeof(_type) * nreduce;                                   \
    _type *tmp_array = NULL;                                                   \
    unsigned mask = 0x1;                                                       \
    long old_pSync = SHCOLL_SYNC_VALUE;                                        \
    long to_receive = 0;                                                       \
    long recv_mask;                                                            \
                                                                               \
    tmp_array = malloc(nbytes);                                                \
    if (!tmp_array) {                                                          \
      /* TODO: raise error */                                                  \
      fprintf(stderr, "PE %d: Cannot allocate memory!\n", me);                 \
      exit(-1);                                                                \
    }                                                                          \
                                                                               \
    if (source != dest) {                                                      \
      memcpy(dest, source, nbytes);                                            \
    }                                                                          \
                                                                               \
    /* Stop if all messages are received or if there are no more PE on right   \
     */                                                                        \
    for (mask = 0x1; !(me_as & mask) && ((me_as | mask) < PE_size);            \
         mask <<= 1) {                                                         \
      to_receive |= mask;                                                      \
    }                                                                          \
                                                                               \
    /* TODO: fix if SHCOLL_SYNC_VALUE not 0 */                                 \
    /* Wait until all messages are received */                                 \
    while (to_receive != 0) {                                                  \
      memcpy(tmp_array, dest, nbytes);                                         \
      shmem_long_wait_until(pSync, SHMEM_CMP_NE, old_pSync);                   \
      recv_mask = shmem_long_atomic_fetch(pSync, me);                          \
                                                                               \
      recv_mask &= to_receive;                                                 \
      recv_mask ^= (recv_mask - 1) & recv_mask;                                \
                                                                               \
      /* Get array and reduce */                                               \
      target_as = (int)(me_as | recv_mask);                                    \
      shmem_getmem(dest, dest, nbytes, PE_start + target_as * stride);         \
                                                                               \
      local_##_name##_reduce(dest, dest, tmp_array, nreduce);                  \
                                                                               \
      /* Mark as received */                                                   \
      to_receive &= ~recv_mask;                                                \
      old_pSync |= recv_mask;                                                  \
    }                                                                          \
                                                                               \
    /* Notify parent */                                                        \
    if (me_as != 0) {                                                          \
      target_as = me_as & (me_as - 1);                                         \
      shmem_long_atomic_add(pSync, me_as ^ target_as,                          \
                            PE_start + target_as * stride);                    \
    }                                                                          \
                                                                               \
    shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);                                \
    shcoll_barrier_linear(PE_start, logPE_stride, PE_size, pSync + 1);         \
                                                                               \
    shcoll_broadcast8_binomial_tree(dest, dest, nreduce * sizeof(_type),       \
                                    PE_start, PE_start, logPE_stride, PE_size, \
                                    pSync + 2);                                \
                                                                               \
    free(tmp_array);                                                           \
  }

/*
 * @brief Helper macro to define recursive doubling reduction operations
 *
 * Implements a recursive doubling algorithm for better scalability.
 *
 * @param _name Name of the reduction operation
 * @param _type Data type to operate on
 * @param _op Binary operator to apply
 */
#define REDUCE_HELPER_REC_DBL(_name, _type, _op)                               \
  void reduce_helper_##_name##_rec_dbl(                                        \
      _type *dest, const _type *source, int nreduce, int PE_start,             \
      int logPE_stride, int PE_size, _type *pWrk, long *pSync) {               \
    const int stride = 1 << logPE_stride;                                      \
    const int me = shmem_my_pe();                                              \
    int peer;                                                                  \
                                                                               \
    size_t nbytes = nreduce * sizeof(_type);                                   \
                                                                               \
    /* Get my index in the active set */                                       \
    int me_as = (me - PE_start) / stride;                                      \
    int mask;                                                                  \
                                                                               \
    int xchg_peer_p2s;                                                         \
    int xchg_peer_as;                                                          \
    int xchg_peer_pe;                                                          \
                                                                               \
    /* Power 2 set */                                                          \
    int me_p2s;                                                                \
    int p2s_size;                                                              \
                                                                               \
    _type *tmp_array = NULL;                                                   \
                                                                               \
    /* Find the greatest power of 2 lower than PE_size */                      \
    for (p2s_size = 1; p2s_size * 2 <= PE_size; p2s_size *= 2)                 \
      ;                                                                        \
                                                                               \
    /* Check if the current PE belongs to the power 2 set */                   \
    me_p2s = me_as * p2s_size / PE_size;                                       \
    if ((me_p2s * PE_size + p2s_size - 1) / p2s_size != me_as) {               \
      me_p2s = -1;                                                             \
    }                                                                          \
                                                                               \
    /* If current PE belongs to the power 2 set, it will need temporary buffer \
     */                                                                        \
    if (me_p2s != -1) {                                                        \
      tmp_array = malloc(nreduce * sizeof(_type));                             \
      if (tmp_array == NULL) {                                                 \
        /* TODO: raise error */                                                \
        fprintf(stderr, "PE %d: Cannot allocate memory!\n", me);               \
        exit(-1);                                                              \
      }                                                                        \
    }                                                                          \
                                                                               \
    /* Check if the current PE should wait/send data to the peer */            \
    if (me_p2s == -1) {                                                        \
      /* Notify peer that the data is ready */                                 \
      peer = PE_start + (me_as - 1) * stride;                                  \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE + 1, peer);                        \
    } else if ((me_as + 1) * p2s_size / PE_size == me_p2s) {                   \
      /* We should wait for the data to be ready */                            \
      peer = PE_start + (me_as + 1) * stride;                                  \
                                                                               \
      shmem_long_wait_until(pSync, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);           \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);                              \
                                                                               \
      /* Get the array and reduce */                                           \
      shmem_getmem(dest, source, nbytes, peer);                                \
      local_##_name##_reduce(tmp_array, dest, source, nreduce);                \
    } else {                                                                   \
      memcpy(tmp_array, source, nbytes);                                       \
    }                                                                          \
                                                                               \
    /* If the current PE belongs to the power 2 set, do recursive doubling */  \
    if (me_p2s != -1) {                                                        \
      int i;                                                                   \
                                                                               \
      for (mask = 0x1, i = 1; mask < p2s_size; mask <<= 1, i++) {              \
        xchg_peer_p2s = me_p2s ^ mask;                                         \
        xchg_peer_as = (xchg_peer_p2s * PE_size + p2s_size - 1) / p2s_size;    \
        xchg_peer_pe = PE_start + xchg_peer_as * stride;                       \
                                                                               \
        /* Notify the peer PE that current PE is ready to accept the data */   \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE + 1, xchg_peer_pe);          \
                                                                               \
        /* Wait until the peer PE is ready to accept the data */               \
        shmem_long_wait_until(pSync + i, SHMEM_CMP_GT, SHCOLL_SYNC_VALUE);     \
                                                                               \
        /* Send the data to the peer */                                        \
        shmem_putmem(dest, tmp_array, nbytes, xchg_peer_pe);                   \
        shmem_fence();                                                         \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE + 2, xchg_peer_pe);          \
                                                                               \
        /* Wait until the data is received and do local reduce */              \
        shmem_long_wait_until(pSync + i, SHMEM_CMP_GT, SHCOLL_SYNC_VALUE + 1); \
        local_##_name##_reduce(tmp_array, tmp_array, dest, nreduce);           \
                                                                               \
        /* Reset the pSync for the current round */                            \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE, me);                        \
      }                                                                        \
                                                                               \
      memcpy(dest, tmp_array, nbytes);                                         \
    }                                                                          \
                                                                               \
    if (me_p2s == -1) {                                                        \
      /* Wait to get the data from a PE that is in the power 2 set */          \
      shmem_long_wait_until(pSync, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);           \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);                              \
    } else if ((me_as + 1) * p2s_size / PE_size == me_p2s) {                   \
      /* Send data to peer PE that is outside the power 2 set */               \
      peer = PE_start + (me_as + 1) * stride;                                  \
                                                                               \
      shmem_putmem(dest, dest, nbytes, peer);                                  \
      shmem_fence();                                                           \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE + 1, peer);                        \
    }                                                                          \
                                                                               \
    if (tmp_array != NULL) {                                                   \
      free(tmp_array);                                                         \
    }                                                                          \
  }

/*
 * @brief Helper macro to define Rabenseifner reduction operations
 *
 * Implements Rabenseifner's reduction algorithm which combines recursive
 * halving with recursive doubling for better scalability and communication
 * efficiency.
 *
 * @param _name Name of the reduction operation
 * @param _type Data type to operate on
 * @param _op Binary operator to apply
 */
#define REDUCE_HELPER_RABENSEIFNER(_name, _type, _op)                          \
  void reduce_helper_##_name##_rabenseifner(                                   \
      _type *dest, const _type *source, int nreduce, int PE_start,             \
      int logPE_stride, int PE_size, _type *pWrk, long *pSync) {               \
    const int stride = 1 << logPE_stride;                                      \
    const int me = shmem_my_pe();                                              \
                                                                               \
    int me_as = (me - PE_start) / stride;                                      \
    int peer;                                                                  \
    size_t i;                                                                  \
    const size_t nelems = (const size_t)nreduce;                               \
                                                                               \
    int block_idx_begin;                                                       \
    int block_idx_end;                                                         \
                                                                               \
    ptrdiff_t block_offset;                                                    \
    ptrdiff_t next_block_offset;                                               \
    size_t block_nelems;                                                       \
                                                                               \
    int xchg_peer_p2s;                                                         \
    int xchg_peer_as;                                                          \
    int xchg_peer_pe;                                                          \
                                                                               \
    /* Power 2 set */                                                          \
    int me_p2s;                                                                \
    int p2s_size;                                                              \
    int log_p2s_size;                                                          \
                                                                               \
    int distance;                                                              \
    _type *tmp_array = NULL;                                                   \
                                                                               \
    /* Find the greatest power of 2 lower than PE_size */                      \
    for (p2s_size = 1, log_p2s_size = 0; p2s_size * 2 <= PE_size;              \
         p2s_size *= 2, log_p2s_size++)                                        \
      ;                                                                        \
                                                                               \
    /* Check if the current PE belongs to the power 2 set */                   \
    me_p2s = me_as * p2s_size / PE_size;                                       \
    if ((me_p2s * PE_size + p2s_size - 1) / p2s_size != me_as) {               \
      me_p2s = -1;                                                             \
    }                                                                          \
                                                                               \
    /* If current PE belongs to the power 2 set, it will need temporary buffer \
     */                                                                        \
    if (me_p2s != -1) {                                                        \
      tmp_array = malloc((nelems / 2 + 1) * sizeof(_type));                    \
      if (tmp_array == NULL) {                                                 \
        /* TODO: raise error */                                                \
        fprintf(stderr, "PE %d: Cannot allocate memory!\n", me);               \
        exit(-1);                                                              \
      }                                                                        \
    }                                                                          \
                                                                               \
    /* Check if the current PE should wait/send data to the peer */            \
    if (me_p2s == -1) {                                                        \
      /* Notify peer that the data is ready */                                 \
      peer = PE_start + (me_as - 1) * stride;                                  \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE + 1, peer);                        \
                                                                               \
      /* Wait until the data on peer node is ready and get the data (upper     \
       * half of the array) */                                                 \
      block_offset = nelems / 2;                                               \
      block_nelems = (size_t)(nelems - block_offset);                          \
                                                                               \
      shmem_long_wait_until(pSync, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);           \
      shmem_getmem(dest + block_offset, source + block_offset,                 \
                   block_nelems * sizeof(_type), peer);                        \
                                                                               \
      /* Reduce the upper half of the array */                                 \
      local_##_name##_reduce(dest + block_offset, dest + block_offset,         \
                             source + block_offset, block_nelems);             \
                                                                               \
      /* Send the upper half of the array to peer */                           \
      shmem_putmem(dest + block_offset, dest + block_offset,                   \
                   block_nelems * sizeof(_type), peer);                        \
      shmem_fence();                                                           \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE + 2, peer);                        \
    } else if ((me_as + 1) * p2s_size / PE_size == me_p2s) {                   \
      /* Notify peer that the data is ready */                                 \
      peer = PE_start + (me_as + 1) * stride;                                  \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE + 1, peer);                        \
                                                                               \
      /* Wait until the data on peer node is ready and get the data (lower     \
       * half of the array) */                                                 \
      block_offset = 0;                                                        \
      block_nelems = (size_t)(nelems / 2 - block_offset);                      \
                                                                               \
      shmem_long_wait_until(pSync, SHMEM_CMP_GT, SHCOLL_SYNC_VALUE);           \
      shmem_getmem(dest, source, block_nelems * sizeof(_type), peer);          \
                                                                               \
      /* Do local reduce */                                                    \
      local_##_name##_reduce(dest, dest, source, block_nelems);                \
                                                                               \
      /* Wait until the upper half is received from peer */                    \
      shmem_long_wait_until(pSync, SHMEM_CMP_GT, SHCOLL_SYNC_VALUE + 1);       \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);                              \
    } else {                                                                   \
      memcpy(dest, source, nelems * sizeof(_type));                            \
    }                                                                          \
                                                                               \
    /* For nodes in the power 2 set, dest contains data that should be reduced \
     */                                                                        \
                                                                               \
    /* Do reduce scatter with the nodes in power 2 set */                      \
    if (me_p2s != -1) {                                                        \
      block_idx_begin = 0;                                                     \
      block_idx_end = p2s_size;                                                \
                                                                               \
      for (distance = 1, i = 1; distance < p2s_size; distance <<= 1, i++) {    \
        xchg_peer_p2s = ((me_p2s & distance) == 0) ? me_p2s + distance         \
                                                   : me_p2s - distance;        \
        xchg_peer_as = (xchg_peer_p2s * PE_size + p2s_size - 1) / p2s_size;    \
        xchg_peer_pe = PE_start + xchg_peer_as * stride;                       \
                                                                               \
        /* Notify the peer PE that the data is ready to be read */             \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE + 1, xchg_peer_pe);          \
                                                                               \
        /* Check if the current PE is responsible for lower half of upper half \
         * of the vector */                                                    \
        if ((me_p2s & distance) == 0) {                                        \
          block_idx_end = (block_idx_begin + block_idx_end) / 2;               \
        } else {                                                               \
          block_idx_begin = (block_idx_begin + block_idx_end) / 2;             \
        }                                                                      \
                                                                               \
        /* TODO: possible overflow */                                          \
        block_offset = (block_idx_begin * nelems) / p2s_size;                  \
        next_block_offset = (block_idx_end * nelems) / p2s_size;               \
        block_nelems = (size_t)(next_block_offset - block_offset);             \
                                                                               \
        /* Wait until the data on peer PE is ready to be read and get the data \
         */                                                                    \
        shmem_long_wait_until(pSync + i, SHMEM_CMP_GE, SHCOLL_SYNC_VALUE + 1); \
        shmem_getmem(tmp_array, dest + block_offset,                           \
                     block_nelems * sizeof(_type), xchg_peer_pe);              \
                                                                               \
        /* Notify the peer PE that the data transfer has completed             \
         * successfully */                                                     \
        shmem_fence();                                                         \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE + 2, xchg_peer_pe);          \
                                                                               \
        /* Do local reduce */                                                  \
        local_##_name##_reduce(dest + block_offset, dest + block_offset,       \
                               tmp_array, block_nelems);                       \
                                                                               \
        /* Wait until the peer PE has read the data */                         \
        shmem_long_wait_until(pSync + i, SHMEM_CMP_GE, SHCOLL_SYNC_VALUE + 2); \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE, me);                        \
      }                                                                        \
    }                                                                          \
                                                                               \
    /* For nodes in the power 2 set, dest contains data that should be reduced \
     */                                                                        \
                                                                               \
    /* Do collect with the nodes in power 2 set */                             \
    if (me_p2s != -1) {                                                        \
      block_offset = 0;                                                        \
      block_idx_begin = reverse_bits(me_p2s, log_p2s_size);                    \
      block_idx_end = block_idx_begin + 1;                                     \
                                                                               \
      for (distance = p2s_size / 2, i = sizeof(int) * CHAR_BIT + 1;            \
           distance > 0; distance >>= 1, i++) {                                \
        xchg_peer_p2s = ((me_p2s & distance) == 0) ? me_p2s + distance         \
                                                   : me_p2s - distance;        \
        xchg_peer_as = (xchg_peer_p2s * PE_size + p2s_size - 1) / p2s_size;    \
        xchg_peer_pe = PE_start + xchg_peer_as * stride;                       \
                                                                               \
        /* TODO: possible overflow */                                          \
        block_offset = (block_idx_begin * nelems) / p2s_size;                  \
        next_block_offset = (block_idx_end * nelems) / p2s_size;               \
        block_nelems = (size_t)(next_block_offset - block_offset);             \
                                                                               \
        shmem_putmem(dest + block_offset, dest + block_offset,                 \
                     block_nelems * sizeof(_type), xchg_peer_pe);              \
        shmem_fence();                                                         \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE + 1, xchg_peer_pe);          \
                                                                               \
        /* Wait until the data has arrived from exchange the peer PE */        \
        shmem_long_wait_until(pSync + i, SHMEM_CMP_GE, SHCOLL_SYNC_VALUE + 1); \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE, me);                        \
                                                                               \
        /* Updated the block range */                                          \
        if ((me_p2s & distance) == 0) {                                        \
          block_idx_end += (block_idx_end - block_idx_begin);                  \
        } else {                                                               \
          block_idx_begin -= (block_idx_end - block_idx_begin);                \
        }                                                                      \
      }                                                                        \
    }                                                                          \
                                                                               \
    /* Check if the current PE should wait/send data to the peer */            \
    if (me_p2s == -1) {                                                        \
      /* Wait until the peer PE sends the data */                              \
      shmem_long_wait_until(pSync + 1, SHMEM_CMP_GE, SHCOLL_SYNC_VALUE + 1);   \
      shmem_long_p(pSync + 1, SHCOLL_SYNC_VALUE, me);                          \
    } else if ((me_as + 1) * p2s_size / PE_size == me_p2s) {                   \
      peer = PE_start + (me_as + 1) * stride;                                  \
      shmem_putmem(dest, dest, nelems * sizeof(_type), peer);                  \
      shmem_fence();                                                           \
      shmem_long_p(pSync + 1, SHCOLL_SYNC_VALUE + 1, peer);                    \
    }                                                                          \
                                                                               \
    if (tmp_array != NULL) {                                                   \
      free(tmp_array);                                                         \
    }                                                                          \
  }

/**
 * @brief Helper macro to define Rabenseifner reduction operations
 *
 * This function implements the Rabenseifner's algorithm for reducing data
 * across all PEs in a team. It uses a parallel reduction approach to
 * efficiently compute the desired operation (e.g., sum, product, etc.)
 * on the input data.
 */
#define REDUCE_HELPER_RABENSEIFNER2(_name, _type, _op)                         \
  void reduce_helper_##_name##_rabenseifner2(                                  \
      _type *dest, const _type *source, int nreduce, int PE_start,             \
      int logPE_stride, int PE_size, _type *pWrk, long *pSync) {               \
    const int stride = 1 << logPE_stride;                                      \
    const int me = shmem_my_pe();                                              \
                                                                               \
    int me_as = (me - PE_start) / stride;                                      \
    int peer;                                                                  \
    size_t i;                                                                  \
                                                                               \
    const size_t nelems = (const size_t)nreduce;                               \
    int block_idx_begin;                                                       \
    int block_idx_end;                                                         \
                                                                               \
    ptrdiff_t block_offset;                                                    \
    ptrdiff_t next_block_offset;                                               \
    size_t block_nelems;                                                       \
                                                                               \
    int xchg_peer_p2s;                                                         \
    int xchg_peer_as;                                                          \
    int xchg_peer_pe;                                                          \
                                                                               \
    int ring_peer_p2s;                                                         \
    int ring_peer_as;                                                          \
    int ring_peer_pe;                                                          \
                                                                               \
    /* Power 2 set */                                                          \
    int me_p2s;                                                                \
    int p2s_size;                                                              \
    int log_p2s_size;                                                          \
                                                                               \
    int distance;                                                              \
    _type *tmp_array = NULL;                                                   \
                                                                               \
    long *collect_pSync = pSync + (1 + sizeof(int) * CHAR_BIT);                \
                                                                               \
    /* Find the greatest power of 2 lower than PE_size */                      \
    for (p2s_size = 1, log_p2s_size = 0; p2s_size * 2 <= PE_size;              \
         p2s_size *= 2, log_p2s_size++)                                        \
      ;                                                                        \
                                                                               \
    /* Check if the current PE belongs to the power 2 set */                   \
    me_p2s = me_as * p2s_size / PE_size;                                       \
    if ((me_p2s * PE_size + p2s_size - 1) / p2s_size != me_as) {               \
      me_p2s = -1;                                                             \
    }                                                                          \
                                                                               \
    /* If current PE belongs to the power 2 set, it will need temporary buffer \
     */                                                                        \
    if (me_p2s != -1) {                                                        \
      tmp_array = malloc((nelems / 2 + 1) * sizeof(_type));                    \
      if (tmp_array == NULL) {                                                 \
        /* TODO: raise error */                                                \
        fprintf(stderr, "PE %d: Cannot allocate memory!\n", me);               \
        exit(-1);                                                              \
      }                                                                        \
    }                                                                          \
                                                                               \
    /* Check if the current PE should wait/send data to the peer */            \
    if (me_p2s == -1) {                                                        \
      /* Notify peer that the data is ready */                                 \
      peer = PE_start + (me_as - 1) * stride;                                  \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE + 1, peer);                        \
                                                                               \
      /* Wait until the data on peer node is ready and get the data (upper     \
       * half of the array) */                                                 \
      block_offset = nelems / 2;                                               \
      block_nelems = (size_t)(nelems - block_offset);                          \
                                                                               \
      shmem_long_wait_until(pSync, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);           \
      shmem_getmem(dest + block_offset, source + block_offset,                 \
                   block_nelems * sizeof(_type), peer);                        \
                                                                               \
      /* Reduce the upper half of the array */                                 \
      local_##_name##_reduce(dest + block_offset, dest + block_offset,         \
                             source + block_offset, block_nelems);             \
                                                                               \
      /* Send the upper half of the array to peer */                           \
      shmem_putmem(dest + block_offset, dest + block_offset,                   \
                   block_nelems * sizeof(_type), peer);                        \
      shmem_fence();                                                           \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE + 2, peer);                        \
    } else if ((me_as + 1) * p2s_size / PE_size == me_p2s) {                   \
      /* Notify peer that the data is ready */                                 \
      peer = PE_start + (me_as + 1) * stride;                                  \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE + 1, peer);                        \
                                                                               \
      /* Wait until the data on peer node is ready and get the data (lower     \
       * half of the array) */                                                 \
      block_offset = 0;                                                        \
      block_nelems = (size_t)(nelems / 2 - block_offset);                      \
                                                                               \
      shmem_long_wait_until(pSync, SHMEM_CMP_GT, SHCOLL_SYNC_VALUE);           \
      shmem_getmem(dest, source, block_nelems * sizeof(_type), peer);          \
                                                                               \
      /* Do local reduce */                                                    \
      local_##_name##_reduce(dest, dest, source, block_nelems);                \
                                                                               \
      /* Wait until the upper half is received from peer */                    \
      shmem_long_wait_until(pSync, SHMEM_CMP_GT, SHCOLL_SYNC_VALUE + 1);       \
      shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);                              \
    } else {                                                                   \
      memcpy(dest, source, nelems * sizeof(_type));                            \
    }                                                                          \
                                                                               \
    /* For nodes in the power 2 set, dest contains data that should be reduced \
     */                                                                        \
                                                                               \
    /* Do reduce scatter with the nodes in power 2 set */                      \
    if (me_p2s != -1) {                                                        \
      block_idx_begin = 0;                                                     \
      block_idx_end = p2s_size;                                                \
                                                                               \
      for (distance = 1, i = 1; distance < p2s_size; distance <<= 1, i++) {    \
        xchg_peer_p2s = ((me_p2s & distance) == 0) ? me_p2s + distance         \
                                                   : me_p2s - distance;        \
        xchg_peer_as = (xchg_peer_p2s * PE_size + p2s_size - 1) / p2s_size;    \
        xchg_peer_pe = PE_start + xchg_peer_as * stride;                       \
                                                                               \
        /* Notify the peer PE that the data is ready to be read */             \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE + 1, xchg_peer_pe);          \
                                                                               \
        /* Check if the current PE is responsible for lower half of upper half \
         * of the vector */                                                    \
        if ((me_p2s & distance) == 0) {                                        \
          block_idx_end = (block_idx_begin + block_idx_end) / 2;               \
        } else {                                                               \
          block_idx_begin = (block_idx_begin + block_idx_end) / 2;             \
        }                                                                      \
                                                                               \
        /* TODO: possible overflow */                                          \
        block_offset = (block_idx_begin * nelems) / p2s_size;                  \
        next_block_offset = (block_idx_end * nelems) / p2s_size;               \
        block_nelems = (size_t)(next_block_offset - block_offset);             \
                                                                               \
        /* Wait until the data on peer PE is ready to be read and get the data \
         */                                                                    \
        shmem_long_wait_until(pSync + i, SHMEM_CMP_GE, SHCOLL_SYNC_VALUE + 1); \
        shmem_getmem(tmp_array, dest + block_offset,                           \
                     block_nelems * sizeof(_type), xchg_peer_pe);              \
                                                                               \
        /* Notify the peer PE that the data transfer has completed             \
         * successfully */                                                     \
        shmem_fence();                                                         \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE + 2, xchg_peer_pe);          \
                                                                               \
        /* Do local reduce */                                                  \
        local_##_name##_reduce(dest + block_offset, dest + block_offset,       \
                               tmp_array, block_nelems);                       \
                                                                               \
        /* Wait until the peer PE has read the data */                         \
        shmem_long_wait_until(pSync + i, SHMEM_CMP_GE, SHCOLL_SYNC_VALUE + 2); \
        shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE, me);                        \
      }                                                                        \
    }                                                                          \
                                                                               \
    /* For nodes in the power 2 set, dest contains data that should be reduced \
     */                                                                        \
                                                                               \
    /* Do collect with the nodes in power 2 set */                             \
    if (me_p2s != -1) {                                                        \
      ring_peer_p2s = (me_p2s + 1) % p2s_size;                                 \
      ring_peer_as = (ring_peer_p2s * PE_size + p2s_size - 1) / p2s_size;      \
      ring_peer_pe = PE_start + ring_peer_as * stride;                         \
                                                                               \
      for (i = 0; i < p2s_size; i++) {                                         \
        block_idx_begin = reverse_bits(                                        \
            (int)((me_p2s - i + p2s_size) % p2s_size), log_p2s_size);          \
        block_idx_end = block_idx_begin + 1;                                   \
                                                                               \
        /* TODO: possible overflow */                                          \
        block_offset = (block_idx_begin * nelems) / p2s_size;                  \
        next_block_offset = (block_idx_end * nelems) / p2s_size;               \
        block_nelems = (size_t)(next_block_offset - block_offset);             \
                                                                               \
        shmem_putmem_nbi(dest + block_offset, dest + block_offset,             \
                         block_nelems * sizeof(_type), ring_peer_pe);          \
        shmem_fence();                                                         \
        shmem_long_p(collect_pSync, SHCOLL_SYNC_VALUE + i + 1, ring_peer_pe);  \
                                                                               \
        shmem_long_wait_until(collect_pSync, SHMEM_CMP_GT,                     \
                              SHCOLL_SYNC_VALUE + i);                          \
      }                                                                        \
                                                                               \
      shmem_long_p(collect_pSync, SHCOLL_SYNC_VALUE, me);                      \
    }                                                                          \
                                                                               \
    /* Check if the current PE should wait/send data to the peer */            \
    if (me_p2s == -1) {                                                        \
      /* Wait until the peer PE sends the data */                              \
      shmem_long_wait_until(pSync + 1, SHMEM_CMP_GE, SHCOLL_SYNC_VALUE + 1);   \
      shmem_long_p(pSync + 1, SHCOLL_SYNC_VALUE, me);                          \
    } else if ((me_as + 1) * p2s_size / PE_size == me_p2s) {                   \
      peer = PE_start + (me_as + 1) * stride;                                  \
      shmem_putmem(dest, dest, nelems * sizeof(_type), peer);                  \
      shmem_fence();                                                           \
      shmem_long_p(pSync + 1, SHCOLL_SYNC_VALUE + 1, peer);                    \
    }                                                                          \
                                                                               \
    if (tmp_array != NULL) {                                                   \
      free(tmp_array);                                                         \
    }                                                                          \
  }

/*
 * Supported reduction operations
 */

#define AND_OP(A, B) ((A) & (B))
#define MAX_OP(A, B) ((A) > (B) ? (A) : (B))
#define MIN_OP(A, B) ((A) < (B) ? (A) : (B))
#define SUM_OP(A, B) ((A) + (B))
#define PROD_OP(A, B) ((A) * (B))
#define OR_OP(A, B) ((A) | (B))
#define XOR_OP(A, B) ((A) ^ (B))

/*
 * Definitions for all reductions
 */

/**

  XXX: should i just add the additional types used in the team-based reductions
  or write separate subroutines?

 */
/**
 * @file reduce.c
 * @brief Collective reduction operations for OpenSHMEM
 *
 * This file implements various reduction operations (AND, OR, XOR, MAX, MIN,
 * SUM, PROD) across multiple processing elements (PEs) using different
 * algorithms.
 */

/**
 * @brief Macro to define reduction operations for all supported types
 *
 * This macro expands to define reduction operations for all combinations of:
 * - Data types (char, int, float, etc.)
 * - Operations (AND, OR, XOR, MAX, MIN, SUM, PROD)
 * - Algorithms (linear, binomial, recursive doubling, Rabenseifner)
 *
 * @param _name The name of the reduction helper function to define
 */

/* Operation-specific macros using REDUCE type tables (supersets of TO_ALL) */
#define SHCOLL_TO_ALL_DEFINE_AND(_name)                                        \
  SHMEM_REDUCE_BITWISE_TYPE_TABLE(_name##_AND_HELPER)

#define SHCOLL_TO_ALL_DEFINE_OR(_name)                                         \
  SHMEM_REDUCE_BITWISE_TYPE_TABLE(_name##_OR_HELPER)

#define SHCOLL_TO_ALL_DEFINE_XOR(_name)                                        \
  SHMEM_REDUCE_BITWISE_TYPE_TABLE(_name##_XOR_HELPER)

#define SHCOLL_TO_ALL_DEFINE_MAX(_name)                                        \
  SHMEM_REDUCE_MINMAX_TYPE_TABLE(_name##_MAX_HELPER)

#define SHCOLL_TO_ALL_DEFINE_MIN(_name)                                        \
  SHMEM_REDUCE_MINMAX_TYPE_TABLE(_name##_MIN_HELPER)

#define SHCOLL_TO_ALL_DEFINE_SUM(_name)                                        \
  SHMEM_REDUCE_ARITH_TYPE_TABLE(_name##_SUM_HELPER)

#define SHCOLL_TO_ALL_DEFINE_PROD(_name)                                       \
  SHMEM_REDUCE_ARITH_TYPE_TABLE(_name##_PROD_HELPER)

/* Helper macros that can be used directly by type tables */
#define REDUCE_HELPER_LOCAL_AND_HELPER(_type, _typename)                       \
  REDUCE_HELPER_LOCAL(_typename##_and, _type, AND_OP)
#define REDUCE_HELPER_LOCAL_OR_HELPER(_type, _typename)                        \
  REDUCE_HELPER_LOCAL(_typename##_or, _type, OR_OP)
#define REDUCE_HELPER_LOCAL_XOR_HELPER(_type, _typename)                       \
  REDUCE_HELPER_LOCAL(_typename##_xor, _type, XOR_OP)
#define REDUCE_HELPER_LOCAL_MAX_HELPER(_type, _typename)                       \
  REDUCE_HELPER_LOCAL(_typename##_max, _type, MAX_OP)
#define REDUCE_HELPER_LOCAL_MIN_HELPER(_type, _typename)                       \
  REDUCE_HELPER_LOCAL(_typename##_min, _type, MIN_OP)
#define REDUCE_HELPER_LOCAL_SUM_HELPER(_type, _typename)                       \
  REDUCE_HELPER_LOCAL(_typename##_sum, _type, SUM_OP)
#define REDUCE_HELPER_LOCAL_PROD_HELPER(_type, _typename)                      \
  REDUCE_HELPER_LOCAL(_typename##_prod, _type, PROD_OP)

#define REDUCE_HELPER_LINEAR_AND_HELPER(_type, _typename)                      \
  REDUCE_HELPER_LINEAR(_typename##_and, _type, AND_OP)
#define REDUCE_HELPER_LINEAR_OR_HELPER(_type, _typename)                       \
  REDUCE_HELPER_LINEAR(_typename##_or, _type, OR_OP)
#define REDUCE_HELPER_LINEAR_XOR_HELPER(_type, _typename)                      \
  REDUCE_HELPER_LINEAR(_typename##_xor, _type, XOR_OP)
#define REDUCE_HELPER_LINEAR_MAX_HELPER(_type, _typename)                      \
  REDUCE_HELPER_LINEAR(_typename##_max, _type, MAX_OP)
#define REDUCE_HELPER_LINEAR_MIN_HELPER(_type, _typename)                      \
  REDUCE_HELPER_LINEAR(_typename##_min, _type, MIN_OP)
#define REDUCE_HELPER_LINEAR_SUM_HELPER(_type, _typename)                      \
  REDUCE_HELPER_LINEAR(_typename##_sum, _type, SUM_OP)
#define REDUCE_HELPER_LINEAR_PROD_HELPER(_type, _typename)                     \
  REDUCE_HELPER_LINEAR(_typename##_prod, _type, PROD_OP)

#define REDUCE_HELPER_BINOMIAL_AND_HELPER(_type, _typename)                    \
  REDUCE_HELPER_BINOMIAL(_typename##_and, _type, AND_OP)
#define REDUCE_HELPER_BINOMIAL_OR_HELPER(_type, _typename)                     \
  REDUCE_HELPER_BINOMIAL(_typename##_or, _type, OR_OP)
#define REDUCE_HELPER_BINOMIAL_XOR_HELPER(_type, _typename)                    \
  REDUCE_HELPER_BINOMIAL(_typename##_xor, _type, XOR_OP)
#define REDUCE_HELPER_BINOMIAL_MAX_HELPER(_type, _typename)                    \
  REDUCE_HELPER_BINOMIAL(_typename##_max, _type, MAX_OP)
#define REDUCE_HELPER_BINOMIAL_MIN_HELPER(_type, _typename)                    \
  REDUCE_HELPER_BINOMIAL(_typename##_min, _type, MIN_OP)
#define REDUCE_HELPER_BINOMIAL_SUM_HELPER(_type, _typename)                    \
  REDUCE_HELPER_BINOMIAL(_typename##_sum, _type, SUM_OP)
#define REDUCE_HELPER_BINOMIAL_PROD_HELPER(_type, _typename)                   \
  REDUCE_HELPER_BINOMIAL(_typename##_prod, _type, PROD_OP)

#define REDUCE_HELPER_REC_DBL_AND_HELPER(_type, _typename)                     \
  REDUCE_HELPER_REC_DBL(_typename##_and, _type, AND_OP)
#define REDUCE_HELPER_REC_DBL_OR_HELPER(_type, _typename)                      \
  REDUCE_HELPER_REC_DBL(_typename##_or, _type, OR_OP)
#define REDUCE_HELPER_REC_DBL_XOR_HELPER(_type, _typename)                     \
  REDUCE_HELPER_REC_DBL(_typename##_xor, _type, XOR_OP)
#define REDUCE_HELPER_REC_DBL_MAX_HELPER(_type, _typename)                     \
  REDUCE_HELPER_REC_DBL(_typename##_max, _type, MAX_OP)
#define REDUCE_HELPER_REC_DBL_MIN_HELPER(_type, _typename)                     \
  REDUCE_HELPER_REC_DBL(_typename##_min, _type, MIN_OP)
#define REDUCE_HELPER_REC_DBL_SUM_HELPER(_type, _typename)                     \
  REDUCE_HELPER_REC_DBL(_typename##_sum, _type, SUM_OP)
#define REDUCE_HELPER_REC_DBL_PROD_HELPER(_type, _typename)                    \
  REDUCE_HELPER_REC_DBL(_typename##_prod, _type, PROD_OP)

#define REDUCE_HELPER_RABENSEIFNER_AND_HELPER(_type, _typename)                \
  REDUCE_HELPER_RABENSEIFNER(_typename##_and, _type, AND_OP)
#define REDUCE_HELPER_RABENSEIFNER_OR_HELPER(_type, _typename)                 \
  REDUCE_HELPER_RABENSEIFNER(_typename##_or, _type, OR_OP)
#define REDUCE_HELPER_RABENSEIFNER_XOR_HELPER(_type, _typename)                \
  REDUCE_HELPER_RABENSEIFNER(_typename##_xor, _type, XOR_OP)
#define REDUCE_HELPER_RABENSEIFNER_MAX_HELPER(_type, _typename)                \
  REDUCE_HELPER_RABENSEIFNER(_typename##_max, _type, MAX_OP)
#define REDUCE_HELPER_RABENSEIFNER_MIN_HELPER(_type, _typename)                \
  REDUCE_HELPER_RABENSEIFNER(_typename##_min, _type, MIN_OP)
#define REDUCE_HELPER_RABENSEIFNER_SUM_HELPER(_type, _typename)                \
  REDUCE_HELPER_RABENSEIFNER(_typename##_sum, _type, SUM_OP)
#define REDUCE_HELPER_RABENSEIFNER_PROD_HELPER(_type, _typename)               \
  REDUCE_HELPER_RABENSEIFNER(_typename##_prod, _type, PROD_OP)

#define REDUCE_HELPER_RABENSEIFNER2_AND_HELPER(_type, _typename)               \
  REDUCE_HELPER_RABENSEIFNER2(_typename##_and, _type, AND_OP)
#define REDUCE_HELPER_RABENSEIFNER2_OR_HELPER(_type, _typename)                \
  REDUCE_HELPER_RABENSEIFNER2(_typename##_or, _type, OR_OP)
#define REDUCE_HELPER_RABENSEIFNER2_XOR_HELPER(_type, _typename)               \
  REDUCE_HELPER_RABENSEIFNER2(_typename##_xor, _type, XOR_OP)
#define REDUCE_HELPER_RABENSEIFNER2_MAX_HELPER(_type, _typename)               \
  REDUCE_HELPER_RABENSEIFNER2(_typename##_max, _type, MAX_OP)
#define REDUCE_HELPER_RABENSEIFNER2_MIN_HELPER(_type, _typename)               \
  REDUCE_HELPER_RABENSEIFNER2(_typename##_min, _type, MIN_OP)
#define REDUCE_HELPER_RABENSEIFNER2_SUM_HELPER(_type, _typename)               \
  REDUCE_HELPER_RABENSEIFNER2(_typename##_sum, _type, SUM_OP)
#define REDUCE_HELPER_RABENSEIFNER2_PROD_HELPER(_type, _typename)              \
  REDUCE_HELPER_RABENSEIFNER2(_typename##_prod, _type, PROD_OP)

/* Combined macro that generates all implementations */
#define SHCOLL_TO_ALL_DEFINE(_name)                                            \
  SHCOLL_TO_ALL_DEFINE_AND(_name)                                              \
  SHCOLL_TO_ALL_DEFINE_OR(_name)                                               \
  SHCOLL_TO_ALL_DEFINE_XOR(_name)                                              \
  SHCOLL_TO_ALL_DEFINE_MAX(_name)                                              \
  SHCOLL_TO_ALL_DEFINE_MIN(_name)                                              \
  SHCOLL_TO_ALL_DEFINE_SUM(_name)                                              \
  SHCOLL_TO_ALL_DEFINE_PROD(_name)

/* Generate all implementations for all types and operations using REDUCE type
 * tables (supersets) */
SHCOLL_TO_ALL_DEFINE(REDUCE_HELPER_LOCAL)
SHCOLL_TO_ALL_DEFINE(REDUCE_HELPER_LINEAR)
SHCOLL_TO_ALL_DEFINE(REDUCE_HELPER_BINOMIAL)
SHCOLL_TO_ALL_DEFINE(REDUCE_HELPER_REC_DBL)
SHCOLL_TO_ALL_DEFINE(REDUCE_HELPER_RABENSEIFNER)
SHCOLL_TO_ALL_DEFINE(REDUCE_HELPER_RABENSEIFNER2)

/* Generate additional helpers for TO_ALL bitwise types (which don't overlap
 * with REDUCE bitwise types) */
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_LOCAL_AND_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_LOCAL_OR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_LOCAL_XOR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_LINEAR_AND_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_LINEAR_OR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_LINEAR_XOR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_BINOMIAL_AND_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_BINOMIAL_OR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_BINOMIAL_XOR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_REC_DBL_AND_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_REC_DBL_OR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_REC_DBL_XOR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_RABENSEIFNER_AND_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_RABENSEIFNER_OR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_RABENSEIFNER_XOR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_RABENSEIFNER2_AND_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_RABENSEIFNER2_OR_HELPER)
SHMEM_TO_ALL_BITWISE_TYPE_TABLE(REDUCE_HELPER_RABENSEIFNER2_XOR_HELPER)

/* @formatter:on */
// clang-format on

/*
 * @brief Macro to define the reduction operation wrapper function
 *
 * @param _typename_op Combined type and operation name (e.g. char_and)
 * @param _type       Actual C type (e.g. char)
 * @param _algo  Algorithm suffix (e.g. linear)
 */
#define SHCOLL_TO_ALL_DEFINITION(_typename_op, _type, _algo)                   \
  void shcoll_##_typename_op##_to_all_##_algo(                                 \
      _type *dest, const _type *source, int nreduce, int PE_start,             \
      int logPE_stride, int PE_size, _type *pWrk, long *pSync) {               \
    /* Sanity Checks */                                                        \
    SHMEMU_CHECK_INIT();                                                       \
    SHMEMU_CHECK_POSITIVE(PE_size, "PE_size");                                 \
    SHMEMU_CHECK_NON_NEGATIVE(PE_start, "PE_start");                           \
    SHMEMU_CHECK_NON_NEGATIVE(logPE_stride, "logPE_stride");                   \
    SHMEMU_CHECK_ACTIVE_SET_RANGE(PE_start, logPE_stride, PE_size);            \
    SHMEMU_CHECK_NULL(dest, "dest");                                           \
    SHMEMU_CHECK_NULL(source, "source");                                       \
    SHMEMU_CHECK_NULL(pWrk, "pWrk");                                           \
    SHMEMU_CHECK_NULL(pSync, "pSync");                                         \
    SHMEMU_CHECK_SYMMETRIC(dest, sizeof(_type) * nreduce);                     \
    SHMEMU_CHECK_SYMMETRIC(source, sizeof(_type) * nreduce);                   \
    SHMEMU_CHECK_SYMMETRIC(pWrk,                                               \
                           sizeof(_type) * SHCOLL_REDUCE_MIN_WRKDATA_SIZE);    \
    SHMEMU_CHECK_SYMMETRIC(pSync, sizeof(long) * SHCOLL_REDUCE_SYNC_SIZE);     \
    SHMEMU_CHECK_BUFFER_OVERLAP(dest, source, sizeof(_type) * nreduce,         \
                                sizeof(_type) * nreduce);                      \
    /* dispatch into the helper routine */                                     \
    reduce_helper_##_typename_op##_##_algo(                                    \
        dest, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync);  \
  }

/*
 * @brief Improved wrapper macros for each reduction algorithm
 */
#define TO_ALL_WRAPPER(_name, _type, _op, _algo)                               \
  SHCOLL_TO_ALL_DEFINITION(_name, _type, _algo)

/* Define specific wrapper macros for each algorithm/operation combination */
#define TO_ALL_WRAPPER_AND_linear(_type, _typename)                            \
  TO_ALL_WRAPPER(_typename##_and, _type, AND_OP, linear)
#define TO_ALL_WRAPPER_OR_linear(_type, _typename)                             \
  TO_ALL_WRAPPER(_typename##_or, _type, OR_OP, linear)
#define TO_ALL_WRAPPER_XOR_linear(_type, _typename)                            \
  TO_ALL_WRAPPER(_typename##_xor, _type, XOR_OP, linear)
#define TO_ALL_WRAPPER_MAX_linear(_type, _typename)                            \
  TO_ALL_WRAPPER(_typename##_max, _type, MAX_OP, linear)
#define TO_ALL_WRAPPER_MIN_linear(_type, _typename)                            \
  TO_ALL_WRAPPER(_typename##_min, _type, MIN_OP, linear)
#define TO_ALL_WRAPPER_SUM_linear(_type, _typename)                            \
  TO_ALL_WRAPPER(_typename##_sum, _type, SUM_OP, linear)
#define TO_ALL_WRAPPER_PROD_linear(_type, _typename)                           \
  TO_ALL_WRAPPER(_typename##_prod, _type, PROD_OP, linear)

#define TO_ALL_WRAPPER_AND_binomial(_type, _typename)                          \
  TO_ALL_WRAPPER(_typename##_and, _type, AND_OP, binomial)
#define TO_ALL_WRAPPER_OR_binomial(_type, _typename)                           \
  TO_ALL_WRAPPER(_typename##_or, _type, OR_OP, binomial)
#define TO_ALL_WRAPPER_XOR_binomial(_type, _typename)                          \
  TO_ALL_WRAPPER(_typename##_xor, _type, XOR_OP, binomial)
#define TO_ALL_WRAPPER_MAX_binomial(_type, _typename)                          \
  TO_ALL_WRAPPER(_typename##_max, _type, MAX_OP, binomial)
#define TO_ALL_WRAPPER_MIN_binomial(_type, _typename)                          \
  TO_ALL_WRAPPER(_typename##_min, _type, MIN_OP, binomial)
#define TO_ALL_WRAPPER_SUM_binomial(_type, _typename)                          \
  TO_ALL_WRAPPER(_typename##_sum, _type, SUM_OP, binomial)
#define TO_ALL_WRAPPER_PROD_binomial(_type, _typename)                         \
  TO_ALL_WRAPPER(_typename##_prod, _type, PROD_OP, binomial)

#define TO_ALL_WRAPPER_AND_rec_dbl(_type, _typename)                           \
  TO_ALL_WRAPPER(_typename##_and, _type, AND_OP, rec_dbl)
#define TO_ALL_WRAPPER_OR_rec_dbl(_type, _typename)                            \
  TO_ALL_WRAPPER(_typename##_or, _type, OR_OP, rec_dbl)
#define TO_ALL_WRAPPER_XOR_rec_dbl(_type, _typename)                           \
  TO_ALL_WRAPPER(_typename##_xor, _type, XOR_OP, rec_dbl)
#define TO_ALL_WRAPPER_MAX_rec_dbl(_type, _typename)                           \
  TO_ALL_WRAPPER(_typename##_max, _type, MAX_OP, rec_dbl)
#define TO_ALL_WRAPPER_MIN_rec_dbl(_type, _typename)                           \
  TO_ALL_WRAPPER(_typename##_min, _type, MIN_OP, rec_dbl)
#define TO_ALL_WRAPPER_SUM_rec_dbl(_type, _typename)                           \
  TO_ALL_WRAPPER(_typename##_sum, _type, SUM_OP, rec_dbl)
#define TO_ALL_WRAPPER_PROD_rec_dbl(_type, _typename)                          \
  TO_ALL_WRAPPER(_typename##_prod, _type, PROD_OP, rec_dbl)

#define TO_ALL_WRAPPER_AND_rabenseifner(_type, _typename)                      \
  TO_ALL_WRAPPER(_typename##_and, _type, AND_OP, rabenseifner)
#define TO_ALL_WRAPPER_OR_rabenseifner(_type, _typename)                       \
  TO_ALL_WRAPPER(_typename##_or, _type, OR_OP, rabenseifner)
#define TO_ALL_WRAPPER_XOR_rabenseifner(_type, _typename)                      \
  TO_ALL_WRAPPER(_typename##_xor, _type, XOR_OP, rabenseifner)
#define TO_ALL_WRAPPER_MAX_rabenseifner(_type, _typename)                      \
  TO_ALL_WRAPPER(_typename##_max, _type, MAX_OP, rabenseifner)
#define TO_ALL_WRAPPER_MIN_rabenseifner(_type, _typename)                      \
  TO_ALL_WRAPPER(_typename##_min, _type, MIN_OP, rabenseifner)
#define TO_ALL_WRAPPER_SUM_rabenseifner(_type, _typename)                      \
  TO_ALL_WRAPPER(_typename##_sum, _type, SUM_OP, rabenseifner)
#define TO_ALL_WRAPPER_PROD_rabenseifner(_type, _typename)                     \
  TO_ALL_WRAPPER(_typename##_prod, _type, PROD_OP, rabenseifner)

#define TO_ALL_WRAPPER_AND_rabenseifner2(_type, _typename)                     \
  TO_ALL_WRAPPER(_typename##_and, _type, AND_OP, rabenseifner2)
#define TO_ALL_WRAPPER_OR_rabenseifner2(_type, _typename)                      \
  TO_ALL_WRAPPER(_typename##_or, _type, OR_OP, rabenseifner2)
#define TO_ALL_WRAPPER_XOR_rabenseifner2(_type, _typename)                     \
  TO_ALL_WRAPPER(_typename##_xor, _type, XOR_OP, rabenseifner2)
#define TO_ALL_WRAPPER_MAX_rabenseifner2(_type, _typename)                     \
  TO_ALL_WRAPPER(_typename##_max, _type, MAX_OP, rabenseifner2)
#define TO_ALL_WRAPPER_MIN_rabenseifner2(_type, _typename)                     \
  TO_ALL_WRAPPER(_typename##_min, _type, MIN_OP, rabenseifner2)
#define TO_ALL_WRAPPER_SUM_rabenseifner2(_type, _typename)                     \
  TO_ALL_WRAPPER(_typename##_sum, _type, SUM_OP, rabenseifner2)
#define TO_ALL_WRAPPER_PROD_rabenseifner2(_type, _typename)                    \
  TO_ALL_WRAPPER(_typename##_prod, _type, PROD_OP, rabenseifner2)

/* Group by operation type using TO_ALL type tables for wrappers (only generate
 * for supported types) */
#define TO_ALL_WRAPPER_BITWISE(_algo)                                          \
  SHMEM_TO_ALL_BITWISE_TYPE_TABLE(TO_ALL_WRAPPER_AND_##_algo)                  \
  SHMEM_TO_ALL_BITWISE_TYPE_TABLE(TO_ALL_WRAPPER_OR_##_algo)                   \
  SHMEM_TO_ALL_BITWISE_TYPE_TABLE(TO_ALL_WRAPPER_XOR_##_algo)

#define TO_ALL_WRAPPER_MINMAX(_algo)                                           \
  SHMEM_TO_ALL_MINMAX_TYPE_TABLE(TO_ALL_WRAPPER_MAX_##_algo)                   \
  SHMEM_TO_ALL_MINMAX_TYPE_TABLE(TO_ALL_WRAPPER_MIN_##_algo)

#define TO_ALL_WRAPPER_ARITH(_algo)                                            \
  SHMEM_TO_ALL_ARITH_TYPE_TABLE(TO_ALL_WRAPPER_SUM_##_algo)                    \
  SHMEM_TO_ALL_ARITH_TYPE_TABLE(TO_ALL_WRAPPER_PROD_##_algo)

/* Combine all operation types into one macro */
#define TO_ALL_WRAPPER_ALL(_algo)                                              \
  TO_ALL_WRAPPER_BITWISE(_algo)                                                \
  TO_ALL_WRAPPER_MINMAX(_algo)                                                 \
  TO_ALL_WRAPPER_ARITH(_algo)

/* generate wrappers for all types and ops for each algorithm */
TO_ALL_WRAPPER_ALL(linear)
TO_ALL_WRAPPER_ALL(binomial)
TO_ALL_WRAPPER_ALL(rec_dbl)
TO_ALL_WRAPPER_ALL(rabenseifner)
TO_ALL_WRAPPER_ALL(rabenseifner2)

/*
 * @brief Macro to define team-based reduction operations
 *
 * @param _typename Type name (e.g. int_sum)
 * @param _type Actual type (e.g. int)
 * @param _op Operation (e.g. sum)
 * @param _algo Algorithm name (e.g. linear)
 *
 *
 * FIXME: branch to check that pwrk is valid should only be done in debug mode
 */
#define SHCOLL_REDUCE_DEFINITION(_typename, _type, _op, _algo)                 \
  int shcoll_##_typename##_##_op##_reduce_##_algo(                             \
      shmem_team_t team, _type *dest, const _type *source, size_t nreduce) {   \
    SHMEMU_CHECK_INIT();                                                       \
    SHMEMU_CHECK_TEAM_VALID(team);                                             \
    SHMEMU_CHECK_SYMMETRIC(dest, "dest");                                      \
    SHMEMU_CHECK_SYMMETRIC(source, "source");                                  \
    shmemc_team_h team_h = (shmemc_team_h)team;                                \
    SHMEMU_CHECK_TEAM_STRIDE(team_h->stride, __func__);                        \
    SHMEMU_CHECK_NULL(shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE),      \
                      "team_h->pSyncs[COLLECTIVE]");                               \
                                                                               \
    _type *pWrk =                                                              \
        shmem_malloc(SHCOLL_REDUCE_MIN_WRKDATA_SIZE * sizeof(_type));          \
                                                                               \
    reduce_helper_##_typename##_##_op##_##_algo(                               \
        dest, source, nreduce, team_h->start,                                  \
        (team_h->stride > 0) ? (int)log2((double)team_h->stride) : 0,          \
        team_h->nranks, pWrk,                                                  \
        shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE));                   \
                                                                               \
    shmemc_team_reset_psync(team_h, SHMEMC_PSYNC_COLLECTIVE);                      \
    shmem_free(pWrk);                                                          \
    return 0;                                                                  \
  }

#define SHIM_REDUCE_DECLARE(_typename, _type, _op, _algo)                      \
  SHCOLL_REDUCE_DEFINITION(_typename, _type, _op, _algo)

/*
 * @brief Macros to define reduction operations for different data types
 */

/* Bitwise reduction operations (AND, OR, XOR) */
#define SHIM_REDUCE_BITWISE_TYPES(_op, _algo)                                  \
  SHMEM_REDUCE_BITWISE_TYPE_TABLE(DECLARE_BITWISE_REDUCE_TYPE_##_op##_##_algo)

/* Min/Max reduction operations */
#define SHIM_REDUCE_MINMAX_TYPES(_op, _algo)                                   \
  SHMEM_REDUCE_MINMAX_TYPE_TABLE(DECLARE_MINMAX_REDUCE_TYPE_##_op##_##_algo)

/* Arithmetic reduction operations (SUM, PROD) */
#define SHIM_REDUCE_ARITH_TYPES(_op, _algo)                                    \
  SHMEM_REDUCE_ARITH_TYPE_TABLE(DECLARE_ARITH_REDUCE_TYPE_##_op##_##_algo)

/* Define specific type declaration macros for each operation/algorithm
 * combination */
#define DECLARE_BITWISE_REDUCE_TYPE_and_linear(_type, _typename)               \
  SHIM_REDUCE_DECLARE(_typename, _type, and, linear)
#define DECLARE_BITWISE_REDUCE_TYPE_or_linear(_type, _typename)                \
  SHIM_REDUCE_DECLARE(_typename, _type, or, linear)
#define DECLARE_BITWISE_REDUCE_TYPE_xor_linear(_type, _typename)               \
  SHIM_REDUCE_DECLARE(_typename, _type, xor, linear)

#define DECLARE_BITWISE_REDUCE_TYPE_and_binomial(_type, _typename)             \
  SHIM_REDUCE_DECLARE(_typename, _type, and, binomial)
#define DECLARE_BITWISE_REDUCE_TYPE_or_binomial(_type, _typename)              \
  SHIM_REDUCE_DECLARE(_typename, _type, or, binomial)
#define DECLARE_BITWISE_REDUCE_TYPE_xor_binomial(_type, _typename)             \
  SHIM_REDUCE_DECLARE(_typename, _type, xor, binomial)

#define DECLARE_BITWISE_REDUCE_TYPE_and_rec_dbl(_type, _typename)              \
  SHIM_REDUCE_DECLARE(_typename, _type, and, rec_dbl)
#define DECLARE_BITWISE_REDUCE_TYPE_or_rec_dbl(_type, _typename)               \
  SHIM_REDUCE_DECLARE(_typename, _type, or, rec_dbl)
#define DECLARE_BITWISE_REDUCE_TYPE_xor_rec_dbl(_type, _typename)              \
  SHIM_REDUCE_DECLARE(_typename, _type, xor, rec_dbl)

#define DECLARE_BITWISE_REDUCE_TYPE_and_rabenseifner(_type, _typename)         \
  SHIM_REDUCE_DECLARE(_typename, _type, and, rabenseifner)
#define DECLARE_BITWISE_REDUCE_TYPE_or_rabenseifner(_type, _typename)          \
  SHIM_REDUCE_DECLARE(_typename, _type, or, rabenseifner)
#define DECLARE_BITWISE_REDUCE_TYPE_xor_rabenseifner(_type, _typename)         \
  SHIM_REDUCE_DECLARE(_typename, _type, xor, rabenseifner)

#define DECLARE_BITWISE_REDUCE_TYPE_and_rabenseifner2(_type, _typename)        \
  SHIM_REDUCE_DECLARE(_typename, _type, and, rabenseifner2)
#define DECLARE_BITWISE_REDUCE_TYPE_or_rabenseifner2(_type, _typename)         \
  SHIM_REDUCE_DECLARE(_typename, _type, or, rabenseifner2)
#define DECLARE_BITWISE_REDUCE_TYPE_xor_rabenseifner2(_type, _typename)        \
  SHIM_REDUCE_DECLARE(_typename, _type, xor, rabenseifner2)

#define DECLARE_MINMAX_REDUCE_TYPE_min_linear(_type, _typename)                \
  SHIM_REDUCE_DECLARE(_typename, _type, min, linear)
#define DECLARE_MINMAX_REDUCE_TYPE_max_linear(_type, _typename)                \
  SHIM_REDUCE_DECLARE(_typename, _type, max, linear)

#define DECLARE_MINMAX_REDUCE_TYPE_min_binomial(_type, _typename)              \
  SHIM_REDUCE_DECLARE(_typename, _type, min, binomial)
#define DECLARE_MINMAX_REDUCE_TYPE_max_binomial(_type, _typename)              \
  SHIM_REDUCE_DECLARE(_typename, _type, max, binomial)

#define DECLARE_MINMAX_REDUCE_TYPE_min_rec_dbl(_type, _typename)               \
  SHIM_REDUCE_DECLARE(_typename, _type, min, rec_dbl)
#define DECLARE_MINMAX_REDUCE_TYPE_max_rec_dbl(_type, _typename)               \
  SHIM_REDUCE_DECLARE(_typename, _type, max, rec_dbl)

#define DECLARE_MINMAX_REDUCE_TYPE_min_rabenseifner(_type, _typename)          \
  SHIM_REDUCE_DECLARE(_typename, _type, min, rabenseifner)
#define DECLARE_MINMAX_REDUCE_TYPE_max_rabenseifner(_type, _typename)          \
  SHIM_REDUCE_DECLARE(_typename, _type, max, rabenseifner)

#define DECLARE_MINMAX_REDUCE_TYPE_min_rabenseifner2(_type, _typename)         \
  SHIM_REDUCE_DECLARE(_typename, _type, min, rabenseifner2)
#define DECLARE_MINMAX_REDUCE_TYPE_max_rabenseifner2(_type, _typename)         \
  SHIM_REDUCE_DECLARE(_typename, _type, max, rabenseifner2)

#define DECLARE_ARITH_REDUCE_TYPE_sum_linear(_type, _typename)                 \
  SHIM_REDUCE_DECLARE(_typename, _type, sum, linear)
#define DECLARE_ARITH_REDUCE_TYPE_prod_linear(_type, _typename)                \
  SHIM_REDUCE_DECLARE(_typename, _type, prod, linear)

#define DECLARE_ARITH_REDUCE_TYPE_sum_binomial(_type, _typename)               \
  SHIM_REDUCE_DECLARE(_typename, _type, sum, binomial)
#define DECLARE_ARITH_REDUCE_TYPE_prod_binomial(_type, _typename)              \
  SHIM_REDUCE_DECLARE(_typename, _type, prod, binomial)

#define DECLARE_ARITH_REDUCE_TYPE_sum_rec_dbl(_type, _typename)                \
  SHIM_REDUCE_DECLARE(_typename, _type, sum, rec_dbl)
#define DECLARE_ARITH_REDUCE_TYPE_prod_rec_dbl(_type, _typename)               \
  SHIM_REDUCE_DECLARE(_typename, _type, prod, rec_dbl)

#define DECLARE_ARITH_REDUCE_TYPE_sum_rabenseifner(_type, _typename)           \
  SHIM_REDUCE_DECLARE(_typename, _type, sum, rabenseifner)
#define DECLARE_ARITH_REDUCE_TYPE_prod_rabenseifner(_type, _typename)          \
  SHIM_REDUCE_DECLARE(_typename, _type, prod, rabenseifner)

#define DECLARE_ARITH_REDUCE_TYPE_sum_rabenseifner2(_type, _typename)          \
  SHIM_REDUCE_DECLARE(_typename, _type, sum, rabenseifner2)
#define DECLARE_ARITH_REDUCE_TYPE_prod_rabenseifner2(_type, _typename)         \
  SHIM_REDUCE_DECLARE(_typename, _type, prod, rabenseifner2)

/*
 * @brief Grouping macros for each algorithm
 */
#define SHIM_REDUCE_BITWISE_ALL(_algo)                                         \
  SHIM_REDUCE_BITWISE_TYPES(or, _algo)                                         \
  SHIM_REDUCE_BITWISE_TYPES(xor, _algo)                                        \
  SHIM_REDUCE_BITWISE_TYPES(and, _algo)

#define SHIM_REDUCE_MINMAX_ALL(_algo)                                          \
  SHIM_REDUCE_MINMAX_TYPES(min, _algo)                                         \
  SHIM_REDUCE_MINMAX_TYPES(max, _algo)

#define SHIM_REDUCE_ARITH_ALL(_algo)                                           \
  SHIM_REDUCE_ARITH_TYPES(sum, _algo)                                          \
  SHIM_REDUCE_ARITH_TYPES(prod, _algo)

#define SHIM_REDUCE_ALL(_algo)                                                 \
  SHIM_REDUCE_BITWISE_ALL(_algo)                                               \
  SHIM_REDUCE_MINMAX_ALL(_algo)                                                \
  SHIM_REDUCE_ARITH_ALL(_algo)

/* Instantiate all shmem_*_reduce wrappers */
SHIM_REDUCE_ALL(linear)
SHIM_REDUCE_ALL(binomial)
SHIM_REDUCE_ALL(rec_dbl)
SHIM_REDUCE_ALL(rabenseifner)
SHIM_REDUCE_ALL(rabenseifner2)
