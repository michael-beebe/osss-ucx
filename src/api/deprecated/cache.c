/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"

/*
 * These are compatibility routines for older SGI architectures.
 * The functions in this file were used for cache management on older SGI
 * systems, but they no longer have any effect on modern architectures.
 * In OpenSHMEM, these functions have been defined to do nothing (no-op).
 * This ensures backward compatibility with legacy code that still calls
 * these routines, but without any actual functionality.
 */

#ifdef ENABLE_PSHMEM
/*
 * PSHMEM (Profiling SHMEM) support: These weak symbols allow profiling
 * libraries to override the cache routines, enabling performance analysis.
 */
#pragma weak shmem_clear_cache_inv = pshmem_clear_cache_inv
#define shmem_clear_cache_inv pshmem_clear_cache_inv
#pragma weak shmem_set_cache_inv = pshmem_set_cache_inv
#define shmem_set_cache_inv pshmem_set_cache_inv
#pragma weak shmem_set_cache_line_inv = pshmem_set_cache_line_inv
#define shmem_set_cache_line_inv pshmem_set_cache_line_inv
#pragma weak shmem_clear_cache_line_inv = pshmem_clear_cache_line_inv
#define shmem_clear_cache_line_inv pshmem_clear_cache_line_inv
#pragma weak shmem_udcflush = pshmem_udcflush
#define shmem_udcflush pshmem_udcflush
#pragma weak shmem_udcflush_line = pshmem_udcflush_line
#define shmem_udcflush_line pshmem_udcflush_line
#endif /* ENABLE_PSHMEM */

/*
 * Compatibility no-op cache routines
 *
 * These functions were originally used to manage cache coherency on
 * SGI systems. As of OpenSHMEM 1.3, they are deprecated and have no effect.
 * Each function now logs a deprecation warning but does nothing else.
 */

static const shmemu_version_t v = { .major = 1, .minor = 3 };  /* Version 1.3: when these functions were deprecated */

/* 
 * This function was originally intended to invalidate the entire cache.
 * It now does nothing and logs a deprecation warning.
 */
void
shmem_set_cache_inv(void)
{
    deprecate(__func__, &v);  /* Log deprecation message */
}

/* 
 * This function was originally intended to clear the cache invalidation.
 * It is now a no-op and logs a deprecation warning.
 */
void
shmem_clear_cache_inv(void)
{
    deprecate(__func__, &v);  /* Log deprecation message */
}

/* 
 * This function was used to invalidate a specific cache line.
 * It now does nothing, but still logs a deprecation warning.
 */
void
shmem_set_cache_line_inv(void *target)
{
    NO_WARN_UNUSED(target);  /* Suppress compiler warnings about unused variables */

    deprecate(__func__, &v);  /* Log deprecation message */
}

/* 
 * This function was used to clear the invalidation for a specific cache line.
 * Like the others, it now does nothing but logs a deprecation warning.
 */
void
shmem_clear_cache_line_inv(void *target)
{
    NO_WARN_UNUSED(target);  /* Suppress compiler warnings about unused variables */

    deprecate(__func__, &v);  /* Log deprecation message */
}

/* 
 * This function was used to flush the entire user cache.
 * Now it does nothing and logs a deprecation warning.
 */
void
shmem_udcflush(void)
{
    deprecate(__func__, &v);  /* Log deprecation message */
}

/* 
 * This function was used to flush a specific cache line.
 * It now does nothing but logs a deprecation warning.
 */
void
shmem_udcflush_line(void *target)
{
    NO_WARN_UNUSED(target);  /* Suppress compiler warnings about unused variables */

    deprecate(__func__, &v);  /* Log deprecation message */
}
