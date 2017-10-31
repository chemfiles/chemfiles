// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in Molden format using Molfile") {
    auto file = Trajectory("data/molden/benzene.molden");
    auto frame = file.read();

    CHECK(frame.size() == 12);
    auto positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(0.930994, 1.038515, 0.000010), 1e-6));
    CHECK(approx_eq(positions[6], Vector3D(-0.771849, 2.358500, 0.000020), 1e-6));

    auto& topology = frame.topology();
    CHECK(topology[0].name() == "C");
    CHECK(topology[6].name() == "H");
}
