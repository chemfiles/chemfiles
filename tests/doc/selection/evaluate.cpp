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
    frame.add_atom(Atom("H"), {1.2, 0.0, 0.0});
    frame.add_atom(Atom("O"), {0.0, 0.0, 0.0});
    frame.add_atom(Atom("H"), {0.0, 1.2, 0.0});

    frame.topology().add_bond(0, 1);
    frame.topology().add_bond(0, 2);

    auto selection = Selection("pairs: name(#1) H and name(#2) O");
    std::vector<Match> matches = selection.evaluate(frame);
    assert(matches.size() == 2);
    assert(matches == std::vector<Match>({{0u, 1u}, {2u, 1u}}));
    // [example]
}
