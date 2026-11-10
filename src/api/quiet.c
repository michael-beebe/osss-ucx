/**
 * @file quiet.c
 * @brief Implementation of OpenSHMEM quiet operations
 *
 * Quiet operations ensure completion of remote memory updates.
 *
 * For license: see LICENSE file at top-level
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"
#include "shmem_mutex.h"

/**
 * @file quiet.c
 * @brief Implementation of OpenSHMEM quiet operations
 *
 * Quiet operations ensure completion of remote memory updates.
 */

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_quiet = pshmem_ctx_quiet
#define shmem_ctx_quiet pshmem_ctx_quiet
#endif /* ENABLE_PSHMEM */

/**
 * @brief Ensures completion of all remote memory updates issued to a context
 *
 * This operation ensures completion of all remote memory updates issued to a
 * specific context prior to this call.
 *
 * @param ctx   The context on which to ensure completion of updates
 */
void shmem_ctx_quiet(shmem_ctx_t ctx) {
  logger(LOG_QUIET, "%s(ctx=%lu)", __func__, shmemc_context_id(ctx));

  SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_quiet(ctx));
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_quiet = pshmem_quiet
#define shmem_quiet pshmem_quiet
#endif /* ENABLE_PSHMEM */

/**
 * @brief Ensures completion of all remote memory updates
 *
 * This operation ensures completion of all remote memory updates issued by the
 * calling PE prior to this call using the default context.
 */
void shmem_quiet(void) {
  logger(LOG_QUIET, "%s()", __func__);

  SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_quiet(SHMEM_CTX_DEFAULT));
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_ctx_pe_quiet = pshmem_ctx_pe_quiet
#define shmem_ctx_pe_quiet pshmem_ctx_pe_quiet
#endif /* ENABLE_PSHMEM */

/**
 * @brief Completes communication to a set of PEs on a context.
 * @param ctx Communication context.
 * @param target_pes Array of target PEs, ignored when npes is zero.
 * @param npes Number of target PEs.
 *
 * Context-wide completion is a conservative implementation of PE quiet. It
 * also submits pending session coalescing buffers before completing them.
 */
void shmem_ctx_pe_quiet(shmem_ctx_t ctx, const int *target_pes, size_t npes) {
  SHMEMU_CHECK_INIT();
  if (npes == 0) {
    return;
  }
  SHMEMU_CHECK_NOT_NULL(target_pes, 2);
  NO_WARN_UNUSED(target_pes);

  logger(LOG_QUIET, "%s(ctx=%lu, npes=%zu)", __func__, shmemc_context_id(ctx),
         npes);
  SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_quiet(ctx));
}

#ifdef ENABLE_PSHMEM
#pragma weak shmem_pe_quiet = pshmem_pe_quiet
#define shmem_pe_quiet pshmem_pe_quiet
#endif /* ENABLE_PSHMEM */

/**
 * @brief Completes communication to a set of PEs on the default context.
 * @param target_pes Array of target PEs, ignored when npes is zero.
 * @param npes Number of target PEs.
 */
void shmem_pe_quiet(const int *target_pes, size_t npes) {
  SHMEMU_CHECK_INIT();
  if (npes == 0) {
    return;
  }
  SHMEMU_CHECK_NOT_NULL(target_pes, 1);
  NO_WARN_UNUSED(target_pes);

  logger(LOG_QUIET, "%s(npes=%zu)", __func__, npes);
  SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_quiet(SHMEM_CTX_DEFAULT));
}
