#include "catch.hpp"

#include "Chemharp.hpp"
using namespace harp;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;
const double EPS = 1e-5;

#define NCDIR SRCDIR "/files/netcdf/"

#include <iostream>
using namespace std;

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in NetCDF format", "[Amber NetCDF]"){
    auto file = Trajectory(NCDIR "water.nc");
    Frame frame;

    SECTION("Stream style reading"){
        file >> frame;
        CHECK(frame.natoms() == 297);
        // Check positions
        auto positions = frame.positions();
        auto p = positions[0];

        CHECK(roughly(positions[0], Vector3D(0.35772f, 8.3064f, 11.7449f), 1e-4));
        CHECK(roughly(positions[296], Vector3D(6.70083f, 11.6056f, 12.8412f), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.type() == UnitCell::ORTHOROMBIC);
        CHECK(fabs(cell.a() - 15.0) < EPS);
    }

    SECTION("Method style reading"){
        frame = file.read_next_step();
        CHECK(frame.natoms() == 297);
        // Check positions
        auto positions = frame.positions();
        CHECK(roughly(positions[0], Vector3D(0.35772f, 8.3064f, 11.7449f), 1e-4));
        CHECK(roughly(positions[296], Vector3D(6.70083f, 11.6056f, 12.8412f), 1e-4));
    }
}
