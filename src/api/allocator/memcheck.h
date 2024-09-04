/* For license: see LICENSE file at top-level */

#ifndef _MEMCHECK_H
#define _MEMCHECK_H 1

/*
 * Memory Check Interface
 *
 * This header defines custom error handling functions for memory 
 * corruption and usage errors that can occur during memory operations 
 * within a custom memory pool (`mspace`). These functions are used 
 * internally by the memory management system to detect and handle 
 * errors during dynamic memory operations.
 */

/**
 * Report memory corruption
 *
 * @param m A pointer to the memory pool (`mspace`) where corruption is detected.
 *
 * This function is called when memory corruption is detected within the 
 * specified memory pool. The implementation of this function should handle 
 * the error, typically by logging the issue and terminating the program 
 * to avoid undefined behavior.
 */
extern void report_corruption(mspace m);

/**
 * Report a usage error in memory operations
 *
 * @param m A pointer to the memory pool (`mspace`) where the error occurred.
 * @param p A pointer to the memory block that caused the error.
 *
 * This function is called when a memory usage error (such as double freeing 
 * memory or accessing memory out of bounds) is detected. The implementation 
 * should log the issue and handle the error appropriately.
 */
extern void report_usage_error(mspace m, void *p);

#endif /* ! _MEMCHECK_H */
