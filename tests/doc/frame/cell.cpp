// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto frame = Frame();

    auto cell = frame.cell();
    assert(cell.shape() == UnitCell::INFINITE);

    cell = UnitCell(23, 34, 11.5);
    frame.set_cell(cell);

    assert(frame.cell().shape() == UnitCell::ORTHORHOMBIC);
    // [example]
    return 0;
}
