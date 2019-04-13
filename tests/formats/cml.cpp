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

#include <iostream>

TEST_CASE("Read files in CML format") {
    SECTION("Check nsteps") {
        Trajectory file1("data/cml/ethane.cml");
        CHECK(file1.nsteps() == 1);

        Trajectory file2("data/cml/drugs.cml");
        CHECK(file2.nsteps() == 4);

        Trajectory file3("data/cml/properties.cml");
        CHECK(file3.nsteps() == 1);
    }

    SECTION("Read next step") {
        Trajectory file("data/cml/drugs.cml");
        auto frame = file.read();
        CHECK(frame.size() == 17);
        CHECK(frame.get("name")->as_string() == "naproxen");

        // Check positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0000, -4.6200, 0.0000), 1e-3));
        CHECK(approx_eq(positions[14], Vector3D(9.3358, -0.7700, 0.0000), 1e-3));

        // Check topology
        auto topology = frame.topology();
        CHECK(topology.size() == 17);
        CHECK(topology[0].type() == "C");
    }

    SECTION("Read a specific step") {
        Trajectory file("data/cml/drugs.cml");
        // Read frame at a specific positions
        auto frame = file.read_step(2);
        CHECK(frame.step() == 2);
        CHECK(frame.get("name")->as_string() == "tylenol");
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0000, 6.1600, 0.0000), 1e-3));
        auto topology = frame.topology();
        CHECK(topology.size() == 11);
        CHECK(topology[0].type() == "C");

        frame = file.read_step(0);
        CHECK(frame.step() == 0);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0000, -4.6200, 0.0000), 1e-3));
        CHECK(approx_eq(positions[14], Vector3D(9.3358, -0.7700, 0.0000), 1e-3));
    }

    SECTION("Read fractional coordinates") {
        Trajectory file("data/cml/ethane.cml");
        auto frame = file.read();

        auto cell = frame.cell();
        CHECK(cell.a() == 3.0);
        CHECK(cell.b() == 3.0);
        CHECK(cell.c() == 3.0);
        CHECK(cell.alpha() == 90.0);
        CHECK(cell.beta() == 90.0);
        CHECK(cell.gamma() == 120.0);

        auto positions = frame.positions();
        auto fract0 = frame.cell().matrix().invert() * positions[0];
        CHECK(approx_eq(fract0, Vector3D(-1.77493, 0.980333, 0.0000), 1e-3));
    }

    SECTION("Read properties") {
        Trajectory file("data/cml/properties.cml");
        auto frame = file.read();
        CHECK(frame.get("num_c")->as_double() == 10.0);
        CHECK(frame.get("is_organic")->as_bool() == true);
        CHECK(frame[6].get("r")->as_string() == "tButyl");
    }
}
