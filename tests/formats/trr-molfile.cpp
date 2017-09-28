// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;


TEST_CASE("Read files in Gromacs .trr format using Molfile"){
    double eps = 1e-4;
    SECTION("Ubiquitin"){
        Trajectory file("data/trr/ubiquitin.trr");
        auto frame = file.read();

        CHECK(frame.natoms() == 20455);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.8277, 24.662, 18.8104), eps));
        CHECK(approx_eq(positions[1], Vector3D(25.5925, 24.9309, 18.208), eps));
        CHECK(approx_eq(positions[678], Vector3D(27.4324, 32.301, 37.6319), eps));
    }

    SECTION("Water"){
        Trajectory file("data/trr/water.trr");
        auto frame = file.read();
        CHECK(frame.natoms() == 297);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.4172191, 8.303366, 11.73717), eps));
        CHECK(approx_eq(positions[296], Vector3D(6.664049, 11.61418, 12.96149), eps));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(fabs(cell.a() - 15.0) < eps);

        file.read();
        frame = file.read();
        CHECK(frame.natoms() == 297);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.2990952, 8.31003, 11.72146), eps));
        CHECK(approx_eq(positions[296], Vector3D(6.797599, 11.50882, 12.70423), eps));
    }
}
