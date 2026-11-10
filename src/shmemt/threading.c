/**
 * @file threading.c
 * @brief Threading wrapper implementation for OpenSHMEM
 *
 * This file provides a wrapper layer around pthread functionality for
 * thread management and synchronization in OpenSHMEM.
 *
 * @copyright See LICENSE file at top-level
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "threading.h"

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

/** Type alias for pthread mutex */
typedef pthread_mutex_t thr_mutex_t;

/**
 * @brief Initialize a mutex
 *
 * @param mp Pointer to mutex to initialize
 * @return 0 on success, non-zero on error
 */
int threadwrap_mutex_init(threadwrap_mutex_t *mp) {
  thr_mutex_t *tp = (thr_mutex_t *)malloc(sizeof(*tp));
  if (tp == NULL) {
    return ENOMEM;
  }

  int status = pthread_mutex_init(tp, NULL);
  if (status != 0) {
    free(tp);
    return status;
  }
  *mp = tp;
  return 0;
}

/**
 * @brief Destroy a mutex
 *
 * @param mp Pointer to mutex to destroy
 * @return 0 on success, non-zero on error
 */
int threadwrap_mutex_destroy(threadwrap_mutex_t *mp) {
  thr_mutex_t *tp = (thr_mutex_t *)*mp;

  int status = pthread_mutex_destroy(tp);
  if (status == 0) {
    free(tp);
    *mp = NULL;
  }
  return status;
}

/**
 * @brief Lock a mutex
 *
 * @param mp Pointer to mutex to lock
 * @return 0 on success, non-zero on error
 */
int threadwrap_mutex_lock(threadwrap_mutex_t *mp) {
  thr_mutex_t *tp = (thr_mutex_t *)*mp;

  return pthread_mutex_lock(tp);
}

/**
 * @brief Unlock a mutex
 *
 * @param mp Pointer to mutex to unlock
 * @return 0 on success, non-zero on error
 */
int threadwrap_mutex_unlock(threadwrap_mutex_t *mp) {
  thr_mutex_t *tp = (thr_mutex_t *)*mp;

  return pthread_mutex_unlock(tp);
}

/**
 * @brief Try to lock a mutex without blocking
 *
 * @param mp Pointer to mutex to try locking
 * @return 0 on success, non-zero on error or if mutex is already locked
 */
int threadwrap_mutex_trylock(threadwrap_mutex_t *mp) {
  thr_mutex_t *tp = (thr_mutex_t *)*mp;

  return pthread_mutex_trylock(tp);
}

/** Type alias for pthread thread handle */
typedef pthread_t thr_thread_t;

/**
 * @brief Create a new thread
 *
 * @param threadp Pointer to store new thread handle
 * @param start_routine Function pointer to thread entry point
 * @param args Arguments to pass to thread function
 * @return 0 on success, non-zero on error
 */
int threadwrap_thread_create(threadwrap_thread_t *threadp,
                             void *(*start_routine)(void *), void *args) {
  thr_thread_t *tp = (thr_thread_t *)threadp;

  return pthread_create(tp, NULL /* attr */, start_routine, args);
}

/**
 * @brief Wait for a thread to complete
 *
 * @param thread Thread handle to join
 * @param retval Pointer to store thread return value
 * @return 0 on success, non-zero on error
 */
int threadwrap_thread_join(threadwrap_thread_t thread, void **retval) {
  thr_thread_t t = (thr_thread_t)thread;

  return pthread_join(t, retval);
}

/**
 * @brief Get current thread ID
 *
 * @return Thread handle for calling thread
 */
threadwrap_thread_t threadwrap_thread_id(void) {
  thr_thread_t id = (thr_thread_t)pthread_self();

  return (threadwrap_thread_t)id;
}

/**
 * @brief Compare two thread handles for equality
 *
 * @param t1 First thread handle
 * @param t2 Second thread handle
 * @return Non-zero if threads are equal, 0 if different
 */
int threadwrap_thread_equal(threadwrap_thread_t t1, threadwrap_thread_t t2) {
  thr_thread_t tt1 = (thr_thread_t)t1;
  thr_thread_t tt2 = (thr_thread_t)t2;

  return pthread_equal(tt1, tt2);
}
