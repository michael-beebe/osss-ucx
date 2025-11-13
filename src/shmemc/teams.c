/**
 * @file teams.c
 * @brief Implementation of OpenSHMEM teams functionality
 * @license See LICENSE file at top-level
 *
 * This file implements the OpenSHMEM teams API, which allows grouping of PEs
 * into teams for collective operations and team-based communication. The
 * implementation includes:
 *
 * - Team creation and management functions
 * - Team translation and mapping between global and team-relative PE numbers
 * - Team synchronization primitives
 * - Support for team contexts and team-based communication
 * - Built-in teams like SHMEM_TEAM_WORLD and SHMEM_TEAM_SHARED
 *
 * The teams implementation uses hash tables to maintain mappings between
 * team-relative and global PE numbers, and supports hierarchical team
 * structures through parent-child relationships.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"
#include "shmem/teams.h"
#include "ucx/api.h"
#include "allocator/memalloc.h"
#include "module.h"

#include <stdlib.h>

/**
 * @brief Default teams that are always available
 * @{
 */
shmemc_team_t shmemc_team_world;  /**< World team containing all PEs */
shmemc_team_t shmemc_team_shared; /**< Team of PEs on same node */
/** @} */

/**
 * @brief Global team handle representing all PEs
 */
shmemc_team_h world = &shmemc_team_world;

/**
 * @brief Global team handle representing PEs on same node
 */
shmemc_team_h shared = &shmemc_team_shared;

/**
 * @brief Invalid team handle used to indicate errors
 */
shmemc_team_h invalid = NULL;

/**
 * @brief Destroy all contexts allocated to a team
 *
 * Iterates through all contexts associated with the team and tears them down,
 * freeing associated resources.
 *
 * @param th Team handle whose contexts should be destroyed
 */
static void shmemc_team_contexts_destroy(shmemc_team_h th) {
  size_t c;

  for (c = 0; c < th->nctxts; ++c) {
    shmemc_ucx_teardown_context(th->ctxts[c]);
  }
  free(th->ctxts);
}

/**
 * @brief Debug function to dump team info
 *
 * Prints detailed information about a team including:
 * - Forward and reverse PE mappings
 * - Team handle and name
 * - Global rank, team rank and size
 *
 * @param th Team handle to dump info for
 */
static void dump_team(shmemc_team_h th) {
  int key, val;

  printf("==========================================\n");

  kh_foreach(th->fwd, key, val, { printf("fwd: %d -> %d\n", key, val); });
  kh_foreach(th->rev, key, val, { printf("rev: %d -> %d\n", key, val); });
  printf("\n");

  printf("Team = %p (%s)\n", (void *)th, th->name);

  printf("  global rank = %d, mype = %4d, npes = %4d\n", proc.li.rank, th->rank,
         th->nranks);
  printf("------------------------------------------\n");
}

/**
 * @brief Initialize synchronization buffers for a team
 *
 * Allocates and initializes the pSync buffers used for team collective
 * operations. Each buffer is initialized to SHMEM_SYNC_VALUE.
 *
 * @param th Team handle to initialize buffers for
 */
static void initialize_psync_buffers(shmemc_team_h th) {
  unsigned nsync;

  /*
   * Use appropriate sync sizes for different collective operations:
   * pSyncs[0]: For team sync/barrier (SHMEM_BARRIER_SYNC_SIZE)
   * pSyncs[1]: For other collectives (SHMEM_REDUCE_SYNC_SIZE is the largest)
   */
  const size_t sync_sizes[SHMEMC_NUM_PSYNCS] = {
      SHMEM_BARRIER_SYNC_SIZE, /* pSyncs[0] for team sync/barrier */
      SHMEM_REDUCE_SYNC_SIZE   /* pSyncs[1] for other collectives */
  };

  for (nsync = 0; nsync < SHMEMC_NUM_PSYNCS; ++nsync) {
    unsigned i;

    const size_t nbytes = sync_sizes[nsync] * sizeof(*(th->pSyncs));
    th->pSyncs[nsync] = (long *)shmema_malloc(nbytes);

    shmemu_assert(th->pSyncs[nsync] != NULL,
                  MODULE ": can't allocate sync memory "
                         "#%u in %s team (%p)",
                  nsync, th->parent == NULL ? th->name : "created", th);

    for (i = 0; i < sync_sizes[nsync]; ++i) {
      th->pSyncs[nsync][i] = SHMEM_SYNC_VALUE;
    }
  }
}

/**
 * @brief Reset a team's pSync buffer to SHMEM_SYNC_VALUE
 *
 * Resets all elements of the specified pSync buffer to SHMEM_SYNC_VALUE.
 * This should be called after using a pSync buffer for a collective operation
 * to ensure it's ready for reuse.
 *
 * @param th Team handle
 * @param psync_idx Index of the pSync buffer to reset (0-4)
 * @return 0 on success, -1 on failure
 */
int shmemc_team_reset_psync(shmemc_team_h th, unsigned psync_idx) {
  /* Validate parameters */
  if (th == NULL) {
    shmemu_warn("shmemc_team_reset_psync: Invalid team handle (NULL)");
    return -1;
  }

  if (psync_idx >= SHMEMC_NUM_PSYNCS) {
    shmemu_warn("shmemc_team_reset_psync: Invalid pSync index %u (max %u)",
                psync_idx, SHMEMC_NUM_PSYNCS - 1);
    return -1;
  }

  if (th->pSyncs[psync_idx] == NULL) {
    shmemu_warn("shmemc_team_reset_psync: pSync buffer at index %u is NULL",
                psync_idx);
    return -1;
  }

  /* Get the appropriate size for this pSync buffer */
  const size_t sync_sizes[SHMEMC_NUM_PSYNCS] = {
      SHMEM_BARRIER_SYNC_SIZE, /* pSyncs[0] for team sync/barrier */
      SHMEM_REDUCE_SYNC_SIZE   /* pSyncs[1] for other collectives */
  };

  /* Reset all elements to SHMEM_SYNC_VALUE */
  for (size_t i = 0; i < sync_sizes[psync_idx]; i++) {
    th->pSyncs[psync_idx][i] = SHMEM_SYNC_VALUE;
  }

  return 0;
}

/**
 * @brief Free synchronization buffers for a team
 *
 * Deallocates all pSync buffers associated with the team.
 *
 * @param th Team handle whose buffers should be freed
 */
static void finalize_psync_buffers(shmemc_team_h th) {
  unsigned nsync;

  for (nsync = 0; nsync < SHMEMC_NUM_PSYNCS; ++nsync) {
    shmema_free(th->pSyncs[nsync]);
  }
}

/**
 * @brief Get the appropriate pSync buffer for a collective operation
 *
 * Returns a pointer to the pSync buffer associated with the specified
 * collective operation type for the given team.
 *
 * @param th Team handle
 * @param psync_type Type of collective operation (SHMEMC_PSYNC_*)
 * @return Pointer to pSync buffer, or NULL if invalid
 */
long *shmemc_team_get_psync(shmemc_team_h th, int psync_type) {
  /* Validate parameters */
  if (th == NULL) {
    shmemu_warn("shmemc_team_get_psync: Invalid team handle (NULL)");
    return NULL;
  }

  if (psync_type < 0 || psync_type > SHMEMC_PSYNC_REDUCE) {
    shmemu_warn(
        "shmemc_team_get_psync: invalid psync type %d, assuming collective",
        psync_type);
  }
  int psync_idx =
      psync_type == 0 ? SHMEMC_PSYNC_BARRIER : SHMEMC_PSYNC_COLLECTIVE;

  if (th->pSyncs[psync_idx] == NULL) {
    shmemu_warn("shmemc_team_get_psync: pSync buffer for type %d is NULL",
                psync_type);
    return NULL;
  }

  return th->pSyncs[psync_idx];
}

/**
 * @brief Initialize common team attributes
 *
 * Sets up the basic attributes and data structures needed by all teams:
 * - Parent pointer and name
 * - Context configuration
 * - PE mapping hash tables
 * - Synchronization buffers
 * - Default geometry values
 *
 * @param th Team handle to initialize
 * @param name Name for the team
 * @param cfg_nctxts Number of contexts to configure for this team
 */
static void initialize_common_team(shmemc_team_h th, const char *name,
                                   int cfg_nctxts) {
  th->parent = NULL;
  th->name = name;

  /* nothing allocated yet */
  th->nctxts = 0;
  th->ctxts = NULL;

  th->cfg.num_contexts = cfg_nctxts;

  th->fwd = kh_init(map);
  th->rev = kh_init(map);

  initialize_psync_buffers(th);

  /* Initialize geometry to sane defaults (overridden below) */
  th->start = -1;
  th->stride = -1;
}

/**
 * @brief Initialize the world team
 *
 * Sets up the SHMEM_TEAM_WORLD team that contains all PEs in the job.
 * Initializes the PE mappings based on the launch information.
 */
static void initialize_team_world(void) {
  int i;
  int absent;

  initialize_common_team(world, "world", proc.env.prealloc_contexts);

  /* populate from launch info */
  world->rank = proc.li.rank;
  world->nranks = proc.li.nranks;
  world->start = 0;  /* SHMEM_TEAM_WORLD starts at PE 0 */
  world->stride = 1; /* SHMEM_TEAM_WORLD has stride 1 */

  for (i = 0; i < proc.li.nranks; ++i) {
    khiter_t k;

    k = kh_put(map, world->fwd, i, &absent);
    kh_val(world->fwd, k) = i;
    k = kh_put(map, world->rev, i, &absent);
    kh_val(world->rev, k) = i;
  }
}

/**
 * @brief Initialize the shared team
 *
 * Sets up the SHMEM_TEAM_SHARED team that contains PEs running on the same
 * node. Initializes PE mappings based on the peer information from launch.
 */
static void initialize_team_shared(void) {
  int i;
  int absent;

  initialize_common_team(shared, "shared",
                         proc.env.prealloc_contexts / proc.li.nnodes);

  shared->rank = -1;
  shared->nranks = proc.li.npeers;
  /* Shared team maps contiguous ranks 0..N-1 to the global PEs in peers */
  /* Start is the global PE of the first peer (rank 0) */
  shared->start = (shared->nranks > 0) ? proc.li.peers[0] : -1;
  /* Stride within the shared team concept is 1 */
  shared->stride = 1;

  for (i = 0; i < proc.li.npeers; ++i) {
    khiter_t k;

    if (proc.li.rank == proc.li.peers[i]) {
      shared->rank = i;
    }

    k = kh_put(map, shared->fwd, i, &absent);
    kh_val(shared->fwd, k) = proc.li.peers[i];
    k = kh_put(map, shared->rev, proc.li.peers[i], &absent);
    kh_val(shared->rev, k) = i;
  }
}

/**
 * @brief Clean up team resources
 *
 * Frees all resources associated with a team:
 * - Synchronization buffers
 * - Team contexts
 *
 * @param th Team handle to clean up
 */
static void finalize_team(shmemc_team_h th) {
  finalize_psync_buffers(th);

  shmemc_team_contexts_destroy(th);
}

/**
 * @brief Initialize teams subsystem
 *
 * Sets up the default teams (WORLD and SHARED) at library initialization time.
 */
void shmemc_teams_init(void) {
  initialize_team_world();
  initialize_team_shared();
}

/**
 * @brief Finalize teams subsystem
 *
 * Cleans up all team resources when shutting down the library.
 */
void shmemc_teams_finalize(void) {
  finalize_team(shared);
  finalize_team(world);
}

/*
 * ----------------------------------------------------------------
 */

/**
 * @brief Get PE rank in team
 *
 * Returns the rank of the calling PE within the specified team.
 *
 * @param th Team handle
 * @return PE rank in team
 */
int shmemc_team_my_pe(shmemc_team_h th) { return th->rank; }

/**
 * @brief Get number of PEs in team
 *
 * Returns the total number of PEs in the specified team.
 *
 * @param th Team handle
 * @return Number of PEs in team
 */
int shmemc_team_n_pes(shmemc_team_h th) { return th->nranks; }

/**
 * @brief Get team configuration
 *
 * Retrieves the configuration parameters for a team based on the config mask.
 *
 * @param th Team handle
 * @param config_mask Configuration mask specifying which parameters to retrieve
 * @param config Configuration structure to populate
 * @return 0 on success, -1 on failure
 */
int shmemc_team_get_config(shmemc_team_h th, long config_mask,
                           shmem_team_config_t *config) {
  /* Initialize config structure to zero */
  memset(config, 0, sizeof(shmem_team_config_t));

  /* Apply the configuration mask to retrieve requested parameters */
  if (config_mask & SHMEM_TEAM_NUM_CONTEXTS) {
    config->num_contexts = th->cfg.num_contexts;
  }

  /* TODO: Add handling for other configuration parameters as they are added */

  return 0;
}

/**
 * @brief Translate PE number between teams
 *
 * Converts a PE number from one team to the equivalent PE number in another
 * team.
 *
 * @param sh Source team handle
 * @param src_pe PE number in source team
 * @param dh Destination team handle
 * @return PE number in destination team, or -1 if invalid
 */
int shmemc_team_translate_pe(shmemc_team_h sh, int src_pe, shmemc_team_h dh) {
  khiter_t k;
  int wpe;

  /* can we find the source PE? */
  k = kh_get(map, sh->fwd, src_pe);
  if (k == kh_end(sh->fwd)) {
    return -1;
    /* NOT REACHED */
  }

  /* world equiv PE */
  wpe = kh_val(sh->fwd, k);

  /* map to world equiv in destination team */
  k = kh_get(map, dh->rev, wpe);
  if (k == kh_end(dh->rev)) {
    return -1;
    /* NOT REACHED */
  }

  /* world equiv is this in destination team */
  return kh_val(dh->rev, k);
}

/**
 * @brief Check if PE is member of strided team
 *
 * Determines if a PE number belongs to a strided team based on start and stride
 * values.
 *
 * @param parent_pe PE number in parent team
 * @param start Start PE
 * @param stride Stride between PEs
 * @return true if PE is member, false otherwise
 */
static bool is_member(int parent_pe, int start, int stride) {
  return ((parent_pe - start) % stride) == 0;
}

/**
 * @brief Split team into strided subteam
 *
 * Creates a new team by selecting PEs from the parent team using a strided
 * pattern.
 *
 * @param parh Parent team handle
 * @param start Start PE
 * @param stride Stride between PEs
 * @param size Size of new team
 * @param config Team configuration
 * @param config_mask Configuration mask
 * @param newh New team handle
 * @return 0 on success, -1 on failure
 */
int shmemc_team_split_strided(shmemc_team_h parh, int start, int stride,
                              int size, const shmem_team_config_t *config,
                              long config_mask, shmemc_team_h *newh) {
  int i;    /* new team PE # */
  int walk; /* iterate over parent PEs */
  shmemc_team_h newt;
  int absent;
  int nc;

  newt = (shmemc_team_h)malloc(sizeof(*newt));
  if (newt == NULL) {
    *newh = SHMEM_TEAM_INVALID;
    return -1;
  }

  nc = (config_mask & SHMEM_TEAM_NUM_CONTEXTS) ? config->num_contexts : 0;

  initialize_common_team(newt, NULL, nc);

  newt->parent = parh;
  newt->nranks = size;
  newt->start = start;   /* Store the start PE */
  newt->stride = stride; /* Store the stride */

  /* Initialize rank to -1 (invalid) */
  newt->rank = -1;

  walk = start;
  for (i = 0; i < size; ++i) {
    khint_t k;

    /* Get the parent PE at position 'walk' */
    k = kh_get(map, parh->fwd, walk);
    if (k == kh_end(parh->fwd)) {
      /* This shouldn't happen if parameters are valid */
      shmemu_warn("Parent PE %d not found in forward map", walk);
      free(newt);
      *newh = SHMEM_TEAM_INVALID;
      return -1;
    }

    const int global_pe = kh_val(parh->fwd, k);

    /* Add this PE to the new team mapping */
    k = kh_put(map, newt->fwd, i, &absent);
    kh_val(newt->fwd, k) = global_pe;

    k = kh_put(map, newt->rev, global_pe, &absent);
    kh_val(newt->rev, k) = i;

    /* If this global PE is me, set my rank in the team */
    if (global_pe == proc.li.rank) {
      newt->rank = i;
    }

    walk += stride;
  }

  /* Verify that the calling PE is part of the team */
  // if (newt->rank == -1) {
  //   shmemu_warn("Calling PE %d is not part of the new team", proc.li.rank);
  // }

  *newh = newt;

  return 0;
}

/**
 * @brief Split team into 2D grid
 *
 * Creates two new teams representing the X and Y axes of a 2D decomposition
 * of the parent team.
 *
 * @param parh Parent team handle
 * @param xrange Size of x dimension
 * @param xaxis_config X-axis team configuration
 * @param xaxis_mask X-axis configuration mask
 * @param xaxish X-axis team handle
 * @param yaxis_config Y-axis team configuration
 * @param yaxis_mask Y-axis configuration mask
 * @param yaxish Y-axis team handle
 * @return 0 on success, -1 on failure
 */
int shmemc_team_split_2d(shmemc_team_h parh, int xrange,
                         const shmem_team_config_t *xaxis_config,
                         long xaxis_mask, shmemc_team_h *xaxish,
                         const shmem_team_config_t *yaxis_config,
                         long yaxis_mask, shmemc_team_h *yaxish) {
  int parent_size, my_pe_in_parent;
  int yrange;
  int my_x, my_y;
  int i, ret;
  shmemc_team_h xaxis_team = NULL;
  shmemc_team_h yaxis_team = NULL;

  /* Get the parent team size and our PE in the parent team */
  parent_size = parh->nranks;
  my_pe_in_parent = parh->rank;

  /* Boundary check for xrange */
  if (xrange <= 0) {
    shmemu_warn("xrange must be positive");
    return -1;
  }

  /* If xrange is greater than parent team size, treat it as equal to parent
   * size */
  if (xrange > parent_size) {
    xrange = parent_size;
  }

  /* Calculate yrange: ceiling of (parent_size / xrange) */
  yrange = (parent_size + xrange - 1) / xrange;

  /* Calculate our coordinates in the 2D grid */
  my_x = my_pe_in_parent % xrange;
  my_y = my_pe_in_parent / xrange;

  /* Create the x-axis team (all PEs with the same y coordinate) */
  xaxis_team = (shmemc_team_h)malloc(sizeof(*xaxis_team));
  if (xaxis_team == NULL) {
    goto cleanup;
  }

  /* Initialize the x-axis team */
  int nc_x =
      (xaxis_mask & SHMEM_TEAM_NUM_CONTEXTS) ? xaxis_config->num_contexts : 0;
  initialize_common_team(xaxis_team, NULL, nc_x);
  xaxis_team->parent = parh;

  /* x-axis team size is minimum of xrange or remaining PEs in last row */
  if (my_y == yrange - 1 && parent_size % xrange != 0) {
    /* Last row might be incomplete */
    xaxis_team->nranks = parent_size % xrange;
  } else {
    xaxis_team->nranks = xrange;
  }

  /* Calculate start and stride for x-axis team */
  int xaxis_global_pe_0 = -1;
  int xaxis_global_pe_1 = -1;
  int parent_pe_0 = my_y * xrange + 0; /* Parent rank for (0, my_y) */
  khint_t k0 = kh_get(map, parh->fwd, parent_pe_0);
  if (k0 != kh_end(parh->fwd)) {
    xaxis_global_pe_0 = kh_val(parh->fwd, k0);
  }
  if (xaxis_team->nranks > 1) {
    int parent_pe_1 = my_y * xrange + 1; /* Parent rank for (1, my_y) */
    khint_t k1 = kh_get(map, parh->fwd, parent_pe_1);
    if (k1 != kh_end(parh->fwd)) {
      xaxis_global_pe_1 = kh_val(parh->fwd, k1);
    }
  }
  xaxis_team->start = xaxis_global_pe_0;
  xaxis_team->stride = (xaxis_team->nranks > 1 && xaxis_global_pe_1 != -1)
                           ? (xaxis_global_pe_1 - xaxis_global_pe_0)
                           : 1;

  /* Initialize rank to -1 (invalid) */
  xaxis_team->rank = -1;

  /* Map PEs to the x-axis team */
  int absent;
  int x_team_idx = 0;

  /* Populate the x-axis team with PEs that have the same y-coordinate */
  for (i = 0; i < parent_size; i++) {
    int pe_y = i / xrange;

    /* Only include PEs with the same y-coordinate as me */
    if (pe_y == my_y) {
      khint_t k;
      int global_pe;

      /* Get the global PE from the parent team */
      k = kh_get(map, parh->fwd, i);
      global_pe = kh_val(parh->fwd, k);

      /* Add to the x-axis team mapping */
      k = kh_put(map, xaxis_team->fwd, x_team_idx, &absent);
      kh_val(xaxis_team->fwd, k) = global_pe;

      k = kh_put(map, xaxis_team->rev, global_pe, &absent);
      kh_val(xaxis_team->rev, k) = x_team_idx;

      /* If this is me, set my rank in the x-axis team */
      if (i == my_pe_in_parent) {
        xaxis_team->rank = x_team_idx;
      }

      x_team_idx++;
    }
  }

  /* Create the y-axis team (all PEs with the same x coordinate) */
  yaxis_team = (shmemc_team_h)malloc(sizeof(*yaxis_team));
  if (yaxis_team == NULL) {
    goto cleanup;
  }

  /* Initialize the y-axis team */
  int nc_y =
      (yaxis_mask & SHMEM_TEAM_NUM_CONTEXTS) ? yaxis_config->num_contexts : 0;
  initialize_common_team(yaxis_team, NULL, nc_y);
  yaxis_team->parent = parh;

  /* y-axis team size is at most yrange */
  int actual_y_size =
      (my_x < parent_size % xrange && parent_size % xrange != 0)
          ? yrange
          : (parent_size % xrange == 0
                 ? yrange
                 : (my_x < parent_size % xrange
                        ? yrange
                        : yrange - 1)); /* Handle incomplete last column */
  yaxis_team->nranks = actual_y_size;

  /* Calculate start and stride for y-axis team */
  int yaxis_global_pe_0 = -1;
  int yaxis_global_pe_1 = -1;
  int parent_pe_y0 = 0 * xrange + my_x; /* Parent rank for (my_x, 0) */
  k0 = kh_get(map, parh->fwd, parent_pe_y0);
  if (k0 != kh_end(parh->fwd)) {
    yaxis_global_pe_0 = kh_val(parh->fwd, k0);
  }
  if (yaxis_team->nranks > 1) {
    int parent_pe_y1 = 1 * xrange + my_x; /* Parent rank for (my_x, 1) */
    khint_t k1 = kh_get(map, parh->fwd, parent_pe_y1);
    if (k1 != kh_end(parh->fwd)) {
      yaxis_global_pe_1 = kh_val(parh->fwd, k1);
    }
  }
  yaxis_team->start = yaxis_global_pe_0;
  yaxis_team->stride = (yaxis_team->nranks > 1 && yaxis_global_pe_1 != -1)
                           ? (yaxis_global_pe_1 - yaxis_global_pe_0)
                           : 1;

  /* Initialize rank to -1 (invalid) */
  yaxis_team->rank = -1;

  /* Map PEs to the y-axis team */
  int y_team_idx = 0;

  /* Populate the y-axis team with PEs that have the same x-coordinate */
  for (i = 0; i < parent_size; i++) {
    int pe_x = i % xrange;

    /* Only include PEs with the same x-coordinate as me */
    if (pe_x == my_x) {
      khint_t k;
      int global_pe;

      /* Get the global PE from the parent team */
      k = kh_get(map, parh->fwd, i);
      global_pe = kh_val(parh->fwd, k);

      /* Add to the y-axis team mapping */
      k = kh_put(map, yaxis_team->fwd, y_team_idx, &absent);
      kh_val(yaxis_team->fwd, k) = global_pe;

      k = kh_put(map, yaxis_team->rev, global_pe, &absent);
      kh_val(yaxis_team->rev, k) = y_team_idx;

      /* If this is me, set my rank in the y-axis team */
      if (i == my_pe_in_parent) {
        yaxis_team->rank = y_team_idx;
      }

      y_team_idx++;
    }
  }

  /* All good, assign the teams and return success */
  *xaxish = xaxis_team;
  *yaxish = yaxis_team;
  return 0;

cleanup:
  /* Clean up in case of error */
  if (xaxis_team != NULL) {
    free(xaxis_team);
  }
  if (yaxis_team != NULL) {
    free(yaxis_team);
  }

  *xaxish = SHMEM_TEAM_INVALID;
  *yaxish = SHMEM_TEAM_INVALID;
  return -1;
}

/**
 * @brief Destroy a team
 *
 * Frees all resources associated with a team. Cannot be used on predefined
 * teams.
 *
 * @param th Team handle to destroy
 * @note Predefined teams cannot be destroyed
 */
void shmemc_team_destroy(shmemc_team_h th) {
  if (th->parent != NULL) {
    size_t c;

    for (c = 0; c < th->nctxts; ++c) {
      if (!th->ctxts[c]->attr.privat) {
        shmemc_context_destroy(th->ctxts[c]);
      }
    }

    free(th);

    th = invalid;
  } else {
    shmemu_fatal("cannot destroy predefined team \"%s\"", th->name);
    /* NOT REACHED */
  }
}

/**
 * @brief Synchronize team contexts
 *
 * Ensures all contexts within a team have completed their operations.
 *
 * @param th Team handle
 * @return 0 on success, -1 on failure
 */
int shmemc_team_sync(shmemc_team_h th) {
  /* Validate the team handle */
  if (th == NULL) {
    shmemu_warn("shmemc_team_sync: Invalid team handle (NULL)");
    return -1;
  }

  /* Iterate through all contexts within the team */
  for (size_t i = 0; i < th->nctxts; ++i) {
    shmemc_context_h ch = th->ctxts[i];

    /* Validate the context handle */
    if (ch == NULL) {
      shmemu_warn("shmemc_team_sync: Context at index %zu is NULL", i);
      continue; /* Skip to the next context */
    }

    /* Perform a fence operation to synchronize */
    ucs_status_t status = ucp_worker_fence(ch->w);
    if (status != UCS_OK) {
      shmemu_warn("shmemc_team_sync: ucp_worker_fence failed on context %zu "
                  "with status %s",
                  i, ucs_status_string(status));
      return -1;
    }
  }

  /* Successful synchronization across all contexts */
  return 0;
}

// /**
//  * @brief Implementation of team-relative pointer access
//  *
//  * @param th The team handle
//  * @param dest The symmetric address of the remotely accessible data object
//  * @param pe Team-relative PE number
//  * @return Pointer to the remote object or NULL if not accessible
//  */
// void *shmemc_team_ptr(shmemc_team_h th, const void *dest, int pe) {
//   int global_pe;

//   /* Validate the PE is within team range */
//   if (pe < 0 || pe >= th->nranks) {
//     return NULL;
//   }

//   /* Convert team-relative PE to global PE */
//   global_pe = shmemc_team_translate_pe(th, pe, &shmemc_team_world);
//   if (global_pe < 0) {
//     return NULL;
//   }

//   /* Call the original shmemc_ptr function with the global PE */
//   return shmemc_ptr(dest, global_pe);
// }
