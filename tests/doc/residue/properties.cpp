// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto residue = Residue("ALA");

    residue.set("a string", "the lazy fox");
    residue.set("a number", 122);

    // Iterator over properties in the frame
    for (auto it: residue.properties()) {
        if (it.first == "a string") {
            assert(it.second.as_string() == "the lazy fox");
        } else if (it.first == "a number") {
            assert(it.second.as_double() == 122);
        }
    }
    // [example]
}
