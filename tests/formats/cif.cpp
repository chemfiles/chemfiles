// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <streambuf>
#include <fstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;

TEST_CASE("Read files in CIF format") {
    SECTION("Read single step") {
        Trajectory file("data/cif/4hhb.cif");
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
        // Note: CIF files are silly and treat all waters as one Residue....
        CHECK(frame.topology().residues().size() == 584);

        // Iron in Heme
        auto residue = *frame.topology().residue_for_atom(4557);
        CHECK(residue.size() == 43);
        CHECK(residue.name() == "HEM");
        CHECK(frame[4557].get("is_hetatm")->as_bool()); // Should be a hetatm

        // Check residue connectivity
        const auto& topo = frame.topology();
        auto residue1 = *frame.topology().residue_for_atom(0);
        // First two atoms are in the same residue
        CHECK(residue1 == *frame.topology().residue_for_atom(1));

        auto residue2 = *frame.topology().residue_for_atom(8);
        CHECK(topo.are_linked(residue1, residue2));

        auto residue3 = *frame.topology().residue_for_atom(17);
        CHECK(!topo.are_linked(residue1, residue3));
        CHECK(topo.are_linked(residue2, residue3));

        // Chain information
        CHECK(residue.get("chainid"));
        CHECK(residue.get("chainname"));

        CHECK(residue.get("chainid")->as_string() == "J");
        CHECK(residue.get("chainname")->as_string() == "D");

        CHECK(residue.contains(4525));

        // All waters for an entry are in the same residues
        auto water_res = *frame.topology().residue_for_atom(4558);
        CHECK(water_res.size() == 56);
        CHECK(water_res.name() == "HOH");

        CHECK(water_res.get("chainid"));
        CHECK(water_res.get("chainname"));

        CHECK(water_res.get("chainid")->as_string() == "K");
        CHECK(water_res.get("chainname")->as_string() == "A");
        
        // All waters for an entry are in the same residue, so this is
        // a different entity.
        auto water_res2 = *frame.topology().residue_for_atom(4614);
        CHECK(water_res2.size() == 57);
        CHECK(water_res2.name() == "HOH");

        CHECK(water_res2.get("chainid"));
        CHECK(water_res2.get("chainname"));

        CHECK(water_res2.get("chainid")->as_string() == "L");
        CHECK(water_res2.get("chainname")->as_string() == "B");
    }

    SECTION("Check nsteps") {
        Trajectory file1("data/cif/1j8k.cif");
        CHECK(file1.nsteps() == 20);
    }

    SECTION("Read next step") {
        Trajectory file("data/cif/1j8k.cif");
        auto frame = file.read();
        CHECK(frame.size() == 1402);

        frame = file.read();
        CHECK(frame.size() == 1402);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D( -9.134, 11.149, 6.990), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(4.437, -13.250, -22.569), 1e-3));
    }

    SECTION("Read a specific step") {
        Trajectory file("data/cif/1j8k.cif");

        auto frame = file.read_step(13);
        CHECK(frame.size() == 1402);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-5.106, 16.212, 4.562), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(5.601, -22.571, -16.631), 1e-3));
        CHECK(!frame[0].get("is_hetatm")->as_bool());

        // Rewind
        frame = file.read_step(1);
        CHECK(frame.size() == 1402);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D( -9.134, 11.149, 6.990), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(4.437, -13.250, -22.569), 1e-3));
    }

    SECTION("Read the entire file") {
        Trajectory file("data/cif/1j8k.cif");
        auto frame = file.read();

        size_t count = 1;
        while (!file.done()) {
            frame = file.read();
            ++count;
        }

        CHECK(count == file.nsteps());
        CHECK(frame.size() == 1402);
    }

    SECTION("Read a COD file") {
        Trajectory file("data/cif/1544173.cif");
        CHECK(file.nsteps() == 1);

        auto frame = file.read();
        CHECK(frame.size() == 50);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D( -0.428, 5.427, 11.536), 1e-3));
        CHECK(approx_eq(positions[1], Vector3D( -0.846, 4.873, 12.011), 1e-3));
        CHECK(approx_eq(positions[10],Vector3D(  2.507, 4.442, 8.863), 1e-3));
    }
}
