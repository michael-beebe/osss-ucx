#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_PSHMEM
#pragma weak shmem_init = pshmem_init
#define shmem_init pshmem_init
#pragma weak shmem_finalize = pshmem_finalize
#define shmem_finalize pshmem_finalize

#pragma weak start_pes = pstart_pes
#define start_pes pstart_pes
#endif /* ENABLE_PSHMEM */

void
shmem_init(void)
{
    shmemu_init();
    shmemc_init();

    /* urgh! */
}

void
shmem_finalize(void)
{
    shmemc_finalize();
    shmemu_finalize();
}

void
start_pes(int n /* unused */)
{
    deprecate(__func__);
    shmem_init();
}
