#!/bin/bash

set -xe

# Install doc dependencies
cd $TRAVIS_BUILD_DIR
python3.6 -m pip install --user --upgrade -r doc/requirements.txt

DOXYGEN_VER=1.8.20
DOXYGEN_URL="https://linuxbrew.bintray.com/bottles/doxygen-${DOXYGEN_VER}.x86_64_linux.bottle.tar.gz"
wget -O - "${DOXYGEN_URL}" | tar xz -C /tmp doxygen/${DOXYGEN_VER}/bin/doxygen

# Explicitly use /lib64/ld-linux-x86-64.so.2 because bash do not do it
# by itself and fails with "No such file or directory" error. The doxygen
# binary requires @@HOMEBREW_PREFIX@@/lib/ld.so, which does not exists since we
# are not using a full linuxbrew installation
sudo patchelf --set-interpreter /lib64/ld-linux-x86-64.so.2 /tmp/doxygen/${DOXYGEN_VER}/bin/doxygen
export PATH=/tmp/doxygen/${DOXYGEN_VER}/bin:$PATH
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
