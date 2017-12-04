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
    frame.add_atom(Atom("0"), {0.0, 0.0, 0.0});
    frame.add_atom(Atom("H"), {0.0, 1.2, 0.0});

    frame.add_bond(0, 1);
    frame.add_bond(0, 2);

    auto selection = Selection("name H");
    std::vector<size_t> matches = selection.list(frame);
    assert(matches.size() == 2);
    assert(matches == std::vector<size_t>({0, 2}));
    // [example]
}
