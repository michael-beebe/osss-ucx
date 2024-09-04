/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmem/api.h"

/*
 * This file provides deprecated memory allocation routines for 
 * OpenSHMEM. These functions, such as `shmalloc` and `shfree`, were 
 * used in earlier versions of OpenSHMEM but have been replaced by 
 * modern equivalents like `shmem_malloc` and `shmem_free`.
 * 
 * These deprecated functions are kept for backward compatibility and 
 * call their modern counterparts after issuing a deprecation warning.
 */

#ifdef ENABLE_PSHMEM
/*
 * PSHMEM (Profiling SHMEM) support: 
 * Weak symbols allow profiling tools to intercept the deprecated 
 * memory functions such as `shmalloc`, `shfree`, and `shrealloc`.
 */
#pragma weak shmalloc = pshmalloc
#define shmalloc pshmalloc
#pragma weak shfree = pshfree
#define shfree pshfree
#pragma weak shrealloc = pshrealloc
#define shrealloc pshrealloc
#pragma weak shmemalign = pshmemalign
#define shmemalign pshmemalign
#endif /* ENABLE_PSHMEM */

/*
 * Define the version in which these functions were deprecated.
 * In this case, they were deprecated in OpenSHMEM 1.2.
 */
static const shmemu_version_t v = { .major = 1, .minor = 2 };

/*
 * Deprecated memory allocation function `shmalloc`:
 * This function allocates `s` bytes of memory in the symmetric heap.
 * It has been replaced by `shmem_malloc()`.
 */
void *
shmalloc(size_t s)
{
    /* Log a deprecation warning */
    deprecate(__func__, &v);

    /* Call the modern equivalent `shmem_malloc()` */
    return shmem_malloc(s);
}

/*
 * Deprecated memory deallocation function `shfree`:
 * This function frees memory pointed to by `p` in the symmetric heap.
 * It has been replaced by `shmem_free()`.
 */
void
shfree(void *p)
{
    /* Log a deprecation warning */
    deprecate(__func__, &v);

    /* Call the modern equivalent `shmem_free()` */
    shmem_free(p);
}

/*
 * Deprecated memory reallocation function `shrealloc`:
 * This function resizes memory at `p` to `s` bytes.
 * It has been replaced by `shmem_realloc()`.
 */
void *
shrealloc(void *p, size_t s)
{
    /* Log a deprecation warning */
    deprecate(__func__, &v);

    /* Call the modern equivalent `shmem_realloc()` */
    return shmem_realloc(p, s);
}

/*
 * Deprecated aligned memory allocation function `shmemalign`:
 * This function allocates `s` bytes of memory aligned to `a` bytes.
 * It has been replaced by `shmem_align()`.
 */
void *
shmemalign(size_t a, size_t s)
{
    /* Log a deprecation warning */
    deprecate(__func__, &v);

    /* Call the modern equivalent `shmem_align()` */
    return shmem_align(a, s);
}
