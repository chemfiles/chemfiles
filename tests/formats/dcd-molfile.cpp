#include "catch.hpp"

#include "Chemharp.hpp"
using namespace harp;

#define DCDDIR SRCDIR "/data/dcd/"

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in DCD format using Molfile", "[Molfile]"){
    double eps = 1e-4;
    Frame frame;
    Trajectory file(DCDDIR "water.dcd");

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
