#!/bin/bash

set -xe

# Install doc dependencies
cd $TRAVIS_BUILD_DIR
python3.6 -m pip install --user --upgrade -r doc/requirements.txt

mkdir -p /tmp/doxygen-1.8.20-for-travis/bin
wget http://luthaf.fr/chemfiles/doxygen-1.8.20-for-travis/doxygen -O /tmp/doxygen-1.8.20-for-travis/bin/doxygen
chmod +x /tmp/doxygen-1.8.20-for-travis/bin/doxygen
export PATH=/tmp/doxygen-1.8.20-for-travis/bin:$PATH
echo $(doxygen --version)

# Build new documentation
mkdir build && cd build
cmake -DCHFL_BUILD_DOCUMENTATION=ON -DSPHINX_LINKCHECK_OUTPUT=ON ..
cmake --build . --target doc_html

# Check that all links in the documentation are valid
cmake --build . --target doc_linkcheck

# Clean extra files
rm -rf doc/html/.doctrees/ doc/html/.buildinfo
rm -rf doc/html/_static/bootswatch-2.3.2/ doc/html/_static/bootstrap-2.3.2/
shopt -s extglob
cd doc/html/_static/bootswatch-* && rm -rf !(flatly) && cd -

# Get previous documentation
git clone https://github.com/$TRAVIS_REPO_SLUG --branch gh-pages gh-pages
rm -rf gh-pages/.git
rm -rf gh-pages/deployed*

# Copy documentation to the right place
if [[ "$TRAVIS_TAG" != "" ]]; then
    mv doc/html/ gh-pages/$TRAVIS_TAG
else
    rm -rf gh-pages/latest
    mv doc/html/ gh-pages/latest
fi

set +xe
