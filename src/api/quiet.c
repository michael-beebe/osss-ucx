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

/**
 * @brief Ensures completion of all remote memory updates issued to a context
 * for a subset of PEs
 *
 * This operation ensures completion of all remote memory updates issued to a
 * specific context prior to this call.
 *
 * @param ctx   The context on which to ensure completion of updates
 */
void shmem_ctx_pe_quiet(shmem_ctx_t ctx, const int *target_pes, size_t npes) {
  logger(LOG_QUIET, "%s(ctx=%lu)", __func__, shmemc_context_id(ctx));

  if (npes == 0){
    return;
  }
  for (size_t i = 0; i < npes; i++){
    SHMEMU_CHECK_PE_ARG_RANGE(target_pes[i], 2);
  }
  SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_pe_quiet(ctx, target_pes, npes));
}

/**
 * @brief Ensures completion of all remote memory updates issued for a subset
 * of PEs
 *
 * This operation ensures completion of all remote memory updates issued to a
 * specific context prior to this call.
 *
 * @param ctx   The context on which to ensure completion of updates
 */
void shmem_pe_quiet(const int *target_pes, size_t npes) {
  logger(LOG_QUIET, "%s()", __func__);

  if (npes == 0){
    return;
  }
  for (size_t i = 0; i < npes; i++){
    SHMEMU_CHECK_PE_ARG_RANGE(target_pes[i], 1);
  }
  SHMEMT_MUTEX_NOPROTECT(shmemc_ctx_pe_quiet(SHMEM_CTX_DEFAULT, target_pes, npes));
}
