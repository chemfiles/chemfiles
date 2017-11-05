// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto frame = Frame();
    assert(frame.size() == 0);
    assert(frame.cell() == UnitCell());
    // [example]
    return 0;
}
