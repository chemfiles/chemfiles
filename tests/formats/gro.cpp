// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in Gromacs .gro format"){
    Trajectory file("data/gro/ubiquitin.gro");
    Frame frame = file.read();

    CHECK(frame.size() == 1405);
    auto positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(24.93, 24.95, 18.87), 1e-2));
    CHECK(approx_eq(positions[1], Vector3D(25.66, 25.37, 18.33), 1e-2));
    CHECK(approx_eq(positions[678], Vector3D(27.57, 32.25, 37.53), 1e-2));

    CHECK(frame[0].name() == "N");
    CHECK(frame[1].name() == "H1");
    CHECK(frame[678].name() == "O");

    CHECK(frame.topology().residues().size() == 134);
    CHECK(frame.topology().residues()[0].name() == "MET");
    CHECK(frame.topology().residues()[75].name() == "GLY");
}
