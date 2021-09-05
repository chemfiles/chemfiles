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

        // FIXME: the molfile plugin does not read residue information, we
        // should add it when re-implementing a PSF reader.
        CHECK(topology.residues().size() == 0);

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
