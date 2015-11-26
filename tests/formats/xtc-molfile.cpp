#include "catch.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#define XTCDIR SRCDIR "/data/xtc/"

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in Gromacs .gro format using Molfile", "[Molfile]"){
    Trajectory file(XTCDIR"ubiquitin.xtc");
    Frame frame = file.read();
    double eps = 1e-2;

    CHECK(frame.natoms() == 20455);
    auto positions = frame.positions();
    CHECK(roughly(positions[0], vector3d(24.8277f, 24.662f, 18.8104f), eps));
    CHECK(roughly(positions[1], vector3d(25.5925f, 24.9309f, 18.208f), eps));
    CHECK(roughly(positions[678], vector3d(27.4324f, 32.301f, 37.6319f), eps));
}
