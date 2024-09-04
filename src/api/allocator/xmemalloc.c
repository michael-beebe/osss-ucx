/* For license: see LICENSE file at top-level */

#include "xmemalloc.h"
#include "../klib/khash.h"  /* Hash table for storing heap names */

#include "internal-malloc.h" /* Memory allocation management functions */

#include <stdio.h>
#include <assert.h>

/**
 * An array of memory spaces (heaps) that the program manages.
 * Each "space" represents a separate memory pool.
 */
static mspace *spaces;

/*
 * Create a hash map (string to heap index) using the khash library.
 * The khash library provides fast operations for managing hash tables.
 */
KHASH_MAP_INIT_STR(heapnames, shmemx_heap_index_t) /* Initializes a hash map with string keys and heap index values */

/**
 * A hash table that maps heap names to indices.
 */
static khash_t(heapnames) *names;

/**
 * The current index for the next heap to be allocated.
 */
static shmemx_heap_index_t idx = 0;

/**
 * lookup_name: Search for the index associated with a heap name.
 *
 * @param name: The name of the heap.
 * @return The index of the heap if found, or -1 if not found.
 *
 * This function looks up a heap by its name in the hash table and returns its index.
 */
inline static shmemx_heap_index_t
lookup_name(const char *name)
{
    const khiter_t k = kh_get(heapnames, names, name); /* Look up the name in the hash table */

    if (k != kh_end(names)) {  /* If found */
        return kh_value(names, k); /* Return the associated index */
    }
    else {  /* Not found */
        return -1;
    }
}

/**
 * record_name: Associate a new heap name with an index.
 *
 * @param name: The name of the heap.
 * @return The index of the newly created heap, or -1 on error.
 *
 * This function assigns a new name to a heap and records it in the hash table.
 */
inline static shmemx_heap_index_t
record_name(const char *name)
{
    int there;
    khiter_t k;
    const shmemx_heap_index_t mine = idx; /* Assign current index */

    k = kh_put(heapnames, names, name, &there); /* Insert the name into the hash table */
    if (there != 1) {
        return -1;  /* If insertion failed, return error */
        /* NOT REACHED */
    }

    kh_value(names, k) = mine;  /* Assign the heap index to the name */

    ++idx;  /* Increment the index for the next heap */

    return mine;  /* Return the assigned index */
}

/**
 * shmemxa_name_to_index: Convert a heap name to an index.
 *
 * @param name: The name of the heap.
 * @return The index associated with the name, or create and return a new index if not found.
 *
 * If the heap name is found, return the corresponding index. Otherwise, create a new entry.
 */
shmemx_heap_index_t
shmemxa_name_to_index(const char *name)
{
    const shmemx_heap_index_t i = lookup_name(name); /* Look up the name */

    if (i == -1) {  /* If name not found, record it */
        return record_name(name);
    }
    else {
        return i;  /* Return the existing index */
    }
}

/**
 * shmemxa_index_to_name: Convert a heap index to a heap name.
 *
 * @param index: The index of the heap.
 * @return The name associated with the heap index, or NULL if not found.
 *
 * This function reverses the mapping: it looks up a heap index and returns the associated name.
 */
char *
shmemxa_index_to_name(shmemx_heap_index_t index)
{
    khiter_t k;

    for (k = kh_begin(names); k != kh_end(names); ++k) {
        if (kh_exist(names, k)) { /* If the entry exists in the hash table */
            if (kh_value(names, k) == index) {  /* If the index matches */
                return (char *) kh_key(names, k); /* Return the name (key) */
                /* NOT REACHED */
            }
        }
    }

    return NULL;  /* Return NULL if not found */
}

/*
 * Boot API: Initialize or finalize the entire heap system.
 */

/**
 * shmemxa_init: Initialize the heap management system with a given number of heaps.
 *
 * @param numheaps: The number of heaps to manage.
 *
 * This function allocates memory for the specified number of heaps and prepares them for use.
 */
void
shmemxa_init(shmemx_heap_index_t numheaps)
{
    spaces = (mspace *) malloc(numheaps * sizeof(*spaces));  /* Allocate space for the heaps */

    assert(spaces != NULL);  /* Ensure memory was allocated */

    nheaps = numheaps;  /* Store the number of heaps */
}

/**
 * shmemxa_finalize: Clean up the heap management system by freeing resources.
 *
 * This function frees the memory allocated for managing heaps.
 */
void
shmemxa_finalize(void)
{
    free(spaces);  /* Free the memory space */
}

/*
 * Manage individual heaps by their index.
 */

/**
 * shmemxa_init_by_index: Initialize a heap by its index.
 *
 * @param index: The index of the heap to initialize.
 * @param base: The base address of the heap.
 * @param capacity: The size of the heap.
 *
 * This function sets up a specific heap by index with the provided base address and capacity.
 */
void
shmemxa_init_by_index(shmemx_heap_index_t index,
                      void *base, size_t capacity)
{
    spaces[index] = create_mspace_with_base(base, capacity, 1); /* Create a memory space for the heap */
}

/**
 * shmemxa_finalize_by_index: Clean up a heap by its index.
 *
 * @param index: The index of the heap to finalize.
 *
 * This function destroys the heap at the given index.
 */
void
shmemxa_finalize_by_index(shmemx_heap_index_t index)
{
    destroy_mspace(spaces[index]);  /* Destroy the memory space for the heap */
}

/*
 * Heap allocation operations.
 */

/**
 * shmemxa_base_by_index: Get the base address of a heap by its index.
 *
 * @param index: The index of the heap.
 * @return The base address of the heap.
 */
void *
shmemxa_base_by_index(shmemx_heap_index_t index)
{
    return spaces[index];  /* Return the base address of the heap */
}

/**
 * shmemxa_malloc_by_index: Allocate memory from a heap by its index.
 *
 * @param index: The index of the heap to allocate from.
 * @param size: The number of bytes to allocate.
 * @return A pointer to the allocated memory.
 */
void *
shmemxa_malloc_by_index(shmemx_heap_index_t index,
                        size_t size)
{
    return mspace_malloc(spaces[index], size);  /* Allocate memory from the heap */
}

/**
 * shmemxa_calloc_by_index: Allocate and zero-initialize memory from a heap by its index.
 *
 * @param index: The index of the heap.
 * @param count: The number of elements to allocate.
 * @param size: The size of each element.
 * @return A pointer to the allocated memory.
 */
void *
shmemxa_calloc_by_index(shmemx_heap_index_t index,
                        size_t count, size_t size)
{
    return mspace_calloc(spaces[index], count, size);  /* Allocate and zero-initialize memory */
}

/**
 * shmemxa_free_by_index: Free memory from a heap by its index.
 *
 * @param index: The index of the heap.
 * @param addr: The address of the memory to free.
 */
void
shmemxa_free_by_index(shmemx_heap_index_t index,
                      void *addr)
{
    mspace_free(spaces[index], addr);  /* Free the memory in the heap */
}

/**
 * shmemxa_realloc_by_index: Reallocate memory from a heap by its index.
 *
 * @param index: The index of the heap.
 * @param addr: The address of the memory to resize.
 * @param new_size: The new size of the memory.
 * @return A pointer to the reallocated memory.
 */
void *
shmemxa_realloc_by_index(shmemx_heap_index_t index,
                         void *addr, size_t new_size)
{
    return mspace_realloc(spaces[index], addr, new_size);  /* Reallocate memory */
}

/**
 * shmemxa_align_by_index: Allocate aligned memory from a heap by its index.
 *
 * @param index: The index of the heap.
 * @param alignment: The alignment requirement (e.g., 16 bytes).
 * @param size: The size of the memory to allocate.
 * @return A pointer to the aligned memory.
 */
void *
shmemxa_align_by_index(shmemx_heap_index_t index,
                       size_t alignment, size_t size)
{
    return mspace_memalign(spaces[index], alignment, size);  /* Allocate aligned memory */
}
