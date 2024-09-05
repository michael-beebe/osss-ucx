/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmem_mutex.h"
#include "shmemu.h"
#include "shmemc.h"
#include "shmemx.h"

/* 
 * If ENABLE_PSHMEM is defined, ensure that the weak versions of 
 * the context routines are defined and the "pshmem" versions 
 * are used when available.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_create = pshmem_ctx_create
#define shmem_ctx_create pshmem_ctx_create
#pragma weak shmem_ctx_destroy = pshmem_ctx_destroy
#define shmem_ctx_destroy pshmem_ctx_destroy
#endif /* ENABLE_PSHMEM */

/*
 * The default context is a link-time constant and must be
 * instantiated at all times.
 */
shmem_ctx_t SHMEM_CTX_DEFAULT = (shmem_ctx_t) &shmemc_default_context;

/*
 * ---------------------------------
 * routine: shmem_ctx_create
 * ---------------------------------
 * Create a new context with the supplied options.
 *
 * options: Context options specified by the user.
 * ctxp:    Pointer to the context to be created.
 *
 * Returns: 1 on success, 0 on failure.
 */
int
shmem_ctx_create(long options, shmem_ctx_t *ctxp)
{
    int s;

    SHMEMU_CHECK_INIT();

    /* Default to the world team */
    SHMEMT_MUTEX_PROTECT(s = shmemc_context_create(SHMEM_TEAM_WORLD,
                                                   options,
                                                   (shmemc_context_h *) ctxp));

    logger(LOG_CONTEXTS,
           "%s(options=%#lx, ctxp->%p)",
           __func__,
           options, *ctxp
           );

    return s;
}

/*
 * ---------------------------------
 * routine: shmem_ctx_destroy
 * ---------------------------------
 * Destroys the specified context.
 *
 * ctx: The context to be destroyed.
 */
void
shmem_ctx_destroy(shmem_ctx_t ctx)
{
    SHMEMU_CHECK_INIT();
    SHMEMU_CHECK_SAME_THREAD(ctx);

    SHMEMT_MUTEX_PROTECT(shmemc_context_destroy(ctx));

    logger(LOG_CONTEXTS,
           "%s(ctx=%p)",
           __func__,
           ctx
           );
}

#ifdef ENABLE_EXPERIMENTAL

/*
 * ---------------------------------
 * routine: shmemx_ctx_session_start
 * ---------------------------------
 * Signals the start of a communication session for the specified context.
 *
 * ctx: The context for the communication session.
 */
void
shmemx_ctx_session_start(shmem_ctx_t ctx)
{
    NO_WARN_UNUSED(ctx);

    SHMEMU_CHECK_INIT();
}

/*
 * ---------------------------------
 * routine: shmemx_ctx_session_estop
 * ---------------------------------
 * Signals the end of a communication session for the specified context.
 *
 * ctx: The context for which the session is being ended.
 */
void
shmemx_ctx_session_estop(shmem_ctx_t ctx)
{
    NO_WARN_UNUSED(ctx);

    SHMEMU_CHECK_INIT();
}

#endif  /* ENABLE_EXPERIMENTAL */
