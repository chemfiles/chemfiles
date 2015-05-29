#include "catch.hpp"

#include "Chemharp.hpp"
using namespace harp;

#define PDBDIR SRCDIR "/data/pdb/"

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in PDB format using Molfile", "[Molfile]"){
    SECTION("Read trajectory") {
        Trajectory file(PDBDIR"water.pdb");
        Frame frame = file.read();

        CHECK(frame.natoms() == 297);
        auto positions = frame.positions();
        CHECK(roughly(positions[0], Vector3D(0.417f, 8.303f, 11.737f), 1e-3));
        CHECK(roughly(positions[296], Vector3D(6.664f, 11.6148f, 12.961f), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.type() == UnitCell::ORTHOROMBIC);
        CHECK(fabs(cell.a() - 15.0) < 1e-5);

        file >> frame;
        file >> frame;

        positions = frame.positions();
        CHECK(roughly(positions[0], Vector3D(0.299f, 8.310f, 11.721f), 1e-4));
        CHECK(roughly(positions[296], Vector3D(6.798f, 11.509f, 12.704f), 1e-4));
    }

    SECTION("Read bonds") {
        Trajectory file(PDBDIR"MOF-5.pdb");
        Frame frame = file.read();

        auto topology = frame.topology();

        CHECK(topology.natoms() == 65);

        CHECK(topology[0].name() == "ZN");
        CHECK(topology[1].name() == "O");

        CHECK(topology.bonds().size() == 68);

        CHECK(topology.isbond(9, 38));
        CHECK(topology.isbond(58, 62));
        CHECK(topology.isbond(37, 24));
        CHECK(topology.isbond(27, 31));
    }
}
