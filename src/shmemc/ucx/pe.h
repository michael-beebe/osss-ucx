/**
 * @file pe.h
 * @brief Header file containing UCX-specific PE and context management
 * structures
 * @license See LICENSE file at top-level
 */

#ifndef _UCP_PE_H
#define _UCP_PE_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "boolean.h"
#include "threading.h"
#include "shmem/teams.h"
#include "../../klib/khash.h" /* TODO */
#include "shmem/defs.h"

#include <sys/types.h>
#include <ucp/api/ucp.h>

/**
 * @brief Information exchanged between PEs at startup
 */
typedef struct worker_info {
  ucp_address_t *addr; /**< worker address */
  char *buf;           /**< allocated to copy remote worker */
  size_t len;          /**< size of worker */
} worker_info_t;

/**
 * @brief Encapsulates remote key data needed for memory region access
 */
typedef struct mem_opaque_rkey {
  void *data;
} mem_opaque_rkey_t;

/**
 * @brief Collection of remote keys for memory region access across PEs
 */
typedef struct mem_opaque {
  mem_opaque_rkey_t *rkeys; /* per PE */
} mem_opaque_t;

/**
 * @brief Memory access information for a single PE
 */
typedef struct mem_access {
  ucp_rkey_h rkey; /* remote key for this heap */
} mem_access_t;

/**
 * @brief Collection of memory access information across all PEs
 */
typedef struct mem_region_access {
  mem_access_t *rinfo; /* nranks remote access info */
} mem_region_access_t;

/**
 * @brief Information about a memory region/heap on a PE
 */
typedef struct mem_info {
  size_t id;     /* number of this heap */
  uint64_t base; /* start of this heap */
  uint64_t end;  /* end of this heap */
  size_t len;    /* its size (b) */
  ucp_mem_h mh;  /* memory handle */
} mem_info_t;

/**
 * @brief Collection of memory region information for PE exchange
 */
typedef struct mem_region {
  mem_info_t *minfo; /**< nranks mem info */
} mem_region_t;

/**
 * @brief Internal OpenSHMEM context management handle
 * @note There is a difference between UCX context and OpenSHMEM context
 */
typedef struct shmemc_context *shmemc_context_h;

/**
 * @brief Handle for team management
 */
typedef struct shmemc_team *shmemc_team_h;

KHASH_MAP_INIT_INT(map, int)

/**
 * @brief Structure representing a team of PEs
 */
typedef struct shmemc_team {
  const char *name; /**< if predef, who we are (else NULL) */

  /* Team geometry */
  int rank;   /* my rank in this team */
  int nranks; /* number of PEs in team */
  int start;  /* starting PE in the parent team's context (usually world) */
  int stride; /* stride between PEs in the parent team's context */

  /* handle -> attributes */
  shmem_team_config_t cfg;

  /* PE mapping */
  khash_t(map) * fwd; /**< Map: team rank -> global PE */
  khash_t(map) * rev; /**< Map: global PE -> team rank */

  shmemc_context_h *ctxts; /**< array of contexts in this team */
  size_t nctxts;           /**< how many contexts allocated */

  shmemc_team_h parent; /**< parent team we split from,
                           NULL if predef */

  /* now need to add pSync arrays for collectives */
#define SHMEMC_NUM_PSYNCS 2 /* For barrier/sync and other collectives */

  // clang-format off
/* Symbolic constants for pSync buffer indices */
#define SHMEMC_PSYNC_BARRIER    0  /* team sync/barrier operations */
#define SHMEMC_PSYNC_COLLECTIVE 1  /* for other collective operations */

/* Old symbolic constants for backward compatibility in switch statement */
#define SHMEMC_PSYNC_BROADCAST  2  /* broadcast operations */
#define SHMEMC_PSYNC_COLLECT    3  /* collect/fcollect operations */
#define SHMEMC_PSYNC_ALLTOALL   4  /* alltoall/alltoalls operations */
#define SHMEMC_PSYNC_REDUCE     5  /* reduction operations */
  // clang-format on

  long *pSyncs[SHMEMC_NUM_PSYNCS];
} shmemc_team_t;

/**
 * @brief Context attributes as defined in OpenSHMEM 1.4 spec, sec. 9.4.1
 */
typedef struct shmemc_context_attr {
  bool serialized;
  bool privat; /* "private" is c++ keyword */
  bool nostore;
} shmemc_context_attr_t;

/**
 * @brief Structure representing an OpenSHMEM context
 */
typedef struct shmemc_context {
  ucp_worker_h w;                     /* for separate context progress */
  ucp_ep_h *eps;                      /* endpoints */
  unsigned long id;                   /* internal tracking */
  threadwrap_thread_t creator_thread; /* thread ID that created me */
  /*
   * parsed options during creation (defaults: no)
   */
  shmemc_context_attr_t attr;

  mem_region_access_t *racc; /* for endpoint remote access */

  shmemc_team_h team; /* team we belong to */

  /*
   * possibly other things
   */
} shmemc_context_t;

/**
 * @brief Communication layer information structure
 */
typedef struct comms_info {
  ucp_context_h ucx_ctxt;        /* local communication context */
  ucp_config_t *ucx_cfg;         /* local config */
  worker_info_t *xchg_wrkr_info; /* nranks worker info exchanged */

#if 0
    shmemc_context_h *ctxts;    /**< PE's contexts */
    size_t nctxts;              /**< how many contexts */
#endif

  mem_region_t *regions; /**< exchanged symmetric regions */
  size_t nregions;       /**< how many regions */

  mem_opaque_t *orks; /* opaque rkeys (nregions * PEs) */
} comms_info_t;

/**
 * @brief Thread descriptor structure
 */
typedef struct thread_desc {
  ucs_thread_mode_t ucx_tl; /**< UCX thread level */
  int osh_tl;               /**< corresponding OpenSHMEM thread level */
  threadwrap_thread_t invoking_thread; /**< who called shmem_init*()? */
} thread_desc_t;

#endif /* ! _UCP_PE_H */
