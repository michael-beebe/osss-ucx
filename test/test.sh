#!/bin/bash

# --- Set UCX stuff
# export UCX_TLS=rc,sm,self
# export UCX_NET_DEVICES=mlx5_2:1
export UCX_TLS=sm,self

export UCX_IB_ENABLE_CRC=0
export UCX_IB_RX_QUEUE_LEN=128
export UCX_IB_TX_QUEUE_LEN=128

export UCX_IB_REG_METHODS=rcache,direct
export UCX_IB_REG_MT_THRESH=64k

# --- Remove bin directory
rm -rf bin

# --- Set OSSS bins from build directory
oshcc="../build/build/bin/oshcc"
oshrun="../build/build/bin/oshrun"
# oshrun="$OMPI_BIN/mpiexec"

# --- Create build directory for tests
mkdir bin

# --- Hello world
$oshcc hello_world.c -o ./bin/hello_world
$oshrun -np 2 ./bin/hello_world

