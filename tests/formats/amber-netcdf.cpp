// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;


TEST_CASE("Read files in NetCDF format") {
    SECTION("Read one frame") {
        Trajectory file("data/netcdf/water.nc");
        auto frame = file.read();
        CHECK(frame.size() == 297);
        // Check positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.4172191, 8.303366, 11.73717), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(6.664049, 11.61418, 12.96149), 1e-4));
    }

    SECTION("Read more than one frame") {
        Trajectory file("data/netcdf/water.nc");
        auto frame = file.read();
        frame = file.read();
        frame = file.read();
        CHECK(frame.size() == 297);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.2990952, 8.31003, 11.72146), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(6.797599, 11.50882, 12.70423), 1e-4));

        while (!file.done()){
            frame = file.read();
        }
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.3185586, 8.776042, 11.8927), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(7.089802, 10.35007, 12.8159), 1e-4));
    }

    SECTION("Missing unit cell") {
        Trajectory file("data/netcdf/no-cell.nc");
        auto frame = file.read();
        CHECK(frame.size() == 1989);
        CHECK(frame.cell() == UnitCell());
    }
}


TEST_CASE("Write files in NetCDF format") {
    auto tmpfile = NamedTempPath(".nc");

    Trajectory file(tmpfile, 'w');
    Frame frame;
    frame.resize(4);
    auto positions = frame.positions();
    for(size_t i=0; i<4; i++) {
        positions[i] = Vector3D(1, 2, 3);
    }

    file.write(frame);
    file.close();

    Trajectory check(tmpfile, 'r');
    frame = check.read();
    positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(1, 2, 3), 1e-4));
    CHECK(approx_eq(positions[1], Vector3D(1, 2, 3), 1e-4));
    CHECK(approx_eq(positions[2], Vector3D(1, 2, 3), 1e-4));
    CHECK(approx_eq(positions[3], Vector3D(1, 2, 3), 1e-4));
}
