/* For license: see LICENSE file at top-level */

#include <shmem.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define N_UPDATES (1lu << 10)
#define N_INDICES (1lu << 6)
#define N_VALUES  (1lu << 16)

int main(void) {
  shmem_init();

  int mype = shmem_my_pe();
  int npes = shmem_n_pes();
  srand((unsigned int)mype);

  uint64_t *table = (uint64_t *)shmem_calloc(N_INDICES, sizeof(uint64_t));
  if (table == NULL) {
    shmem_global_exit(1);
  }

  shmem_ctx_t ctx;
  int ret = shmem_ctx_create(0, &ctx);
  if (ret != 0) {
    printf("%d: Error creating context (%d)\n", mype, ret);
    shmem_global_exit(1);
  }

  shmem_ctx_session_config_t config;
  config.total_ops = N_UPDATES;
  long config_mask = SHMEM_CTX_SESSION_TOTAL_OPS;

  shmem_ctx_session_start(ctx, SHMEM_CTX_SESSION_BATCH, &config, config_mask);

  for (size_t i = 0; i < N_UPDATES; i++) {
    int random_pe = rand() % npes;
    size_t random_idx = rand() % N_INDICES;
    uint64_t random_val = rand() % N_VALUES;
    shmem_ctx_uint64_atomic_xor(ctx, &table[random_idx], random_val, random_pe);
  }

  shmem_ctx_session_stop(ctx);
  shmem_ctx_quiet(ctx);
  shmem_sync_all();

  shmem_ctx_session_start(SHMEM_CTX_DEFAULT, SHMEM_CTX_SESSION_BATCH, NULL, 0);
  shmem_ctx_session_stop(SHMEM_CTX_DEFAULT);

  if (mype == 0) {
    printf("Session example completed successfully on %d PEs\n", npes);
  }

  shmem_ctx_destroy(ctx);
  shmem_free(table);
  shmem_finalize();

  return 0;
}
