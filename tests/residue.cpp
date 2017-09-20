// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Residue class usage"){
    auto residue = Residue("ALA", 4);
    CHECK(residue.name() == "ALA");
    CHECK(residue.id() == 4);

    residue = Residue("GUA");
    CHECK(residue.name() == "GUA");
    CHECK(residue.id() == static_cast<uint64_t>(-1));

    CHECK(residue.size() == 0);
    residue.add_atom(0);
    residue.add_atom(56);
    residue.add_atom(30);
    CHECK(residue.size() == 3);

    residue.add_atom(56);
    CHECK(residue.size() == 3);

    auto atoms = std::vector<size_t>(residue.begin(), residue.end());
    auto expected = std::vector<size_t>{0, 30, 56};
    CHECK(atoms == expected);

    CHECK(residue.contains(56));
}
