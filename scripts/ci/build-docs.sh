#!/bin/bash -xe

# Install doc dependencies
cd $TRAVIS_BUILD_DIR
pip install --user sphinx
pip install --user -r doc/requirements.txt
doxygen --version

cd build

# Get previous documentation
git clone https://github.com/$TRAVIS_REPO_SLUG --branch gh-pages gh-pages
rm -rf gh-pages/.git

# Build new documentation
cmake -DCHFL_BUILD_DOCUMENTATION=ON .
cmake --build .
rm -rf doc/html/.doctrees/ doc/html/.buildinfo
rm -rf doc/html/_static/bootswatch-* doc/html/_static/bootstrap-2.3.2/

# Copy documentation to the right place
if [[ "$TRAVIS_TAG" != "" ]]; then
    mv doc/html/ gh-pages/$TRAVIS_TAG
else
    rm -rf gh-pages/latest
    mv doc/html/ gh-pages/latest
fi
