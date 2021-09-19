// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include <thread>
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
}

TEST_CASE("Guessing format") {
    CHECK(guess_format("not-a-file.xyz") == "XYZ");
    CHECK(guess_format("not-a-file.pdb") == "PDB");
    CHECK(guess_format("not-a-file.nc") == "Amber NetCDF");

    CHECK(guess_format("not-a-file.xyz.gz") == "XYZ / GZ");
    CHECK(guess_format("not-a-file.xyz.bz2") == "XYZ / BZ2");
    CHECK(guess_format("not-a-file.xyz.xz") == "XYZ / XZ");

    CHECK_THROWS_WITH(
        guess_format("not-a-file.unknown"),
        "can not find a format associated with the '.unknown' extension"
    );
    CHECK_THROWS_WITH(
        guess_format("not-a-file"),
        "file at 'not-a-file' does not have an extension, provide a format name to read it"
    );
}

static void read_from_multiple_threads(std::string filename, size_t n_atoms) {
    auto n_steps = Trajectory(filename).nsteps();
    size_t n_threads = 4;

    auto thread_1 = std::thread([=](){
        auto file = Trajectory(filename);
        for (size_t i=0; i<n_steps; i += n_threads) {
            auto frame = file.read_step(i);
            CHECK(frame.size() == n_atoms);
        }
    });

    auto thread_2 = std::thread([=](){
        auto file = Trajectory(filename);
        for (size_t i=1; i<n_steps; i += n_threads) {
            auto frame = file.read_step(i);
            CHECK(frame.size() == n_atoms);
        }
    });

    auto thread_3 = std::thread([=](){
        auto file = Trajectory(filename);
        for (size_t i=2; i<n_steps; i += n_threads) {
            auto frame = file.read_step(i);
            CHECK(frame.size() == n_atoms);
        }
    });

    auto thread_4 = std::thread([=](){
        auto file = Trajectory(filename);
        for (size_t i=3; i<n_steps; i += n_threads) {
            auto frame = file.read_step(i);
            CHECK(frame.size() == n_atoms);
        }
    });

    thread_1.join();
    thread_2.join();
    thread_3.join();
    thread_4.join();
}

// don't run threading tests on wasm/emscripten
#ifndef __EMSCRIPTEN__

TEST_CASE("reading a files from multiple threads") {
    // text file
    read_from_multiple_threads("data/xyz/water.xyz", 297);

    // compressed files
    read_from_multiple_threads("data/xyz/water.9.xyz.gz", 297);
    read_from_multiple_threads("data/xyz/water.9.xyz.bz2", 297);
    read_from_multiple_threads("data/xyz/water.blocks.xyz.xz", 297);

    // non text files
    read_from_multiple_threads("data/mmtf/1HTQ_reduced.mmtf", 12336);
    read_from_multiple_threads("data/dcd/water.dcd", 297);
    read_from_multiple_threads("data/trr/1aki.trr", 38376);
    read_from_multiple_threads("data/xtc/ubiquitin.xtc", 20455);
    read_from_multiple_threads("data/tng/1aki.tng", 38376);

    // FIXME: these fail, probably because the underlying library maintains a
    // pool of ressources without synchronization

    // read_from_multiple_threads("data/netcdf/water.nc", 297);
}

#endif

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
