/**
 * @file alltoalls.c
 * @brief Implementation of strided all-to-all collective operations
 *        (correct stride semantics and safety checks)
 * @author
 *   Original: Srdan Milakovic, Michael Beebe (5/21/18)
 *   Edits:    Michael Beebe (1/17/2025)
 *   Update:   8/20/2025 — fix elem_size usage, correct stride math, add guards
 */

#include "shcoll.h"
#include "shcoll/compat.h"
#include "shcoll/barrier.h"
#include <shmem/api_types.h>

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/* Edge-color helper for color exchange                                       */
/* -------------------------------------------------------------------------- */
inline static int edge_color(int i, int me, int npes) {
  int chr_idx = (npes % 2 == 1) ? npes : (npes - 1);
  int v;
  if (me < chr_idx) {
    v = (i + chr_idx - me) % chr_idx;
  } else {
    v = (i % 2 == 1) ? (((i + chr_idx) / 2) % chr_idx) : (i / 2);
  }
  if ((npes % 2 == 1) && (v == me))
    return -1;
  if (v == me)
    return chr_idx;
  return v;
}

/* -------------------------------------------------------------------------- */
/* Strided indexing (per spec)
 *
 * For PE i (k-th in team) sending to PE j (l-th in team):
 *   source offsets:  (l + t) * sst_stride * elem_size, t = 0..nelems-1
 *   dest   offsets:  (k + t) * dst_stride * elem_size, t = 0..nelems-1
 *
 * NOTE: block base is (index * nelems + t) * stride for element t of PE index.
 * Minimal bytes required for an array (per OpenSHMEM spec):
 *   bytes >= elem_size * dst_stride * nelems * team_size
 *   bytes >= elem_size * sst_stride * nelems * team_size
 * -------------------------------------------------------------------------- */

/* ======================= Helper kernels ======================= */

inline static void alltoalls_helper_shift_exchange_barrier(
    void *dest, const void *source, ptrdiff_t dst_stride, ptrdiff_t sst_stride,
    size_t elem_size, size_t nelems, int PE_start, int logPE_stride,
    int PE_size, long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;

  char *d = (char *)dest;
  const char *s = (const char *)source;

  /* Self-copy (k = me_as, l = me_as) */
  for (size_t t = 0; t < nelems; ++t) {
    size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
    size_t soff = (size_t)(me_as * nelems + t) * (size_t)sst_stride;
    memcpy(d + doff * elem_size, s + soff * elem_size, elem_size);
  }

  /* Exchange with others */
  for (int i = 1; i < PE_size; i++) {
    const int peer_as = (me_as + i) % PE_size;      /* l */
    const int target = PE_start + peer_as * stride; /* destination PE j */

    /* Send my block k to peer; read peer's l-th block from my source */
    for (size_t t = 0; t < nelems; ++t) {
      size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
      size_t soff = (size_t)(peer_as * nelems + t) * (size_t)sst_stride;

      shmem_putmem_nbi(d + doff * elem_size, s + soff * elem_size, elem_size,
                       target);
    }
  }

  /* Ensure completion of NBI puts before synchronizing */
  shmem_quiet();
  shcoll_barrier_binomial_tree(PE_start, logPE_stride, PE_size, pSync);
}

inline static void alltoalls_helper_shift_exchange_counter(
    void *dest, const void *source, ptrdiff_t dst_stride, ptrdiff_t sst_stride,
    size_t elem_size, size_t nelems, int PE_start, int logPE_stride,
    int PE_size, long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;

  char *d = (char *)dest;
  const char *s = (const char *)source;

  /* Exchange with others first */
  for (int i = 1; i < PE_size; i++) {
    const int peer_as = (me_as + i) % PE_size;      /* l */
    const int target = PE_start + peer_as * stride; /* destination PE j */
    for (size_t t = 0; t < nelems; ++t) {
      size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
      size_t soff = (size_t)(peer_as * nelems + t) * (size_t)sst_stride;
      shmem_putmem_nbi(d + doff * elem_size, s + soff * elem_size, elem_size,
                       target);
    }
  }

  /* Self-copy */
  for (size_t t = 0; t < nelems; ++t) {
    size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
    size_t soff = (size_t)(me_as * nelems + t) * (size_t)sst_stride;
    memcpy(d + doff * elem_size, s + soff * elem_size, elem_size);
  }

  /* All my puts complete locally before I signal */
  shmem_quiet();

  /* Signal completion to peers using atomic increments */
  for (int i = 1; i < PE_size; i++) {
    const int peer_as = (me_as + i) % PE_size;
    shmem_long_atomic_inc(pSync, PE_start + peer_as * stride);
  }

  /* Wait for all peers' signals, then reset my pSync */
  shmem_long_wait_until(pSync, SHMEM_CMP_EQ, SHCOLL_SYNC_VALUE + PE_size - 1);
  shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);
}

inline static void alltoalls_helper_xor_pairwise_exchange_barrier(
    void *dest, const void *source, ptrdiff_t dst_stride, ptrdiff_t sst_stride,
    size_t elem_size, size_t nelems, int PE_start, int logPE_stride,
    int PE_size, long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;

  char *d = (char *)dest;
  const char *s = (const char *)source;

  /* power-of-two team size */
  assert(((unsigned)PE_size & (unsigned)(PE_size - 1)) == 0);

  /* Self-copy */
  for (size_t t = 0; t < nelems; ++t) {
    size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
    size_t soff = (size_t)(me_as * nelems + t) * (size_t)sst_stride;
    memcpy(d + doff * elem_size, s + soff * elem_size, elem_size);
  }

  /* XOR partner rounds */
  for (int i = 1; i < PE_size; i++) {
    const int peer_as = i ^ me_as;                  /* l */
    const int target = PE_start + peer_as * stride; /* destination PE j */
    for (size_t t = 0; t < nelems; ++t) {
      size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
      size_t soff = (size_t)(peer_as * nelems + t) * (size_t)sst_stride;
      shmem_putmem_nbi(d + doff * elem_size, s + soff * elem_size, elem_size,
                       target);
    }
  }

  shmem_quiet();
  shcoll_barrier_binomial_tree(PE_start, logPE_stride, PE_size, pSync);
}

inline static void alltoalls_helper_xor_pairwise_exchange_counter(
    void *dest, const void *source, ptrdiff_t dst_stride, ptrdiff_t sst_stride,
    size_t elem_size, size_t nelems, int PE_start, int logPE_stride,
    int PE_size, long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;

  char *d = (char *)dest;
  const char *s = (const char *)source;

  assert(((unsigned)PE_size & (unsigned)(PE_size - 1)) == 0);

  /* XOR partner rounds */
  for (int i = 1; i < PE_size; i++) {
    const int peer_as = i ^ me_as;                  /* l */
    const int target = PE_start + peer_as * stride; /* destination PE j */
    for (size_t t = 0; t < nelems; ++t) {
      size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
      size_t soff = (size_t)(peer_as * nelems + t) * (size_t)sst_stride;
      shmem_putmem_nbi(d + doff * elem_size, s + soff * elem_size, elem_size,
                       target);
    }
  }

  /* Self-copy */
  for (size_t t = 0; t < nelems; ++t) {
    size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
    size_t soff = (size_t)(me_as * nelems + t) * (size_t)sst_stride;
    memcpy(d + doff * elem_size, s + soff * elem_size, elem_size);
  }

  shmem_quiet();

  for (int i = 1; i < PE_size; i++) {
    const int peer_as = i ^ me_as;
    shmem_long_atomic_inc(pSync, PE_start + peer_as * stride);
  }

  shmem_long_wait_until(pSync, SHMEM_CMP_EQ, SHCOLL_SYNC_VALUE + PE_size - 1);
  shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);
}

inline static void alltoalls_helper_color_pairwise_exchange_barrier(
    void *dest, const void *source, ptrdiff_t dst_stride, ptrdiff_t sst_stride,
    size_t elem_size, size_t nelems, int PE_start, int logPE_stride,
    int PE_size, long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;

  char *d = (char *)dest;
  const char *s = (const char *)source;

  assert((PE_size % 2) == 0);

  /* Self-copy */
  for (size_t t = 0; t < nelems; ++t) {
    size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
    size_t soff = (size_t)(me_as * nelems + t) * (size_t)sst_stride;
    memcpy(d + doff * elem_size, s + soff * elem_size, elem_size);
  }

  for (int i = 1; i < PE_size; i++) {
    const int peer_as = edge_color(i, me_as, PE_size); /* l */
    if (peer_as >= 0 && peer_as < PE_size) {
      const int target = PE_start + peer_as * stride; /* destination PE j */
      for (size_t t = 0; t < nelems; ++t) {
        size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
        size_t soff = (size_t)(peer_as * nelems + t) * (size_t)sst_stride;
        shmem_putmem_nbi(d + doff * elem_size, s + soff * elem_size, elem_size,
                         target);
      }
    }
  }

  shmem_quiet();
  shcoll_barrier_binomial_tree(PE_start, logPE_stride, PE_size, pSync);
}

inline static void alltoalls_helper_color_pairwise_exchange_counter(
    void *dest, const void *source, ptrdiff_t dst_stride, ptrdiff_t sst_stride,
    size_t elem_size, size_t nelems, int PE_start, int logPE_stride,
    int PE_size, long *pSync) {
  const int stride = 1 << logPE_stride;
  const int me = shmem_my_pe();
  const int me_as = (me - PE_start) / stride;

  char *d = (char *)dest;
  const char *s = (const char *)source;

  assert((PE_size % 2) == 0);

  for (int i = 1; i < PE_size; i++) {
    const int peer_as = edge_color(i, me_as, PE_size); /* l */
    if (peer_as >= 0 && peer_as < PE_size) {
      const int target = PE_start + peer_as * stride; /* destination PE j */
      for (size_t t = 0; t < nelems; ++t) {
        size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
        size_t soff = (size_t)(peer_as * nelems + t) * (size_t)sst_stride;
        shmem_putmem_nbi(d + doff * elem_size, s + soff * elem_size, elem_size,
                         target);
      }
    }
  }

  /* Self-copy */
  for (size_t t = 0; t < nelems; ++t) {
    size_t doff = (size_t)(me_as * nelems + t) * (size_t)dst_stride;
    size_t soff = (size_t)(me_as * nelems + t) * (size_t)sst_stride;
    memcpy(d + doff * elem_size, s + soff * elem_size, elem_size);
  }

  shmem_quiet();

  for (int i = 1; i < PE_size; i++) {
    const int peer_as = edge_color(i, me_as, PE_size);
    if (peer_as >= 0 && peer_as < PE_size) {
      shmem_long_atomic_inc(pSync, PE_start + peer_as * stride);
    }
  }

  shmem_long_wait_until(pSync, SHMEM_CMP_EQ, SHCOLL_SYNC_VALUE + PE_size - 1);
  shmem_long_p(pSync, SHCOLL_SYNC_VALUE, me);
}

/* ======================= Front-ends (size) ======================= */
/* Element size is _size bits; _esz is bytes/element. Required bytes include
 * stride and (team_size + nelems - 1) per spec indexing.
 */
#define SHCOLL_ALLTOALLS_SIZE_DEFINITION(_algo, _size)                         \
  void shcoll_alltoalls##_size##_##_algo(                                      \
      void *dest, const void *source, ptrdiff_t dst_stride,                    \
      ptrdiff_t sst_stride, size_t nelems, int PE_start, int logPE_stride,     \
      int PE_size, long *pSync) {                                              \
    SHMEMU_CHECK_INIT();                                                       \
    SHMEMU_CHECK_POSITIVE(PE_size, "PE_size");                                 \
    SHMEMU_CHECK_NON_NEGATIVE(PE_start, "PE_start");                           \
    SHMEMU_CHECK_NON_NEGATIVE(logPE_stride, "logPE_stride");                   \
    SHMEMU_CHECK_ACTIVE_SET_RANGE(PE_start, logPE_stride, PE_size);            \
    SHMEMU_CHECK_NULL(dest, "dest");                                           \
    SHMEMU_CHECK_NULL(source, "source");                                       \
    SHMEMU_CHECK_NULL(pSync, "pSync");                                         \
    SHMEMU_CHECK_POSITIVE(dst_stride, "dst");                                  \
    SHMEMU_CHECK_POSITIVE(sst_stride, "sst");                                  \
    SHMEMU_CHECK_POSITIVE(nelems, "nelems");                                   \
    size_t _esz = (_size) / (CHAR_BIT);                                        \
    size_t need_dst = _esz * (size_t)dst_stride * nelems * (size_t)PE_size;    \
    size_t need_src = _esz * (size_t)sst_stride * nelems * (size_t)PE_size;    \
    SHMEMU_CHECK_SYMMETRIC(dest, need_dst);                                    \
    SHMEMU_CHECK_SYMMETRIC(source, need_src);                                  \
    SHMEMU_CHECK_SYMMETRIC(pSync, sizeof(long) * SHCOLL_ALLTOALL_SYNC_SIZE);   \
    SHMEMU_CHECK_BUFFER_OVERLAP(dest, source, need_dst, need_src);             \
    alltoalls_helper_##_algo(dest, source, dst_stride, sst_stride, _esz,       \
                             nelems, PE_start, logPE_stride, PE_size, pSync);  \
  }

/* 32/64-bit size variants (used by the library where appropriate) */
SHCOLL_ALLTOALLS_SIZE_DEFINITION(shift_exchange_barrier, 32)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(shift_exchange_barrier, 64)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(shift_exchange_counter, 32)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(shift_exchange_counter, 64)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(xor_pairwise_exchange_barrier, 32)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(xor_pairwise_exchange_barrier, 64)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(xor_pairwise_exchange_counter, 32)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(xor_pairwise_exchange_counter, 64)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(color_pairwise_exchange_barrier, 32)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(color_pairwise_exchange_barrier, 64)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(color_pairwise_exchange_counter, 32)
SHCOLL_ALLTOALLS_SIZE_DEFINITION(color_pairwise_exchange_counter, 64)

/* ======================= Front-ends (typed) ======================= */

#define SHCOLL_ALLTOALLS_TYPE_DEFINITION(_algo, _type, _typename)              \
  int shcoll_##_typename##_alltoalls_##_algo(                                  \
      shmem_team_t team, _type *dest, const _type *source, ptrdiff_t dst,      \
      ptrdiff_t sst, size_t nelems) {                                          \
    SHMEMU_CHECK_INIT();                                                       \
    SHMEMU_CHECK_TEAM_VALID(team);                                             \
    SHMEMU_CHECK_NULL(dest, "dest");                                           \
    SHMEMU_CHECK_NULL(source, "source");                                       \
    SHMEMU_CHECK_POSITIVE(dst, "dst");                                         \
    SHMEMU_CHECK_POSITIVE(sst, "sst");                                         \
    SHMEMU_CHECK_POSITIVE(nelems, "nelems");                                   \
    shmemc_team_h team_h = (shmemc_team_h)team;                                \
    SHMEMU_CHECK_TEAM_STRIDE(team_h->stride, __func__);                        \
    size_t need_dst =                                                          \
        sizeof(_type) * (size_t)dst * nelems * (size_t)team_h->nranks;         \
    size_t need_src =                                                          \
        sizeof(_type) * (size_t)sst * nelems * (size_t)team_h->nranks;         \
    SHMEMU_CHECK_SYMMETRIC(dest, need_dst);                                    \
    SHMEMU_CHECK_SYMMETRIC(source, need_src);                                  \
    SHMEMU_CHECK_BUFFER_OVERLAP(dest, source, need_dst, need_src);             \
    long *ps = shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE);         \
    SHMEMU_CHECK_NULL(ps, "team_h->pSyncs[COLLECTIVE]");                       \
                                                                               \
    alltoalls_helper_##_algo(                                                  \
        dest, source, dst, sst, sizeof(_type), nelems, team_h->start,          \
        (team_h->stride > 0) ? (int)log2((double)team_h->stride) : 0,          \
        team_h->nranks, ps);                                                   \
                                                                               \
    shmemc_team_reset_psync(team_h, SHMEMC_PSYNC_COLLECTIVE);                  \
    return 0;                                                                  \
  }

#define DEFINE_ALLTOALLS_TYPES(_type, _typename)                               \
  SHCOLL_ALLTOALLS_TYPE_DEFINITION(shift_exchange_barrier, _type, _typename)   \
  SHCOLL_ALLTOALLS_TYPE_DEFINITION(shift_exchange_counter, _type, _typename)   \
  SHCOLL_ALLTOALLS_TYPE_DEFINITION(xor_pairwise_exchange_barrier, _type,       \
                                   _typename)                                  \
  SHCOLL_ALLTOALLS_TYPE_DEFINITION(xor_pairwise_exchange_counter, _type,       \
                                   _typename)                                  \
  SHCOLL_ALLTOALLS_TYPE_DEFINITION(color_pairwise_exchange_barrier, _type,     \
                                   _typename)                                  \
  SHCOLL_ALLTOALLS_TYPE_DEFINITION(color_pairwise_exchange_counter, _type,     \
                                   _typename)

SHMEM_STANDARD_RMA_TYPE_TABLE(DEFINE_ALLTOALLS_TYPES)
#undef DEFINE_ALLTOALLS_TYPES

/* ======================= alltoallsmem() front-ends ======================= */
/* API: shmem_alltoallsmem(team, dest, source, dst, sst, elem_size_bytes)
 * We move ONE element per PE (nelems == 1) whose size is elem_size_bytes.
 */
#define SHCOLL_ALLTOALLSMEM_DEFINITION(_algo)                                  \
  int shcoll_alltoallsmem_##_algo(shmem_team_t team, void *dest,               \
                                  const void *source, ptrdiff_t dst,           \
                                  ptrdiff_t sst, size_t elem_size) {           \
    SHMEMU_CHECK_INIT();                                                       \
    SHMEMU_CHECK_TEAM_VALID(team);                                             \
    SHMEMU_CHECK_NULL(dest, "dest");                                           \
    SHMEMU_CHECK_NULL(source, "source");                                       \
    SHMEMU_CHECK_POSITIVE(dst, "dst");                                         \
    SHMEMU_CHECK_POSITIVE(sst, "sst");                                         \
    SHMEMU_CHECK_POSITIVE(elem_size, "elem_size");                             \
    shmemc_team_h team_h = (shmemc_team_h)team;                                \
    SHMEMU_CHECK_TEAM_STRIDE(team_h->stride, __func__);                        \
    /* Minimal sizes per spec indexing with nelems=1 */                        \
    size_t need_dst =                                                          \
        (size_t)elem_size * (size_t)dst * 1 * (size_t)team_h->nranks;          \
    size_t need_src =                                                          \
        (size_t)elem_size * (size_t)sst * 1 * (size_t)team_h->nranks;          \
    SHMEMU_CHECK_SYMMETRIC(dest, need_dst);                                    \
    SHMEMU_CHECK_SYMMETRIC(source, need_src);                                  \
    SHMEMU_CHECK_BUFFER_OVERLAP(dest, source, need_dst, need_src);             \
    long *ps = shmemc_team_get_psync(team_h, SHMEMC_PSYNC_COLLECTIVE);         \
    SHMEMU_CHECK_NULL(ps, "team_h->pSyncs[COLLECTIVE]");                       \
                                                                               \
    alltoalls_helper_##_algo(                                                  \
        dest, source, dst, sst, 1, elem_size, team_h->start,                   \
        (team_h->stride > 0) ? (int)log2((double)team_h->stride) : 0,          \
        team_h->nranks, ps);                                                   \
                                                                               \
    shmemc_team_reset_psync(team_h, SHMEMC_PSYNC_COLLECTIVE);                  \
    return 0;                                                                  \
  }

SHCOLL_ALLTOALLSMEM_DEFINITION(shift_exchange_barrier)
SHCOLL_ALLTOALLSMEM_DEFINITION(shift_exchange_counter)
SHCOLL_ALLTOALLSMEM_DEFINITION(xor_pairwise_exchange_barrier)
SHCOLL_ALLTOALLSMEM_DEFINITION(xor_pairwise_exchange_counter)
SHCOLL_ALLTOALLSMEM_DEFINITION(color_pairwise_exchange_barrier)
SHCOLL_ALLTOALLSMEM_DEFINITION(color_pairwise_exchange_counter)
