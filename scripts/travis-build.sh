#!/bin/bash
set -ev

cmake -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE="release" .
make
ctest
