#include "catch.hpp"

#include "Chemharp.hpp"
using namespace harp;

#include <cstdlib>
#include <iostream>

#define PDBDIR SRCDIR "/data/pdb/"

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in PDB format using Molfile", "[Molfile]"){
    Trajectory file(PDBDIR"water.pdb");

    Frame frame = file.read();

    CHECK(frame.natoms() == 297);
    auto positions = frame.positions();
    CHECK(roughly(positions[0], Vector3D(0.417f, 8.303f, 11.737f), 1e-3));
    CHECK(roughly(positions[296], Vector3D(6.664f, 11.6148f, 12.961f), 1e-3));

    auto cell = frame.cell();
    CHECK(cell.type() == UnitCell::ORTHOROMBIC);
    CHECK(fabs(cell.a() - 15.0) < 1e-5);
}
