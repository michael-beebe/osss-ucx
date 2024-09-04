/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"

/*
 * This file provides deprecated functions for querying the rank (PE)
 * and the total number of PEs in an OpenSHMEM program. These functions
 * are provided for backward compatibility but have been replaced by
 * newer versions in the modern OpenSHMEM specification.
 */

#ifdef ENABLE_PSHMEM
/*
 * PSHMEM (Profiling SHMEM) support: 
 * These weak symbols allow profiling tools to intercept the `_my_pe` 
 * and `_num_pes` functions to gather performance data. 
 */
#pragma weak _my_pe = p_my_pe
#define _my_pe p_my_pe
#pragma weak _num_pes = p_num_pes
#define _num_pes p_num_pes
#endif /* ENABLE_PSHMEM */

/* 
 * Define the version in which these functions were deprecated. 
 * In this case, they were deprecated in OpenSHMEM 1.2.
 */
static const shmemu_version_t v = { .major = 1, .minor = 2 };

/*
 * Deprecated function `_my_pe`:
 * This function returns the rank (PE) of the calling process.
 * It has been replaced by `shmem_my_pe()` in newer OpenSHMEM versions.
 */
int
_my_pe(void)
{
    /* Log deprecation warning */
    deprecate(__func__, &v);

    /* Call the modern equivalent `shmemc_my_pe()` */
    return shmemc_my_pe();
}

/*
 * Deprecated function `_num_pes`:
 * This function returns the total number of PEs (processing elements) 
 * in the program. It has been replaced by `shmem_n_pes()` in newer 
 * OpenSHMEM versions.
 */
int
_num_pes(void)
{
    /* Log deprecation warning */
    deprecate(__func__, &v);

    /* Call the modern equivalent `shmemc_n_pes()` */
    return shmemc_n_pes();
}
