/* For license: see LICENSE file at top-level */

/**
 * @file contexts.c
 * @brief Implementation of OpenSHMEM context management routines
 *
 * This file contains implementations of routines to create and destroy
 * OpenSHMEM communication contexts, which provide separate logical
 * communication channels that can be used to organize and isolate
 * communication operations.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmem_mutex.h"

#include "shmemu.h"
#include "shmemc.h"
#include "shmemx.h"

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_create = pshmem_ctx_create
#define shmem_ctx_create pshmem_ctx_create
#pragma weak shmem_ctx_destroy = pshmem_ctx_destroy
#define shmem_ctx_destroy pshmem_ctx_destroy
#pragma weak shmem_ctx_session_start = pshmem_ctx_session_start
#define shmem_ctx_session_start pshmem_ctx_session_start
#pragma weak shmem_ctx_session_stop = pshmem_ctx_session_stop
#define shmem_ctx_session_stop pshmem_ctx_session_stop
#endif /* ENABLE_PSHMEM */

/*
 * the default context is a link-time constant and has to be
 * instantiated at all times
 */

shmem_ctx_t SHMEM_CTX_DEFAULT = (shmem_ctx_t)&shmemc_default_context;

/**
 * @brief Create a new communication context with specified options
 *
 * @param options Context creation options
 * @param ctxp Pointer to store the created context
 * @return 1 on success, 0 on failure
 *
 * Creates a new communication context with the supplied options. The context
 * provides a separate logical communication channel that can be used to
 * organize and isolate communication operations.
 */
int shmem_ctx_create(long options, shmem_ctx_t *ctxp) {
  int s;

  SHMEMU_CHECK_INIT();

  /* defaults to world team */
  SHMEMT_MUTEX_PROTECT(s = shmemc_context_create(SHMEM_TEAM_WORLD, options,
                                                 (shmemc_context_h *)ctxp));

  logger(LOG_CONTEXTS, "%s(options=%#lx, ctxp->%p)", __func__, options, *ctxp);

  return s;
}

/**
 * @brief Destroy a communication context
 *
 * @param ctx Context to destroy
 *
 * Destroys the given communication context and releases associated resources.
 * The context should not be used after calling this function.
 */
void shmem_ctx_destroy(shmem_ctx_t ctx) {
  SHMEMU_CHECK_INIT();
  SHMEMU_CHECK_SAME_THREAD(ctx);

  SHMEMT_MUTEX_PROTECT(shmemc_context_destroy(ctx));

  logger(LOG_CONTEXTS, "%s(ctx=%p)", __func__, ctx);
}

/**
 * @brief Start a communication session on a context
 *
 * @param ctx Context handle
 * @param options Session hint options
 * @param config Pointer to session configuration parameters
 * @param config_mask Bitwise mask of configuration parameters
 */
void shmem_ctx_session_start(shmem_ctx_t ctx, long options,
                             const shmem_ctx_session_config_t *config,
                             long config_mask) {
  SHMEMU_CHECK_INIT();

  if (ctx == SHMEM_CTX_INVALID) {
    return;
  }

  SHMEMT_MUTEX_PROTECT(shmemc_ctx_session_start((shmemc_context_h)ctx, options,
                                                config, config_mask));

  logger(LOG_CONTEXTS, "%s(ctx=%p, options=%#lx, config_mask=%#lx)", __func__,
         ctx, options, config_mask);
}

/**
 * @brief Stop a communication session on a context
 *
 * @param ctx Context handle
 */
void shmem_ctx_session_stop(shmem_ctx_t ctx) {
  SHMEMU_CHECK_INIT();

  if (ctx == SHMEM_CTX_INVALID) {
    return;
  }

  SHMEMT_MUTEX_PROTECT(shmemc_ctx_session_stop((shmemc_context_h)ctx));

  logger(LOG_CONTEXTS, "%s(ctx=%p)", __func__, ctx);
}

#ifdef ENABLE_EXPERIMENTAL

/**
 * @brief Signal the start of a communication session (experimental)
 *
 * @param ctx Context for the communication session
 */
void shmemx_ctx_session_start(shmem_ctx_t ctx) {
  shmem_ctx_session_start(ctx, 0, NULL, 0);
}

/**
 * @brief Signal the end of a communication session (experimental)
 *
 * @param ctx Context for the communication session
 */
void shmemx_ctx_session_stop(shmem_ctx_t ctx) {
  shmem_ctx_session_stop(ctx);
}

#endif /* ENABLE_EXPERIMENTAL */
