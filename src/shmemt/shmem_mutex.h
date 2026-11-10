/**
 * @file shmem_mutex.h
 * @brief Threading mutex interface for OpenSHMEM
 *
 * This header defines the mutex operations used for thread synchronization
 * in OpenSHMEM. The mutex functionality is only enabled when threading
 * support is compiled in.
 *
 * @copyright See LICENSE file at top-level
 */

#ifndef _SHMEM_MUTEX_H
#define _SHMEM_MUTEX_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef ENABLE_THREADS

/**
 * @brief Initialize the threading subsystem
 */
void shmemt_init(void);

/**
 * @brief Finalize the threading subsystem
 *
 * Destroy the communications mutex for the current generation.
 */
void shmemt_finalize(void);

/**
 * @brief Initialize mutex for thread synchronization
 */
void shmemt_mutex_init(void);

/**
 * @brief Destroy mutex used for thread synchronization
 */
void shmemt_mutex_destroy(void);

/**
 * @brief Acquire the communications mutex lock
 */
void shmemt_mutex_lock(void);

/**
 * @brief Release the communications mutex lock
 */
void shmemt_mutex_unlock(void);

/**
 * @brief Execute a function with mutex protection
 *
 * @param _fn Function to execute within mutex lock/unlock
 */
#define SHMEMT_MUTEX_PROTECT(_fn)                                              \
  do {                                                                         \
    shmemt_mutex_lock();                                                       \
    _fn;                                                                       \
    shmemt_mutex_unlock();                                                     \
  } while (0)

/**
 * @brief Execute a function without mutex protection
 *
 * @param _fn Function to execute without mutex protection
 */
#define SHMEMT_MUTEX_NOPROTECT(_fn)                                            \
  do {                                                                         \
    _fn;                                                                       \
  } while (0)

#else

#define shmemt_init()
#define shmemt_finalize()

#define SHMEMT_MUTEX_PROTECT(_fn) _fn
#define SHMEMT_MUTEX_NOPROTECT(_fn) _fn

#endif /* ENABLE_THREADS */

#endif /* ! _SHMEM_MUTEX_H */
