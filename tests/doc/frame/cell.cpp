// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto frame = Frame();

    auto cell = frame.cell();
    assert(cell.shape() == UnitCell::INFINITE);

    cell = UnitCell(23, 34, 11.5);
    frame.set_cell(cell);

    assert(frame.cell().shape() == UnitCell::ORTHORHOMBIC);
    // [example]
}
