#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in Gromacs .gro format using Molfile", "[Molfile]"){
    Trajectory file("data/xtc/ubiquitin.xtc");
    Frame frame = file.read();
    double eps = 1e-2;

    CHECK(frame.natoms() == 20455);
    auto positions = frame.positions();
    CHECK(approx_eq(positions[0], vector3d(24.8277, 24.662, 18.8104), eps));
    CHECK(approx_eq(positions[1], vector3d(25.5925, 24.9309, 18.208), eps));
    CHECK(approx_eq(positions[678], vector3d(27.4324, 32.301, 37.6319), eps));
}
