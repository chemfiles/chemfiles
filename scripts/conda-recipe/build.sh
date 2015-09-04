#!/usr/bin/env bash

cd $SRC_DIR
rm -rf build
mkdir build
cd build
export MACOSX_DEPLOYMENT_TARGET=""

cmake -DBUILD_TESTS=ON -DPYTHON_BINDING=ON -DCMAKE_INSTALL_PREFIX=$PREFIX ..
cmake --build . --target install
