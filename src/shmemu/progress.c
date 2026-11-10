/**
 * @file progress.c
 * @brief Progress thread implementation for OpenSHMEM
 *
 * This file implements a background progress thread for handling communication
 * operations when using UCX's emulation mode. The progress thread is only
 * activated when:
 * - Running on legacy hardware or without direct transport-supported RDMA/AMO
 * - Explicitly enabled by the user
 *
 * @note This code only gets activated if UCX's emulation mode is being
 * used: where we're running on legacy hardware, or where we don't
 * have direct transport-supported RDMA and AMO.
 *
 * @note Further, this is only enabled if asked for by the end-user.
 *
 * @note Hope in the future to fine-tune this if UCX can tell us whether its
 * emulation mode is being used.
 *
 * @copyright See LICENSE file at top-level
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "boolean.h"
#include "shmemc.h"
#include "shmemu.h"
#include "threading.h"
#include "module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309
#endif /* _POSIX_C_SOURCE */
#include <time.h>

/** Thread handle for progress thread */
static threadwrap_thread_t thr;

/**
 * Delay between progress calls in nanoseconds
 * @note mutable in case we want to look at adaptive polling
 */
static long delay_ns;

/** Flag to control progress thread execution */
static bool done = false;

/** Nanoseconds per second constant */
static const long billion = 1e9;

/**
 * @brief Progress thread main function
 *
 * Continuously calls communication progress function with configurable delay
 * until signaled to stop.
 *
 * @param args Thread arguments (unused)
 * @return NULL
 */
static void *start_progress(void *args) {
  NO_WARN_UNUSED(args);

  do {
    const struct timespec ts = {.tv_sec = delay_ns / billion,
                                .tv_nsec = delay_ns % billion};

    shmemc_progress();

    nanosleep(&ts, NULL); /* back off */
  } while (!__atomic_load_n(&done, __ATOMIC_ACQUIRE));

  return NULL;
}

/**
 * @brief Check if progress thread should be enabled for this PE
 *
 * Parses environment configuration to determine if progress thread
 * should run on this PE.
 *
 * @return 1 if progress required, 0 otherwise
 */
static int check_if_progress_required(void) {
  int *res = NULL;
  size_t nres;
  int s;
  char *copy;
  int ret = 0;

  if (proc.env.progress_threads == NULL) {
    goto out;
    /* NOT REACHED */
  }

  /* something like "yes" or "all"? */
  if (proc.leader) {
    const char first = tolower(*proc.env.progress_threads);

    if ((first == 'y') || (first == 'a')) {
      ret = 1;
      goto out;
      /* NOT REACHED */
    }
  }

  /* shmemu_parse_csv zaps the input string */
  copy = strdup(proc.env.progress_threads);
  if (copy == NULL) {
    shmemu_fatal(MODULE ": unable to allocate memory during "
                        "progress thread check: %s",
                 strerror(errno));
    /* NOT REACHED */
  }

  s = shmemu_parse_csv(copy, &res, &nres);
  if (s > 0) {
    size_t i;

    for (i = 0; i < nres; ++i) {
      if (res[i] == shmemc_my_pe()) {
        ret = 1;
        goto out_free;
        /* NOT REACHED */
      }
    }
  }

out_free:
  free(res);
  free(copy);
out:
  return ret;
}

/**
 * @brief Initialize progress thread
 *
 * Checks if progress thread is required and starts it if needed.
 */
void shmemu_progress_init(void) {
  __atomic_store_n(&done, false, __ATOMIC_RELEASE);
  proc.progress_thread = check_if_progress_required();

  if (proc.progress_thread) {
    int s;

    /* pull in progress timing */
    delay_ns = (long)proc.env.progress_delay_ns;

    logger(LOG_INIT, "progress thread delay = %ldns", delay_ns);

    s = threadwrap_thread_create(&thr, start_progress, NULL);
    shmemu_assert(s == 0, MODULE ": could not create progress thread (%s)",
                  strerror(s));
  }
}

/**
 * @brief Finalize progress thread
 *
 * Signals progress thread to stop and waits for it to terminate.
 */
void shmemu_progress_finalize(void) {
  if (proc.progress_thread) {
    int s;

    __atomic_store_n(&done, true, __ATOMIC_RELEASE);

    s = threadwrap_thread_join(thr, NULL);
    shmemu_assert(s == 0, MODULE ": could not terminate progress thread (%s)",
                  strerror(s));
    proc.progress_thread = false;
  }
}

/**
 * @brief Set progress thread delay
 *
 * Updates the delay between progress calls.
 *
 * @param newdelay New delay value in nanoseconds
 */
void shmemu_progress_set_delay(long newdelay) { delay_ns = newdelay; }
