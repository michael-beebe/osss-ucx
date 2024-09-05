/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"

/*
 * ---------------------------------
 * routine: shmem_global_exit
 * ---------------------------------
 * Terminates all OpenSHMEM processing elements and exits with the given status.
 *
 * status: The exit status to be passed to the underlying system.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_global_exit = pshmem_global_exit
#define shmem_global_exit pshmem_global_exit
#endif /* ENABLE_PSHMEM */

void
shmem_global_exit(int status)
{
    /* Log the routine call */
    logger(LOG_FINALIZE, "%s(status=%d)", __func__, status);

    /* Call the implementation-specific global exit function */
    shmemc_global_exit(status);
}
