#include "catch.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#define GRODIR SRCDIR "/data/gro/"

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in Gromacs .gro format using Molfile", "[Molfile]"){
    Trajectory file(GRODIR"ubiquitin.gro");
    Frame frame = file.read();

    CHECK(frame.natoms() == 1405);
    auto positions = frame.positions();
    CHECK(roughly(positions[0], vector3d(24.93f, 24.95f, 18.87f), 1e-2));
    CHECK(roughly(positions[1], vector3d(25.66f, 25.37f, 18.33f), 1e-2));
    CHECK(roughly(positions[678], vector3d(27.57f, 32.25f, 37.53f), 1e-2));
}
