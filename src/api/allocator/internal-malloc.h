/* For license: see LICENSE file at top-level */

/*
 * This header is adapted from dlmalloc.c to provide a standalone
 * memory management interface specifically for OpenSHMEM.
 * 
 * dlmalloc is a widely-used memory allocator, and this file exposes 
 * a subset of its functionality that OpenSHMEM depends on.
 *
 * The license for this file follows the same terms as dlmalloc.c.
 */

#ifndef _DLMALLOC_H
#define _DLMALLOC_H 1

#include <sys/types.h>

/*
 * Definition of `mspace`:
 * 
 * `mspace` is an opaque data type that represents a memory space 
 * used for dynamic memory management. Each `mspace` is a separate 
 * heap area that can allocate and free memory independently.
 */
typedef void *mspace;

/*
 * Functions to manage mspaces.
 *
 * These functions are wrappers around the underlying dlmalloc routines
 * but are adapted to be used with OpenSHMEM's symmetric heap memory model.
 */

/* 
 * Creates an mspace with a specific base address and capacity. 
 * The `locked` parameter specifies whether or not to use locking for 
 * thread safety.
 */
extern mspace  create_mspace_with_base(void *base,
                                       size_t capacity,
                                       int locked);

/* 
 * Destroys the given mspace and frees associated resources.
 * Returns the size of the space that was allocated to the mspace.
 */
extern size_t  destroy_mspace(mspace msp);

/* 
 * Allocates `bytes` of memory from the specified mspace.
 */
extern void   *mspace_malloc(mspace msp, size_t bytes);

/* 
 * Allocates memory for an array of `count` elements, each of size `bytes`, 
 * from the mspace, and initializes the memory to zero.
 */
extern void   *mspace_calloc(mspace msp, size_t count, size_t bytes);

/* 
 * Resizes the memory block `mem` in the specified mspace to the new size `newsize`.
 */
extern void   *mspace_realloc(mspace msp, void *mem, size_t newsize);

/* 
 * Allocates `bytes` of memory aligned to `alignment` bytes from the mspace.
 */
extern void   *mspace_memalign(mspace msp, size_t alignment, size_t bytes);

/* 
 * Frees the memory block `mem` in the specified mspace.
 */
extern void    mspace_free(mspace msp, void *mem);

/* 
 * Returns the current size (footprint) of the given mspace.
 */
extern size_t  mspace_footprint(mspace msp);

#endif /* ! _DLMALLOC_H */
