#include "../build/build/include/shmem.h"
#include "../build/build/include/shmem/defs.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  shmem_init();
  int npes = shmem_n_pes();
  int mype = shmem_my_pe();

  for ( int i = 0; i < npes; i++ ) {
    if (mype == i) {
      printf("Hello from PE %d out of %d PEs\n", mype, npes);
    }
  }

  shmem_finalize();
  return 0;
}
