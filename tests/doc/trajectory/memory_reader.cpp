// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

TEST_CASE() {
    // [example]
    // Text based formats and some binary support reading from memory
    auto aromatics = std::string("c1ccccc1\nc1ccco1\nc1ccccn1\n");
    auto trajectory = Trajectory::memory_reader(aromatics.data(), aromatics.size(), "SMI");
    auto benzene = trajectory.read();
    CHECK(benzene.size() == 6);

    // Other formats do not and will throw an error
    CHECK_THROWS_WITH(
        Trajectory::memory_reader(aromatics.data(), aromatics.size(), "DCD"),
        "in-memory IO is not supported for the 'DCD' format"
    );

    // [example]
}
