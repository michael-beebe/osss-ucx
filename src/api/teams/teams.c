/**
 * @file teams.c
 * @brief Implementation of team-related routines for SHMEM.
 *
 * This file provides a set of wrapper functions that call underlying
 * implementations provided by shmemc.
 */

/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"
#include "thispe.h"

/*
 * these point to underlying objects to be constant initialized
 */
shmem_team_t SHMEM_TEAM_WORLD = (shmem_team_t)&shmemc_team_world;
shmem_team_t SHMEM_TEAM_SHARED = (shmem_team_t)&shmemc_team_shared;

/*
 * a bad team
 */
shmem_team_t SHMEM_TEAM_INVALID = NULL;

#ifdef ENABLE_PSHMEM
#pragma weak shmem_team_my_pe = pshmem_team_my_pe
#define shmem_team_my_pe pshmem_team_my_pe
#pragma weak shmem_team_n_pes = pshmem_team_n_pes
#define shmem_team_n_pes pshmem_team_n_pes
#pragma weak shmem_team_get_config = pshmem_team_get_config
#define shmem_team_get_config pshmem_team_get_config
#pragma weak shmem_team_translate_pe = pshmem_team_translate_pe
#define shmem_team_translate_pe pshmem_team_translate_pe
#pragma weak shmem_team_split_strided = pshmem_team_split_strided
#define shmem_team_split_strided pshmem_team_split_strided
#pragma weak shmem_team_split_2d = pshmem_team_split_2d
#define shmem_team_split_2d pshmem_team_split_2d
#pragma weak shmem_team_destroy = pshmem_team_destroy
#define shmem_team_destroy pshmem_team_destroy
#pragma weak shmem_team_create_ctx = pshmem_team_create_ctx
#define shmem_team_create_ctx pshmem_team_create_ctx
#pragma weak shmem_ctx_get_team = pshmem_ctx_get_team
#define shmem_ctx_get_team pshmem_ctx_get_team
#pragma weak shmem_team_ptr = pshmem_team_ptr
#define shmem_team_ptr pshmem_team_ptr
#endif /* ENABLE_PSHMEM */

/**
 * @brief Get the calling PE's index in team.
 *
 * @param team A valid team handle.
 * @return The PE index on success, or -1 if the team is invalid.
 */
int shmem_team_my_pe(shmem_team_t team) {
  if (team != SHMEM_TEAM_INVALID) {
    shmemc_team_h th = (shmemc_team_h)team;
    return shmemc_team_my_pe(th);
  } else {
    return -1;
  }
}

/**
 * @brief Get the number of processing elements (PEs) in the team.
 *
 * @param team A valid team handle.
 * @return The number of PEs on success, or -1 if the team is invalid.
 */
int shmem_team_n_pes(shmem_team_t team) {
  if (team != SHMEM_TEAM_INVALID) {
    shmemc_team_h th = (shmemc_team_h)team;
    return shmemc_team_n_pes(th);
  } else {
    return -1;
  }
}

/**
 * @brief Get the configuration of the team.
 *
 * @param team A valid team handle.
 * @param config_mask Bitwise mask representing the set of configuration
 * parameters to fetch.
 * @param config Pointer to the configuration structure to be filled.
 * @return 0 on success or -1 if the team is invalid.
 */
int shmem_team_get_config(shmem_team_t team, long config_mask,
                          shmem_team_config_t *config) {
  if (team != SHMEM_TEAM_INVALID) {
    shmemc_team_h th = (shmemc_team_h)team;
    return shmemc_team_get_config(th, config_mask, config);
  } else {
    return -1;
  }
}

/**
 * @brief Translate a PE index from one team to another.
 *
 * @param src_team The source team.
 * @param src_pe The PE index in the source team.
 * @param dest_team The destination team.
 * @return The translated PE index on success, or -1 if the source team is
 * invalid.
 */
int shmem_team_translate_pe(shmem_team_t src_team, int src_pe,
                            shmem_team_t dest_team) {
  if (src_team != SHMEM_TEAM_INVALID) {
    shmemc_team_h sh = (shmemc_team_h)src_team;
    shmemc_team_h dh = (shmemc_team_h)dest_team;
    return shmemc_team_translate_pe(sh, src_pe, dh);
  } else {
    return -1;
  }
}

/**
 * @brief Split a team into a strided subgroup.
 *
 * @param parent_team The parent team.
 * @param start Starting index in the parent team.
 * @param stride Stride between indices.
 * @param size Number of elements in the new team.
 * @param config Configuration for the new team.
 * @param config_mask Mask for configuration options.
 * @param new_team Pointer to store the new team handle.
 * @return 0 on success or -1 if the parent team is invalid.
 */
int shmem_team_split_strided(shmem_team_t parent_team, int start, int stride,
                             int size, const shmem_team_config_t *config,
                             long config_mask, shmem_team_t *new_team) {
  if (parent_team != SHMEM_TEAM_INVALID && parent_team != NULL) {
    shmemc_team_h parh = (shmemc_team_h)parent_team;
    shmemc_team_h *newhh = (shmemc_team_h *)new_team;
    return shmemc_team_split_strided(parh, start, stride, size, config,
                                     config_mask, newhh);
  } else {
    if (new_team != NULL) {
      *new_team = SHMEM_TEAM_INVALID;
    }
    return -1;
  }
}

/**
 * @brief Split a team into two 2D subteams.
 *
 * @param parent_team The parent team.
 * @param xrange Number of PEs in the horizontal axis.
 * @param xaxis_config Configuration for the x-axis subteam.
 * @param xaxis_mask Mask for x-axis configuration options.
 * @param xaxis_team Pointer to store the x-axis subteam handle.
 * @param yaxis_config Configuration for the y-axis subteam.
 * @param yaxis_mask Mask for y-axis configuration options.
 * @param yaxis_team Pointer to store the y-axis subteam handle.
 * @return 0 on success or -1 if the parent team is invalid.
 */
int shmem_team_split_2d(shmem_team_t parent_team, int xrange,
                        const shmem_team_config_t *xaxis_config,
                        long xaxis_mask, shmem_team_t *xaxis_team,
                        const shmem_team_config_t *yaxis_config,
                        long yaxis_mask, shmem_team_t *yaxis_team) {
  if (parent_team != SHMEM_TEAM_INVALID && parent_team != NULL) {
    shmemc_team_h parh = (shmemc_team_h)parent_team;
    shmemc_team_h *xhh = (shmemc_team_h *)xaxis_team;
    shmemc_team_h *yhh = (shmemc_team_h *)yaxis_team;
    return shmemc_team_split_2d(parh, xrange, xaxis_config, xaxis_mask, xhh,
                                yaxis_config, yaxis_mask, yhh);
  } else {
    if (xaxis_team != NULL) {
      *xaxis_team = SHMEM_TEAM_INVALID;
    }
    if (yaxis_team != NULL) {
      *yaxis_team = SHMEM_TEAM_INVALID;
    }
    return -1;
  }
}

/**
 * @brief Destroy a team and free its resources.
 *
 * @param team The team handle to be destroyed.
 */
void shmem_team_destroy(shmem_team_t team) {
  if (team == SHMEM_TEAM_INVALID || team == NULL) {
    return;
  }
  shmemc_team_h th = (shmemc_team_h)team;
  shmemc_team_destroy(th);
}

/**
 * @brief Create an execution context for the team.
 *
 * @param team The team handle.
 * @param options Options for context creation.
 * @param ctxp Pointer to store the created context.
 * @return 0 on success, or -1 if the team or the underlying handle is invalid.
 */
int shmem_team_create_ctx(shmem_team_t team, long options, shmem_ctx_t *ctxp) {
  if (team == SHMEM_TEAM_INVALID) {
    return -1;
  }
  shmemc_team_h th = (shmemc_team_h)team;
  if (th == NULL) {
    return -1;
  }
  int ret = shmemc_context_create(th, options, (shmemc_context_h *)ctxp);
  if (ret != 0) {
    return ret;
  }
  return 0;
}

/**
 * @brief Retrieve the team associated with a context.
 *
 * @param ctx The context handle.
 * @param team Pointer to store the associated team handle.
 * @return 0 on success, or -1 if the context or its team is invalid.
 */
int shmem_ctx_get_team(shmem_ctx_t ctx, shmem_team_t *team) {
  if (ctx == SHMEM_CTX_INVALID) {
    return -1;
  }
  shmemc_context_h ch = (shmemc_context_h)ctx;
  if (ch == NULL) {
    return -1;
  }
  if (ch->team == NULL) {
    return -1;
  }
  *team = (shmem_team_t)ch->team;
  return 0;
}

// /**
//  * @brief Synchronize all PEs in the team.
//  *
//  * @param team The team handle.
//  * @return 0 on success, or -1 if the team is invalid.
//  */
// int shmem_team_sync(shmem_team_t team) {
//   if (team == SHMEM_TEAM_INVALID || team == NULL) {
//     return -1;
//   }
//   shmemc_team_h th = (shmemc_team_h)team;
//   return shmemc_team_sync(th);
// }

/**
 * @brief Returns a local pointer to a symmetric data object on the specified PE
 * in the specified team.
 *
 * This routine can provide efficient means to accomplish communication when a
 * sequence of reads and writes to a data object on a remote PE does not match
 * the access pattern provided in an OpenSHMEM data transfer routine.
 *
 * @param team A handle to the specified team.
 * @param dest The symmetric address of the remotely accessible data object to
 * be referenced.
 * @param pe An integer that indicates the PE number in the provided team on
 * which dest is to be accessed.
 * @return A local pointer to the remotely accessible data object when it can be
 *         accessed using memory loads and stores. Otherwise, a null pointer is
 * returned.
 */
void *shmem_team_ptr(shmem_team_t team, const void *dest, int pe) {
  void *rw;

  SHMEMU_CHECK_INIT();

  /* a null pointer is returned for the invalid team */
  if (team == SHMEM_TEAM_INVALID) {
    rw = NULL;
  } else {
    rw = shmemc_team_ptr((shmemc_team_h)team, dest, pe);
  }

  logger(LOG_MEMORY, "%s(team=%p, dest=%p, pe=%d) -> %p", __func__,
         (void *)team, dest, pe, rw);

  return rw;
}
