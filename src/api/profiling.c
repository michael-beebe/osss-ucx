/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"

#include <stdio.h>
#include <stdarg.h>

/*
 * ----------------------------------------
 * Routine: shmem_pcontrol
 * ----------------------------------------
 * Controls the profiling level of the OpenSHMEM library.
 *
 * The profiling levels are as follows:
 *  - level <= 0: Profiling is disabled.
 *  - level == 1: Profiling is enabled with default detail.
 *  - level == 2: Profiling is enabled, and profile buffers are flushed.
 *  - level >  2: Profiling is enabled with profile library-defined effects
 *                and additional arguments.
 *
 * Default profiling level is 1.
 *
 * Parameters:
 *  - level: The profiling level to set.
 *
 * This routine does not return a value.
 *
 * References:
 *  - OpenSHMEM Specification v1.5, p. 141.
 *    Note: The specification contains a typo regarding level 2.
 *    Assume the last entry corresponds to `level > 2`, not `>= 2`.
 */

static int profiling_level = 1; /* Default profiling level */

void
shmem_pcontrol(const int level, ...)
{
    char *msg;

    /* Determine the appropriate message based on the profiling level */
    if (level <= 0) {
        msg = "disabled";
    }
    else if (level == 1) {
        msg = "enabled (default detail)";
    }
    else if (level == 2) {
        msg = "enabled (profile buffers flushed)";
    }
    else {  /* For levels greater than 2 */
        msg = "enabled (profile library defined effects and additional arguments)";
    }

    /* Update the profiling level */
    profiling_level = level;

    /* Log the profiling level change */
    logger(LOG_INFO,
           "shmem_pcontrol(level = %d) set to \"%s\"",
           level, msg);

#ifndef ENABLE_LOGGING
    /* Avoid unused variable warnings if logging is disabled */
    NO_WARN_UNUSED(msg);
#endif /* ! ENABLE_LOGGING */
}
