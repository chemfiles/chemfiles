#include <streambuf>
#include <fstream>
#include <cstdio>
#include "catch.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#define XYZDIR SRCDIR "/data/xyz/"

// This file only perform basic testing of the trajectory class. All the differents
// formats are tested in the formats folder
TEST_CASE("Associate a topology and a trajectory", "[Trajectory]"){
    SECTION("Reading"){
        Trajectory file(XYZDIR "trajectory.xyz");
        Frame frame;

        SECTION("From a file"){
            file.topology(XYZDIR "topology.xyz");
            file >> frame;

            CHECK(frame.natoms() == 9);
            auto topology = frame.topology();
            CHECK(topology.natoms() == 9);
            CHECK(topology[0] == Atom("Zn"));
            CHECK(topology[1] == Atom("Cs"));
            CHECK(topology[2] == Atom("Ar"));
        }

        SECTION("Directely"){
            Topology top;
            for (size_t i=0; i<9; i++)
                top.append(Atom("Fe"));

            file.topology(top);
            file >> frame;

            CHECK(frame.natoms() == 9);
            auto topology = frame.topology();
            CHECK(topology.natoms() == 9);
            CHECK(topology[0] == Atom("Fe"));
            CHECK(topology[1] == Atom("Fe"));
            CHECK(topology[8] == Atom("Fe"));
        }
    }

    SECTION("Writing"){
        const auto expected_content =
        "5\n"
        "Written by the chemfiles library\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n";

        Trajectory file("tmp.xyz", "w");

        Array3D positions(5);
        for(size_t i=0; i<5; i++) {
            positions[i] = vector3d(1, 2, 3);
        }

        Frame frame;
        frame.positions(positions);
        frame.topology(dummy_topology(5));

        Topology top;
        for (size_t i=0; i<5; i++)
            top.append(Atom("Fe"));

        file.topology(top);
        file << frame;
        file.sync();

        std::ifstream checking("tmp.xyz");
        std::string content((std::istreambuf_iterator<char>(checking)),
                                 std::istreambuf_iterator<char>());
        checking.close();

        CHECK(content == expected_content);
        remove("tmp.xyz");
    }
}

TEST_CASE("Associate an unit cell and a trajectory", "[Trajectory]"){
    SECTION("Reading"){
        Trajectory file(XYZDIR "trajectory.xyz");
        Frame frame;

        file.cell(UnitCell(25, 32, 94));
        file >> frame;

        CHECK(frame.cell() == UnitCell(25, 32, 94));
    }
}

TEST_CASE("Specify a format parameter", "[Trajectory]"){
    Trajectory file(XYZDIR "helium.xyz.but.not.really", "r", "XYZ");
    Frame frame;

    file >> frame;
    CHECK(frame.natoms() == 125);
}
