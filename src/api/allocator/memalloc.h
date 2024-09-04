/* For license: see LICENSE file at top-level */

#ifndef _SHMEMA_MEMALLOC_H
#define _SHMEMA_MEMALLOC_H 1

#include <sys/types.h>  /* For size_t type */

/*
 * Memory Allocation Interface
 *
 * This header defines the interface for the custom memory allocation 
 * module (`shmema_*` functions), which handles dynamic memory management 
 * using a memory pool (`mspace`) created and managed by the underlying 
 * dlmalloc system.
 */

/**
 * Initialize the memory pool
 * 
 * @param base The starting address of the memory pool.
 * @param capacity The size of the memory pool in bytes.
 *
 * This function sets up a memory pool with a specified base address and size, 
 * which will be used for dynamic memory allocation within the OpenSHMEM library.
 */
void shmema_init(void *base, size_t capacity);

/**
 * Finalize the memory pool
 *
 * This function releases all resources used by the memory pool. It should 
 * be called when the memory pool is no longer needed.
 */
void shmema_finalize(void);

/**
 * Get the base address of the memory pool
 *
 * @return The starting address of the memory pool.
 *
 * This function returns the base address of the memory space managed 
 * by the memory pool. It may be used for internal debugging or low-level 
 * memory operations.
 */
void *shmema_base(void);

/**
 * Allocate memory from the memory pool
 * 
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or `NULL` if the allocation fails.
 *
 * This function allocates a block of memory of `size` bytes from the memory pool.
 */
void *shmema_malloc(size_t size);

/**
 * Allocate and zero memory from the memory pool
 *
 * @param count The number of elements to allocate.
 * @param size The size of each element.
 * @return A pointer to the allocated memory, or `NULL` if the allocation fails.
 *
 * This function allocates memory for an array of `count` elements, 
 * each of `size` bytes, and initializes the allocated memory to zero.
 */
void *shmema_calloc(size_t count, size_t size);

/**
 * Free memory previously allocated from the memory pool
 *
 * @param addr A pointer to the memory to be freed.
 *
 * This function frees the memory block pointed to by `addr`, returning 
 * it back to the memory pool for future allocations.
 */
void shmema_free(void *addr);

/**
 * Resize a memory block allocated from the memory pool
 *
 * @param addr A pointer to the memory block to be resized.
 * @param new_size The new size for the memory block.
 * @return A pointer to the resized memory block, or `NULL` if resizing fails.
 *
 * This function changes the size of the memory block pointed to by `addr` 
 * to `new_size`. The block may be moved to a new location if necessary.
 */
void *shmema_realloc(void *addr, size_t new_size);

/**
 * Allocate aligned memory from the memory pool
 *
 * @param alignment The alignment requirement for the allocated memory.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or `NULL` if the allocation fails.
 *
 * This function allocates `size` bytes of memory from the memory pool, 
 * ensuring that the memory address is aligned to the specified `alignment`.
 */
void *shmema_align(size_t alignment, size_t size);

#endif /* ! _SHMEMA_MEMALLOC_H */
