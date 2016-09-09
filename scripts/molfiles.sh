#!/bin/bash

# This is the script used to generate the archive of the molfiles plugin
# used in chemfiles.

set -e

if [ $(uname) == "Darwin" ] ; then
    echo "Error: this script assume a case sensitive filesystem."
    echo "OS X filesystem is not by default. Please double check."
    exit 1
fi

if [[ "$1" == "" ]]; then
    echo "usage: $0 path/to/vmd/plugins"
    exit 1
fi

if [[ ! -d $1/molfile_plugin ]]; then
    echo "usage: $0 path/to/vmd/plugins"
    exit 1
fi

PLUGINS=$1

TMPDIR=`mktemp -d`
OUT=$TMPDIR/molfiles
mkdir -p $OUT/src
mkdir -p $OUT/include

# Get the include files
cp $PLUGINS/include/molfile_plugin.h $OUT/include
cp $PLUGINS/include/vmdplugin.h $OUT/include
cp $PLUGINS/include/vmdconio.h $OUT/include

cp $PLUGINS/molfile_plugin/LICENSE $OUT

cp $PLUGINS/molfile_plugin/src/*.h $OUT/src
cp $PLUGINS/molfile_plugin/src/*.hxx $OUT/src
cp $PLUGINS/molfile_plugin/src/*.cxx $OUT/src
cp $PLUGINS/molfile_plugin/src/*.c $OUT/src

# Do not use the capital C letter as extension for C++ ...
for file in $PLUGINS/molfile_plugin/src/*.C; do
    cp $file $OUT/src/$(basename ${file/.C/.cxx})
done

chmod -x $OUT/LICENSE
chmod -x $OUT/include/*
chmod -x $OUT/src/*

patch -d $OUT -p1 < molfiles.patch

tar czf molfiles.tar.gz -C $TMPDIR molfiles
rm -rf $TMPDIR

echo molfiles.tar.gz created
