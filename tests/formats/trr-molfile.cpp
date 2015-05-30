#include "catch.hpp"

#include "Chemharp.hpp"
using namespace harp;

#define TRRDIR SRCDIR "/data/trr/"

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in Gromacs .trr format using Molfile", "[Molfile]"){
    double eps = 1e-4;
    Frame frame;
    SECTION("Ubiquitin"){
        Trajectory file(TRRDIR"ubiquitin.trr");
        file >> frame;

        CHECK(frame.natoms() == 20455);
        auto positions = frame.positions();
        CHECK(roughly(positions[0], Vector3D(24.8277f, 24.662f, 18.8104f), eps));
        CHECK(roughly(positions[1], Vector3D(25.5925f, 24.9309f, 18.208f), eps));
        CHECK(roughly(positions[678], Vector3D(27.4324f, 32.301f, 37.6319f), eps));
    }

    SECTION("Water"){
        Trajectory file(TRRDIR "water.trr");
        Frame frame;

        file >> frame;
        CHECK(frame.natoms() == 297);

        auto positions = frame.positions();
        CHECK(roughly(positions[0], Vector3D(0.4172191f, 8.303366f, 11.73717f), eps));
        CHECK(roughly(positions[296], Vector3D(6.664049f, 11.61418f, 12.96149f), eps));

        auto cell = frame.cell();
        CHECK(cell.type() == UnitCell::ORTHOROMBIC);
        CHECK(fabs(cell.a() - 15.0) < eps);

        file >> frame;
        file >> frame;
        CHECK(frame.natoms() == 297);

        positions = frame.positions();
        CHECK(roughly(positions[0], Vector3D(0.2990952f, 8.31003f, 11.72146f), eps));
        CHECK(roughly(positions[296], Vector3D(6.797599f, 11.50882f, 12.70423f), eps));
    }
}
