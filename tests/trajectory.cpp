// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include <catch.hpp>

#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

// This file only perform basic testing of the trajectory class. All the differents
// formats are tested in the formats folder
TEST_CASE("Associate a topology and a trajectory") {
    SECTION("Reading") {
        SECTION("From a file") {
            Trajectory file("data/xyz/trajectory.xyz");
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
            Trajectory file("data/xyz/trajectory.xyz");

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
        "Written by the chemfiles library\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n";

        auto topology = Topology();
        for (size_t i=0; i<5; i++) {
            topology.add_atom(Atom("Fe"));
        }

        auto frame = Frame(topology);
        auto positions = frame.positions();
        for(size_t i=0; i<5; i++) {
            positions[i] = Vector3D(1, 2, 3);
        }

        {
            Trajectory file(tmpfile, 'w');
            file.set_topology(topology);
            file.write(frame);
        }

        std::ifstream checking(tmpfile);
        std::string content{
            std::istreambuf_iterator<char>(checking),
            std::istreambuf_iterator<char>()
        };
        CHECK(content == EXPECTED_CONTENT);
    }
}


TEST_CASE("Setting frame step") {
    Trajectory file("data/xyz/helium.xyz");
    auto frame = file.read();
    CHECK(frame.step() == 0);

    frame = file.read();
    CHECK(frame.step() == 1);

    frame = file.read_step(10);
    CHECK(frame.step() == 10);
}


TEST_CASE("Associate an unit cell and a trajectory") {
    SECTION("Reading") {
        Trajectory file("data/xyz/trajectory.xyz");
        file.set_cell(UnitCell(25, 32, 94));
        auto frame = file.read();

        CHECK(frame.cell() == UnitCell(25, 32, 94));
    }

    SECTION("Writing") {
        auto tmpfile = NamedTempPath(".pdb");

        auto frame = Frame();
        frame.resize(3);
        auto positions = frame.positions();
        for(size_t i=0; i<3; i++) {
            positions[i] = Vector3D(1, 2, 3);
        }

        Trajectory file(tmpfile, 'w');
        file.set_cell(UnitCell(3, 4, 5));
        file.write(frame);
        file.close();

        SECTION("Directly") {
            const auto EXPECTED_CONTENT =
            "MODEL    1\n"
            "CRYST1    3.000    4.000    5.000  90.00  90.00  90.00 P 1           1\n"
            "HETATM    1      XXX X   1       1.000   2.000   3.000  1.00  0.00            \n"
            "HETATM    2      XXX X   2       1.000   2.000   3.000  1.00  0.00            \n"
            "HETATM    3      XXX X   3       1.000   2.000   3.000  1.00  0.00            \n"
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
            "HETATM    1      XXX X   1       1.000   2.000   3.000  1.00  0.00            \n"
            "HETATM    2      XXX X   2       1.000   2.000   3.000  1.00  0.00            \n"
            "HETATM    3      XXX X   3       1.000   2.000   3.000  1.00  0.00            \n"
            "ENDMDL\n"
            "END\n"
            "MODEL    1\n"
            "CRYST1    0.000    0.000    0.000  90.00  90.00  90.00 P 1           1\n"
            "HETATM    1      XXX X   1       1.000   2.000   3.000  1.00  0.00            \n"
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
    Trajectory file("data/xyz/helium.xyz.but.not.really", 'r', "XYZ");
    auto frame = file.read();
    CHECK(frame.size() == 125);
}

TEST_CASE("Errors") {
    SECTION("Unknow opening mode") {
        CHECK_THROWS_AS(Trajectory("trajectory.xyz", 'z'), FileError);
    }

    SECTION("Bad opening mode") {
        auto tmpfile = NamedTempPath(".xyz");
        // Try to read a write-only file
        Trajectory file(tmpfile, 'w');
        CHECK_THROWS_AS(file.read(), FileError);
        CHECK_THROWS_AS(file.read_step(5), FileError);

        // Try to write a read-only file
        file = Trajectory("data/xyz/trajectory.xyz", 'r');
        CHECK_THROWS_AS(file.write(Frame()), FileError);
    }

    SECTION("Read file past end") {
        Trajectory file("data/xyz/trajectory.xyz", 'r');
        CHECK_THROWS_AS(file.read_step(2), FileError);

        file.read();
        file.read();
        CHECK_THROWS_AS(file.read(), FileError);
    }

    SECTION("Closed file") {
        Trajectory file("data/xyz/trajectory.xyz", 'r');
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
