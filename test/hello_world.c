#include "/home/mbeebe/sw/el9-x86_64/osss-ucx/include/shmem.h"
#include "/home/mbeebe/sw/el9-x86_64/osss-ucx/include/shmem/defs.h"

#include <stdio.h>

// #include <shmem.h>

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
