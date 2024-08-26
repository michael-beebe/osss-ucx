#!/bin/bash

./clean.sh

# --- Set OSSS bins from build directory
oshcc="../build/build/bin/oshcc"
# oshrun="../build/build/bin/oshrun"
oshrun="$OMPI_BIN/mpiexec"

# --- Hello world
$oshcc hello_world.c -o hello_world
$oshrun -np 2 ./hello_world