/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"

/* 
 * If ENABLE_PSHMEM is defined, ensure that the weak versions of 
 * the accessible routines are defined and the "pshmem" versions 
 * are used when available.
 */
#ifdef ENABLE_PSHMEM
#pragma weak shmem_pe_accessible = pshmem_pe_accessible
#define shmem_pe_accessible pshmem_pe_accessible
#pragma weak shmem_addr_accessible = pshmem_addr_accessible
#define shmem_addr_accessible pshmem_addr_accessible
#endif /* ENABLE_PSHMEM */

/*
 * -------------------------------
 * routine: shmem_pe_accessible
 * ------------------------------
 * Checks if the specified PE is accessible.
 *
 * pe: The processing element to check.
 *
 * Returns: An integer indicating whether the PE is accessible (1) or not (0).
 */
int
shmem_pe_accessible(int pe)
{
    const int s = shmemc_pe_accessible(pe);

    /* Log the routine call and its result */
    logger(LOG_INFO, "%s(pe=%d) -> %d", __func__, pe, s);

    return s;
}

/*
 * -------------------------------
 * routine: shmem_addr_accessible
 * -------------------------------
 * Checks if the specified address is accessible on the given PE.
 *
 * addr: The address to check.
 * pe:   The processing element to check.
 *
 * Returns: An integer indicating whether the address is accessible (1) or not (0).
 */
int
shmem_addr_accessible(const void *addr, int pe)
{
    const int s = shmemc_addr_accessible(addr, pe);

    /* Log the routine call and its result */
    logger(LOG_INFO, "%s(addr=%p, pe=%d) -> %d", __func__, addr, pe, s);

    return s;
}
