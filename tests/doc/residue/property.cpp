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

    residue.set("first", "alanine group");
    residue.set("second", 42.5);

    assert(residue.get("second")->as_double() == 42.5);
    assert(residue.get("first")->as_string() == "alanine group");

    // Typed access to properties
    assert(residue.get<Property::STRING>("first").value() == "alanine group");
    assert(!residue.get<Property::BOOL>("first"));

    assert(!residue.get("non-existant property"));

    // Override the "first" property
    residue.set("first", false);
    assert(residue.get("first")->as_bool() == false);
    // [example]
}
