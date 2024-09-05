/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemc.h"
#include "shmemu.h"

/*
 * --------------------------------------------
 * Routine: shmem_my_pe
 * --------------------------------------------
 * Returns the calling PE's (Processing Element) number.
 * This is used to identify which PE is making the call.
 * 
 * Logging: Logs the PE number of the calling process.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_my_pe = pshmem_my_pe
#define shmem_my_pe pshmem_my_pe
#endif /* ENABLE_PSHMEM */

int
shmem_my_pe(void)
{
    int my;

    /* Ensure OpenSHMEM is initialized before proceeding */
    SHMEMU_CHECK_INIT();

    /* Get the PE number of the calling process */
    my = shmemc_my_pe();

    /* Log the PE number */
    logger(LOG_RANKS, "%s() -> %d", __func__, my);

    return my;
}

/*
 * --------------------------------------------
 * Routine: shmem_n_pes
 * --------------------------------------------
 * Returns the total number of PEs in the program.
 * This provides information about how many PEs are
 * participating in the OpenSHMEM execution.
 * 
 * Logging: Logs the number of PEs.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_n_pes = pshmem_n_pes
#define shmem_n_pes pshmem_n_pes
#endif /* ENABLE_PSHMEM */

int
shmem_n_pes(void)
{
    int n;

    /* Ensure OpenSHMEM is initialized before proceeding */
    SHMEMU_CHECK_INIT();

    /* Get the total number of PEs */
    n = shmemc_n_pes();

    /* Log the number of PEs */
    logger(LOG_RANKS, "%s() -> %d", __func__, n);

    return n;
}
