// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"
using namespace chemfiles;

TEST_CASE("Read files in TRR format") {
    SECTION("Read trajectory: Ubiquitin") {
        auto file = Trajectory("data/trr/ubiquitin.trr");
        CHECK(file.nsteps() == 2);
        auto frame = file.read();

        CHECK(frame.step() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 20455);
        CHECK(frame.velocities());

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.8277, 24.6620, 18.8104), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(23.7713, 24.5589, 21.4702), 1e-4));

        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[100], Vector3D(-2.8750, 2.8159, 1.2047), 1e-4));
        CHECK(approx_eq(velocities[111], Vector3D(-3.0103, 3.3177, -0.8265), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 55.6800, 1e-4));
        CHECK(approx_eq(cell.b(), 58.8700, 1e-4));
        CHECK(approx_eq(cell.c(), 62.5700, 1e-4));

        frame = file.read();

        CHECK(frame.step() == 25000);
        CHECK(approx_eq(frame.get("time")->as_double(), 50));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 20455);
        CHECK(frame.velocities());

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.8625, 25.0285, 18.5973), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(23.7971, 24.2192, 21.1569), 1e-4));

        velocities = *frame.velocities();
        CHECK(approx_eq(velocities[100], Vector3D(-5.3413, -1.2646, 1.0216), 1e-4));
        CHECK(approx_eq(velocities[111], Vector3D(-1.7052, 1.0418, 5.3836), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 55.6800, 1e-4));
        CHECK(approx_eq(cell.b(), 58.8700, 1e-4));
        CHECK(approx_eq(cell.c(), 62.5700, 1e-4));
    }

    SECTION("Read trajectory: Water") {
        auto file = Trajectory("data/trr/water.trr");
        CHECK(file.nsteps() == 100);
        auto frame = file.read();

        CHECK(frame.step() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 297);
        CHECK(!frame.velocities());

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.4172, 8.3034, 11.7372), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(10.4311, 10.1225, 4.1827), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 15.0, 1e-4));
        CHECK(approx_eq(cell.b(), 15.0, 1e-4));
        CHECK(approx_eq(cell.c(), 15.0, 1e-4));

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.step() == 2);
        CHECK(approx_eq(frame.get("time")->as_double(), 0.2, 1e-4));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 297);
        CHECK(!frame.velocities());

        positions = frame.positions();
        CHECK(approx_eq(positions[100], Vector3D(11.8583, 12.2360, 1.3922), 1e-4));
        CHECK(approx_eq(positions[111], Vector3D(4.2604, 12.1676, 7.6417), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 15.0, 1e-4));
        CHECK(approx_eq(cell.b(), 15.0, 1e-4));
        CHECK(approx_eq(cell.c(), 15.0, 1e-4));

        frame = file.read_step(75); // skip forward

        CHECK(frame.step() == 75);
        CHECK(approx_eq(frame.get("time")->as_double(), 7.5));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 297);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.8856, 8.9006, 11.4770), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(9.5294, 9.4827, 5.2602), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 15.0, 1e-4));
        CHECK(approx_eq(cell.b(), 15.0, 1e-4));
        CHECK(approx_eq(cell.c(), 15.0, 1e-4));

        frame = file.read_step(50); // skip behind previous step

        CHECK(frame.step() == 50);
        CHECK(approx_eq(frame.get("time")->as_double(), 5.0));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 297);

        positions = frame.positions();
        CHECK(approx_eq(positions[100], Vector3D(12.9620, 12.5275, 0.6565), 1e-4));
        CHECK(approx_eq(positions[111], Vector3D(4.5618, 12.8612, 8.5790), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 15.0, 1e-4));
        CHECK(approx_eq(cell.b(), 15.0, 1e-4));
        CHECK(approx_eq(cell.c(), 15.0, 1e-4));
    }

    SECTION("Read trajectory: 1AKI") {
        auto file = Trajectory("data/trr/1aki.trr");
        CHECK(file.nsteps() == 6);
        auto frame = file.read();

        CHECK(frame.step() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 38376);
        CHECK(frame.velocities());

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(44.2396, 34.0603, 24.6208), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(46.3897, 31.1119, 26.4815), 1e-4));

        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[0], Vector3D(-2.5418, 3.0469, 0.8539), 1e-4));
        CHECK(approx_eq(velocities[11], Vector3D(21.9733, 5.7779, -4.5049), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 73.3925, 1e-4));
        CHECK(approx_eq(cell.b(), 73.3925, 1e-4));
        CHECK(approx_eq(cell.c(), 73.3925, 1e-4));

        frame = file.read_step(5); // skip forward

        CHECK(frame.step() == 50);
        CHECK(approx_eq(frame.get("time")->as_double(), 0.1, 1e-4));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 38376);
        CHECK(frame.velocities());

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(44.3276, 33.9384, 24.4364), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(47.0237, 31.4641, 25.9076), 1e-4));

        velocities = *frame.velocities();
        CHECK(approx_eq(velocities[0], Vector3D(-0.5480, 1.1550, 2.5022), 1e-4));
        CHECK(approx_eq(velocities[11], Vector3D(-6.2324, 5.5043, -6.6486), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 73.3925, 1e-4));
        CHECK(approx_eq(cell.b(), 73.3925, 1e-4));
        CHECK(approx_eq(cell.c(), 73.3925, 1e-4));
    }
}
