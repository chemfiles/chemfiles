#!/bin/bash
set -ev

wget http://www.cmake.org/files/v3.2/cmake-3.2.1-Linux-x86_64.sh
sh cmake-3.2.1-Linux-x86_64.sh --prefix=. --include-subdir

./cmake-3.2.1-Linux-x86_64/bin/cmake -DBUILD_TESTS=ON -DPYTHON_BINDING=ON .

make -j2

ctest --output-on-failure
