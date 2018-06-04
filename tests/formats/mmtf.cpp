// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include <iostream>
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"

#include "chemfiles/files/MMTFFile.hpp"

using namespace chemfiles;

TEST_CASE("Read files in MMTF format") {

    SECTION("Read") {
        // Just checking constructor and destructor
        MMTFFile file("data/mmtf/4HHB.mmtf", File::READ);
    }

    SECTION("Read single step") {
        Trajectory file("data/mmtf/4HHB.mmtf");
        Frame frame = file.read();

        // If comparing to the RCSB-PDB file,
        // remember that TER increases the number of atoms
        CHECK(frame.size() == 4779);

        // Check reading positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(6.204, 16.869, 4.854), 1e-3));
        CHECK(approx_eq(positions[296], Vector3D(10.167, -7.889, -16.138 ), 1e-3));
        CHECK(approx_eq(positions[4778], Vector3D(-1.263, -2.837, -21.251 ), 1e-3));

        // Check the unit cell
        auto cell = frame.cell();
        CHECK(approx_eq(cell.a(), 63.150, 1e-3));
        CHECK(approx_eq(cell.b(), 83.590, 1e-3));
        CHECK(approx_eq(cell.c(), 53.800, 1e-3));
        CHECK(approx_eq(cell.alpha(), 90.00, 1e-3));
        CHECK(approx_eq(cell.beta(), 99.34, 1e-3));
        CHECK(approx_eq(cell.gamma(), 90.00, 1e-3));

        // Check residue information
        CHECK(frame.topology().residues().size() == 801);

        // Iron in Heme
        auto residue = *frame.topology().residue_for_atom(4557);
        CHECK(residue.size() == 43);
        CHECK(residue.name() == "HEM");

        // Check residue connectivity
        const auto& topo = frame.topology();
        CHECK(topo.are_linked(topo.residue(0), topo.residue(1)));
        CHECK(!topo.are_linked(topo.residue(0), topo.residue(2)));

        // Chain information
        CHECK(residue.get("chainid"));
        CHECK(residue.get("chainname"));
        CHECK(residue.get("chainindex"));

        CHECK(residue.get("chainid")->as_string() == "J");
        CHECK(residue.get("chainname")->as_string() == "D");
        CHECK(residue.get("chainindex")->as_double() == 9 );

        CHECK(residue.contains(4525));

        auto water_res = *frame.topology().residue_for_atom(4558);
        CHECK(water_res.size() == 1);
        CHECK(water_res.name() == "HOH");

        CHECK(water_res.get("chainid"));
        CHECK(water_res.get("chainname"));
        CHECK(water_res.get("chainindex"));

        CHECK(water_res.get("chainid")->as_string() == "K");
        CHECK(water_res.get("chainname")->as_string() == "A");
        CHECK(water_res.get("chainindex")->as_double() == 10 );

        auto water_res2 = *frame.topology().residue_for_atom(4614);
        CHECK(water_res2.size() == 1);
        CHECK(water_res2.name() == "HOH");

        CHECK(water_res2.get("chainid"));
        CHECK(water_res2.get("chainname"));
        CHECK(water_res2.get("chainindex"));

        CHECK(water_res2.get("chainid")->as_string() == "L");
        CHECK(water_res2.get("chainname")->as_string() == "B");
        CHECK(water_res2.get("chainindex")->as_double() == 11 );
    }

    SECTION("Skip steps") {
        Trajectory file("data/mmtf/1J8K.mmtf");

        auto frame = file.read_step(13);
        CHECK(frame.size() == 1402);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-5.106, 16.212, 4.562), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(5.601, -22.571, -16.631), 1e-3));

        const auto& topo = frame.topology();
        CHECK(topo.are_linked(topo.residue(0), topo.residue(1)));
        CHECK(!topo.are_linked(topo.residue(0), topo.residue(2)));

        frame = file.read_step(1);
        CHECK(frame.size() == 1402);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D( -9.134, 11.149, 6.990), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(4.437, -13.250, -22.569), 1e-3));

        const auto& topo2 = frame.topology();
        CHECK(topo2.are_linked(topo.residue(0), topo2.residue(1)));
        CHECK(!topo2.are_linked(topo.residue(0), topo2.residue(2)));
    }

    SECTION("Successive steps") {
        Trajectory file("data/mmtf/1J8K.mmtf");

        auto frame = file.read();

        const auto& topo = frame.topology();
        CHECK(topo.are_linked(topo.residue(0), topo.residue(1)));
        CHECK(!topo.are_linked(topo.residue(0), topo.residue(2)));

        auto frame2= file.read();

        const auto& topo2 = frame.topology();
        CHECK(topo2.are_linked(topo.residue(0), topo2.residue(1)));
        CHECK(!topo2.are_linked(topo.residue(0), topo2.residue(2)));

        auto frame3= file.read();
    }

}
