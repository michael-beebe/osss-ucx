/**
 * @file collect.c
 * @brief Implementation of collective communication routines for OpenSHMEM
 * @author Srdan Milakovic
 * @date 5/15/18
 *
 * This file contains implementations of various collect algorithms for
 * OpenSHMEM, including linear, recursive doubling, ring, and Bruck's algorithm
 * variants.
 */

#include "shcoll.h"
#include "shcoll/compat.h"
#include "util/rotate.h"
#include "util/scan.h"
#include "util/broadcast-size.h"
#include <shmem/api_types.h>

#include <string.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

/**
 * @brief Simple collect helper that uses prefix sum and memcpy
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
inline static void collect_helper_simple(void *dest, const void *source,
                                         size_t nbytes, int PE_start,
                                         int logPE_stride, int PE_size,
                                         long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  size_t block_offset;

  exclusive_prefix_sum(&block_offset, nbytes, PE_start, logPE_stride, PE_size,
                       pSync);

  // Copy local data
  memcpy((char *)dest + block_offset, source, nbytes);

  // Barrier to ensure all PEs have copied their local data
  shcoll_barrier_binomial_tree(PE_start, logPE_stride, PE_size, pSync);
}

/**
 * @brief Linear collect helper that uses PE 0 as coordinator
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
inline static void collect_helper_linear(void *dest, const void *source,
                                         size_t nbytes, int PE_start,
                                         int logPE_stride, int PE_size,
                                         long *pSync) {
  /* pSync[0] is used for barrier
   * pSync[1] is used for broadcast
   * next sizeof(size_t) bytes are used for the offset */

  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;
  size_t *offset = (size_t *)(pSync + 2);
  int i;

  /* set offset to 0 */
  shmem_size_p(offset, 0, me);
  shcoll_barrier_linear(PE_start, logPE_stride, PE_size, pSync);

  if (me_as == 0) {
    shmem_size_atomic_add(offset, nbytes + 1, me + stride);
    memcpy(dest, source, nbytes);

    /* Wait for the full array size and notify everybody */
    shmem_size_wait_until(offset, SHMEM_CMP_NE, 0);

    /* Send offset to everybody */
    for (i = 1; i < PE_size; i++) {
      shmem_size_p(offset, *offset, PE_start + i * stride);
    }
  } else {
    shmem_size_wait_until(offset, SHMEM_CMP_NE, 0);

    /* Write data to PE 0 */
    shmem_putmem_nbi((char *)dest + *offset - 1, source, nbytes, PE_start);

    /* Send offset to the next PE, PE_start will contain full array size */
    shmem_size_atomic_add(offset, nbytes + *offset,
                          PE_start + ((me_as + 1) % PE_size) * stride);
  }

  /* Wait for all PEs to send the data to PE_start */
  shcoll_barrier_linear(PE_start, logPE_stride, PE_size, pSync);

  shcoll_broadcast8_linear(dest, dest, *offset - 1, PE_start, PE_start,
                           logPE_stride, PE_size, pSync + 1);

  shmem_size_p(offset, SHCOLL_SYNC_VALUE, me);
}

/**
 * @brief All-to-all linear collect helper
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
inline static void collect_helper_all_linear(void *dest, const void *source,
                                             size_t nbytes, int PE_start,
                                             int logPE_stride, int PE_size,
                                             long *pSync) {
  /* pSync[0] is used for counting received messages
   * pSync[1..1+PREFIX_SUM_SYNC_SIZE) is used for prefix sum
   * next sizeof(size_t) bytes are used for the offset */

  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;
  size_t block_offset;

  int i;
  int target;

  exclusive_prefix_sum(&block_offset, nbytes, PE_start, logPE_stride, PE_size,
                       pSync + 1);

  for (i = 1; i < PE_size; i++) {
    target = PE_start + ((i + me_as) % PE_size) * stride;
    shmem_putmem_nbi((char *)dest + block_offset, source, nbytes, target);
  }

  memcpy((char *)dest + block_offset, source, nbytes);

  shmem_fence();

  for (i = 1; i < PE_size; i++) {
    target = PE_start + ((i + me_as) % PE_size) * stride;
    shmem_long_atomic_inc(pSync, target);
  }

  shmem_long_wait_until(pSync, SHMEM_CMP_EQ, SHCOLL_SYNC_VALUE + PE_size - 1);
  shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);
}

/**
 * @brief All-to-all linear collect helper variant using binomial tree barrier
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
inline static void collect_helper_all_linear1(void *dest, const void *source,
                                              size_t nbytes, int PE_start,
                                              int logPE_stride, int PE_size,
                                              long *pSync) {
  /* pSync[0] is used for barrier
   * pSync[1..1+PREFIX_SUM_SYNC_SIZE) is used for prefix sum
   * next sizeof(size_t) bytes are used for the offset */

  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;
  size_t block_offset;

  int i;
  int target;

  exclusive_prefix_sum(&block_offset, nbytes, PE_start, logPE_stride, PE_size,
                       pSync + 1);

  for (i = 1; i < PE_size; i++) {
    target = PE_start + ((i + me_as) % PE_size) * stride;
    shmem_putmem_nbi((char *)dest + block_offset, source, nbytes, target);
  }

  memcpy((char *)dest + block_offset, source, nbytes);

  shcoll_barrier_binomial_tree(PE_start, logPE_stride, PE_size, pSync);
}

/**
 * @brief Recursive doubling collect helper
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
inline static void collect_helper_rec_dbl(void *dest, const void *source,
                                          size_t nbytes, int PE_start,
                                          int logPE_stride, int PE_size,
                                          long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;
  int mask;
  int peer;
  int i;
  size_t round_block_size;
  size_t block_offset;
  size_t block_size = nbytes;

  /* pSync */
  long *prefix_sum_pSync = pSync;
  size_t *block_sizes = (size_t *)(prefix_sum_pSync + PREFIX_SUM_SYNC_SIZE);

  assert(((PE_size - 1) & PE_size) == 0);

  exclusive_prefix_sum(&block_offset, nbytes, PE_start, logPE_stride, PE_size,
                       prefix_sum_pSync);

  memcpy((char *)dest + block_offset, source, nbytes);

  for (mask = 0x1, i = 0; mask < PE_size; mask <<= 1, i++) {
    peer = PE_start + (me_as ^ mask) * stride;

    shmem_putmem_nbi((char *)dest + block_offset, (char *)dest + block_offset,
                     block_size, peer);
    shmem_fence();
    shmem_size_p(block_sizes + i, block_size + 1 + SHCOLL_SYNC_VALUE, peer);

    shmem_size_wait_until(block_sizes + i, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);
    round_block_size = *(block_sizes + i) - 1;
    shmem_size_p(block_sizes + i, SHCOLL_SYNC_VALUE, me);

    if (me > peer) {
      block_offset -= round_block_size;
    }
    block_size += round_block_size;
  }
}

/**
 * @brief Recursive doubling collect helper using signal operations
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
inline static void collect_helper_rec_dbl_signal(void *dest, const void *source,
                                                 size_t nbytes, int PE_start,
                                                 int logPE_stride, int PE_size,
                                                 long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;
  int mask;
  int peer;
  int i;
  size_t round_block_size;
  size_t block_offset;
  size_t block_size = nbytes;

  /* pSync */
  long *prefix_sum_pSync = pSync;
  size_t *block_sizes = (size_t *)(prefix_sum_pSync + PREFIX_SUM_SYNC_SIZE);

  assert(((PE_size - 1) & PE_size) == 0);

  exclusive_prefix_sum(&block_offset, nbytes, PE_start, logPE_stride, PE_size,
                       prefix_sum_pSync);

  memcpy((char *)dest + block_offset, source, nbytes);

  for (mask = 0x1, i = 0; mask < PE_size; mask <<= 1, i++) {
    peer = PE_start + (me_as ^ mask) * stride;

    shmem_putmem_signal_nb((char *)dest + block_offset,
                           (char *)dest + block_offset, block_size,
                           (uint64_t *)(block_sizes + i),
                           block_size + 1 + SHCOLL_SYNC_VALUE, peer, NULL);

    shmem_size_wait_until(block_sizes + i, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);
    round_block_size = *(block_sizes + i) - 1 - SHCOLL_SYNC_VALUE;
    shmem_size_p(block_sizes + i, SHCOLL_SYNC_VALUE, me);

    if (me > peer) {
      block_offset -= round_block_size;
    }
    block_size += round_block_size;
  }
}

/* TODO Find a better way to choose this value */
#define RING_DIFF 10

/**
 * @brief Ring algorithm collect helper
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
inline static void collect_helper_ring(void *dest, const void *source,
                                       size_t nbytes, int PE_start,
                                       int logPE_stride, int PE_size,
                                       long *pSync) {
  /*
   * pSync[0] is to track the progress of the left PE
   * pSync[1..RING_DIFF] is used to receive block sizes
   * pSync[RING_DIFF..] is used for exclusive prefix sum
   */
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  int me_as = (me - PE_start) / stride;
  int recv_from_pe = PE_start + ((me_as + 1) % PE_size) * stride;
  int send_to_pe = PE_start + ((me_as - 1 + PE_size) % PE_size) * stride;

  int round;
  long *receiver_progress = pSync;
  size_t *block_sizes = (size_t *)(pSync + 1);
  size_t *block_size_round;
  size_t nbytes_round = nbytes;

  size_t block_offset;

  exclusive_prefix_sum(&block_offset, nbytes, PE_start, logPE_stride, PE_size,
                       pSync + 1 + RING_DIFF);

  memcpy(((char *)dest) + block_offset, source, nbytes_round);

  for (round = 0; round < PE_size - 1; round++) {

    shmem_putmem_nbi(((char *)dest) + block_offset,
                     ((char *)dest) + block_offset, nbytes_round, send_to_pe);
    shmem_fence();

    /* Wait until it's safe to use block_size buffer */
    shmem_long_wait_until(receiver_progress, SHMEM_CMP_GT,
                          round - RING_DIFF + SHCOLL_SYNC_VALUE);
    block_size_round = block_sizes + (round % RING_DIFF);

    // TODO: fix -> shmem_size_p(block_size_round, nbytes_round + 1 +
    // SHCOLL_SYNC_VALUE, send_to_pe);
    shmem_size_atomic_set(block_size_round,
                          nbytes_round + 1 + SHCOLL_SYNC_VALUE, send_to_pe);

    /* If writing block 0, reset offset to 0 */
    block_offset =
        (me_as + round + 1 == PE_size) ? 0 : block_offset + nbytes_round;

    /* Wait to receive the data in this round */
    shmem_size_wait_until(block_size_round, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);
    nbytes_round = *block_size_round - 1 - SHCOLL_SYNC_VALUE;

    /* Reset the block size from the current round */
    shmem_size_p(block_size_round, SHCOLL_SYNC_VALUE, me);
    shmem_size_wait_until(block_size_round, SHMEM_CMP_EQ, SHCOLL_SYNC_VALUE);

    /* Notify sender that one counter is freed */
    shmem_long_atomic_inc(receiver_progress, recv_from_pe);
  }

  /* Must be atomic fadd because there may be some PE that did not finish with
   * sends */
  shmem_long_atomic_add(receiver_progress, -round, me);
}

/**
 * @brief Bruck's algorithm collect helper
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
inline static void collect_helper_bruck(void *dest, const void *source,
                                        size_t nbytes, int PE_start,
                                        int logPE_stride, int PE_size,
                                        long *pSync) {
  /* pSync[0] is used for barrier
   * pSync[1] is used for broadcast
   * pSync[2..2+PREFIX_SUM_SYNC_SIZE) bytes are used for the prefix sum
   * pSync[2+PREFIX_SUM_SYNC_SIZE..2+PREFIX_SUM_SYNC_SIZE+32) bytes are used for
   * the Bruck's algorithm, block sizes */
  /* TODO change 32 with a constant */

  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;
  size_t distance;
  int round;
  int send_to;
  int recv_from;
  size_t recv_nbytes = nbytes;
  size_t round_nbytes;

  /* pSyncs */
  long *barrier_pSync = pSync;
  long *broadcast_pSync = barrier_pSync + 1;
  long *prefix_sum_pSync = (broadcast_pSync + 1);
  size_t *block_sizes = (size_t *)(prefix_sum_pSync + PREFIX_SUM_SYNC_SIZE);

  size_t block_offset;
  size_t total_nbytes;

  /* Calculate prefix sum */
  exclusive_prefix_sum(&block_offset, nbytes, PE_start, logPE_stride, PE_size,
                       prefix_sum_pSync);

  /* Broadcast the total size */
  if (me_as == PE_size - 1) {
    total_nbytes = block_offset + nbytes;
  }

  broadcast_size(&total_nbytes, PE_start + (PE_size - 1) * stride, PE_start,
                 logPE_stride, PE_size, broadcast_pSync);

  /* Copy the local block to the destination */
  memcpy(dest, source, nbytes);

  for (distance = 1, round = 0; distance < PE_size; distance <<= 1, round++) {
    send_to =
        (int)(PE_start + ((me_as - distance + PE_size) % PE_size) * stride);
    recv_from = (int)(PE_start + ((me_as + distance) % PE_size) * stride);

    /* Notify partner that the data is ready */
    shmem_size_atomic_set(block_sizes + round,
                          recv_nbytes + 1 + SHCOLL_SYNC_VALUE, send_to);

    /* Wait until the data is ready to be read */
    shmem_size_wait_until(block_sizes + round, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);
    round_nbytes = *(block_sizes + round) - 1 - SHCOLL_SYNC_VALUE;

    round_nbytes = recv_nbytes + round_nbytes < total_nbytes
                       ? round_nbytes
                       : total_nbytes - recv_nbytes;

    shmem_getmem(((char *)dest) + recv_nbytes, dest, round_nbytes, recv_from);
    recv_nbytes += round_nbytes;

    /* Reset the block size from the current round */
    shmem_size_p(block_sizes + round, SHCOLL_SYNC_VALUE, me);
    shmem_size_wait_until(block_sizes + round, SHMEM_CMP_EQ, SHCOLL_SYNC_VALUE);
  }

  shcoll_barrier_binomial_tree(PE_start, logPE_stride, PE_size, barrier_pSync);

  rotate(dest, total_nbytes, block_offset);
}

/**
 * @brief Bruck's algorithm collect helper without final rotation
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log2 of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array
 */
inline static void collect_helper_bruck_no_rotate(void *dest,
                                                  const void *source,
                                                  size_t nbytes, int PE_start,
                                                  int logPE_stride, int PE_size,
                                                  long *pSync) {
  /* pSync[0] is used for barrier
   * pSync[1] is used for broadcast
   * pSync[2..2+PREFIX_SUM_SYNC_SIZE) bytes are used for the prefix sum
   * pSync[2+PREFIX_SUM_SYNC_SIZE..2+PREFIX_SUM_SYNC_SIZE+32) bytes are used for
   * the Bruck's algorithm, block sizes */
  /* TODO change 32 with a constant */

  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;
  size_t distance;
  int round;
  int send_to;
  int recv_from;
  size_t recv_nbytes = nbytes;
  size_t round_nbytes;

  /* pSyncs */
  long *barrier_pSync = pSync;
  long *broadcast_pSync = barrier_pSync + 1;
  long *prefix_sum_pSync = (broadcast_pSync + 1);
  size_t *block_sizes = (size_t *)(prefix_sum_pSync + PREFIX_SUM_SYNC_SIZE);

  size_t block_offset;
  size_t total_nbytes;

  size_t next_block_start;

  /* Calculate prefix sum */
  exclusive_prefix_sum(&block_offset, nbytes, PE_start, logPE_stride, PE_size,
                       prefix_sum_pSync);

  /* Broadcast the total size */
  if (me_as == PE_size - 1) {
    total_nbytes = block_offset + nbytes;
  }

  broadcast_size(&total_nbytes, PE_start + (PE_size - 1) * stride, PE_start,
                 logPE_stride, PE_size, broadcast_pSync);

  /* Copy the local block to the destination */
  memcpy((char *)dest + block_offset, source, nbytes);

  for (distance = 1, round = 0; distance < PE_size; distance <<= 1, round++) {
    send_to =
        (int)(PE_start + ((me_as - distance + PE_size) % PE_size) * stride);
    recv_from = (int)(PE_start + ((me_as + distance) % PE_size) * stride);

    /* Notify partner that the data is ready */
    shmem_size_atomic_set(block_sizes + round,
                          recv_nbytes + 1 + SHCOLL_SYNC_VALUE, send_to);

    /* Wait until the data is ready to be read */
    shmem_size_wait_until(block_sizes + round, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);
    round_nbytes = *(block_sizes + round) - 1 - SHCOLL_SYNC_VALUE;

    round_nbytes = recv_nbytes + round_nbytes < total_nbytes
                       ? round_nbytes
                       : total_nbytes - recv_nbytes;

    next_block_start = block_offset + recv_nbytes < total_nbytes
                           ? block_offset + recv_nbytes
                           : block_offset + recv_nbytes - total_nbytes;

    if (next_block_start + round_nbytes <= total_nbytes) {
      shmem_getmem((char *)dest + next_block_start,
                   (char *)dest + next_block_start, round_nbytes, recv_from);
    } else {
      shmem_getmem_nbi((char *)dest + next_block_start,
                       (char *)dest + next_block_start,
                       total_nbytes - next_block_start, recv_from);

      shmem_getmem_nbi(dest, dest,
                       round_nbytes - (total_nbytes - next_block_start),
                       recv_from);

      shmem_quiet();
    }

    recv_nbytes += round_nbytes;

    /* Reset the block size from the current round */
    shmem_size_p(block_sizes + round, SHCOLL_SYNC_VALUE, me);
    shmem_size_wait_until(block_sizes + round, SHMEM_CMP_EQ, SHCOLL_SYNC_VALUE);
  }

  shcoll_barrier_binomial_tree(PE_start, logPE_stride, PE_size, barrier_pSync);
}

/**
 * @brief Macro to define collect functions for different data sizes
 */
#define SHCOLL_COLLECT_SIZE_DEFINITION(_algo, _size)                           \
  void shcoll_collect##_size##_##_algo(                                        \
      void *dest, const void *source, size_t nelems, int PE_start,             \
      int logPE_stride, int PE_size, long *pSync) {                            \
    /* Sanity checks */                                                        \
    SHMEMU_CHECK_INIT();                                                       \
    SHMEMU_CHECK_POSITIVE(PE_size, "PE_size");                                 \
    SHMEMU_CHECK_NON_NEGATIVE(PE_start, "PE_start");                           \
    SHMEMU_CHECK_NON_NEGATIVE(logPE_stride, "logPE_stride");                   \
    SHMEMU_CHECK_ACTIVE_SET_RANGE(PE_start, logPE_stride, PE_size);            \
    SHMEMU_CHECK_SYMMETRIC(dest, (_size) / (CHAR_BIT) * nelems * PE_size);     \
    SHMEMU_CHECK_SYMMETRIC(source, (_size) / (CHAR_BIT) * nelems);             \
    SHMEMU_CHECK_SYMMETRIC(pSync, sizeof(long) * SHCOLL_COLLECT_SYNC_SIZE);    \
    SHMEMU_CHECK_BUFFER_OVERLAP(dest, source,                                  \
                                (_size) / (CHAR_BIT) * nelems * PE_size,       \
                                (_size) / (CHAR_BIT) * nelems);                \
    /* Perform collect */                                                      \
    collect_helper_##_algo(dest, source, (_size) / CHAR_BIT * nelems,          \
                           PE_start, logPE_stride, PE_size, pSync);            \
  }

/* @formatter:off */

SHCOLL_COLLECT_SIZE_DEFINITION(linear, 32)
SHCOLL_COLLECT_SIZE_DEFINITION(linear, 64)

SHCOLL_COLLECT_SIZE_DEFINITION(all_linear, 32)
SHCOLL_COLLECT_SIZE_DEFINITION(all_linear, 64)

SHCOLL_COLLECT_SIZE_DEFINITION(all_linear1, 32)
SHCOLL_COLLECT_SIZE_DEFINITION(all_linear1, 64)

SHCOLL_COLLECT_SIZE_DEFINITION(rec_dbl, 32)
SHCOLL_COLLECT_SIZE_DEFINITION(rec_dbl, 64)

SHCOLL_COLLECT_SIZE_DEFINITION(rec_dbl_signal, 32)
SHCOLL_COLLECT_SIZE_DEFINITION(rec_dbl_signal, 64)

SHCOLL_COLLECT_SIZE_DEFINITION(ring, 32)
SHCOLL_COLLECT_SIZE_DEFINITION(ring, 64)

SHCOLL_COLLECT_SIZE_DEFINITION(bruck, 32)
SHCOLL_COLLECT_SIZE_DEFINITION(bruck, 64)

SHCOLL_COLLECT_SIZE_DEFINITION(bruck_no_rotate, 32)
SHCOLL_COLLECT_SIZE_DEFINITION(bruck_no_rotate, 64)

SHCOLL_COLLECT_SIZE_DEFINITION(simple, 32)
SHCOLL_COLLECT_SIZE_DEFINITION(simple, 64)

/* @formatter:on */

/**
 * @brief Macro to define collect functions for different data types
 */
#define SHCOLL_COLLECT_TYPE_DEFINITION(_algo, _type, _typename)                \
  int shcoll_##_typename##_collect_##_algo(                                    \
      shmem_team_t team, _type *dest, const _type *source, size_t nelems) {    \
    /* Sanity Checks */                                                        \
    SHMEMU_CHECK_INIT();                                                       \
    SHMEMU_CHECK_TEAM_VALID(team);                                             \
    SHMEMU_CHECK_NULL(dest, "dest");                                           \
    SHMEMU_CHECK_NULL(source, "source");                                       \
    shmemc_team_h team_h = (shmemc_team_h)team;                                \
    SHMEMU_CHECK_TEAM_STRIDE(team_h->stride, __func__);                        \
    SHMEMU_CHECK_SYMMETRIC(dest, sizeof(_type) * nelems * team_h->nranks);     \
    SHMEMU_CHECK_SYMMETRIC(source, sizeof(_type) * nelems);                    \
    SHMEMU_CHECK_BUFFER_OVERLAP(dest, source,                                  \
                                sizeof(_type) * nelems * team_h->nranks,       \
                                sizeof(_type) * nelems);                       \
    SHMEMU_CHECK_NULL(shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE),     \
                      "team_h->pSyncs[COLLECT]");                              \
                                                                               \
    /* FIXME: WE DO NOT WANT THIS SYNC TO BE HERE */                           \
    shmem_team_sync(team_h);                                                   \
                                                                               \
    collect_helper_##_algo(                                                    \
        dest, source, sizeof(_type) * nelems, /* total bytes per PE */         \
        team_h->start,                                                         \
        (team_h->stride > 0) ? (int)log2((double)team_h->stride) : 0,          \
        team_h->nranks, shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE));  \
                                                                               \
    shmemc_team_reset_psync(team_h, SHMEMC_PSYNC_COLLECTIVE);                     \
                                                                               \
    return 0;                                                                  \
  }

/* @formatter:off */

#define DEFINE_COLLECT_TYPES(_type, _typename)                                 \
  SHCOLL_COLLECT_TYPE_DEFINITION(linear, _type, _typename)                     \
  SHCOLL_COLLECT_TYPE_DEFINITION(all_linear, _type, _typename)                 \
  SHCOLL_COLLECT_TYPE_DEFINITION(all_linear1, _type, _typename)                \
  SHCOLL_COLLECT_TYPE_DEFINITION(rec_dbl, _type, _typename)                    \
  SHCOLL_COLLECT_TYPE_DEFINITION(rec_dbl_signal, _type, _typename)             \
  SHCOLL_COLLECT_TYPE_DEFINITION(ring, _type, _typename)                       \
  SHCOLL_COLLECT_TYPE_DEFINITION(bruck, _type, _typename)                      \
  SHCOLL_COLLECT_TYPE_DEFINITION(bruck_no_rotate, _type, _typename)            \
  SHCOLL_COLLECT_TYPE_DEFINITION(simple, _type, _typename)

SHMEM_STANDARD_RMA_TYPE_TABLE(DEFINE_COLLECT_TYPES)
#undef DEFINE_COLLECT_TYPES

/**
 * @brief Macro to declare collectmem implementations for different algorithms
 */
#define SHCOLL_COLLECTMEM_DEFINITION(_algo)                                    \
  int shcoll_collectmem_##_algo(shmem_team_t team, void *dest,                 \
                                const void *source, size_t nelems) {           \
    SHMEMU_CHECK_INIT();                                                       \
    SHMEMU_CHECK_TEAM_VALID(team);                                             \
    SHMEMU_CHECK_NULL(dest, "dest");                                           \
    SHMEMU_CHECK_NULL(source, "source");                                       \
    shmemc_team_h team_h = (shmemc_team_h)team;                                \
    SHMEMU_CHECK_TEAM_STRIDE(team_h->stride, __func__);                        \
    SHMEMU_CHECK_SYMMETRIC(dest, nelems * team_h->nranks);                     \
    SHMEMU_CHECK_SYMMETRIC(source, nelems);                                    \
    SHMEMU_CHECK_BUFFER_OVERLAP(dest, source, nelems * team_h->nranks,         \
                                nelems);                                       \
                                                                               \
    SHMEMU_CHECK_NULL(shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE),     \
                      "team_h->pSyncs[COLLECT]");                              \
                                                                               \
    /* FIXME: WE DO NOT WANT THIS SYNC TO BE HERE */                           \
    shmem_team_sync(team_h);                                                   \
                                                                               \
    collect_helper_##_algo(                                                    \
        dest, source, nelems, /* total bytes per PE */                         \
        team_h->start,                                                         \
        (team_h->stride > 0) ? (int)log2((double)team_h->stride) : 0,          \
        team_h->nranks, shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE));  \
                                                                               \
    shmemc_team_reset_psync(team_h, SHMEMC_PSYNC_COLLECTIVE);                     \
                                                                               \
    return 0;                                                                  \
  }

SHCOLL_COLLECTMEM_DEFINITION(linear)
SHCOLL_COLLECTMEM_DEFINITION(all_linear)
SHCOLL_COLLECTMEM_DEFINITION(all_linear1)
SHCOLL_COLLECTMEM_DEFINITION(rec_dbl)
SHCOLL_COLLECTMEM_DEFINITION(rec_dbl_signal)
SHCOLL_COLLECTMEM_DEFINITION(ring)
SHCOLL_COLLECTMEM_DEFINITION(bruck)
SHCOLL_COLLECTMEM_DEFINITION(bruck_no_rotate)
SHCOLL_COLLECTMEM_DEFINITION(simple)
