// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"
using namespace chemfiles;

TEST_CASE("Read files in TNG format") {
    SECTION("Read trajectory") {
        auto file = Trajectory("data/tng/example.tng");
        CHECK(file.size() == 10);
        auto frame = file.read();

        CHECK(frame.size() == 15);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(10.0, 10.0, 10.0), 1e-5));
        CHECK(approx_eq(positions[11], Vector3D(85.0, 330.0, 340.0), 1e-5));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::INFINITE);

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.size() == 15);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(10.1562, 10.2344, 10.3125), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(85.0, 330.0, 340.0), 1e-5));
    }

    SECTION("Read velocities") {
        auto file = Trajectory("data/tng/1aki.tng");
        CHECK(file.size() == 6);
        auto frame = file.read();
        CHECK(frame.size() == 38376);
        CHECK(frame.index() == 0);
        CHECK(frame.get("simulation_step")->as_double() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0.0, 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), Vector3D(73.39250, 73.39250, 73.39250), 1e-5));

        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[450], Vector3D(-1.44889, 6.50066e-1, -7.64032), 1e-4));
        CHECK(approx_eq(velocities[4653], Vector3D(-16.5949, -4.62240, -7.01133), 1e-4));

        frame = file.read_at(5);
        CHECK(frame.size() == 38376);
        CHECK(frame.index() == 5);
        CHECK(frame.get("simulation_step")->as_double() == 50);
        CHECK(approx_eq(frame.get("time")->as_double(), 0.1, 1e-4));

        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), Vector3D(73.39250, 73.39250, 73.39250), 1e-5));

        velocities = *frame.velocities();
        CHECK(approx_eq(velocities[450], Vector3D(8.23913, 2.99123, 10.5270), 1e-4));
        CHECK(approx_eq(velocities[4653], Vector3D(-48.8318, -5.90270, -6.86679), 1e-4));
    }

    SECTION("Read cell") {
        auto file = Trajectory("data/tng/water.tng");
        auto frame = file.read();
        CHECK(frame.size() == 29700);

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(cell.lengths() == Vector3D(15.0, 15.0, 15.0));

        file = Trajectory("data/tng/1vln-triclinic.tng");
        frame = file.read();
        CHECK(frame.size() == 14520);

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::TRICLINIC);

        CHECK(approx_eq(cell.lengths(), Vector3D(78.8, 79.3, 133.3), 1e-5));
        CHECK(approx_eq(cell.angles(), Vector3D(97.1, 90.2, 97.5), 1e-5));
    }

    SECTION("Read topology") {
        auto file = Trajectory("data/tng/example.tng");
        auto topology = file.read().topology();

        CHECK(topology.size() == 15);
        CHECK(topology[0].name() == "O");
        CHECK(topology[0].type() == "O");
        CHECK(topology[1].name() == "HO1");
        CHECK(topology[1].type() == "H");
        CHECK(topology[2].name() == "HO2");
        CHECK(topology[2].type() == "H");

        CHECK(topology.residues().size() == 5);
        auto residue = topology.residues()[0];
        CHECK(residue.size() == 3);
        CHECK(residue.contains(0));
        CHECK(residue.contains(1));
        CHECK(residue.contains(2));

        auto bonds = topology.bonds();
        auto expected = std::vector<Bond>{{0, 1}, {0, 2},  {3, 4},  {3, 5},   {6, 7},
                                          {6, 8}, {9, 10}, {9, 11}, {12, 13}, {12, 14}};

        CHECK(bonds.size() == expected.size());
        for (auto bond : expected) {
            CHECK(std::find(bonds.begin(), bonds.end(), bond) != bonds.end());
        }
    }

    SECTION("Non-consecutive frame indexes") {
        // cf https://github.com/chemfiles/chemfiles/issues/242
        auto file = Trajectory("data/tng/cobrotoxin.tng");
        REQUIRE(file.size() == 3);

        auto frame = file.read();
        CHECK(frame.index() == 0);
        CHECK(frame.get("simulation_step")->as_double() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0.0, 1e-4));

        frame = file.read();
        CHECK(frame.index() == 1);
        CHECK(frame.get("simulation_step")->as_double() == 25000);
        CHECK(approx_eq(frame.get("time")->as_double(), 50.0, 1e-4));

        frame = file.read();
        CHECK(frame.index() == 2);
        CHECK(frame.get("simulation_step")->as_double() == 50000);
        CHECK(approx_eq(frame.get("time")->as_double(), 100.0, 1e-4));

        frame = file.read_at(0);
        CHECK(frame.size() == 19385);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[5569], Vector3D(14.94, 4.03, 19.89), 1e-5));
        CHECK(approx_eq(positions[11675], Vector3D(44.75, 16.05, 6.1), 1e-5));
    }
}
