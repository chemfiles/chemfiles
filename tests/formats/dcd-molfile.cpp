#include "catch.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;


bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in DCD format using Molfile", "[Molfile]"){
    double eps = 1e-4;
    Trajectory file("data/dcd/water.dcd");

    auto frame = file.read();
    CHECK(frame.natoms() == 297);

    auto positions = frame.positions();
    CHECK(roughly(positions[0], vector3d(0.4172191, 8.303366, 11.73717), eps));
    CHECK(roughly(positions[296], vector3d(6.664049, 11.61418, 12.96149), eps));

    auto cell = frame.cell();
    CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
    CHECK(fabs(cell.a() - 15.0) < eps);

    file.read(); // Skip a frame
    frame = file.read();
    CHECK(frame.natoms() == 297);

    positions = frame.positions();
    CHECK(roughly(positions[0], vector3d(0.2990952, 8.31003, 11.72146), eps));
    CHECK(roughly(positions[296], vector3d(6.797599, 11.50882, 12.70423), eps));
}
