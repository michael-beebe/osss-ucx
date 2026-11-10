/* For license: see LICENSE file at top-level */

/**
 * @file init.c
 * @brief Implementation of OpenSHMEM initialization and finalization routines
 *
 * This file contains implementations of routines to initialize and finalize
 * the OpenSHMEM library, including thread initialization and cleanup.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"
#include "shmemc.h"
#include "state.h"
#include "info.h"
#include "threading.h"
#include "shmem_mutex.h"
#include "collectives/collectives.h"
#include "module.h"
#include "shmem/api.h"

#ifdef ENABLE_EXPERIMENTAL
#include "allocator/xmemalloc.h"
#endif /* ENABLE_EXPERIMENTAL */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef ENABLE_PSHMEM
#pragma weak shmem_init_thread = pshmem_init_thread
#define shmem_init_thread pshmem_init_thread
#pragma weak shmem_init = pshmem_init
#define shmem_init pshmem_init
#pragma weak shmem_finalize = pshmem_finalize
#define shmem_finalize pshmem_finalize
#pragma weak shmem_query_initialized = pshmem_query_initialized
#define shmem_query_initialized pshmem_query_initialized
#endif /* ENABLE_PSHMEM */

/*
 * finish SHMEM portion of program, release resources
 */

/**
 * @brief Helper function to finalize the OpenSHMEM library
 *
 * This internal function handles the cleanup of OpenSHMEM resources,
 * including thread management, communications, collectives, and other
 * subsystems.
 */
static void finalize_helper(bool at_exit) {
  threadwrap_thread_t this;
  const int references = __atomic_load_n(&proc.refcount, __ATOMIC_ACQUIRE);

  /* do nothing if multiple finalizes */
  if (references < 1) {
    return;
  }

  logger(LOG_FINALIZE, "%s()", __func__);

  this = threadwrap_thread_id();
  if (this != proc.td.invoking_thread) {

    logger(LOG_FINALIZE, "mis-match: thread %lu initialized, but %lu finalized",
           (unsigned long)proc.td.invoking_thread, (unsigned long)this);
  }

  /* implicit barrier on finalize */
  shmem_barrier_all();

  /* Intermediate finalizes are barriers and keep the runtime usable. */
  if (!at_exit && references > 1) {
    __atomic_sub_fetch(&proc.refcount, 1, __ATOMIC_RELEASE);
    return;
  }

  shmemu_progress_finalize();

  shmemc_finalize();
  collectives_finalize();
  shmemt_finalize();
  shmemu_finalize();

#ifdef ENABLE_EXPERIMENTAL
  shmemxa_finalize();
#endif /* ENABLE_EXPERIMENTAL */

  proc.status = SHMEMC_PE_SHUTDOWN;
  __atomic_store_n(&proc.refcount, 0, __ATOMIC_RELEASE);
}

/** @brief Release the active generation on process exit, regardless of nesting.
 */
static void finalize_at_exit(void) { finalize_helper(true); }

/** Register the process exit handler once, across all runtime generations. */
static bool exit_handler_registered = false;

/**
 * @brief Helper function to initialize the OpenSHMEM library with threading
 * support
 *
 * @param requested The requested threading level
 * @param provided Pointer to store the provided threading level
 * @return 0 on success, non-zero on failure
 *
 * This internal function handles the initialization of OpenSHMEM resources,
 * including communications, thread management, and other subsystems.
 */
inline static int init_thread_helper(int requested, int *provided) {
  int s;

  /* Retain the established thread support for nested initialization calls. */
  if (__atomic_load_n(&proc.refcount, __ATOMIC_ACQUIRE) > 0) {
    if (provided != NULL) {
      *provided = proc.td.osh_tl;
    }
    __atomic_add_fetch(&proc.refcount, 1, __ATOMIC_RELEASE);
    return 0;
  }

  /* UCX setup must see the thread level of this generation. */
#ifdef ENABLE_THREADS
  switch (requested) {
  case SHMEM_THREAD_SINGLE:
  case SHMEM_THREAD_FUNNELED:
  case SHMEM_THREAD_SERIALIZED:
  case SHMEM_THREAD_MULTIPLE:
    break; /* nothing to do for now */
  default:
    shmemu_fatal(MODULE ": unknown thread level %d requested", requested);
    /* NOT REACHED */
    break;
  }

  proc.td.osh_tl = requested;
#else
  proc.td.osh_tl = SHMEM_THREAD_SINGLE;
#endif /* ENABLE_THREADS */

  proc.td.invoking_thread = threadwrap_thread_id();

  /* set up comms, read environment */
  shmemc_init();
  shmemt_init();
  shmemu_init();
  collectives_init();

#ifdef ENABLE_ALIGNED_ADDRESSES
  shmemu_test_asr_mismatch();
#endif /* ENABLE_ALIGNED_ADDRESSES */

  shmemu_progress_init();

  if (provided != NULL) {
    *provided = proc.td.osh_tl;
  }

#ifdef ENABLE_EXPERIMENTAL
  shmemxa_init(proc.heaps.nheaps);
#endif /* ENABLE_EXPERIMENTAL */

  if (!exit_handler_registered) {
    s = atexit(finalize_at_exit);
    if (s != 0) {
      shmemu_fatal(MODULE ": unable to register atexit() handler: %s",
                   strerror(errno));
      /* NOT REACHED */
    }
    exit_handler_registered = true;
  }

  proc.status = SHMEMC_PE_RUNNING;

  __atomic_add_fetch(&proc.refcount, 1, __ATOMIC_RELEASE);

  if (shmemc_my_pe() == 0) {
    if (proc.env.print_version) {
      info_output_package_version(stdout, "# ", "", 0);
    }
    if (proc.env.print_info) {
      shmemc_print_env_vars(stdout, "# ");
    }
  }

  logger(LOG_INIT, "%s(requested=%s [%d], provided->%s [%d])", __func__,
         shmemu_thread_name(requested), requested,
         shmemu_thread_name(proc.td.osh_tl), proc.td.osh_tl);

  /* make sure all symmetric memory ready */
  shmem_barrier_all();

  /* just declare success */
  return 0;
}

/*
 * initialize/finalize SHMEM portion of program with threading model
 */

/**
 * @brief Finalize the OpenSHMEM library
 *
 * Every call is collective. Only the final matching call releases resources;
 * subsequent initialization starts a fresh OpenSHMEM generation.
 */
void shmem_finalize(void) { finalize_helper(false); }

/**
 * @brief Initialize the OpenSHMEM library with threading support
 *
 * @param requested The requested threading level
 * @param provided Pointer to store the provided threading level
 * @return 0 on success, non-zero on failure
 *
 * Initialize the OpenSHMEM library with a specified threading level.
 */
int shmem_init_thread(int requested, int *provided) {
  return init_thread_helper(requested, provided);
}

/**
 * @brief Initialize the OpenSHMEM library
 *
 * Initialize the OpenSHMEM library with single threading level.
 */
void shmem_init(void) { (void)init_thread_helper(SHMEM_THREAD_SINGLE, NULL); }

/**
 * @brief Reports initialization status without using initialized subsystems.
 * @param[out] initialized Nonzero while the library is initialized.
 *
 * Atomic access permits queries from any thread, including before the mutex,
 * logger, or communications layer has been initialized.
 */
void shmem_query_initialized(int *initialized) {
  *initialized = __atomic_load_n(&proc.refcount, __ATOMIC_ACQUIRE) > 0;
}

#ifdef PR470

/**
 * @brief Check if OpenSHMEM library is initialized
 *
 * @return Non-zero if initialized, 0 otherwise
 */
int shmem_initialized(void) {
  return __atomic_load_n(&proc.refcount, __ATOMIC_ACQUIRE) > 0;
}

/**
 * @brief Check if OpenSHMEM library is finalized
 *
 * @return Non-zero if finalized, 0 otherwise
 */
int shmem_finalized(void) {
  return __atomic_load_n(&proc.refcount, __ATOMIC_ACQUIRE) < 1;
}

#endif /* PR470 */
