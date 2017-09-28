// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto atom = Atom("C");
    assert(atom.name() == "C");

    atom.set_name("C1");
    assert(atom.name() == "C1");
    // [example]
    return 0;
}
