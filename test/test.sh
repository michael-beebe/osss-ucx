#!/bin/bash

################################################################
#                          SETUP
################################################################
# --- Set UCX configuration
export UCX_TLS=sm
export UCX_RC_DEVX_ENABLED=n
# export UCX_LOG_LEVEL=debug

# --- Turn on verbose SHMEM logging
export SHMEM_DEBUG=on

# --- Set OSSS bins from build directory
oshcc="../build/build/bin/oshcc"
oshrun="../build/build/bin/oshrun"

# --- Clean bin directory
rm -rf bin
mkdir bin

# --- Set flags for mpiexec
flags="--bind-to core --map-by core"
hline="-------------------------------"

# --- Print log levels ---
echo "UCX_LOG_LEVEL=$UCX_LOG_LEVEL"
echo "SHMEM_DEBUG=$SHMEM_DEBUG"
$oshcc --version

################################################################
#                          TESTS
################################################################

# # --- Hello world ---
# echo $hline ; echo "  Running Hello World test" ; echo $hline
# $oshcc hello_world.c -o ./bin/hello_world
# $oshrun $flags -np 2 ./bin/hello_world
# echo

# # --- shmem_put ---
# echo $hline ; echo "  Running shmem_put test" ; echo $hline
# $oshcc shmem_put.c -o ./bin/shmem_put
# $oshrun $flags -np 2 ./bin/shmem_put
# echo

# # --- shmem_info ---
# echo $hline ; echo "  Running shmem_info test" ; echo $hline
# $oshcc shmem_info.c -o ./bin/shmem_info
# $oshrun $flags -np 2 ./bin/shmem_info
# echo

# # --- shmem_team_create_ctx ---
# echo $hline ; echo "  Running shmem_team_create_ctx test" ; echo $hline
# $oshcc shmem_team_create_ctx.c -o ./bin/shmem_team_create_ctx
# $oshrun $flags -np 2 ./bin/shmem_team_create_ctx
# echo

# # --- shmem_ctx_get_team ---
# echo $hline ; echo "  Running shmem_ctx_get_team test" ; echo $hline
# $oshcc shmem_ctx_get_team.c -o ./bin/shmem_ctx_get_team
# # $oshrun $flags -np 2 ./bin/shmem_ctx_get_team
# $oshrun $flags -np 2 \
#          -x UCX_TLS \
#          -x UCX_RC_DEVX_ENABLED \
#          -x UCX_LOG_LEVEL \
#          -x SHMEM_DEBUG \
#          ./bin/shmem_ctx_get_team
# echo

# # --- shmem_team_sync ---
# echo $hline ; echo "  Running shmem_team_sync test" ; echo $hline
# $oshcc shmem_team_sync.c -o ./bin/shmem_team_sync
# $oshrun $flags -np 2 ./bin/shmem_team_sync
# echo

# --- shmem_alltoall ---
echo $hline ; echo "  Running shmem_alltoall test" ; echo $hline
$oshcc shmem_alltoall.c -o ./bin/shmem_alltoall
$oshrun $flags -np 10 ./bin/shmem_alltoall
echo

