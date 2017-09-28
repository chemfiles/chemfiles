// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto atom = Atom("C");
    assert(atom.charge() == 0.0);

    atom.set_charge(0.2);
    assert(atom.charge() == 0.2);
    // [example]
    return 0;
}
