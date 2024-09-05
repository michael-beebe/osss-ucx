/* For license: see LICENSE file at top-level */

/* Check if the config file exists and include it if present */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"   /* Include for SHMEM utilities and error checking */
#include "state.h"    /* Include for accessing SHMEM state variables */

#ifdef ENABLE_PSHMEM
/*
 * If profiling (PSHMEM) is enabled, create a weak symbol for the profiling version
 * of shmem_query_thread. If not, it will link to the default implementation.
 */
#pragma weak shmem_query_thread = pshmem_query_thread
#define shmem_query_thread pshmem_query_thread
#endif /* ENABLE_PSHMEM */

/*
 * Query the current thread level support provided by the OpenSHMEM implementation.
 * The provided value is filled with the thread level support that was set during initialization.
 * 
 * @param provided: Output parameter where the thread level support will be stored.
 * 
 * Thread levels:
 * - SHMEM_THREAD_SINGLE: Only one thread will execute.
 * - SHMEM_THREAD_FUNNELED: Multiple threads but only one can make SHMEM calls.
 * - SHMEM_THREAD_SERIALIZED: Multiple threads, one at a time can make SHMEM calls.
 * - SHMEM_THREAD_MULTIPLE: Multiple threads can call SHMEM concurrently.
 */
void
shmem_query_thread(int *provided)
{
    /* Ensure SHMEM is initialized before querying thread level */
    SHMEMU_CHECK_INIT();

    /* Check that the provided pointer is not NULL to avoid dereferencing a null pointer */
    SHMEMU_CHECK_NOT_NULL(provided, 1);

    /* Log the query and the current thread level support */
    logger(LOG_INFO, "%s() -> %d", __func__, proc.td.osh_tl);

    /* Set the provided thread level to the value stored in proc.td.osh_tl */
    *provided = proc.td.osh_tl;
}
