#include "catch.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#define LAMMPSDIR SRCDIR "/data/lammps/"

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in LAMMPS .lammpstrj format using Molfile", "[Molfile]"){
    Trajectory file(LAMMPSDIR "polymer.lammpstrj");
    Frame frame = file.read();
    double eps = 1e-3;

    CHECK(frame.natoms() == 1714);
    auto positions = frame.positions();
    CHECK(roughly(positions[0], vector3d(51.8474, 100.348, 116.516), eps));
    // this one has a non zero image index (1 0 0)
    CHECK(roughly(positions[1189], vector3d(116.829, 91.2404, 79.8858), eps));
}
