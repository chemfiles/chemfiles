#!/usr/bin/env bash
#
# This file is only intended for use with CI systems, and should be
# configurated by cmake before usage.
#
# We do not build chemharp, but only move it from the build path to $PREFIX

export CHEMHARP_BUILD_DIR=@CMAKE_BINARY_DIR@

cd $CHEMHARP_BUILD_DIR
cmake --build . --target install -- DESTDIR=$PREFIX
mv $PREFIX/usr/local/lib/* $PREFIX/lib
mv $PREFIX/usr/local/include/* $PREFIX/include
mkdir -p lib/python2.7/site-packages
find . -name 'chemharp.so' -exec cp {} $PREFIX/lib/python2.7/site-packages \;
rm -rf $PREFIX/usr/local
