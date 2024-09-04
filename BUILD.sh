#!/bin/bash

HLINE="--------------------------------------------"

# --- Clean up generated files
echo $HLINE
echo "            CLEANING BUILD"
echo $HLINE
./CLEAN.sh
echo ; echo

# --- Run autogen and configure
echo $HLINE
echo "            RUNNING AUTOGEN"
echo $HLINE
mkdir build
./autogen.sh
cd build
echo ; echo

# --- Configure build
echo $HLINE
echo "            CONFIGURING"
echo $HLINE

PREFIX="$(pwd)/build"
# PREFIX=$OSSS_DIR

export SHMEM_LAUNCHER="$OMPI_BIN/mpiexec"

../configure \
  --prefix=$PREFIX \
  --with-pmix=$PMIX_DIR \
  --with-ucx=$UCX_DIR \
  --enable-debug \
  --enable-logging \
  --with-heap-size=10G

#../configure \
#  --prefix=$PREFIX \
#  --with-pmix=$PMIX_DIR \
#  --with-ucx=$UCX_DIR \
#  --with-shcoll=$SHCOLL_DIR \
#  --enable-debug \
#  --enable-logging \
#  --enable-aligned-addresses \
#  --with-heap-size=10G

# ---  Compile
echo $HLINE
echo "            COMPILING"
echo $HLINE
make -j 50 install


