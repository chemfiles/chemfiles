#include <fstream>
#include <catch.hpp>
#include "chemfiles.hpp"
using namespace chemfiles;


// This file only perform basic testing of the trajectory class. All the differents
// formats are tested in the formats folder
TEST_CASE("Associate a topology and a trajectory", "[Trajectory]"){
    SECTION("Reading"){
        SECTION("From a file"){
            Trajectory file("data/xyz/trajectory.xyz");
            file.set_topology("data/xyz/topology.xyz.topology", "XYZ");
            auto frame = file.read();

            CHECK(frame.natoms() == 9);
            auto topology = frame.topology();
            CHECK(topology.natoms() == 9);
            CHECK(topology[0] == Atom("Zn"));
            CHECK(topology[1] == Atom("Cs"));
            CHECK(topology[2] == Atom("Ar"));
        }

        SECTION("Directely"){
            Trajectory file("data/xyz/trajectory.xyz");

            Topology topology;
            for (size_t i=0; i<9; i++) {
                topology.append(Atom("Fe"));
            }

            file.set_topology(topology);
            auto frame = file.read();

            CHECK(frame.natoms() == 9);
            topology = frame.topology();
            CHECK(topology.natoms() == 9);
            CHECK(topology[0] == Atom("Fe"));
            CHECK(topology[1] == Atom("Fe"));
            CHECK(topology[8] == Atom("Fe"));
        }
    }

    SECTION("Writing"){
        const auto EXPECTED_CONTENT =
        "5\n"
        "Written by the chemfiles library\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n";

        Frame frame(5);
        auto positions = frame.positions();
        for(size_t i=0; i<5; i++) {
            positions[i] = vector3d(1, 2, 3);
        }

        Topology top;
        for (size_t i=0; i<5; i++)
            top.append(Atom("Fe"));

        {
            Trajectory file("tmp.xyz", 'w');
            file.set_topology(top);
            file.write(frame);
        }

        std::ifstream checking("tmp.xyz");
        std::string content{
            std::istreambuf_iterator<char>(checking),
            std::istreambuf_iterator<char>()
        };
        checking.close();

        CHECK(content == EXPECTED_CONTENT);
        remove("tmp.xyz");
    }
}

TEST_CASE("Associate an unit cell and a trajectory", "[Trajectory]"){
    SECTION("Reading"){
        Trajectory file("data/xyz/trajectory.xyz");
        file.set_cell(UnitCell(25, 32, 94));
        auto frame = file.read();

        CHECK(frame.cell() == UnitCell(25, 32, 94));
    }

    SECTION("Writing"){
        const auto EXPECTED_CONTENT =
        "CRYST1    3.000    4.000    5.000  90.00  90.00  90.00 P 1           1\n"
        "HETATM    1      RES X   1       1.000   2.000   3.000  1.00  0.00            \n"
        "HETATM    2      RES X   2       1.000   2.000   3.000  1.00  0.00            \n"
        "HETATM    3      RES X   3       1.000   2.000   3.000  1.00  0.00            \n"
        "END\n";

        Frame frame(3);
        auto positions = frame.positions();
        for(size_t i=0; i<3; i++) {
            positions[i] = vector3d(1, 2, 3);
        }

        {
            Trajectory file("tmp.pdb", 'w');
            file.set_cell(UnitCell(3, 4, 5));
            file.write(frame);
        }

        std::ifstream checking("tmp.pdb");
        std::string content{
            std::istreambuf_iterator<char>(checking),
            std::istreambuf_iterator<char>()
        };
        checking.close();

        CHECK(content == EXPECTED_CONTENT);
        remove("tmp.pdb");
    }
}

TEST_CASE("Specify a format parameter", "[Trajectory]"){
    Trajectory file("data/xyz/helium.xyz.but.not.really", 'r', "XYZ");
    auto frame = file.read();
    CHECK(frame.natoms() == 125);
}

TEST_CASE("Errors", "[Trajectory]"){
    SECTION("Unknow opening mode") {
        CHECK_THROWS_AS(Trajectory("trajectory.xyz", 'z'), FileError);
    }

    SECTION("Bad opening mode") {
        // Try to read a write-only file
        Trajectory file("tmp.xyz", 'w');
        CHECK_THROWS_AS(file.read(), FileError);
        CHECK_THROWS_AS(file.read_step(5), FileError);
        remove("tmp.xyz");

        // Try to write a read-only file
        file = Trajectory("data/xyz/trajectory.xyz", 'r');
        CHECK_THROWS_AS(file.write(Frame()), FileError);
    }

    SECTION("Read file past end") {
        Trajectory file("data/xyz/trajectory.xyz", 'r');
        CHECK_THROWS_AS(file.read_step(5), FileError);

        file.read();
        file.read();
        CHECK_THROWS_AS(file.read(), FileError);
    }
}
