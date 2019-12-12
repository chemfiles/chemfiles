#!/bin/bash

set -x

cd $TRAVIS_BUILD_DIR
export CMAKE_ARGS="-DCMAKE_BUILD_TYPE=debug -DCHFL_BUILD_TESTS=ON $CMAKE_EXTRA"
export BUILD_ARGS="-j2"
export CMAKE_BUILD_TYPE="Debug"

if [[ "$CMAKE_GENERATOR" == "" ]]; then
    export CMAKE_GENERATOR="Unix Makefiles"
fi

if [[ "$STATIC_LIBS" == "ON" ]]; then
    export CMAKE_ARGS="$CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF"
else
    export CMAKE_ARGS="$CMAKE_ARGS -DBUILD_SHARED_LIBS=ON"
fi

# Remove /opt/pyenv/shims from PATH. https://unix.stackexchange.com/a/178822
export PATH=$(echo ":$PATH:" | sed "s|:/opt/pyenv/shims:|:|g;s|^:\(.*\):\$|\1|")

if [[ "$DO_COVERAGE" == "ON" ]]; then
    export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_C_FLAGS=\"--coverage\" -DCMAKE_CXX_FLAGS=\"--coverage\""
    curl https://bootstrap.pypa.io/get-pip.py > get-pip.py
    python3.6 get-pip.py --user
    python3.6 -m pip install --user codecov
fi

cd $TRAVIS_BUILD_DIR
pip install --user -r doc/requirements.txt

if [[ "$EMSCRIPTEN" == "ON" ]]; then
    # Install a Travis compatible emscripten SDK
    wget https://github.com/emscripten-core/emsdk/archive/master.tar.gz
    tar xf master.tar.gz
    ./emsdk-master/emsdk install 1.39.2
    ./emsdk-master/emsdk activate 1.39.2
    source ./emsdk-master/emsdk_env.sh

    export CMAKE_CONFIGURE='emcmake'
    export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=release -DCHFL_BUILD_DOCTESTS=OFF"
    export CMAKE_BUILD_TYPE="Release"

    # Install a modern cmake
    cd $HOME
    wget https://cmake.org/files/v3.9/cmake-3.9.3-Linux-x86_64.tar.gz
    tar xf cmake-3.9.3-Linux-x86_64.tar.gz
    export PATH=$HOME/cmake-3.9.3-Linux-x86_64/bin:$PATH

    export CC=emcc
    export CXX=em++

    return
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    if [[ "$TRAVIS_COMPILER" == "gcc" ]]; then
        export CC=gcc-4.8
        export CXX=g++-4.8
    fi

    if [[ "$VALGRIND" == "ON" ]]; then
        export CMAKE_ARGS="$CMAKE_ARGS -DCHFL_TESTS_USE_VALGRIND=ON"
    fi
fi

if [[ "$USE_ICC" == "ON" ]]; then
    if [[ "$TRAVIS_PULL_REQUEST" != "false" ]]; then
        echo "not runnning intel builder on Pull-Request"
        exit
    fi
    /bin/sh $TRAVIS_BUILD_DIR/scripts/ci/install-icc.sh
    source ~/.bashrc
    export CC=icc
    export CXX=icpc
fi

if [[ "$USE_PGI" == "ON" ]]; then
    /bin/sh $TRAVIS_BUILD_DIR/scripts/ci/install-pgi.sh
    export CC=pgcc
    export CXX=pgc++
fi

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    if [[ "$TRAVIS_COMPILER" == "gcc" ]]; then
        export CC=gcc-5
        export CXX=g++-5

        # Filter out 'warning: section "__textcoal_nt" is deprecated'
        # from the compiler output, as it makes the log reach the size limit
        export BUILD_ARGS="$BUILD_ARGS 2> >(python $TRAVIS_BUILD_DIR/scripts/ci/filter-textcoal-warnings.py)"
    fi
fi


if [[ "$ARCH" == "x86" ]]; then
    export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32"
fi

set +x
