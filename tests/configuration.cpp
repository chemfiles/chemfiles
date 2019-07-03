// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include <fstream>

#include "helpers.hpp"
#include "chemfiles/Configuration.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Configuration") {
    CHECK(Configuration::rename("foo") == "foo");
    CHECK(Configuration::rename("Oh") == "O");
    CHECK(Configuration::rename("Oz") == "O");
    CHECK(Configuration::rename("N2") == "N4");

    chemfiles::add_configuration("local-file.toml");
    CHECK(Configuration::rename("Oz") == "F");
}

TEST_CASE("Atom type renaming") {
    SECTION("Same name and type") {
        auto tmpfile = NamedTempPath(".xyz");

        std::ofstream file(tmpfile);
        file << "3" << std::endl << std::endl;
        file << "Oh 1 2 3" << std::endl;
        file << "Oz 3 2 1" << std::endl;
        file << "N  0 3 9" << std::endl;
        file.close();

        auto frame = Trajectory(tmpfile).read();
        CHECK(frame.size() == 3);
        auto topology = frame.topology();
        CHECK(topology[0].name() == "Oh");
        CHECK(topology[0].type() == "O");

        CHECK(topology[1].name() == "Oz");
        CHECK(topology[1].type() == "F");

        CHECK(topology[2].name() == "N");
        CHECK(topology[2].type() == "N");
    }

    SECTION("Different name and type") {
        auto tmpfile = NamedTempPath(".pdb");

        std::ofstream file(tmpfile);
        file <<
"ATOM      1 Oh   LIG     1       1.000   0.000   0.000  1.00  0.00          Oh\n"
"ATOM      1 Oh   LIG     1       2.000   0.000   0.000  1.00  0.00          F \n"
"ATOM      1  N   LIG     1       3.000   0.000   0.000  1.00  0.00          N \n"
"ATOM      1  N   LIG     1       4.000   0.000   0.000  1.00  0.00          Zn\n"
"END\n" << std::endl;
        file.close();

        auto frame = Trajectory(tmpfile).read();
        CHECK(frame.size() == 4);
        auto topology = frame.topology();
        CHECK(topology[0].name() == "Oh");
        CHECK(topology[0].type() == "O");

        CHECK(topology[1].name() == "Oh");
        CHECK(topology[1].type() == "F");

        CHECK(topology[2].name() == "N");
        CHECK(topology[2].type() == "N");

        CHECK(topology[3].name() == "N");
        CHECK(topology[3].type() == "Zn");
    }
}

TEST_CASE("Atomic data") {
    // Non-existing element
    auto ch3 = Atom("CH3");
    CHECK(ch3.mass() == 15);
    CHECK(ch3.charge() == 0);
    CHECK(ch3.full_name().value() == "methyl");
    CHECK(ch3.vdw_radius() == nullopt);
    CHECK(ch3.covalent_radius() == nullopt);

    // charge from configuration, everything else from periodic table
    auto zn = Atom("Zn");
    CHECK(zn.mass() == 65.38);
    CHECK(zn.charge() == 1.8);
    CHECK(zn.full_name().value() == "Zinc");
    CHECK(zn.vdw_radius().value() == 2.1);
    CHECK(zn.covalent_radius().value() == 1.31);

    // everything from configuration
    auto so4 = Atom("SO4");
    CHECK(so4.mass() == 96.0);
    CHECK(so4.charge() == -2);
    CHECK(so4.full_name().value() == "sulfate");
    CHECK(so4.vdw_radius().value() == 3.68);
    CHECK(so4.covalent_radius().value() == 2.42);

    // everything from periodic table
    auto f = Atom("F");
    CHECK(f.mass() == 18.9984032);
    CHECK(f.charge() == 0);
    CHECK(f.full_name().value() == "Fluorine");
    CHECK(f.vdw_radius().value() == 1.5);
    CHECK(f.covalent_radius().value() == 0.71);
}

TEST_CASE("Configuration errors") {
    auto tmpfile = NamedTempPath(".toml");

    SECTION("Invalid TOML") {
        // Invalid toml
        std::ofstream file(tmpfile);
        file << "[types]\nfoo: 'bar'\n" << std::endl;
        file.close();

        CHECK_THROWS_WITH(
            chemfiles::add_configuration(tmpfile),
            Catch::Contains("configuration file at '" + tmpfile.path() + "' is invalid TOML")
        );
    }

    SECTION("Invalid 'types' data") {
        std::ofstream file(tmpfile);
        file << "[types]\nfoo = 4\n" << std::endl;
        file.close();

        CHECK_THROWS_WITH(
            chemfiles::add_configuration(tmpfile),
            "invalid configuration file at '" + tmpfile.path() + "': "
            "type for 'foo' must be a string"
        );
    }

    SECTION("Invalid 'atoms' data") {
        std::ofstream file(tmpfile);
        file << "[atoms.O]\nmass = '4'\n" << std::endl;
        file.close();
        CHECK_THROWS_WITH(
            chemfiles::add_configuration(tmpfile),
            "invalid configuration file at '" + tmpfile.path() + "': "
            "mass for 'O' must be a number"
        );

        file.open(tmpfile, std::ios_base::out | std::ios_base::trunc);
        file << "[atoms.O]\ncharge = '4'\n" << std::endl;
        file.close();
        CHECK_THROWS_WITH(
            chemfiles::add_configuration(tmpfile),
            "invalid configuration file at '" + tmpfile.path() + "': "
            "charge for 'O' must be a number"
        );

        file.open(tmpfile, std::ios_base::out | std::ios_base::trunc);
        file << "[atoms.O]\ncovalent_radius = '4'\n" << std::endl;
        file.close();
        CHECK_THROWS_WITH(
            chemfiles::add_configuration(tmpfile),
            "invalid configuration file at '" + tmpfile.path() + "': "
            "covalent_radius for 'O' must be a number"
        );

        file.open(tmpfile, std::ios_base::out | std::ios_base::trunc);
        file << "[atoms.O]\nvdw_radius = '4'\n" << std::endl;
        file.close();
        CHECK_THROWS_WITH(
            chemfiles::add_configuration(tmpfile),
            "invalid configuration file at '" + tmpfile.path() + "': "
            "vdw_radius for 'O' must be a number"
        );

        file.open(tmpfile, std::ios_base::out | std::ios_base::trunc);
        file << "[atoms.O]\nfull_name = false\n" << std::endl;
        file.close();
        CHECK_THROWS_WITH(
            chemfiles::add_configuration(tmpfile),
            "invalid configuration file at '" + tmpfile.path() + "': "
            "full_name for 'O' must be a string"
        );
    }

    SECTION("Could not read file") {
        CHECK_THROWS_WITH(
            chemfiles::add_configuration(tmpfile),
            "can not open configuration file at '" + tmpfile.path() + "'"
        );
    }
}
