// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto atom = Atom("C");

    atom.set("a string", "this is a carbon");
    atom.set("a number", 42.5);

    // Iterator over properties in the atom
    for (auto it: atom.properties()) {
        if (it.first == "a string") {
            assert(it.second.as_string() == "this is a carbon");
        } else if (it.first == "a number") {
            assert(it.second.as_double() == 42.5);
        }
    }
    // [example]
}
