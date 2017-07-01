// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#include<iostream>

TEST_CASE("Read files in LAMMPS data format", "[LAMMPS]") {
    SECTION("File created by VMD/Topotools") {
        Trajectory file("data/lammps-data/solvated.lmp", 'r', "LAMMPS Data");
        Frame frame = file.read();

        CHECK(frame.natoms() == 7772);

        CHECK(frame.cell().shape() == UnitCell::ORTHORHOMBIC);
        CHECK(frame.cell().a() == 34.023997999999999);
        CHECK(frame.cell().b() == 34.023998000000006);
        CHECK(frame.cell().c() == 163.03599500000001);

        auto positions = frame.positions();
        CHECK(positions[0] == vector3d(4.253000, 12.759000, 63.506001));
        CHECK(positions[364] == vector3d(8.134000, 2.322000, 82.219002));
        CHECK(positions[653] == vector3d(6.184000, 8.134000, 104.334000));

        auto& topology = frame.topology();
        CHECK(topology.bonds().size() == 6248);
        CHECK(topology[3].name() == "Zn");
        CHECK(topology[3].type() == "Zn");
        CHECK(topology[3].mass() == 65.408997);

        CHECK(topology[12].name() == "C1");
        CHECK(topology[12].type() == "C1");
        CHECK(topology[12].mass() == 42.0);

        // Check the read_step function
        frame = file.read_step(0);
        positions = frame.positions();
        CHECK(positions[0] == vector3d(4.253000, 12.759000, 63.506001));
        CHECK(positions[364] == vector3d(8.134000, 2.322000, 82.219002));
        CHECK(positions[653] == vector3d(6.184000, 8.134000, 104.334000));
    }

    SECTION("File created with LAMMPS") {
        Trajectory file("data/lammps-data/data.body", 'r', "LAMMPS Data");
        Frame frame = file.read();

        CHECK(frame.natoms() == 100);
        CHECK(frame.cell() == UnitCell(31.064449134, 31.064449134, 1.0));

        auto positions = frame.positions();
        CHECK(positions[0] == vector3d(-15.5322, -15.5322, 0.0));
        CHECK(positions[22] == vector3d(-9.31933, -9.31933, 0.0));

        REQUIRE(frame.velocities());
        auto velocities = *frame.velocities();
        CHECK(velocities[5] == vector3d(1.14438145745, 4.42784814304, 1.75516442452));
        CHECK(velocities[0] == vector3d(1.02255489961, 2.92322463726, 4.88805110017));
        CHECK(velocities[1] == vector3d(0.111646059519, 0.474226666855, 0.68604865644));
        CHECK(velocities[42] == vector3d(4.70147770939, 2.13317266836, 1.29333445263));

        auto& topology = frame.topology();
        CHECK(topology.bonds().size() == 0);
        CHECK(topology[0].mass() == 6);
        CHECK(topology[1].mass() == 4);
        CHECK(topology[2].mass() == 3);

        CHECK(topology[12].name() == "1");
        CHECK(topology[12].type() == "1");
    }
}
