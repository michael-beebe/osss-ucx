/* For license: see LICENSE file at top-level */

#ifndef _SHMEM_API_TEAMS_H
#define _SHMEM_API_TEAMS_H 1

#include <shmem/defs.h>

/**
 * @file teams.h
 * @brief OpenSHMEM teams API
 */

/**
 * @brief An opaque OpenSHMEM team type
 */
typedef void *shmem_team_t;

/**
 * @brief Pre-defined teams available at program start
 */
// clang-format off
extern shmem_team_t SHMEM_TEAM_WORLD;   /**< Team containing all PEs */
extern shmem_team_t SHMEM_TEAM_SHARED;  /**< Team of PEs with shared memory access */
extern shmem_team_t SHMEM_TEAM_INVALID; /**< Invalid team handle */
// clang-format on

/**
 * @brief Team configuration structure
 */
typedef struct shmem_team_config {
  int num_contexts; /**< Expected number of contexts used by team */
} shmem_team_config_t;

/**
 * @brief Get the PE number of the calling PE in the given team
 * @param team The team to query
 * @return PE number in the team
 */
int shmem_team_my_pe(shmem_team_t team);

/**
 * @brief Get the total number of PEs in the given team
 * @param team The team to query
 * @return Number of PEs in the team
 */
int shmem_team_n_pes(shmem_team_t team);

/**
 * @brief Get the configuration of a team
 * @param team The team to query
 * @param config_mask Mask indicating which config values to retrieve
 * @param config Configuration structure to fill
 * @return 0 on success, nonzero on failure
 */
int shmem_team_get_config(shmem_team_t team, long config_mask,
                          shmem_team_config_t *config);

/**
 * @brief Translate a PE number from one team to another
 * @param src_team Source team
 * @param src_pe PE number in source team
 * @param dest_team Destination team
 * @return Translated PE number in destination team
 */
int shmem_team_translate_pe(shmem_team_t src_team, int src_pe,
                            shmem_team_t dest_team);

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
int shmem_team_split_strided(shmem_team_t parent_team, int start, int stride,
                             int size, const shmem_team_config_t *config,
                             long config_mask, shmem_team_t *new_team);

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
int shmem_team_split_2d(shmem_team_t parent_team, int xrange,
                        const shmem_team_config_t *xaxis_config,
                        long xaxis_mask, shmem_team_t *xaxis_team,
                        const shmem_team_config_t *yaxis_config,
                        long yaxis_mask, shmem_team_t *yaxis_team);

/**
 * @brief Destroy a team
 * @param team Team to destroy
 */
void shmem_team_destroy(shmem_team_t team);

/**
 * @brief Create a communication context from a team
 * @param team Team to create context from
 * @param options Context creation options
 * @param ctxp Handle for new context
 * @return 0 on success, nonzero on failure
 */
int shmem_team_create_ctx(shmem_team_t team, long options, shmem_ctx_t *ctxp);

/**
 * @brief Get the team associated with a context
 * @param ctx Context to query
 * @param team Handle to store associated team
 * @return 0 on success, nonzero on failure
 */
int shmem_ctx_get_team(shmem_ctx_t ctx, shmem_team_t *team);

/**
 * @brief Synchronize all PEs in a team
 * @param team Team to synchronize
 * @return 0 on success, nonzero on failure
 */
int shmem_team_sync(shmem_team_t team);

/**
 * @brief Get a pointer to a symmetric data object on a remote PE in a team
 * @param team Team containing the PE
 * @param dest Symmetric data object on remote PE
 * @param pe PE number in the team
 * @return Local pointer to remote data object
 */
void *shmem_team_ptr(shmem_team_t team, const void *dest, int pe) _WUR;

#endif /* ! _SHMEM_API_TEAMS_H */
