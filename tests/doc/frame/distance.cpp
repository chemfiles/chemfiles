// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto frame = Frame();
    frame.add_atom(Atom(""), {0.0, 0.0, 0.0});
    frame.add_atom(Atom(""), {1.0, 2.0, 3.0});

    assert(frame.distance(0, 1) == sqrt(14));
    // [example]
    return 0;
}
