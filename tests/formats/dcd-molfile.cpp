// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in DCD format using Molfile"){
    double eps = 1e-4;
    Trajectory file("data/dcd/water.dcd");

    auto frame = file.read();
    CHECK(frame.size() == 297);

    auto positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(0.4172191, 8.303366, 11.73717), eps));
    CHECK(approx_eq(positions[296], Vector3D(6.664049, 11.61418, 12.96149), eps));

    auto cell = frame.cell();
    CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
    CHECK(fabs(cell.a() - 15.0) < eps);

    file.read(); // Skip a frame
    frame = file.read();
    CHECK(frame.size() == 297);

    positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(0.2990952, 8.31003, 11.72146), eps));
    CHECK(approx_eq(positions[296], Vector3D(6.797599, 11.50882, 12.70423), eps));
}
