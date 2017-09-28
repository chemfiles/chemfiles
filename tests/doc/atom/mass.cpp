// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto atom = Atom("C");
    assert(atom.mass() == 12.011);

    atom.set_mass(42.5);
    assert(atom.mass() == 42.5);
    // [example]
    return 0;
}
