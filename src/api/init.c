/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"
#include "state.h"
#include "info.h"
#include "threading.h"
#include "shmem_mutex.h"
#include "collectives/collectives.h"
#include "module.h"
#include "shmem/api.h"

#ifdef ENABLE_EXPERIMENTAL
#include "allocator/xmemalloc.h"
#endif  /* ENABLE_EXPERIMENTAL */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef ENABLE_PSHMEM
#pragma weak shmem_init_thread = pshmem_init_thread
#define shmem_init_thread pshmem_init_thread
#pragma weak shmem_init = pshmem_init
#define shmem_init pshmem_init
#pragma weak shmem_finalize = pshmem_finalize
#define shmem_finalize pshmem_finalize
#endif /* ENABLE_PSHMEM */

/*
 * ---------------------------------
 * Static helper routine: finalize_helper
 * ---------------------------------
 * Helper routine to complete the SHMEM finalization process. 
 * Ensures that resources are released and the environment is cleaned up.
 */
static void
finalize_helper(void)
{
    threadwrap_thread_t this;

    /* Do nothing if already finalized */
    if (proc.refcount < 1) {
        return;
    }

    logger(LOG_FINALIZE, "%s()", __func__);

    this = threadwrap_thread_id();
    if (this != proc.td.invoking_thread) {
        logger(LOG_FINALIZE,
               "Mismatch: thread %lu initialized, but %lu finalized",
               (unsigned long) proc.td.invoking_thread,
               (unsigned long) this);
    }

    /* Implicit barrier on finalize */
    shmem_barrier_all();

    /* Finalize all SHMEM components */
    shmemu_progress_finalize();
    shmemc_finalize();
    collectives_finalize();
    shmemt_finalize();
    shmemu_finalize();

#ifdef ENABLE_EXPERIMENTAL
    shmemxa_finalize();
#endif  /* ENABLE_EXPERIMENTAL */

    --proc.refcount;
    proc.status = SHMEMC_PE_SHUTDOWN;
}

/*
 * ---------------------------------
 * Static helper routine: init_thread_helper
 * ---------------------------------
 * Helper routine to initialize SHMEM with a specific threading level.
 *
 * requested: The desired threading level.
 * provided:  Pointer to store the provided threading level.
 *
 * Returns: 0 on success.
 */
inline static int
init_thread_helper(int requested, int *provided)
{
    int s;

    /* Do nothing if already initialized */
    if (proc.refcount > 0) {
        return 0;
    }

    /* Initialize SHMEM components */
    shmemc_init();    /* Comms */
    shmemt_init();    /* Utilities */
    shmemu_init();
    collectives_init();

#ifdef ENABLE_ALIGNED_ADDRESSES
    shmemu_test_asr_mismatch();
#endif /* ENABLE_ALIGNED_ADDRESSES */

    shmemu_progress_init();

    /* Handle threading level */
#ifdef ENABLE_THREADS
    switch(requested) {
    case SHMEM_THREAD_SINGLE:
    case SHMEM_THREAD_FUNNELED:
    case SHMEM_THREAD_SERIALIZED:
    case SHMEM_THREAD_MULTIPLE:
        break;  /* Supported threading levels */
    default:
        shmemu_fatal(MODULE ": unknown thread level %d requested", requested);
        break;
    }

    proc.td.osh_tl = requested;
#else
    proc.td.osh_tl = SHMEM_THREAD_SINGLE;
#endif /* ENABLE_THREADS */

    if (provided != NULL) {
        *provided = proc.td.osh_tl;
    }

    proc.td.invoking_thread = threadwrap_thread_id();

#ifdef ENABLE_EXPERIMENTAL
    shmemxa_init(proc.heaps.nheaps);
#endif  /* ENABLE_EXPERIMENTAL */

    /* Register atexit handler to ensure finalization */
    s = atexit(finalize_helper);
    if (s != 0) {
        shmemu_fatal(MODULE ": unable to register atexit() handler: %s",
                     strerror(errno));
    }

    proc.status = SHMEMC_PE_RUNNING;
    ++proc.refcount;

    /* Print version and environment info if requested */
    if (shmemc_my_pe() == 0) {
        if (proc.env.print_version) {
            info_output_package_version(stdout, "# ", "", 0);
        }
        if (proc.env.print_info) {
            shmemc_print_env_vars(stdout, "# ");
        }
    }

    logger(LOG_INIT,
           "%s(requested=%s [%d], provided->%s [%d])",
           __func__,
           shmemu_thread_name(requested), requested,
           shmemu_thread_name(proc.td.osh_tl), proc.td.osh_tl);

    /* Ensure symmetric memory is ready */
    shmem_barrier_all();

    return 0;  /* Success */
}

/*
 * ---------------------------------
 * Routine: shmem_finalize
 * ---------------------------------
 * Finalizes the SHMEM environment and cleans up resources.
 */
void
shmem_finalize(void)
{
    finalize_helper();
}

/*
 * ---------------------------------
 * Routine: shmem_init_thread
 * ---------------------------------
 * Initializes SHMEM with a specific threading level.
 *
 * requested: The desired threading level.
 * provided:  Pointer to store the provided threading level.
 *
 * Returns: 0 on success.
 */
int
shmem_init_thread(int requested, int *provided)
{
    return init_thread_helper(requested, provided);
}

/*
 * ---------------------------------
 * Routine: shmem_init
 * ---------------------------------
 * Initializes SHMEM with the default threading level (SHMEM_THREAD_SINGLE).
 */
void
shmem_init(void)
{
    (void) init_thread_helper(SHMEM_THREAD_SINGLE, NULL);
}

#ifdef PR470
/*
 * ---------------------------------
 * Routine: shmem_initialized
 * ---------------------------------
 * Checks if SHMEM has been initialized.
 *
 * Returns: 1 if initialized, 0 otherwise.
 */
int
shmem_initialized(void)
{
    return proc.refcount > 0;
}

/*
 * ---------------------------------
 * Routine: shmem_finalized
 * ---------------------------------
 * Checks if SHMEM has been finalized.
 *
 * Returns: 1 if finalized, 0 otherwise.
 */
int
shmem_finalized(void)
{
    return proc.refcount < 1;
}

#endif /* PR470 */
