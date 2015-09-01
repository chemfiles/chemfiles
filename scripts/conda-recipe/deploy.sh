#!/usr/bin/env bash
#
# This script is intended to be used on Travis, to build and deploy the conda recipe.
# The meta.yaml file should be configured by CMake first.

set -evx

cd ${TRAVIS_BUILD_DIR}

if [ ${TRAVIS_OS_NAME} == "linux" ]
then
    export CONDA_URL="http://repo.continuum.io/miniconda/Miniconda-3.9.1-Linux-x86_64.sh"
elif [ ${TRAVIS_OS_NAME} == "osx" ]
then
    export CONDA_URL="http://repo.continuum.io/miniconda/Miniconda-3.9.1-MacOSX-x86_64.sh"
    # Enforce the netcdf and numpy version for conda build
    brew rm netcdf numpy
fi

# Ensure that the meta.yaml file is created
mkdir -p build
cd build
$CMAKE $CMAKE_ARGS ..

wget $CONDA_URL -O Miniconda.sh
bash Miniconda.sh -b
export PATH=$HOME/miniconda/bin:$PATH
conda update --yes conda
conda install --yes conda-build jinja2 anaconda-client numpy
export CONDA_RECIPE=${TRAVIS_BUILD_DIR}/scripts/conda-recipe

# Build conda recipe
cd ${TRAVIS_BUILD_DIR}
conda build $CONDA_RECIPE

# Submit binary build to binstar for conda
python $CONDA_RECIPE/move-conda-package.py
binstar -t $BINSTAR_TOKEN upload --force *.tar.bz2

exit 0
