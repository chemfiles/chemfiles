#!/bin/bash -xe

# Exit early if we don't want to deploy the docs
if [[ "$TRAVIS_BRANCH" != "master" && "$TRAVIS_TAG" == "" ]]; then
    exit 0
fi

# Install doc dependencies
cd $TRAVIS_BUILD_DIR
pip install --user sphinx
pip install --user -r doc/requirements.txt
doxygen --version

# Build documentation
cd build
cmake -DBUILD_DOCUMENTATION=ON .
cmake --build .
rm -rf doc/html/.doctrees/ doc/html/.buildinfo
rm -rf doc/html/_static/bootswatch-* doc/html/_static/bootstrap-2.3.2/

cd ../gh-pages
git checkout gh-pages

# Copy the right directory
if [[ "$TRAVIS_BRANCH == master" ]]; then
    rm -rf latest
    mv ../build/doc/html/ latest
elif [[ "$TRAVIS_TAG" != "" ]]; then
    mv ../build/doc/html/ $TRAVIS_TAG
else
    echo "We should have exited earlier"
    exit 1
fi

git add .
# Skip push if there is no change
if git diff --cached --exit-code --quiet; then
    echo "No changes to the output on this push; exiting."
    exit 0
fi

# Git configuration
git config --global user.email "luthaf@luthaf.fr"
git config --global user.name "Travis-CI autobuild"
git config --global push.default simple
git config credential.helper "store --file=~/.git/credentials"

# Commit the new doc
git commit -a -m "[AUTO-COMMIT] Documentation update" -m "[ci skip]"
git push origin HEAD:gh-pages
