// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include <catch.hpp>

#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

// This file only perform basic testing of the trajectory class. All the
// different formats are tested in the formats folder
TEST_CASE("Associate a topology and a trajectory") {
    SECTION("Reading") {
        SECTION("From a file") {
            auto file = Trajectory("data/xyz/trajectory.xyz");
            file.set_topology("data/xyz/topology.xyz.topology", "XYZ");
            auto frame = file.read();

            CHECK(frame.size() == 9);
            auto topology = frame.topology();
            CHECK(topology.size() == 9);
            CHECK(topology[0] == Atom("Zn"));
            CHECK(topology[1] == Atom("Cs"));
            CHECK(topology[2] == Atom("Ar"));
        }

        SECTION("Directely") {
            auto file = Trajectory("data/xyz/trajectory.xyz");

            Topology topology;
            for (size_t i=0; i<9; i++) {
                topology.add_atom(Atom("Fe"));
            }

            file.set_topology(topology);
            auto frame = file.read();

            CHECK(frame.size() == 9);
            topology = frame.topology();
            CHECK(topology.size() == 9);
            CHECK(topology[0] == Atom("Fe"));
            CHECK(topology[1] == Atom("Fe"));
            CHECK(topology[8] == Atom("Fe"));
        }
    }

    SECTION("Writing") {
        auto tmpfile = NamedTempPath(".xyz");
        const auto EXPECTED_CONTENT =
        "5\n"
        "Properties=species:S:1:pos:R:3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n";

        auto frame = Frame();
        auto topology = Topology();
        for (size_t i=0; i<5; i++) {
            topology.add_atom(Atom("Fe"));
            frame.add_atom(Atom("Ar"), {1, 2, 3});
        }

        auto file = Trajectory(tmpfile, 'w');
        file.set_topology(topology);
        file.write(frame);
        file.close();

        std::ifstream checking(tmpfile);
        std::string content{
            std::istreambuf_iterator<char>(checking),
            std::istreambuf_iterator<char>()
        };
        CHECK(content == EXPECTED_CONTENT);
    }
}


TEST_CASE("Setting frame step") {
    auto file = Trajectory("data/xyz/helium.xyz");
    auto frame = file.read();
    CHECK(frame.step() == 0);

    frame = file.read();
    CHECK(frame.step() == 1);

    frame = file.read_step(10);
    CHECK(frame.step() == 10);
}


TEST_CASE("Associate an unit cell and a trajectory") {
    SECTION("Reading") {
        auto file = Trajectory("data/xyz/trajectory.xyz");
        file.set_cell(UnitCell({25, 32, 94}));
        auto frame = file.read();

        CHECK(frame.cell() == UnitCell({25, 32, 94}));
    }

    SECTION("Writing") {
        auto tmpfile = NamedTempPath(".pdb");

        auto frame = Frame();
        frame.resize(3);
        auto positions = frame.positions();
        for(size_t i=0; i<3; i++) {
            positions[i] = Vector3D(1, 2, 3);
        }

        auto file = Trajectory(tmpfile, 'w');
        file.set_cell(UnitCell({3, 4, 5}));
        file.write(frame);
        file.close();

        SECTION("Directly") {
            const auto EXPECTED_CONTENT =
            "MODEL    1\n"
            "CRYST1    3.000    4.000    5.000  90.00  90.00  90.00 P 1           1\n"
            "HETATM    1              1       1.000   2.000   3.000  1.00  0.00            \n"
            "HETATM    2              2       1.000   2.000   3.000  1.00  0.00            \n"
            "HETATM    3              3       1.000   2.000   3.000  1.00  0.00            \n"
            "ENDMDL\n"
            "END\n";

            std::ifstream checking(tmpfile);
            std::string content{
                std::istreambuf_iterator<char>(checking),
                std::istreambuf_iterator<char>()
            };
            CHECK(content == EXPECTED_CONTENT);
        }

        SECTION("Append") {
            const auto EXPECTED_CONTENT =
            "MODEL    1\n"
            "CRYST1    3.000    4.000    5.000  90.00  90.00  90.00 P 1           1\n"
            "HETATM    1              1       1.000   2.000   3.000  1.00  0.00            \n"
            "HETATM    2              2       1.000   2.000   3.000  1.00  0.00            \n"
            "HETATM    3              3       1.000   2.000   3.000  1.00  0.00            \n"
            "ENDMDL\n"
            "END\n"
            "MODEL    1\n"
            "CRYST1    0.000    0.000    0.000  90.00  90.00  90.00 P 1           1\n"
            "HETATM    1              1       1.000   2.000   3.000  1.00  0.00            \n"
            "ENDMDL\n"
            "END\n";

            frame.resize(1);
            file = Trajectory(tmpfile, 'a');
            file.write(frame);
            file.close();

            std::ifstream checking(tmpfile);
            std::string content{
                std::istreambuf_iterator<char>(checking),
                std::istreambuf_iterator<char>()
            };
            CHECK(content == EXPECTED_CONTENT);
        }
    }
}

TEST_CASE("Specify a format parameter") {
    auto file = Trajectory("data/xyz/helium.xyz.but.not.really", 'r', "XYZ");
    auto frame = file.read();
    CHECK(frame.size() == 125);

    auto tmpfile = NamedTempPath(".xyz");
    file = Trajectory(tmpfile, 'w', "XYZ / GZ");
    frame = Frame();
    frame.add_atom(Atom("Fe"), {0, 1, 2});
    file.write(frame);
    file.close();

    // Full format specification
    frame = Trajectory(tmpfile, 'r', "XYZ / GZ").read();
    CHECK(frame.size() == 1);
    CHECK(frame[0].name() == "Fe");

    frame = Trajectory(tmpfile, 'r', "XYZ/ GZ").read();
    CHECK(frame.size() == 1);
    CHECK(frame[0].name() == "Fe");

    frame = Trajectory(tmpfile, 'r', "XYZ/GZ").read();
    CHECK(frame.size() == 1);
    CHECK(frame[0].name() == "Fe");

    frame = Trajectory(tmpfile, 'r', "XYZ /GZ").read();
    CHECK(frame.size() == 1);
    CHECK(frame[0].name() == "Fe");

    // only the compression method, the format will be guessed from extension
    frame = Trajectory(tmpfile, 'r', "/ GZ").read();
    CHECK(frame.size() == 1);
    CHECK(frame[0].name() == "Fe");
}

TEST_CASE("Errors") {
    SECTION("Unknow opening mode") {
        CHECK_THROWS_AS(Trajectory("trajectory.xyz", 'z'), FileError);
    }

    SECTION("Unknow compression method") {
        CHECK_THROWS_AS(Trajectory("trajectory.xyz", 'r', "XYZ / FOOzip"), FileError);
        CHECK_THROWS_AS(Trajectory("trajectory.xyz", 'r', "XYZ /"), FileError);
    }

    SECTION("Bad opening mode") {
        auto tmpfile = NamedTempPath(".xyz");
        // Try to read a write-only file
        auto file = Trajectory(tmpfile, 'w');
        CHECK_THROWS_AS(file.read(), FileError);
        CHECK_THROWS_AS(file.read_step(5), FileError);

        // Try to write a read-only file
        file = Trajectory("data/xyz/trajectory.xyz", 'r');
        CHECK_THROWS_AS(file.write(Frame()), FileError);
    }

    SECTION("Read file past end") {
        auto file = Trajectory("data/xyz/trajectory.xyz", 'r');
        CHECK_THROWS_AS(file.read_step(2), FileError);

        file.read();
        file.read();
        CHECK_THROWS_AS(file.read(), FileError);
    }

    SECTION("Closed file") {
        auto file = Trajectory("data/xyz/trajectory.xyz", 'r');
        file.close();

        CHECK_THROWS_AS(file.read(), FileError);
        CHECK_THROWS_AS(file.read_step(0), FileError);
        CHECK_THROWS_AS(file.write(Frame()), FileError);
        CHECK_THROWS_AS(file.nsteps(), FileError);
        CHECK_THROWS_AS(file.done(), FileError);
        CHECK_THROWS_AS(file.set_cell(UnitCell()), FileError);
        CHECK_THROWS_AS(file.set_topology(Topology()), FileError);
        CHECK_THROWS_AS(file.set_topology("topology"), FileError);
    }
}
