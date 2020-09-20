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
fi

if [[ "$DO_COVERAGE" == "ON" || "$DEPLOY_DOCS" == "ON" ]]; then
    curl https://bootstrap.pypa.io/get-pip.py > get-pip.py
    python3.6 get-pip.py --user
    python3.6 -m pip install --user codecov
fi

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

    export CC=emcc
    export CXX=em++

    return
fi

# Install a different version of cmake
if [[ "${ALTERNATIVE_CMAKE}" != "" ]]; then
    cd $HOME
    wget https://cmake.org/files/v${ALTERNATIVE_CMAKE:0:3}/cmake-${ALTERNATIVE_CMAKE}.tar.gz
    tar xf cmake-${ALTERNATIVE_CMAKE}.tar.gz
    export PATH=$HOME/cmake-${ALTERNATIVE_CMAKE}/bin:$PATH
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

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    if [[ "$TRAVIS_COMPILER" == "gcc" ]]; then
        export HOMEBREW_NO_INSTALL_CLEANUP=1
        export HOMEBREW_NO_AUTO_UPDATE=1
        brew install gcc@5
        export CC=gcc-5
        export CXX=g++-5

        # Filter out 'warning: section "__textcoal_nt" is deprecated'
        # from the compiler output, as it makes the log reach the size limit
        export BUILD_ARGS="$BUILD_ARGS 2> >(python $TRAVIS_BUILD_DIR/scripts/ci/filter-textcoal-warnings.py)"
    fi
fi

if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
    export PATH="/c/Program Files/CMake/bin":$PATH
    export CMAKE_ARGS="$CMAKE_ARGS -DCHFL_BUILD_DOCTESTS=OFF"

    if [[ "$CMAKE_GENERATOR" == "Visual Studio"* ]]; then
        choco install -y vcbuildtools
        export PATH=$PATH:"/c/Program Files (x86)/Microsoft Visual Studio/2015/BuildTools/MSBuild/14.0/Bin"
        export BUILD_ARGS="-verbosity:minimal -m:2"
    fi

    if [[ "$CMAKE_GENERATOR" == "MinGW Makefiles" ]]; then
        export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_SH=CMAKE_SH-NOTFOUND"
        export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=release"
        export CMAKE_BUILD_TYPE="Release"

        # Remove sh.exe from git in the PATH for MinGW generator
        # 1) add extra ':' to allow the REMOVE below to work
        TMP_PATH=:$PATH:
        # 2) remove sh.exe from the PATH
        REMOVE='/c/Program Files/Git/usr/bin'
        TMP_PATH=${TMP_PATH/:$REMOVE:/:}
        # 3) remove extra ':'
        TMP_PATH=${TMP_PATH%:}
        TMP_PATH=${TMP_PATH#:}
        export PATH=$TMP_PATH
    fi
fi

if [[ "$ARCH" == "x86" ]]; then
    export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32"
fi

set +x
