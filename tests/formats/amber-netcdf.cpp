#include <cstdio>

#include "catch.hpp"

#include "config.hpp"
#if HAVE_NETCDF

#include "Chemharp.hpp"
using namespace harp;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;
const double EPS = 1e-5;

#define NCDIR SRCDIR "/data/netcdf/"

#include <iostream>
using namespace std;

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in NetCDF format", "[Amber NetCDF]"){
    Trajectory file(NCDIR "water.nc");
    Frame frame;

    SECTION("Stream style reading"){
        file >> frame;
        CHECK(frame.natoms() == 297);
        // Check positions
        auto positions = frame.positions();

        CHECK(roughly(positions[0], Vector3D(0.4172191f, 8.303366f, 11.73717f), 1e-4));
        CHECK(roughly(positions[296], Vector3D(6.664049f, 11.61418f, 12.96149f), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.type() == UnitCell::ORTHOROMBIC);
        CHECK(fabs(cell.a() - 15.0) < EPS);
    }

    SECTION("Method style reading"){
        frame = file.read();
        CHECK(frame.natoms() == 297);
        // Check positions
        auto positions = frame.positions();
        CHECK(roughly(positions[0], Vector3D(0.4172191f, 8.303366f, 11.73717f), 1e-4));
        CHECK(roughly(positions[296], Vector3D(6.664049f, 11.61418f, 12.96149f), 1e-4));
    }

    SECTION("Read more than one frame"){
        file >> frame;
        file >> frame;
        file >> frame;
        CHECK(frame.natoms() == 297);

        auto positions = frame.positions();
        CHECK(roughly(positions[0], Vector3D(0.2990952f, 8.31003f, 11.72146f), 1e-4));
        CHECK(roughly(positions[296], Vector3D(6.797599f, 11.50882f, 12.70423f), 1e-4));

        while (not file.done()){
            file >> frame;
        }
        positions = frame.positions();
        CHECK(roughly(positions[0], Vector3D(0.3185586f, 8.776042f, 11.8927f), 1e-4));
        CHECK(roughly(positions[296], Vector3D(7.089802f, 10.35007f, 12.8159f), 1e-4));
    }
}


TEST_CASE("Write files in NetCDF format", "[Amber NetCDF]"){
    SECTION("Write the file") {
        Trajectory file("tmp.nc", "w");

        Array3D positions(4);
        for(size_t i=0; i<4; i++)
            positions[i] = Vector3D(1, 2, 3);

        Frame frame;
        frame.topology(dummy_topology(4));
        frame.positions(positions);

        file << frame;
    }

    SECTION("Check the file") {
        Trajectory check("tmp.nc", "r");
        auto frame = check.read();
        auto positions = frame.positions();
        CHECK(roughly(positions[0], Vector3D(1, 2, 3), 1e-4));
        CHECK(roughly(positions[1], Vector3D(1, 2, 3), 1e-4));
        CHECK(roughly(positions[2], Vector3D(1, 2, 3), 1e-4));
        CHECK(roughly(positions[3], Vector3D(1, 2, 3), 1e-4));
        remove("tmp.nc");
    }
}

#endif // HAVE_NETCDF
