#!/usr/bin/env bash

export CHEMHARP_LIBDIR=$RECIPE_DIR/../../build/clang

cd $CHEMHARP_LIBDIR
cmake --build . --target install -- DESTDIR=$PREFIX
mv $PREFIX/usr/local/lib/* $PREFIX/lib
mv $PREFIX/usr/local/include/* $PREFIX/include
rm -rf $PREFIX/usr/local

cd $SRC_DIR
mkdir conda-build
cd conda-build
export MACOSX_DEPLOYMENT_TARGET=""
cmake -DCMAKE_INSTALL_PREFIX=$PREFIX ..
cmake --build . --target install --config release
