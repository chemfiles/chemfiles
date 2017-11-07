// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto residue = Residue("CLU");

    residue.add_atom(56);
    residue.add_atom(22);
    residue.add_atom(31);

    std::vector<size_t> atoms;
    for (auto atom: residue) {
        atoms.push_back(atom);
    }

    assert(atoms == std::vector<size_t>({22, 31, 56}));
    // [example]
}
