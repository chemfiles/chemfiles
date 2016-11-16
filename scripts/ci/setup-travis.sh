#!/bin/bash

export C_COMPILER="$CC"

export CMAKE_ARGS="-DCMAKE_BUILD_TYPE=debug -DCHFL_BUILD_TESTS=ON -DCHFL_ENABLE_NETCDF=ON -DBUILD_SHARED_LIBS=${SHARED_LIBS}"

if [[ "$TRAVIS_OS_NAME" == "linux" && "$CC" == "gcc" && "$SHARED_LIBS" == "ON" ]]; then
    export EXTRA_WORK=true
    export CMAKE_ARGS="$CMAKE_ARGS -DCHFL_CODE_COVERAGE=ON"
    pip install --user codecov
else
    export EXTRA_WORK=false
fi


if [[ "${TRAVIS_OS_NAME}" == "linux" ]]; then
    if test "${CC}" == "gcc"; then
        export CC=gcc-4.8
        export CXX=g++-4.8
    fi

    if [[ "$ARCH" != "x86" ]]; then
        export CMAKE_ARGS="$CMAKE_ARGS -DCHFL_TEST_RUNNER=valgrind"
    fi
fi


if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    brew tap homebrew/science
    brew update
    brew install netcdf
    if test "${CC}" == "gcc"; then
        brew rm gcc
        brew install gcc
        export CC=gcc-6
        export CXX=g++-6
    fi
fi


if [[ "$ARCH" == "x86" ]]; then
    cd $TRAVIS_BUILD_DIR
    ./scripts/ci/install-netcdf.sh
    export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32"
    export CMAKE_ARGS="$CMAKE_ARGS -DNETCDF_INCLUDES=$HOME/netcdf/include"
    export CMAKE_ARGS="$CMAKE_ARGS -DNETCDF_LIBRARIES=$HOME/netcdf/lib/libnetcdf.a"
fi
