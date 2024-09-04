#!/bin/bash

# --- Set UCX configuration
#export UCX_TLS=rc_x  # Simplified to rc_x only
#export UCX_LOG_LEVEL=debug  # Increased log level for detailed logs
#export UCX_IB_TX_QUEUE_LEN=256  # Adjusted TX queue length
#export UCX_IB_RX_QUEUE_LEN=8192  # Adjusted RX queue length
#export UCX_NET_DEVICES=mlx5_0:1  # Explicitly set to use the active device mlx5_0

# --- Remove bin directory
rm -rf bin

# --- Set OSSS bins from build directory
oshcc="../build/build/bin/oshcc"
oshrun="../build/build/bin/oshrun"

# --- Create build directory for tests
mkdir bin

# --- Set flags for mpiexec
flags="--bind-to core --map-by core"

# --- Hello world
$oshcc hello_world.c -o ./bin/hello_world
$oshrun $flags -np 20 ./bin/hello_world


# --- shmem_put
$oshcc shmem_put.c -o ./bin/shmem_put
$oshrun $flags -np 2 ./bin/shmem_put