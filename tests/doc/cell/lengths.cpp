// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto cell = UnitCell({11, 22, 33});

    assert(cell.lengths() == Vector3D(11, 22, 33));

    cell.set_lengths({111, 222, 333});
    assert(cell.lengths() == Vector3D(111, 222, 333));
    // [example]
}
