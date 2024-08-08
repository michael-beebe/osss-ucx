#!/bin/bash

# Clean up generated files
rm -rf \
  build \
  m4 \
  autom4te.cache \
  config.h.in \
  config.guess \
  config.sub \
  configure \
  depcomp \
  install-sh \
  ltmain.sh \
  missing

# Ensure pkgconfig directory and files exist
mkdir -p pkgconfig

# Check and create pkgconfig/osss-ucx.pc.in if it doesn't exist
if [ ! -f pkgconfig/osss-ucx.pc.in ]; then
  cat <<EOF >pkgconfig/osss-ucx.pc.in
prefix=@prefix@
exec_prefix=@exec_prefix@
libdir=@libdir@
includedir=@includedir@

Name: OSSS-UCX
Description: OSSS-UCX library
Version: 1.0.0
Libs: -L\${libdir} -losss-ucx
Cflags: -I\${includedir}
EOF
fi

# Ensure configure.ac references pkgconfig files
grep -qxF 'AC_CONFIG_FILES([pkgconfig/Makefile])' configure.ac || echo 'AC_CONFIG_FILES([pkgconfig/Makefile])' >>configure.ac
grep -qxF 'AC_CONFIG_FILES([pkgconfig/osss-ucx.pc])' configure.ac || echo 'AC_CONFIG_FILES([pkgconfig/osss-ucx.pc])' >>configure.ac

# Run autogen and configure
mkdir build
./autogen.sh
cd build

../configure \
  --with-pmix=$PMIX_DIR \
  --with-ucx=$UCX_DIR \
  --with-shcoll=$SHCOLL_DIR \
  --with-heap-size=10G #\
# --with-launcher=`which mpirun`

make -j 50
