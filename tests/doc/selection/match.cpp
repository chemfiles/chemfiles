// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    auto selection = Selection("three: all");
    auto frame = Frame();
    frame.add_atom(Atom("H"), {0.0, 0.0, 0.0});
    frame.add_atom(Atom("O"), {0.0, 0.0, 0.0});
    frame.add_atom(Atom("H"), {0.0, 0.0, 0.0});
    // [example]
    // selection and frame are already created
    auto matches = selection.evaluate(frame);

    auto match = matches[2];
    assert(match.size() == selection.size());
    assert(match.size() == 3);

    assert(match[0] == 1);
    assert(match[1] == 0);
    assert(match[2] == 2);
    // [example]
}
