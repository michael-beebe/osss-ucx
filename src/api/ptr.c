/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"

#ifdef ENABLE_PSHMEM
/*
 * Define weak version of shmem_ptr for profiling if ENABLE_PSHMEM is set
 */
#pragma weak shmem_ptr = pshmem_ptr
#define shmem_ptr pshmem_ptr
#endif /* ENABLE_PSHMEM */

/*
 * ----------------------------------------
 * Routine: shmem_ptr
 * ----------------------------------------
 * Provides a local address that can be used to directly access the
 * remote memory of the specified processing element (PE).
 *
 * Parameters:
 *  - target: The address of the symmetric data object on the remote PE.
 *  - pe: The processing element to which the target address belongs.
 *
 * Returns:
 *  - A pointer to the symmetric object if it is accessible in the local
 *    address space. Returns NULL if the symmetric object is not accessible
 *    locally.
 */
void *
shmem_ptr(const void *target, int pe)
{
    /* Use the default context to retrieve the pointer to the remote memory */
    void *rw = shmemc_ctx_ptr(SHMEM_CTX_DEFAULT, target, pe);

    /* Log the result of the shmem_ptr call */
    logger(LOG_MEMORY,
           "%s(target=%p, pe=%d) -> %p",
           __func__,
           target, pe, rw
           );

    return rw; /* Return the local pointer to the remote memory */
}
