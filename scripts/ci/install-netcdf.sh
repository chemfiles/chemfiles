#!/bin/bash

set -xe

if [ -f "$HOME/netcdf/include/netcdf.h" ]; then
    echo "Using cached NetCDF"
    exit
fi

if [[ "$ARCH" == "x86" ]]; then
    CMAKE_FLAGS="-DCMAKE_C_FLAGS=-m32"
else
    CMAKE_FLAGS=""
fi

cd $HOME
wget https://github.com/Unidata/netcdf-c/archive/v4.4.1.tar.gz -O netcdf-c-4.4.1.tar.gz
tar xf netcdf-c-4.4.1.tar.gz
cd netcdf-c-4.4.1
cmake -DBUILD_TESTING=OFF -DENABLE_TESTS=OFF -DENABLE_DAP=OFF -DENABLE_DISKLESS=OFF \
      -DUSE_HDF5=OFF -DUSE_NETCDF4=OFF -DENABLE_NETCDF_4=OFF -DHAVE_LIBM=/lib32/libm.so.6 \
      -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$HOME/netcdf \
      $CMAKE_FLAGS .

make install
