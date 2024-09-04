/* For license: see LICENSE file at top-level */

#include "internal-malloc.h"  /* Provides mspace-related functions */
#include "memalloc.h"         /* Declares the interface for this module */

/**
 * The memory area we manage in this unit.
 *
 * This `mspace` instance, `myspace`, is the memory pool that this module manages. 
 * It is initialized in `shmema_init` and used for all dynamic memory allocations.
 * 
 * This variable is not visible outside this file, ensuring encapsulation of the
 * memory management logic.
 */
static mspace myspace;

/**
 * Initialize the memory pool
 *
 * This function is responsible for setting up the memory space (`mspace`) with a given 
 * base address and capacity. The memory pool is created using the 
 * `create_mspace_with_base()` function from `internal-malloc.h`.
 *
 * The `locked` argument (set to 1) ensures that the space is thread-safe.
 */
void
shmema_init(void *base, size_t capacity)
{
    myspace = create_mspace_with_base(base, capacity, 1);
}

/**
 * Clean up the memory pool
 *
 * `shmema_finalize()` destroys the memory pool (`mspace`) created earlier 
 * in `shmema_init()`. This should be called to free resources when the 
 * memory pool is no longer needed.
 */
void
shmema_finalize(void)
{
    destroy_mspace(myspace);
}

/**
 * Return the start of the memory pool
 *
 * This function returns the base address of the memory pool (`mspace`). 
 * It is typically used to get a reference to the starting point of the memory 
 * space for low-level operations or for debugging purposes.
 */
void *
shmema_base(void)
{
    return myspace;
}

/**
 * Allocate `size` bytes from the memory pool
 *
 * `shmema_malloc()` allocates a block of memory of `size` bytes from the 
 * memory pool and returns a pointer to the allocated memory. The allocation 
 * happens within the `myspace` memory pool.
 */
void *
shmema_malloc(size_t size)
{
    return mspace_malloc(myspace, size);
}

/**
 * Allocate `count * size` bytes from the memory pool, zeroed out
 *
 * `shmema_calloc()` allocates a block of memory for an array of `count` 
 * elements, each of `size` bytes, and initializes the memory to zero.
 */
void *
shmema_calloc(size_t count, size_t size)
{
    return mspace_calloc(myspace, count, size);
}

/**
 * Free memory previously allocated at `addr`
 *
 * `shmema_free()` releases the memory pointed to by `addr` back to the memory pool.
 */
void
shmema_free(void *addr)
{
    mspace_free(myspace, addr);
}

/**
 * Resize a previously allocated block of memory
 *
 * `shmema_realloc()` changes the size of the memory block pointed to by `addr` 
 * to `new_size`. It may move the block to a new location and returns a pointer 
 * to the new block.
 */
void *
shmema_realloc(void *addr, size_t new_size)
{
    return mspace_realloc(myspace, addr, new_size);
}

/**
 * Allocate memory of `size` bytes, aligned to `alignment`
 *
 * `shmema_align()` allocates `size` bytes of memory, ensuring that the memory 
 * address is aligned to the specified `alignment`.
 */
void *
shmema_align(size_t alignment, size_t size)
{
    return mspace_memalign(myspace, alignment, size);
}
