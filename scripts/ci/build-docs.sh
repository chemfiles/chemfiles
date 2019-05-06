#!/bin/bash

set -xe

# Install doc dependencies
cd $TRAVIS_BUILD_DIR
python3.6 -m pip install --user --upgrade -r doc/requirements.txt

DOXYGEN_VER=1.8.13_1
DOXYGEN_URL="https://linuxbrew.bintray.com/bottles/doxygen-${DOXYGEN_VER}.x86_64_linux.bottle.1.tar.gz"
wget -O - "${DOXYGEN_URL}" | tar xz -C /tmp doxygen/${DOXYGEN_VER}/bin/doxygen
export PATH="/tmp/doxygen/${DOXYGEN_VER}/bin:$PATH"

doxygen --version

cd build

# Get previous documentation
git clone https://github.com/$TRAVIS_REPO_SLUG --branch gh-pages gh-pages
rm -rf gh-pages/.git
rm -rf gh-pages/deployed*

# Build new documentation
cmake -DCHFL_BUILD_DOCUMENTATION=ON .
cmake --build . --target doc_html
rm -rf doc/html/.doctrees/ doc/html/.buildinfo
rm -rf doc/html/_static/bootswatch-2.3.2/ doc/html/_static/bootstrap-2.3.2/
shopt -s extglob
cd doc/html/_static/bootswatch-* && rm -rf !(flatly) && cd -

# Copy documentation to the right place
if [[ "$TRAVIS_TAG" != "" ]]; then
    mv doc/html/ gh-pages/$TRAVIS_TAG
else
    rm -rf gh-pages/latest
    mv doc/html/ gh-pages/latest
fi

set +xe
