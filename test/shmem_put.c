#include <shmem.h>
#include <stdio.h>

int main(void) {
    int my_pe, npes;
    long src[10];
    static long dest[10];  /* static ensures symmetric memory allocation */

    /* Initialize OpenSHMEM */
    shmem_init();

    my_pe = shmem_my_pe();
    npes = shmem_n_pes();

    /* Ensure there are at least two PEs */
    if (npes < 2) {
        if (my_pe == 0) {
            printf("This program requires at least two PEs.\n");
        }
        shmem_finalize();
        return 1;
    }

    /* Initialize the source array on PE 0 */
    if (my_pe == 0) {
        for (int i = 0; i < 10; i++) {
            src[i] = i + 1;
        }
        printf("PE %d: Source data initialized.\n", my_pe);
    }

    /* Synchronize all PEs */
    shmem_barrier_all();

    /* PE 0 performs a put to PE 1 */
    if (my_pe == 0) {
        shmem_put(dest, src, 10, 1);  /* Put 10 elements into PE 1's dest array */
        printf("PE %d: shmem_put completed.\n", my_pe);
    }

    /* Synchronize all PEs again */
    shmem_barrier_all();

    /* PE 1 prints the received data */
    if (my_pe == 1) {
        printf("PE %d: Received data: ", my_pe);
        for (int i = 0; i < 10; i++) {
            printf("%ld ", dest[i]);
        }
        printf("\n");
    }

    /* Finalize OpenSHMEM */
    shmem_finalize();

    return 0;
}
