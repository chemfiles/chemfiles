// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto topology = Topology();
    assert(topology.size() == 0);

    topology.resize(10);
    assert(topology.size() == 10);

    // reserve allocate memory, but does not change the size
    topology.reserve(100);
    assert(topology.size() == 10);
    // [example]
}
