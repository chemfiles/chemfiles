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
        CHECK_FALSE(frame.get("name"));
        // Check positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.4172191, 8.303366, 11.73717), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(6.664049, 11.61418, 12.96149), 1e-4));
        // Check time
        CHECK(approx_eq(frame.get("time")->as_double(), 2.02));
    }

    SECTION("Read more than one frame") {
        auto file = Trajectory("data/netcdf/water.nc");
        auto frame = file.read();
        frame = file.read();
        frame = file.read();
        CHECK(frame.size() == 297);
        CHECK_FALSE(frame.get("name"));

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.2990952, 8.31003, 11.72146), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(6.797599, 11.50882, 12.70423), 1e-4));
        CHECK(approx_eq(frame.get("time")->as_double(), 2.04));

        while (!file.done()) {
            frame = file.read();
        }
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.3185586, 8.776042, 11.8927), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(7.089802, 10.35007, 12.8159), 1e-4));
        CHECK(approx_eq(frame.get("time")->as_double(), 3.01));
    }

    SECTION("Missing unit cell") {
        auto file = Trajectory("data/netcdf/no-cell.nc");
        auto frame = file.read();
        CHECK(frame.size() == 1989);
        CHECK(frame.get("name").value() == "Cpptraj Generated trajectory");
        CHECK(frame.cell() == UnitCell());
    }

    SECTION("Scale factor") {
        auto file = Trajectory("data/netcdf/scaled_traj.nc");
        CHECK(file.nsteps() == 26);
        auto frame = file.read_step(12);
        CHECK(frame.size() == 1938);
        CHECK_FALSE(frame.get("name"));

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
    auto check_frame = [](const Frame& frame) {
        CHECK(frame.get("name").value() == "Test Title 123");
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], {0, 0, 0}, 1e-6));
        CHECK(approx_eq(positions[1], {1, 2, 3}, 1e-6));
        CHECK(approx_eq(positions[2], {2, 4, 6}, 1e-6));
        CHECK(approx_eq(positions[3], {3, 6, 9}, 1e-6));

        auto velocities = frame.velocities().value();
        CHECK(approx_eq(velocities[0], {-3, -2, -1}, 1e-6));
        CHECK(approx_eq(velocities[1], {-3, -2, -1}, 1e-6));
        CHECK(approx_eq(velocities[2], {-3, -2, -1}, 1e-6));
        CHECK(approx_eq(velocities[3], {-3, -2, -1}, 1e-6));

        auto cell = frame.cell();
        CHECK(approx_eq(cell.lengths(), {2, 3, 4}, 1e-6));
        CHECK(approx_eq(cell.angles(), {80, 90, 120}, 1e-6));
    };

    auto frame = Frame(UnitCell({2, 3, 4}, {80, 90, 120}));
    frame.set("name", "Test Title 123");
    frame.add_velocities();
    for(size_t i=0; i<4; i++) {
        double d = static_cast<double>(i);
        frame.add_atom(
            Atom("X"),
            {1.0 * d, 2.0 * d, 3.0 * d},
            {-3, -2, -1}
        );
    }

    SECTION("Write new file") {
        auto tmpfile = NamedTempPath(".nc");

        auto file = Trajectory(tmpfile, 'w');
        file.write(frame);
        file.write(frame);
        file.close();

        file = Trajectory(tmpfile, 'r');
        CHECK(file.nsteps() == 2);
        check_frame(file.read());
        check_frame(file.read());
    }

    SECTION("Append to existing file") {
        auto tmpfile = NamedTempPath(".nc");

        {
            auto file = Trajectory(tmpfile, 'w');
            file.write(frame);
        }

        {
            auto file = Trajectory(tmpfile, 'a');
            file.write(frame);
        }

        auto file = Trajectory(tmpfile, 'r');
        CHECK(file.nsteps() == 2);
        check_frame(file.read());
        check_frame(file.read());
    }

    SECTION("Append to new file") {
        auto tmpfile = NamedTempPath(".nc");

        auto file = Trajectory(tmpfile, 'a');
        file.write(frame);
        file.write(frame);
        file.close();

        file = Trajectory(tmpfile, 'r');
        CHECK(file.nsteps() == 2);
        check_frame(file.read());
        check_frame(file.read());
    }
}
