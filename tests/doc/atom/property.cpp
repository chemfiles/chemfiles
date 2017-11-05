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

    atom.set("first", "this is a carbon");
    atom.set("second", 42.5);

    assert(atom.get("second")->as_double() == 42.5);
    assert(atom.get("first")->as_string() == "this is a carbon");

    assert(!atom.get("non-existant property"));

    // Override the "first" property
    atom.set("first", false);
    assert(atom.get("first")->as_bool() == false);
    // [example]
}
