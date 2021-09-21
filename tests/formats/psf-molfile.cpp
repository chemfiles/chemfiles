// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read PSF format") {
    SECTION("Standard PSF") {
        auto file = Trajectory("data/psf/776wat_1Ca.psf");
        CHECK(file.nsteps() == 1);
        auto frame = file.read();

        CHECK(frame.size() == 3105);

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "OH2");
        CHECK(topology[0].type() == "OT");
        CHECK(topology[0].charge() == 0.0);
        CHECK(topology[0].mass() == static_cast<double>(15.9994f));

        CHECK(topology[1].name() == "H1");
        CHECK(topology[1].type() == "HT");
        CHECK(topology[1].charge() == static_cast<double>(0.556400f));
        CHECK(topology[1].mass() == static_cast<double>(1.0080f));

        CHECK(topology[3104].name() == "CAL");
        CHECK(topology[3104].type() == "CAL");
        CHECK(topology[3104].charge() == 2.0);
        CHECK(topology[3104].mass() == static_cast<double>(40.0800f));

        CHECK(topology.residues().size() == 777);
        auto residue = topology.residue_for_atom(0).value();
        CHECK(residue.id().value() == 1);
        CHECK(residue.name() == "TIP4");
        CHECK(residue.get("segname")->as_string() == "WT1");
        CHECK(residue.get("chainname")->as_string() == "W");
        CHECK(residue.get("chainid")->as_string() == "W");

        // a different residue with the same resid
        residue = topology.residue_for_atom(2098).value();
        CHECK(residue.id().value() == 1);
        CHECK(residue.name() == "TIP4");
        CHECK(residue.get("segname")->as_string() == "WT5");
        CHECK(residue.get("chainname")->as_string() == "W");
        CHECK(residue.get("chainid")->as_string() == "W");

        CHECK(residue.size() == 4);
        CHECK(residue.contains(2096));
        CHECK(residue.contains(2097));
        CHECK(residue.contains(2098));
        CHECK(residue.contains(2099));

        auto& bonds = topology.bonds();
        CHECK(bonds.size() == 3104);

        CHECK(bonds[0][0] == 0);
        CHECK(bonds[0][1] == 1);
    }

    SECTION("Extended PSF") {
        auto file = Trajectory("data/psf/ligandrm.psf");
        CHECK(file.nsteps() == 1);
        auto frame = file.read();

        CHECK(frame.size() == 4);

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "C1");
        CHECK(topology[0].type() == "CG2O6");
        CHECK(topology[0].charge() == static_cast<double>(1.42000f));
        CHECK(topology[0].mass() == static_cast<double>(12.0110f));

        CHECK(topology[1].name() == "O1");
        CHECK(topology[1].type() == "OG2D2");
        CHECK(topology[3].charge() == static_cast<double>(-1.14000f));
        CHECK(topology[3].mass() == static_cast<double>(15.9994f));

        CHECK(topology[2].name() == "O2");
        CHECK(topology[2].type() == "OG2D2");
        CHECK(topology[3].charge() == static_cast<double>(-1.14000f));
        CHECK(topology[3].mass() == static_cast<double>(15.9994f));

        CHECK(topology[3].name() == "O3");
        CHECK(topology[3].type() == "OG2D2");
        CHECK(topology[3].charge() == static_cast<double>(-1.14000f));
        CHECK(topology[3].mass() == static_cast<double>(15.9994f));

        auto& bonds = topology.bonds();
        CHECK(bonds.size() == 3);

        CHECK(bonds[0][0] == 0);
        CHECK(bonds[0][1] == 1);

        CHECK(bonds[1][0] == 0);
        CHECK(bonds[1][1] == 2);

        CHECK(bonds[2][0] == 0);
        CHECK(bonds[2][1] == 3);
    }
}
