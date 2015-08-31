#!/usr/bin/env bash

set -evx

conda install --yes conda-build jinja2 anaconda-client
export CONDA_RECIPE=${TRAVIS_BUILD_DIR}/scripts/conda-recipe

# Build conda recipe
cd ${TRAVIS_BUILD_DIR}
conda build $CONDA_RECIPE

# Submit binary build to binstar for conda
python $CONDA_RECIPE/move-conda-package.py
binstar -t $BINSTAR_TOKEN upload --force *.tar.bz2

exit 0
