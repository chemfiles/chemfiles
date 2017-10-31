// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in LAMMPS .lammpstrj format using Molfile"){
    SECTION("Polymer") {
        Trajectory file("data/lammps/polymer.lammpstrj");
        Frame frame = file.read();
        double eps = 1e-3;

        CHECK(frame.size() == 1714);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(51.8474, 100.348, 116.516), eps));
        // this one has a non zero image index (1 0 0)
        CHECK(approx_eq(positions[1189], Vector3D(116.829, 91.2404, 79.8858), eps));
    }

    SECTION("NaCl") {
        Trajectory file("data/lammps/nacl.lammpstrj");
        Frame frame = file.read();

        CHECK(frame.size() == 512);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0, 0.0, 0.0), 1e-3));
        CHECK(approx_eq(positions[222], Vector3D(14.1005, 0.0, 8.4603), 1e-3));

        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[0], Vector3D(-0.00258494, 0.00270859, -0.00314039), 1e-7));
        CHECK(approx_eq(velocities[222], Vector3D(-0.00466812, -0.00196397, -0.000147051), 1e-7));
    }
}
