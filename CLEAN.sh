#!/bin/bash

# Remove the 'build' directory
echo "Cleaning build directory..."
rm -rf build

# Clean up autogenerated files
echo "Cleaning up autogenerated files..."
rm -f aclocal.m4
rm -f compile
rm -f config.guess
rm -f config.sub
rm -f configure
rm -f depcomp
rm -f install-sh
rm -f ltmain.sh
rm -f missing
rm -f Makefile.in
rm -f config.h.in~
rm -f config.h.in
rm -f INSTALL
rm -f COPYING

# Clean up Makefiles and related files
echo "Cleaning Makefiles..."
find . -name "Makefile" -exec rm -f {} \;
find . -name "Makefile.in" -exec rm -f {} \;

# Remove auto-generated directories
echo "Cleaning auto-generated directories..."
rm -rf autom4te.cache
rm -rf .deps
rm -rf .libs
rm -rf m4

# Remove any other temporary files or directories as needed
# Add additional clean-up commands here if necessary

echo "Build cleaned."
