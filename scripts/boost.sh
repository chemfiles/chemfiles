#!/bin/bash

# This is the script used to generate the archive of boost used in chemfiles.

set -e

if [[ "$1" == "" ]]; then
    echo "usage: $0 path/to/boost"
    exit 1
fi

if [[ ! -d $1/boost ]]; then
    echo "usage: $0 path/to/boost"
    exit 1
fi

BOOST_ROOT=$1

INCLUDES=(
    assert.hpp cstdint.hpp io_fwd.hpp noncopyable.hpp preprocessor.hpp
    smart_ptr.hpp cerrno.hpp current_function.hpp filesystem.hpp
    static_assert.hpp utility.hpp checked_delete.hpp iterator.hpp range.hpp
    scoped_array.hpp throw_exception.hpp version.hpp shared_ptr.hpp limits.hpp
    predef.h type_traits.hpp functional.hpp config.hpp exception_ptr.hpp
    operators.hpp asio.hpp is_placeholder.hpp regex_fwd.hpp

    iterator range config detail exception functional predef system type_traits
    core io utility filesystem mpl preprocessor smart_ptr asio date_time bind
    regex
)

TMPDIR=`mktemp -d -t boost-XXXX-XXXX`
OUT=$TMPDIR/boost
mkdir -p $OUT/src
mkdir -p $OUT/include/boost

cp $BOOST_ROOT/LICENSE_1_0.txt $OUT
for file in "${INCLUDES[@]}"; do
    cp -r $BOOST_ROOT/boost/$file $OUT/include/boost/
done
cp $BOOST_ROOT/libs/filesystem/src/* $OUT/src
cp $BOOST_ROOT/libs/system/src/* $OUT/src

# Prevent OS X from creating ._(*) files in the archive
export COPYFILE_DISABLE=1

tar czf boost.tar.gz -C $TMPDIR boost
rm -rf $TMPDIR

echo boost.tar.gz created
