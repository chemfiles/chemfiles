// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"
using namespace chemfiles;

TEST_CASE("Read files in Amber Restart format") {
    SECTION("Water") {
        auto file = Trajectory("data/netcdf/water.ncrst");
        CHECK(file.nsteps() == 1);
        auto frame = file.read();
        CHECK(frame.size() == 297);
        CHECK(frame.get("name").value() == "Cpptraj Generated Restart");

        // Check cell
        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), Vector3D(15.0, 15.0, 15.0), 1e-4));

        // Check positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.4172191, 8.303366, 11.73717), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(6.664049, 11.61418, 12.96149), 1e-4));

        // Check time
        // time in water.ncrst is in ps, but in water.nc it's in fs
        CHECK(approx_eq(frame.get("time")->as_double(), 2020.0));
    }

    SECTION("Missing unit cell") {
        auto file = Trajectory("data/netcdf/no-cell.ncrst");
        // Check `read_step`
        auto frame = file.read_step(0);
        CHECK(frame.size() == 1989);
        CHECK(frame.get("name").value() == "Cpptraj Generated Restart");
        CHECK(frame.cell() == UnitCell());
    }

    SECTION("Scale factor") {
        auto file = Trajectory("data/netcdf/scaled_traj.ncrst");
        auto frame = file.read();
        CHECK(frame.size() == 1938);
        CHECK(frame.get("name").value() == "Cpptraj Generated Restart");

        // Check cell
        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), 1.765 * Vector3D(60.9682, 60.9682, 0), 1e-4));

        // Check positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.39, 1.39, 0) * 0.455, 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(29.10, 37.41, 0) * 0.455, 1e-4));

        // Check velocities
        auto velocities = *frame.velocities();
        CHECK(
            approx_eq(velocities[1400], Vector3D(-0.042603, -0.146347, 12.803150) * -0.856, 1e-4));
        CHECK(approx_eq(velocities[1600], Vector3D(0.002168, 0.125240, 4.188500) * -0.856, 1e-4));
    }
}

TEST_CASE("Write files in Amber Restart format") {
    auto check_frame = [](const Frame& frame) {
        CHECK(frame.get("name").value() == "Test Title 123");
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], {0, 0, 0}, 1e-9));
        CHECK(approx_eq(positions[1], {1, 2, 3}, 1e-9));
        CHECK(approx_eq(positions[2], {2, 4, 6}, 1e-9));
        CHECK(approx_eq(positions[3], {3, 6, 9}, 1e-9));

        auto velocities = frame.velocities().value();
        CHECK(approx_eq(velocities[0], {-3, -2, -1}, 1e-9));
        CHECK(approx_eq(velocities[1], {-3, -2, -1}, 1e-9));
        CHECK(approx_eq(velocities[2], {-3, -2, -1}, 1e-9));
        CHECK(approx_eq(velocities[3], {-3, -2, -1}, 1e-9));

        auto cell = frame.cell();
        CHECK(approx_eq(cell.lengths(), {2, 3, 4}, 1e-9));
        CHECK(approx_eq(cell.angles(), {80, 90, 120}, 1e-9));
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
        auto tmpfile = NamedTempPath(".ncrst");

        {
            auto file = Trajectory(tmpfile, 'w');
            file.write(frame);
            CHECK_THROWS_WITH(file.write(frame), "AMBER Restart format only supports writing one frame");
        }

        {
            auto file = Trajectory(tmpfile, 'a');
            CHECK_THROWS_WITH(file.write(frame), "AMBER Restart format only supports writing one frame");
        }

        auto file = Trajectory(tmpfile, 'r');
        CHECK(file.nsteps() == 1);
        check_frame(file.read());
    }

    SECTION("Append to new file") {
        auto tmpfile = NamedTempPath(".ncrst");

        auto file = Trajectory(tmpfile, 'a');
        file.write(frame);
        CHECK_THROWS_WITH(file.write(frame), "AMBER Restart format only supports writing one frame");
        file.close();

        file = Trajectory(tmpfile, 'r');
        CHECK(file.nsteps() == 1);
        check_frame(file.read());
    }
}
