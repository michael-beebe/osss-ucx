/**
 * @file contexts.c
 * @brief Implementation of OpenSHMEM context management
 *
 * This file provides the implementation for managing OpenSHMEM contexts,
 * including creation, destruction, and reuse of contexts. It handles the
 * freelist of reusable contexts and context attributes.
 *
 * @copyright See LICENSE file at top-level
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "state.h"
#include "shmemc.h"
#include "shmemu.h"
#include "boolean.h"
#include "shmem/defs.h"
#include "ucx/api.h"

#include <stdlib.h>

/**
 * @brief Resize the context array block
 *
 * @param th Team handle
 * @param n New size to allocate
 * @return Pointer to resized context array
 */
inline static shmemc_context_h *resize_spill_block(shmemc_team_h th, size_t n) {
  shmemc_context_h *chp =
      (shmemc_context_h *)realloc(th->ctxts, n * sizeof(*(th->ctxts)));

  if (chp == NULL) {
    shmemu_fatal("can't allocate %lu bytes for context freelist",
                 (unsigned long)n);
    /* NOT REACHED */
  }

  return chp;
}

/**
 * @brief Allocate a new context
 *
 * @return Newly allocated context handle
 */
inline static shmemc_context_h alloc_freelist_slot(void) {
  shmemc_context_h ch = (shmemc_context_h)malloc(sizeof(shmemc_context_t));

  if (ch == NULL) {
    shmemu_fatal("unable to allocate memory for new context");
    /* NOT REACHED */
  }

  return ch;
}

/**
 * @brief Get a usable context slot in a team, reusing a freed one if possible
 *
 * Slot capacity and reuse are tracked per team: context ids index the
 * owning team's array, so slots cannot be shared between teams.
 *
 * @param th Team handle
 * @param reused Set to true if context was reused
 * @return Index of allocated context
 */
static size_t get_usable_context(shmemc_team_h th, bool *reused) {
  size_t idx;

  for (idx = 0; idx < th->nctxts; ++idx) {
    if (th->ctxts[idx]->attr.freed) {
      logger(LOG_CONTEXTS, "reclaiming context #%lu from free list",
             (unsigned long)idx);
      *reused = true;
      return idx;
    }
  }

  /* if out of space, grab some more slots */
  if (idx == th->ctxts_cap) {
    const size_t block =
        proc.env.prealloc_contexts > 0 ? proc.env.prealloc_contexts : 1;

    th->ctxts_cap += block;
    th->ctxts = resize_spill_block(th, th->ctxts_cap);
  }

  /* allocate context in current slot */
  th->ctxts[idx] = alloc_freelist_slot();

  ++th->nctxts; /* for next one */
  *reused = false;
  return idx;
}

/**
 * @brief Register a context in PE state
 *
 * @param ch Context handle to register
 */
inline static void context_register(shmemc_context_h ch) {
  NO_WARN_UNUSED(ch);

  logger(LOG_CONTEXTS, "using context #%lu", ch->id);
}

/**
 * @brief Deregister a context and add it to freelist
 *
 * @param ch Context handle to deregister
 */
inline static void context_deregister(shmemc_context_h ch) {
  /* this one is re-usable */
  ch->attr.freed = true;

  logger(LOG_CONTEXTS, "context #%lu can be reused", ch->id);
}

/**
 * @brief Set context options from provided flags
 *
 * @param options Option flags
 * @param ch Context handle to configure
 */
inline static void context_set_options(long options, shmemc_context_h ch) {
  ch->attr.serialized = options & SHMEM_CTX_SERIALIZED;
  ch->attr.privat = options & SHMEM_CTX_PRIVATE;
  ch->attr.nostore = options & SHMEM_CTX_NOSTORE;
}

/**
 * @brief Allocate space for contexts in a team
 *
 * @param th Team handle
 * @return Array of context handles or NULL if none needed
 */
shmemc_context_h *shmemc_alloc_contexts(shmemc_team_h th) {
  if (th->nctxts > 0) {
    return resize_spill_block(th, th->nctxts);
  } else {
    return NULL;
  }
}

/**
 * @brief Create a new context in a team
 *
 * @param th Team handle
 * @param options Context option flags
 * @param ctxp Pointer to store new context handle
 * @return 0 on success, non-zero on failure
 */
int shmemc_context_create(shmemc_team_h th, long options,
                          shmemc_context_h *ctxp) {
  bool reuse;
  size_t idx;
  shmemc_context_h ch;

  /* identify context to use */
  idx = get_usable_context(th, &reuse);
  ch = th->ctxts[idx];

  /* set SHMEM context behavior */
  context_set_options(options, ch);
  ch->attr.freed = false;

  /* is this reclaimed from free list or do we have to set up? */
  if (!reuse) {
    ucs_status_t s;

    const int ret = shmemc_ucx_context_progress(ch);

    if (ret != 0) {
      shmemu_fatal("shmemc_context_create: shmemc_ucx_context_progress failed "
                   "with ret=%d",
                   ret);
      free(ch);
      return ret;
    }
    shmemc_ucx_make_eps(ch);

    s = shmemc_ucx_worker_wireup(ch);

    if (s != UCS_OK) {
      shmemu_fatal("cannot complete new context worker wireup");
    }
  }

  ch->creator_thread = threadwrap_thread_id();
  ch->id = idx;
  ch->team = th; /* connect context to its owning team */

  context_register(ch);

  *ctxp = ch;

  return 0;
}

/**
 * @brief Destroy an existing context
 *
 * @param ctx Context to destroy
 *
 * It is illegal to destroy the default context. Attempting to do so
 * will result in program termination.
 */
void shmemc_context_destroy(shmem_ctx_t ctx) {
  if (ctx == SHMEM_CTX_INVALID) {
    shmemu_warn("ignoring attempt to destroy invalid context");
  } else if (ctx == SHMEM_CTX_DEFAULT) {
    shmemu_fatal("cannot destroy the default context");
    /* NOT REACHED */
  } else {
    shmemc_context_h ch = (shmemc_context_h)ctx;

    /* spec 1.4 ++ has implicit quiet for storable contexts */
    shmemc_ctx_quiet(ch);

    context_deregister(ch);
  }
}

/**
 * @brief Get the ID of a context
 *
 * @param ctx Context handle
 * @return Context ID number
 */
size_t shmemc_context_id(shmem_ctx_t ctx) {
  const shmemc_context_h ch = (shmemc_context_h)ctx;

  return ch->id;
}

/**
 * @brief Default context instance and handle
 *
 * The first, default context gets a special SHMEM handle and needs
 * address exchange through PMI.
 */
shmemc_context_t shmemc_default_context;
shmemc_context_h defcp = &shmemc_default_context;

/**
 * @brief Initialize the default context
 *
 * @return 0 on success, non-zero on failure
 */
int shmemc_context_init_default(void) {
  context_set_options(0L, defcp);

  shmemc_ucx_context_progress(defcp);

  return shmemc_ucx_context_default_set_info();
}
