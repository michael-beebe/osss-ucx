#!/bin/bash

# --- Test simple hello world program
rm hello_world
$OSSS_BIN/oshcc hello_world.c -o hello_world
$OMPI_BIN/mpirun -np 4 ./hello_world
