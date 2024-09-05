/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"
#include "shmem_mutex.h"

/*
 * --------------------------------------------
 * Routine: shmem_ctx_quiet
 * --------------------------------------------
 * Ensures that all previously initiated memory
 * operations within the specified OpenSHMEM context
 * 'ctx' are completed before continuing. It is a 
 * synchronization routine used to maintain consistency 
 * between the calling PE and the target PE.
 * 
 * Context: An opaque handle representing a SHMEM context.
 *
 * Logging: Logs the context ID being used.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_quiet = pshmem_ctx_quiet
#define shmem_ctx_quiet pshmem_ctx_quiet
#endif /* ENABLE_PSHMEM */

void
shmem_ctx_quiet(shmem_ctx_t ctx)
{
    logger(LOG_QUIET, "%s(ctx=%lu)", __func__, shmemc_context_id(ctx));

    /* Ensures all pending memory operations in the context are complete */
    SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_quiet(ctx));
}

/*
 * --------------------------------------------
 * Routine: shmem_quiet
 * --------------------------------------------
 * Ensures that all previously initiated memory
 * operations on the default OpenSHMEM context are 
 * completed before continuing. This is a global 
 * synchronization routine used for memory consistency 
 * across PEs.
 * 
 * Logging: Logs the use of the default context.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_quiet = pshmem_quiet
#define shmem_quiet pshmem_quiet
#endif /* ENABLE_PSHMEM */

void
shmem_quiet(void)
{
    logger(LOG_QUIET, "%s()", __func__);

    /* Ensures all pending memory operations in the default context are complete */
    SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_quiet(SHMEM_CTX_DEFAULT));
}
