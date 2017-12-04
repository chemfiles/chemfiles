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
    frame.add_atom(Atom("O"), {0.0, 0.0, 0.0});
    frame.add_atom(Atom("H"), {0.0, 1.0, 0.0});

    frame.add_bond(0, 1);
    frame.add_bond(1, 2);

    assert(frame.topology().bonds() == std::vector<Bond>({{0, 1}, {1, 2}}));

    frame.remove_bond(1, 0);
    assert(frame.topology().bonds() == std::vector<Bond>({{1, 2}}));

    // This does nothing
    frame.remove_bond(0, 2);
    assert(frame.topology().bonds() == std::vector<Bond>({{1, 2}}));
    // [example]
}
