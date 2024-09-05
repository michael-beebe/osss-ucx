/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"
#include "shmem_mutex.h"

/*
 * ---------------------------------
 * routine: shmem_ctx_fence
 * ---------------------------------
 * Ensures ordering of memory updates across a specified context.
 *
 * ctx: The context for which to enforce memory ordering.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_fence = pshmem_ctx_fence
#define shmem_ctx_fence pshmem_ctx_fence
#endif /* ENABLE_PSHMEM */

void
shmem_ctx_fence(shmem_ctx_t ctx)
{
    /* Log the routine call */
    logger(LOG_FENCE, "%s(ctx=%lu)", __func__, shmemc_context_id(ctx));

    SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_fence(ctx));
}

/*
 * ---------------------------------
 * routine: shmem_fence
 * ---------------------------------
 * Ensures ordering of memory updates across the default context.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_fence = pshmem_fence
#define shmem_fence pshmem_fence
#endif /* ENABLE_PSHMEM */

void
shmem_fence(void)
{
    /* Log the routine call */
    logger(LOG_FENCE, "%s()", __func__);

    SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_fence(SHMEM_CTX_DEFAULT));
}
