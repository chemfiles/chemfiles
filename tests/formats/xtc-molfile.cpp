// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in Gromacs .gro format using Molfile"){
    Trajectory file("data/xtc/ubiquitin.xtc");
    Frame frame = file.read();
    double eps = 1e-2;

    CHECK(frame.size() == 20455);
    auto positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(24.8277, 24.662, 18.8104), eps));
    CHECK(approx_eq(positions[1], Vector3D(25.5925, 24.9309, 18.208), eps));
    CHECK(approx_eq(positions[678], Vector3D(27.4324, 32.301, 37.6319), eps));
}
