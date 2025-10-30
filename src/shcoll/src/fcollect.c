/**
 * @file fcollect.c
 * @brief Implementation of various fcollect algorithms for OpenSHMEM
 * collectives
 * @author Srdan Milakovic
 * @author Michael Beebe
 * @date Created on 5/17/18, edited on 1/4/25
 */

#include "shcoll.h"
#include "shcoll/compat.h"
#include "../tests/util/debug.h"
#include "util/rotate.h"

#include <limits.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <shmem/api_types.h>

/**
 * Helper function implementing linear fcollect algorithm
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array of size >= 2
 */
inline static void fcollect_helper_linear(void *dest, const void *source,
                                          size_t nbytes, int PE_start,
                                          int logPE_stride, int PE_size,
                                          long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;

  shcoll_barrier_linear(PE_start, logPE_stride, PE_size, pSync);
  if (me != PE_start) {
    shmem_putmem_nbi((char *)dest + me_as * nbytes, source, nbytes, PE_start);
  } else {
    memcpy(dest, source, nbytes);
  }
  shcoll_barrier_linear(PE_start, logPE_stride, PE_size, pSync);

  shcoll_broadcast8_linear(dest, dest, nbytes * shmem_n_pes(), PE_start,
                           PE_start, logPE_stride, PE_size, pSync + 1);
}

/**
 * Helper function implementing all-to-all linear fcollect algorithm
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array of size >= 1
 */
inline static void fcollect_helper_all_linear(void *dest, const void *source,
                                              size_t nbytes, int PE_start,
                                              int logPE_stride, int PE_size,
                                              long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;

  int i;
  int target;

  for (i = 1; i < PE_size; i++) {
    target = PE_start + ((i + me_as) % PE_size) * stride;
    shmem_putmem_nbi((char *)dest + me_as * nbytes, source, nbytes, target);
  }

  memcpy((char *)dest + me_as * nbytes, source, nbytes);

  shmem_fence();

  for (i = 1; i < PE_size; i++) {
    target = PE_start + ((i + me_as) % PE_size) * stride;
    shmem_long_atomic_inc(pSync, target);
  }

  shmem_long_wait_until(pSync, SHMEM_CMP_EQ, SHCOLL_SYNC_VALUE + PE_size - 1);
  shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);
}

/**
 * Helper function implementing all-to-all linear fcollect algorithm variant 1
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array of size >= 1
 */
inline static void fcollect_helper_all_linear1(void *dest, const void *source,
                                               size_t nbytes, int PE_start,
                                               int logPE_stride, int PE_size,
                                               long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;

  int i;
  int target;

  for (i = 1; i < PE_size; i++) {
    target = PE_start + ((i + me_as) % PE_size) * stride;
    shmem_putmem_nbi((char *)dest + me_as * nbytes, source, nbytes, target);
  }

  memcpy((char *)dest + me_as * nbytes, source, nbytes);

  shcoll_barrier_binomial_tree(PE_start, logPE_stride, PE_size, pSync);
}

/**
 * Helper function implementing recursive doubling fcollect algorithm
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array of size >= ⌈log(max_rank)⌉
 */
inline static void fcollect_helper_rec_dbl(void *dest, const void *source,
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
  int data_block = me_as;

  assert(((PE_size - 1) & PE_size) == 0);

  memcpy((char *)dest + me_as * nbytes, source, nbytes);

  for (mask = 0x1, i = 0; mask < PE_size; mask <<= 1, i++) {
    peer = PE_start + (me_as ^ mask) * stride;

    shmem_putmem_nbi((char *)dest + data_block * nbytes,
                     (char *)dest + data_block * nbytes, nbytes * mask, peer);
    shmem_fence();
    shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE + 1, peer);

    data_block &= ~mask;

    shmem_long_wait_until(pSync + i, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);
    shmem_long_p(pSync + i, SHCOLL_SYNC_VALUE, me);
  }
}

/**
 * Helper function implementing ring-based fcollect algorithm
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array of size >= 1
 */
inline static void fcollect_helper_ring(void *dest, const void *source,
                                        size_t nbytes, int PE_start,
                                        int logPE_stride, int PE_size,
                                        long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;
  int peer = PE_start + ((me_as + 1) % PE_size) * stride;
  int data_block = me_as;
  int i;

  memcpy((char *)dest + data_block * nbytes, source, nbytes);

  for (i = 1; i < PE_size; i++) {
    shmem_putmem_nbi((char *)dest + data_block * nbytes,
                     (char *)dest + data_block * nbytes, nbytes, peer);
    shmem_fence();
    shmem_long_atomic_inc(pSync, peer);

    data_block = (data_block - 1 + PE_size) % PE_size;
    shmem_long_wait_until(pSync, SHMEM_CMP_GE, SHCOLL_SYNC_VALUE + i);
  }

  shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);
}

/**
 * Helper function implementing Bruck's fcollect algorithm
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array of size >= ⌈log(max_rank)⌉
 */
inline static void fcollect_helper_bruck(void *dest, const void *source,
                                         size_t nbytes, int PE_start,
                                         int logPE_stride, int PE_size,
                                         long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;
  size_t distance;
  int round;
  int peer;
  size_t sent_bytes = nbytes;
  size_t total_nbytes = PE_size * nbytes;
  size_t to_send;

  memcpy(dest, source, nbytes);

  for (distance = 1, round = 0; distance < PE_size; distance <<= 1, round++) {
    peer = (int)(PE_start + ((me_as - distance + PE_size) % PE_size) * stride);
    to_send = (2 * sent_bytes <= total_nbytes) ? sent_bytes
                                               : total_nbytes - sent_bytes;

    shmem_putmem_nbi((char *)dest + sent_bytes, dest, to_send, peer);
    shmem_fence();
    shmem_long_p(pSync + round, SHCOLL_SYNC_VALUE + 1, peer);

    sent_bytes += distance * nbytes;
    shmem_long_wait_until(pSync + round, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);
    shmem_long_p(pSync + round, SHCOLL_SYNC_VALUE, me);
  }

  rotate(dest, total_nbytes, me_as * nbytes);
}

/**
 * Helper function implementing Bruck's fcollect algorithm without final
 * rotation
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array of size >= ⌈log(max_rank)⌉
 */
inline static void fcollect_helper_bruck_no_rotate(void *dest,
                                                   const void *source,
                                                   size_t nbytes, int PE_start,
                                                   int logPE_stride,
                                                   int PE_size, long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;
  size_t distance;
  int round;
  int peer;
  size_t sent_bytes = nbytes;
  size_t total_nbytes = PE_size * nbytes;
  size_t to_send;

  size_t my_offset_nbytes = nbytes * me_as;
  char *my_offset = (char *)dest + my_offset_nbytes;

  memcpy(my_offset, source, nbytes);

  for (distance = 1, round = 0; distance < PE_size; distance <<= 1, round++) {
    peer = (int)(PE_start + ((me_as - distance + PE_size) % PE_size) * stride);
    to_send = (2 * sent_bytes <= total_nbytes) ? sent_bytes
                                               : total_nbytes - sent_bytes;

    if (my_offset_nbytes + to_send <= total_nbytes) {
      shmem_putmem_nbi(my_offset, my_offset, to_send, peer);
    } else {
      shmem_putmem_nbi(my_offset, my_offset, total_nbytes - my_offset_nbytes,
                       peer);
      shmem_putmem_nbi(dest, dest, to_send - (total_nbytes - my_offset_nbytes),
                       peer);
    }

    shmem_fence();
    shmem_long_p(pSync + round, SHCOLL_SYNC_VALUE + 1, peer);

    sent_bytes += distance * nbytes;
    shmem_long_wait_until(pSync + round, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);
    shmem_long_p(pSync + round, SHCOLL_SYNC_VALUE, me);
  }
}

/**
 * Helper function implementing Bruck's fcollect algorithm with signal
 * operations
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array of size >= ⌈log(max_rank)⌉
 */
inline static void fcollect_helper_bruck_signal(void *dest, const void *source,
                                                size_t nbytes, int PE_start,
                                                int logPE_stride, int PE_size,
                                                long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;
  size_t distance;
  int round;
  int peer;
  size_t sent_bytes = nbytes;
  size_t total_nbytes = PE_size * nbytes;
  size_t to_send;

  memcpy(dest, source, nbytes);

  for (distance = 1, round = 0; distance < PE_size; distance <<= 1, round++) {
    peer = (int)(PE_start + ((me_as - distance + PE_size) % PE_size) * stride);
    to_send = (2 * sent_bytes <= total_nbytes) ? sent_bytes
                                               : total_nbytes - sent_bytes;

    shmem_putmem_signal_nb((char *)dest + sent_bytes, dest, to_send,
                           (uint64_t *)(pSync + round), SHCOLL_SYNC_VALUE + 1,
                           peer, NULL);

    sent_bytes += distance * nbytes;
    shmem_long_wait_until(pSync + round, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);
    shmem_long_p(pSync + round, SHCOLL_SYNC_VALUE, me);
  }

  rotate(dest, total_nbytes, me_as * nbytes);
}

/**
 * Helper function implementing in-place Bruck's fcollect algorithm
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array of size >= ⌈log(max_rank)⌉
 */
inline static void fcollect_helper_bruck_inplace(void *dest, const void *source,
                                                 size_t nbytes, int PE_start,
                                                 int logPE_stride, int PE_size,
                                                 long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;
  size_t distance;
  int round;
  int peer;
  size_t sent_bytes = nbytes;
  size_t total_nbytes = PE_size * nbytes;
  size_t to_send;

  memcpy(dest, source, nbytes);

  for (distance = 1, round = 0; distance < PE_size; distance <<= 1, round++) {
    peer = (int)(PE_start + ((me_as - distance + PE_size) % PE_size) * stride);
    to_send = (2 * sent_bytes <= total_nbytes) ? sent_bytes
                                               : total_nbytes - sent_bytes;

    shmem_putmem_nbi((char *)dest + sent_bytes, dest, to_send, peer);
    shmem_fence();
    shmem_long_p(pSync + round, SHCOLL_SYNC_VALUE + 1, peer);

    sent_bytes += distance * nbytes;
    shmem_long_wait_until(pSync + round, SHMEM_CMP_NE, SHCOLL_SYNC_VALUE);
    shmem_long_p(pSync + round, SHCOLL_SYNC_VALUE, me);
  }

  rotate_inplace(dest, total_nbytes, me_as * nbytes);
}

/**
 * Helper function implementing neighbor exchange fcollect algorithm
 *
 * @param dest Destination buffer on all PEs
 * @param source Source buffer containing local data
 * @param nbytes Number of bytes to collect from each PE
 * @param PE_start First PE in the active set
 * @param logPE_stride Log (base 2) of stride between consecutive PEs
 * @param PE_size Number of PEs in the active set
 * @param pSync Symmetric work array of size >= 2
 */
inline static void
fcollect_helper_neighbor_exchange(void *dest, const void *source, size_t nbytes,
                                  int PE_start, int logPE_stride, int PE_size,
                                  long *pSync) {
  assert(PE_size % 2 == 0);

  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();

  int neighbor_pe[2];
  int send_offset[2];
  int send_offset_diff;

  int i, parity;
  void *data;

  /* Get my index in the active set */
  int me_as = (me - PE_start) / stride;

  if (me_as % 2 == 0) {
    neighbor_pe[0] = PE_start + ((me_as + 1) % PE_size) * stride;
    neighbor_pe[1] = PE_start + ((me_as - 1 + PE_size) % PE_size) * stride;

    send_offset[0] = (me_as - 2 + PE_size) % PE_size & ~0x1;
    send_offset[1] = me_as & ~0x1;

    send_offset_diff = 2;
  } else {
    neighbor_pe[0] = PE_start + ((me_as - 1 + PE_size) % PE_size) * stride;
    neighbor_pe[1] = PE_start + ((me_as + 1) % PE_size) * stride;

    send_offset[0] = (me_as + 2) % PE_size & ~0x1;
    send_offset[1] = me_as & ~0x1;

    send_offset_diff = -2 + PE_size;
  }

  /* First round */
  data = (char *)dest + me_as * nbytes;

  memcpy(data, source, nbytes);

  shmem_putmem_nbi(data, data, nbytes, neighbor_pe[0]);
  shmem_fence();
  shmem_long_atomic_inc(pSync, neighbor_pe[0]);

  shmem_long_wait_until(pSync, SHMEM_CMP_GE, 1);

  /* Remaining npes/2 - 1 rounds */
  for (i = 1; i < PE_size / 2; i++) {
    parity = (i % 2) ? 1 : 0;
    data = ((char *)dest) + send_offset[parity] * nbytes;

    /* Send data */
    shmem_putmem_nbi(data, data, 2 * nbytes, neighbor_pe[parity]);
    shmem_fence();
    shmem_long_atomic_inc(pSync + parity, neighbor_pe[parity]);

    /* Calculate offset for the next round */
    send_offset[parity] = (send_offset[parity] + send_offset_diff) % PE_size;
    send_offset_diff = PE_size - send_offset_diff;

    /* Wait for the data from the neighbor */
    shmem_long_wait_until(pSync + parity, SHMEM_CMP_GT, i / 2);
  }

  pSync[0] = SHCOLL_SYNC_VALUE;
  pSync[1] = SHCOLL_SYNC_VALUE;
}

/**
 * Macro to define fcollect functions for different data sizes
 *
 * @param _algo Algorithm name
 * @param _size Data size in bits
 */
#define SHCOLL_FCOLLECT_SIZE_DEFINITION(_algo, _size)                          \
  void shcoll_fcollect##_size##_##_algo(                                       \
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
    /* Perform fcollect */                                                     \
    fcollect_helper_##_algo(dest, source, (_size) / CHAR_BIT * nelems,         \
                            PE_start, logPE_stride, PE_size, pSync);           \
  }

/* @formatter:off */

SHCOLL_FCOLLECT_SIZE_DEFINITION(linear, 32)
SHCOLL_FCOLLECT_SIZE_DEFINITION(linear, 64)

SHCOLL_FCOLLECT_SIZE_DEFINITION(all_linear, 32)
SHCOLL_FCOLLECT_SIZE_DEFINITION(all_linear, 64)

SHCOLL_FCOLLECT_SIZE_DEFINITION(all_linear1, 32)
SHCOLL_FCOLLECT_SIZE_DEFINITION(all_linear1, 64)

SHCOLL_FCOLLECT_SIZE_DEFINITION(rec_dbl, 32)
SHCOLL_FCOLLECT_SIZE_DEFINITION(rec_dbl, 64)

SHCOLL_FCOLLECT_SIZE_DEFINITION(ring, 32)
SHCOLL_FCOLLECT_SIZE_DEFINITION(ring, 64)

SHCOLL_FCOLLECT_SIZE_DEFINITION(bruck, 32)
SHCOLL_FCOLLECT_SIZE_DEFINITION(bruck, 64)

SHCOLL_FCOLLECT_SIZE_DEFINITION(bruck_no_rotate, 32)
SHCOLL_FCOLLECT_SIZE_DEFINITION(bruck_no_rotate, 64)

SHCOLL_FCOLLECT_SIZE_DEFINITION(bruck_signal, 32)
SHCOLL_FCOLLECT_SIZE_DEFINITION(bruck_signal, 64)

SHCOLL_FCOLLECT_SIZE_DEFINITION(bruck_inplace, 32)
SHCOLL_FCOLLECT_SIZE_DEFINITION(bruck_inplace, 64)

SHCOLL_FCOLLECT_SIZE_DEFINITION(neighbor_exchange, 32)
SHCOLL_FCOLLECT_SIZE_DEFINITION(neighbor_exchange, 64)

/* @formatter:on */

/**
 * Macro to define fcollect functions for different data types
 *
 * @param _algo Algorithm name
 * @param type Data type
 * @param _typename Type name string
 */
#define SHCOLL_FCOLLECT_TYPE_DEFINITION(_algo, type, _typename)                \
  int shcoll_##_typename##_fcollect_##_algo(                                   \
      shmem_team_t team, type *dest, const type *source, size_t nelems) {      \
    /* Sanity Checks */                                                        \
    SHMEMU_CHECK_INIT();                                                       \
    SHMEMU_CHECK_TEAM_VALID(team);                                             \
    SHMEMU_CHECK_NULL(dest, "dest");                                           \
    SHMEMU_CHECK_NULL(source, "source");                                       \
    shmemc_team_h team_h = (shmemc_team_h)team;                                \
    SHMEMU_CHECK_TEAM_STRIDE(team_h->stride, __func__);                        \
    SHMEMU_CHECK_SYMMETRIC(dest, sizeof(type) * nelems * team_h->nranks);      \
    SHMEMU_CHECK_SYMMETRIC(source, sizeof(type) * nelems);                     \
    SHMEMU_CHECK_BUFFER_OVERLAP(dest, source,                                  \
                                sizeof(type) * nelems * team_h->nranks,        \
                                sizeof(type) * nelems);                        \
                                                                               \
    SHMEMU_CHECK_NULL(shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE),     \
                      "team_h->pSyncs[COLLECTIVE]");                              \
                                                                               \
    /* FIXME: WE DO NOT WANT THIS SYNC TO BE HERE */                           \
    shmem_team_sync(team_h);                                                   \
                                                                               \
    fcollect_helper_##_algo(                                                   \
        dest, source, sizeof(type) * nelems, team_h->start,                    \
        (team_h->stride > 0) ? (int)log2((double)team_h->stride) : 0,          \
        team_h->nranks, shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE));  \
                                                                               \
    shmemc_team_reset_psync(team_h, SHMEMC_PSYNC_COLLECTIVE);                     \
                                                                               \
    return 0;                                                                  \
  }

/* @formatter:off */

/**
 * Macro to define fcollect functions for all supported data types
 *
 * @param _algo Algorithm name
 */
#define DEFINE_FCOLLECT_TYPES(_type, _typename)                                \
  SHCOLL_FCOLLECT_TYPE_DEFINITION(linear, _type, _typename)                    \
  SHCOLL_FCOLLECT_TYPE_DEFINITION(all_linear, _type, _typename)                \
  SHCOLL_FCOLLECT_TYPE_DEFINITION(all_linear1, _type, _typename)               \
  SHCOLL_FCOLLECT_TYPE_DEFINITION(rec_dbl, _type, _typename)                   \
  SHCOLL_FCOLLECT_TYPE_DEFINITION(ring, _type, _typename)                      \
  SHCOLL_FCOLLECT_TYPE_DEFINITION(bruck, _type, _typename)                     \
  SHCOLL_FCOLLECT_TYPE_DEFINITION(bruck_no_rotate, _type, _typename)           \
  SHCOLL_FCOLLECT_TYPE_DEFINITION(bruck_signal, _type, _typename)              \
  SHCOLL_FCOLLECT_TYPE_DEFINITION(bruck_inplace, _type, _typename)             \
  SHCOLL_FCOLLECT_TYPE_DEFINITION(neighbor_exchange, _type, _typename)

SHMEM_STANDARD_RMA_TYPE_TABLE(DEFINE_FCOLLECT_TYPES)
#undef DEFINE_FCOLLECT_TYPES

/* @formatter:on */

/**
 * @brief Macro to declare fcollectmem implementations for different algorithms
 *
 */
#define SHCOLL_FCOLLECTMEM_DEFINITION(_algo)                                   \
  int shcoll_fcollectmem_##_algo(shmem_team_t team, void *dest,                \
                                 const void *source, size_t nelems) {          \
    /* Sanity Checks */                                                        \
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
    SHMEMU_CHECK_NULL(shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE),     \
                      "team_h->pSyncs[COLLECTIVE]");                              \
                                                                               \
    /* FIXME: WE DO NOT WANT THIS SYNC TO BE HERE */                           \
    shmem_team_sync(team_h);                                                   \
                                                                               \
    fcollect_helper_##_algo(                                                   \
        dest, source, nelems, team_h->start,                                   \
        (team_h->stride > 0) ? (int)log2((double)team_h->stride) : 0,          \
        team_h->nranks, shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE));  \
                                                                               \
    shmemc_team_reset_psync(team_h, SHMEMC_PSYNC_COLLECTIVE);                     \
                                                                               \
    return 0;                                                                  \
  }

SHCOLL_FCOLLECTMEM_DEFINITION(linear)
SHCOLL_FCOLLECTMEM_DEFINITION(all_linear)
SHCOLL_FCOLLECTMEM_DEFINITION(all_linear1)
SHCOLL_FCOLLECTMEM_DEFINITION(rec_dbl)
SHCOLL_FCOLLECTMEM_DEFINITION(ring)
SHCOLL_FCOLLECTMEM_DEFINITION(bruck)
SHCOLL_FCOLLECTMEM_DEFINITION(bruck_no_rotate)
SHCOLL_FCOLLECTMEM_DEFINITION(bruck_signal)
SHCOLL_FCOLLECTMEM_DEFINITION(bruck_inplace)
SHCOLL_FCOLLECTMEM_DEFINITION(neighbor_exchange)
