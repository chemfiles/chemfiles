#!/bin/bash -e

# This script clone the tng_io repo for the command line argument, and then
# cleanup the repo for inclusion as an "external" dependency for chemfiles.

function error() {
    echo $1
    exit 1
}

CLONE_DIR=tng_io
ARCHIVE=tng_io.tar.gz

if [[ $1 == "" ]]; then
    error "usage: $0 git://url/of/tng_io/repository"
else
    REPO_URL=$1
fi

if [[ -e $CLONE_DIR ]]; then
    error "error: $CLONE_DIR directory already exists"
fi

git clone -q $REPO_URL $CLONE_DIR
if [[ $? != 0 ]]; then
    error "git clone $REPO_URL failed"
fi

cd $CLONE_DIR
git apply ../tng_io.patch

rm -rf build
rm -rf example_files/
rm -rf .git

cd ..

tar czf $ARCHIVE $CLONE_DIR

echo "TNG library is ready to use in $ARCHIVE"

rm -rf $CLONE_DIR
