#!/bin/bash

export CMAKE_ARGS="-DCMAKE_BUILD_TYPE=debug -DCHFL_BUILD_TESTS=ON"

if [[ "${STATIC_LIBS}" == "ON" ]]; then
    export CMAKE_ARGS="$CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF"
else
    export CMAKE_ARGS="$CMAKE_ARGS -DBUILD_SHARED_LIBS=ON"
fi

if [[ "${DO_COVERAGE}" == "ON" ]]; then
    export CMAKE_ARGS="$CMAKE_ARGS -DCHFL_CODE_COVERAGE=ON"
    pip install --user codecov
fi

cd $TRAVIS_BUILD_DIR
pip install --user -r doc/requirements.txt

if [[ "$EMSCRIPTEN" == "ON" ]]; then
    # Install a Travis compatible emscripten SDK
    wget https://github.com/chemfiles/emscripten-sdk/archive/master.tar.gz
    tar xf master.tar.gz
    ./emscripten-sdk-master/emsdk activate
    source ./emscripten-sdk-master/emsdk_env.sh

    export CMAKE_CONFIGURE='emcmake'
    export CMAKE_ARGS="$CMAKE_ARGS -DCHFL_TEST_RUNNER=node -DCMAKE_BUILD_TYPE=release -DCHFL_BUILD_DOCTESTS=OFF"

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
    if [[ "$CC" == "gcc" ]]; then
        export CC=gcc-4.8
        export CXX=g++-4.8
    fi

    if [[ "$VALGRIND" == "ON" ]]; then
        export CMAKE_ARGS="$CMAKE_ARGS -DCHFL_TEST_RUNNER=valgrind"
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
    brew update
    if [[ "$CC" == "gcc" ]]; then
        brew install gcc@5
        export CC=gcc-5
        export CXX=g++-5
    fi
fi


if [[ "$ARCH" == "x86" ]]; then
    export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32"
fi
