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
    frame.add_atom(Atom("H"), {1.0, 0.0, 0.0});
    frame.add_atom(Atom("O"), {0.0, 1.0, 0.0});
    frame.add_atom(Atom("H"), {0.0, 0.0, 1.0});
    assert(frame.size() == 3);

    assert(frame.topology()[1].name() == "O");
    assert(frame.positions()[1] == Vector3D(0.0, 1.0, 0.0));

    frame.remove(1);
    assert(frame.size() == 2);

    // Removing an atom changes the indexes of atoms after the one removed
    assert(frame.topology()[1].name() == "H");
    assert(frame.positions()[1] == Vector3D(0.0, 0.0, 1.0));
    // [example]
}
