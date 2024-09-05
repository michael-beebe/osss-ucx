/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"
#include "thispe.h"

/*
 * These are constant-initialized team objects for use in OpenSHMEM.
 * SHMEM_TEAM_WORLD: Represents all PEs (processing elements) in the program.
 * SHMEM_TEAM_SHARED: Represents PEs that share a memory context.
 * SHMEM_TEAM_INVALID: Used to indicate an invalid or non-existent team.
 */
shmem_team_t SHMEM_TEAM_WORLD = (shmem_team_t) &shmemc_team_world;
shmem_team_t SHMEM_TEAM_SHARED = (shmem_team_t) &shmemc_team_shared;

/*
 * A bad or invalid team, used as a placeholder for errors or invalid team state.
 */
shmem_team_t SHMEM_TEAM_INVALID = NULL;

/*
 * Get the current PE number in the specified team.
 * Returns -1 if the team is invalid.
 */
int
shmem_team_my_pe(shmem_team_t team)
{
    if (team != SHMEM_TEAM_INVALID) {
        shmemc_team_h th = (shmemc_team_h) team;

        return shmemc_team_my_pe(th);
    }
    else {
        return -1;
    }
}

/*
 * Get the number of PEs in the specified team.
 * Returns -1 if the team is invalid.
 */
int
shmem_team_n_pes(shmem_team_t team)
{
    if (team != SHMEM_TEAM_INVALID) {
        shmemc_team_h th = (shmemc_team_h) team;

        return shmemc_team_n_pes(th);
    }
    else {
        return -1;
    }
}

/*
 * Get the configuration of the specified team and store it in the provided config struct.
 * Returns -1 if the team is invalid.
 */
int
shmem_team_get_config(shmem_team_t team,
                      shmem_team_config_t *config)
{
    if (team != SHMEM_TEAM_INVALID) {
        shmemc_team_h th = (shmemc_team_h) team;

        return shmemc_team_get_config(th, config);
    }
    else {
        return -1;
    }
}

/*
 * Translate the PE number from one team to another team.
 * Returns the translated PE number or -1 if the source team is invalid.
 */
int
shmem_team_translate_pe(shmem_team_t src_team, int src_pe,
                        shmem_team_t dest_team)
{
    if (src_team != SHMEM_TEAM_INVALID) {
        shmemc_team_h sh = (shmemc_team_h) src_team;
        shmemc_team_h dh = (shmemc_team_h) dest_team;

        return shmemc_team_translate_pe(sh, src_pe, dh);
    }
    else {
        return -1;
    }
}

/*
 * Split a parent team into a new team based on a strided pattern.
 * Returns the status of the split operation, or -1 if the parent team is invalid.
 */
int
shmem_team_split_strided(shmem_team_t parent_team,
                         int start, int stride, int size,
                         const shmem_team_config_t *config,
                         long config_mask,
                         shmem_team_t *new_team)
{
    if (parent_team != SHMEM_TEAM_INVALID) {
        shmemc_team_h parh = (shmemc_team_h) parent_team;
        shmemc_team_h *newhh = (shmemc_team_h *) new_team;

        return shmemc_team_split_strided(parh,
                                         start, stride, size,
                                         config,
                                         config_mask,
                                         newhh);
    }
    else {
        return -1;
    }
}

/*
 * Split a parent team into two new teams (x-axis and y-axis) based on a 2D pattern.
 * Returns the status of the split operation, or -1 if the parent team is invalid.
 */
int
shmem_team_split_2d(shmem_team_t parent_team,
                    int xrange,
                    const shmem_team_config_t *xaxis_config,
                    long xaxis_mask,
                    shmem_team_t *xaxis_team,
                    const shmem_team_config_t *yaxis_config,
                    long yaxis_mask,
                    shmem_team_t *yaxis_team)
{
    if (parent_team != SHMEM_TEAM_INVALID) {
        shmemc_team_h parh = (shmemc_team_h) parent_team;
        shmemc_team_h *xhh = (shmemc_team_h *) xaxis_team;
        shmemc_team_h *yhh = (shmemc_team_h *) yaxis_team;

        return shmemc_team_split_2d(parh,
                                    xrange,
                                    xaxis_config,
                                    xaxis_mask,
                                    xhh,
                                    yaxis_config,
                                    yaxis_mask,
                                    yhh);
    }
    else {
        return -1;
    }
}

/*
 * Destroy a team and free its resources.
 * No return value is needed.
 */
void
shmem_team_destroy(shmem_team_t team)
{
    shmemc_team_h th = (shmemc_team_h) team;

    shmemc_team_destroy(th);
}
