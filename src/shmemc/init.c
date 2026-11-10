/**
 * @file init.c
 * @brief Initialization and finalization routines for the OpenSHMEM
 * communications layer
 *
 * This file provides the core initialization and cleanup functionality for the
 * OpenSHMEM communications layer (shmemc). It handles setting up and tearing
 * down all required resources including node names, PMI client, heaps, UCX
 * transport, contexts, and teams.
 *
 * @copyright See LICENSE file at top-level
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "ucx/api.h"
#include "boolean.h"
#include "shmemc.h"
#include "nodename.h"
#include "pmi_client.h"
#include "heaps.h"
#include "env.h"

/**
 * @brief Initialize the OpenSHMEM communications layer
 *
 * This function performs the complete initialization sequence:
 * - Sets up node name identification
 * - Initializes PMI client for process management
 * - Processes environment variables and user settings
 * - Sets up symmetric heaps
 * - Initializes UCX transport layer
 * - Creates default context and teams
 * - Exchanges worker and heap information between PEs
 */
void shmemc_init(void) {
  shmemc_nodename_init();

  /* find launch info */
  shmemc_pmi_client_init();

  /* user-supplied setup */
  shmemc_env_init();

  shmemc_heaps_init();

  /* launch and connect my heap to network resources */
  shmemc_ucx_init();

  shmemc_context_init_default();

  shmemc_teams_init();

  /* now heap registered... */

  /* publish worker info, everyone has it and exchanges */
  shmemc_pmi_publish_worker();
  shmemc_pmi_barrier_all(true);
  shmemc_pmi_exchange_workers();

  /* publish rkeys (& maybe heaps), everyone has it and exchanges */
  shmemc_pmi_publish_rkeys_and_heaps();
  shmemc_pmi_barrier_all(true);
  shmemc_pmi_exchange_rkeys_and_heaps();

  shmemc_ucx_make_eps(defcp);

  /* just sync, no collect */
  shmemc_pmi_barrier_all(false);
}

/**
 * @brief Clean up and finalize the OpenSHMEM communications layer
 *
 * This function performs the complete cleanup sequence in reverse order:
 * - Finalizes teams
 * - Destroys default context
 * - Cleans up UCX resources
 * - Frees symmetric heaps
 * - Cleans up environment settings
 * - Finalizes PMI client
 * - Frees node name resources
 */
void shmemc_finalize(void) {
  shmemc_teams_finalize();
  shmemc_contexts_finalize();

  shmemc_ucx_context_default_destroy();

  shmemc_pmi_barrier_all(false);

  shmemc_ucx_finalize();

  shmemc_heaps_finalize();

  shmemc_env_finalize();

  shmemc_pmi_client_finalize();

  shmemc_nodename_finalize();
}
