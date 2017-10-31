// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in TNG format"){
    SECTION("Read trajectory") {
        auto file = Trajectory("data/tng/example.tng");
        auto frame = file.read();

        CHECK(frame.size() == 15);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.0f, 1.0f, 1.0f), 1e-6));
        CHECK(approx_eq(positions[11], Vector3D(8.5f, 33.0f, 34.0f), 1e-6));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::INFINITE);

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.size() == 15);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.01562f, 1.02344f, 1.03125f), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(8.5f, 33.0f, 34.0f), 1e-6));
    }

    SECTION("Read velocities"){
        Trajectory file("data/tng/1aki.tng");
        auto frame = file.read();
        CHECK(frame.size() == 38376);

        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[450], Vector3D(-0.144889, 6.50066e-2, -0.764032), 1e-5));
        CHECK(approx_eq(velocities[4653], Vector3D(-1.65949, -0.462240, -0.701133), 1e-5));
    }

    SECTION("Read cell") {
        auto file = Trajectory("data/tng/water.tng");
        auto frame = file.read();

        CHECK(frame.size() == 29700);

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(cell.a() == 15.0);
        CHECK(cell.b() == 15.0);
        CHECK(cell.c() == 15.0);
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
        auto expected = std::vector<Bond>{
            {0, 1}, {0, 2}, {3, 4}, {3, 5}, {6, 7}, {6, 8},
            {9, 10}, {9, 11}, {12, 13}, {12, 14}
        };

        CHECK(bonds.size() == expected.size());
        for (auto bond: expected) {
            CHECK(std::find(bonds.begin(), bonds.end(), bond) != bonds.end());
        }
    }
}
