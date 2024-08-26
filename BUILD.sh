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
../configure \
  --prefix=$OSSS_DIR \
  --with-pmix=$PMIX_DIR \
  --with-ucx=$UCX_DIR \
  --with-shcoll=$SHCOLL_DIR \
  --enable-debug \
  --enable-logging \
  --enable-aligned-addresses \
  --with-heap-size=10G \
  --with-launcher=$OMPI_BIN/mpiexec

# ---  Compile
echo $HLINE
echo "            COMPILING"
echo $HLINE
make -j 50 install


