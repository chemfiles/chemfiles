// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in NetCDF format") {
    SECTION("Read one frame") {
        auto file = Trajectory("data/netcdf/water.nc");
        auto frame = file.read();
        CHECK(frame.size() == 297);
        // Check positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.4172191, 8.303366, 11.73717), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(6.664049, 11.61418, 12.96149), 1e-4));
    }

    SECTION("Read more than one frame") {
        auto file = Trajectory("data/netcdf/water.nc");
        auto frame = file.read();
        frame = file.read();
        frame = file.read();
        CHECK(frame.size() == 297);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.2990952, 8.31003, 11.72146), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(6.797599, 11.50882, 12.70423), 1e-4));

        while (!file.done()) {
            frame = file.read();
        }
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.3185586, 8.776042, 11.8927), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(7.089802, 10.35007, 12.8159), 1e-4));
    }

    SECTION("Missing unit cell") {
        auto file = Trajectory("data/netcdf/no-cell.nc");
        auto frame = file.read();
        CHECK(frame.size() == 1989);
        CHECK(frame.cell() == UnitCell());
    }

    SECTION("Scale factor") {
        auto file = Trajectory("data/netcdf/scaled_traj.nc");
        CHECK(file.nsteps() == 26);
        auto frame = file.read_step(12);
        CHECK(frame.size() == 1938);

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), 1.765 * Vector3D(60.9682, 60.9682, 0), 1e-4));

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.39, 1.39, 0) * 0.455, 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(29.1, 37.41, 0) * 0.455, 1e-4));

        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[1400], Vector3D(0.6854072, 0.09196011, 2.260214) * -0.856, 1e-4));
        CHECK(approx_eq(velocities[1600], Vector3D(-0.3342645, 0.322594, -2.446901) * -0.856, 1e-4));
    }
}

TEST_CASE("Write files in NetCDF format") {
    auto tmpfile = NamedTempPath(".nc");

    auto file = Trajectory(tmpfile, 'w');
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
    CHECK(approx_eq(positions[0], {1, 2, 3}, 1e-4));
    CHECK(approx_eq(positions[1], {1, 2, 3}, 1e-4));
    CHECK(approx_eq(positions[2], {1, 2, 3}, 1e-4));
    CHECK(approx_eq(positions[3], {1, 2, 3}, 1e-4));
}
