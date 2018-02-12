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

//MMTF is not supported on asm.js
#ifndef __EMSCRIPTEN__

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
        CHECK(residue.contains(4525));
    }

    SECTION("Skip steps") {
        Trajectory file("data/mmtf/1J8K.mmtf");

        auto frame = file.read_step(13);
        CHECK(frame.size() == 1402);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-5.106, 16.212, 4.562), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(5.601, -22.571, -16.631), 1e-3));

        frame = file.read_step(1);
        CHECK(frame.size() == 1402);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D( -9.134, 11.149, 6.990), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(4.437, -13.250, -22.569), 1e-3));
    }

#endif

}
