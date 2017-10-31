// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

static bool contains_bond(const Topology& topology, Bond bond) {
    for (auto& exist: topology.bonds()) {
        if (bond == exist) {
            return true;
        }
    }
    return false;
}

static double EPS = 1e-4;

TEST_CASE("Read files in mol2 format using Molfile") {
    SECTION("Molecules.mol2") {
        auto file = Trajectory("data/mol2/Molecules.mol2");
        CHECK(file.nsteps() == 200);

        auto frame = file.read();
        CHECK(frame.size() == 49);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(6.8420, 9.9900, 22.7430), EPS));
        CHECK(approx_eq(positions[33], Vector3D(4.5540, 11.1000, 22.5880), EPS));

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "N1");
        CHECK(topology[0].type() == "N.am");
        CHECK(approx_eq(topology[0].charge(), -0.8960, 1e-4));
        CHECK(topology[33].name() == "H131");
        CHECK(topology[33].type() == "H");
        CHECK(approx_eq(topology[33].charge(), 0.0720, 1e-4));

        CHECK(topology.bonds().size() == 51);
        CHECK(contains_bond(topology, {7, 34}));
        CHECK(contains_bond(topology, {13, 19}));
    }

    SECTION("imatinib.mol2") {
        auto file = Trajectory("data/mol2/imatinib.mol2");
        CHECK(file.nsteps() == 1);

        auto frame = file.read();
        CHECK(frame.size() == 68);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[3], Vector3D(-0.1070, -1.8330, -0.2330), EPS));
        CHECK(approx_eq(positions[61], Vector3D(-5.5050, -4.7850, -0.1660), EPS));

        auto& topology = frame.topology();
        // FIXME: this is weird. the plugin insist on using N4 as name, but the
        // file contains N2.
        CHECK(topology[3].name() == "N2");
        CHECK(topology[3].type() == "n");
        CHECK(approx_eq(topology[3].charge(), -0.471100, 1e-5));
        CHECK(topology[61].name() == "H24");
        CHECK(topology[61].type() == "hc");
        CHECK(approx_eq(topology[61].charge(), 0.044367, 1e-5));

        CHECK(topology.bonds().size() == 72);
        CHECK(contains_bond(topology, {12, 14}));
        CHECK(contains_bond(topology, {34, 65}));

        // FIXME: the plugin is not reading residue information
        // CHECK(topology.residue(33)->name() == "LIG");
    }

    SECTION("Various files") {
        // Just checking that we can read them without error
        auto file = Trajectory("data/mol2/lysozyme-ligand-tripos.mol2");
        CHECK(file.nsteps() == 1);
        auto frame = file.read();
        CHECK(frame.size() == 18);

        file = Trajectory("data/mol2/zinc_856218.mol2");
        CHECK(file.nsteps() == 1);
        frame = file.read();
        CHECK(frame.size() == 45);

        file = Trajectory("data/mol2/adp.mol2");
        CHECK(file.nsteps() == 1);
        frame = file.read();
        CHECK(frame.size() == 39);

        file = Trajectory("data/mol2/li.mol2");
        CHECK(file.nsteps() == 1);
        frame = file.read();
        CHECK(frame.size() == 1);

        file = Trajectory("data/mol2/status-bits.mol2");
        CHECK(file.nsteps() == 1);
        frame = file.read();
        CHECK(frame.size() == 18);
    }
}
