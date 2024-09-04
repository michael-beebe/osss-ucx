/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemc.h"
#include "shmemu.h"

/*
 * This file contains a deprecated initialization routine for OpenSHMEM.
 * The `start_pes` function was part of the earlier OpenSHMEM specification but
 * has since been deprecated in favor of newer initialization routines.
 * The current version logs a deprecation warning and calls the new initialization
 * function `shmemc_init()`.
 */

/*
 * Define the version in which this function was deprecated. In this case, 
 * it was deprecated in OpenSHMEM 1.2.
 */
static const shmemu_version_t v = { .major = 1, .minor = 2 };

#ifdef ENABLE_PSHMEM
/*
 * PSHMEM (Profiling SHMEM) support: Create weak symbols for the 
 * `start_pes` function, allowing profiling tools to intercept and 
 * profile its usage.
 */
#pragma weak start_pes = pstart_pes
#define start_pes pstart_pes
#endif /* ENABLE_PSHMEM */

/*
 * Deprecated `start_pes` function. This was the original way to
 * initialize an OpenSHMEM program, but it has been replaced by newer
 * routines like `shmem_init`.
 */
void
start_pes(int n)
{
    /*
     * The argument `n` is unused, as newer OpenSHMEM specifications do not 
     * require it for initialization.
     */
    NO_WARN_UNUSED(n);  /* Suppress compiler warnings about the unused variable */

    /*
     * Call the current initialization routine. `shmemc_init` is the
     * modern equivalent that should be used instead of `start_pes`.
     */
    shmemc_init();

    /*
     * Log a deprecation warning to inform users that this function is
     * outdated and should be replaced by `shmem_init()`.
     */
    deprecate(__func__, &v);
}
