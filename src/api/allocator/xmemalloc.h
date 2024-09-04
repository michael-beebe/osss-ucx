/* For license: see LICENSE file at top-level */

#ifndef _SHMEMXA_MEMALLOC_H
#define _SHMEMXA_MEMALLOC_H 1

#include <sys/types.h>  /* size_t for defining memory sizes */

/*
 * translate between heap names and indices
 *
 * This section defines functions to map heap names (strings) to indices,
 * which are used to reference specific heaps for memory management.
 */

/* Define a type for heap index, used to identify different heaps */
typedef int shmemx_heap_index_t;

/* Function to convert a heap name to its index (creates one if not found) */
shmemx_heap_index_t shmemxa_name_to_index(const char *name);

/* Function to retrieve the name of a heap given its index */
char *shmemxa_index_to_name(shmemx_heap_index_t index);

/*
 * memory allocation
 *
 * Functions to initialize, allocate, and manage memory across multiple heaps.
 * Heaps can be individually initialized and managed by their indices.
 */

/* Initialize the memory system for a given number of heaps */
void shmemxa_init(shmemx_heap_index_t numheaps);

/* Finalize and clean up the memory system */
void shmemxa_finalize(void);

/* Initialize a specific heap by its index, with a base address and capacity */
void shmemxa_init_by_index(shmemx_heap_index_t index,
                           void *base, size_t capacity);

/* Finalize a specific heap by its index */
void shmemxa_finalize_by_index(shmemx_heap_index_t index);

/* Retrieve the base address of a heap by its index */
void *shmemxa_base_by_index(shmemx_heap_index_t index);

/* Allocate memory from a heap by its index */
void *shmemxa_malloc_by_index(shmemx_heap_index_t index,
                              size_t size);

/* Allocate and zero-initialize memory from a heap by its index */
void *shmemxa_calloc_by_index(shmemx_heap_index_t index,
                              size_t count, size_t size);

/* Free memory from a heap by its index */
void shmemxa_free_by_index(shmemx_heap_index_t index,
                           void *addr);

/* Reallocate memory from a heap by its index */
void *shmemxa_realloc_by_index(shmemx_heap_index_t index,
                               void *addr, size_t new_size);

/* Allocate aligned memory from a heap by its index */
void *shmemxa_align_by_index(shmemx_heap_index_t index,
                             size_t alignment, size_t size);

#endif /* ! _SHMEMXA_MEMALLOC_H */
