#include <streambuf>
#include <fstream>
#include <cstdio>

#include "catch.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;

#define XYZDIR SRCDIR "/data/xyz/"

TEST_CASE("Read files in XYZ format", "[XYZ]"){
    SECTION("Check nsteps"){
        Trajectory file1(XYZDIR "trajectory.xyz");
        CHECK(file1.nsteps() == 2);

        Trajectory file2(XYZDIR "helium.xyz");
        CHECK(file2.nsteps() == 397);

        Trajectory file3(XYZDIR "topology.xyz");
        CHECK(file3.nsteps() == 1);
    }

    Trajectory file(XYZDIR"helium.xyz");
    Frame frame;

    SECTION("Stream style reading"){
        file >> frame;
        CHECK(frame.natoms() == 125);
        // Check positions
        auto positions = frame.positions();
        CHECK(positions[0] == vector3d(0.49053f, 8.41351f, 0.0777257f));
        CHECK(positions[124] == vector3d(8.57951f, 8.65712f, 8.06678f));
        // Check topology
        auto topology = frame.topology();
        CHECK(topology.natom_types() == 1);
        CHECK(topology.natoms() == 125);
        CHECK(topology[0] == Atom("He"));
    }

    SECTION("Method style reading"){
        frame = file.read();
        CHECK(frame.natoms() == 125);
        // Check positions
        auto positions = frame.positions();
        CHECK(positions[0] == vector3d(0.49053f, 8.41351f, 0.0777257f));
        CHECK(positions[124] == vector3d(8.57951f, 8.65712f, 8.06678f));
        // Check topology
        auto topology = frame.topology();
        CHECK(topology.natom_types() == 1);
        CHECK(topology.natoms() == 125);
        CHECK(topology[0] == Atom("He"));
    }

    SECTION("Read a specific step"){
        // Read frame at a specific positions
        frame = file.read_step(42);
        auto positions = frame.positions();
        CHECK(positions[0] == vector3d(-0.145821f, 8.540648f, 1.090281f));
        CHECK(positions[124] == vector3d(8.446093f, 8.168162f, 9.350953f));
        auto topology = frame.topology();
        CHECK(topology.natom_types() == 1);
        CHECK(topology.natoms() == 125);
        CHECK(topology[0] == Atom("He"));

        frame = file.read_step(0);
        positions = frame.positions();
        CHECK(positions[0] == vector3d(0.49053f, 8.41351f, 0.0777257f));
        CHECK(positions[124] == vector3d(8.57951f, 8.65712f, 8.06678f));
    }

    SECTION("Read the whole file"){
        CHECK(file.nsteps() == 397);

        while (!file.done()){
            file >> frame;
        }
        auto positions = frame.positions();
        CHECK(positions[0] == vector3d(-1.186037f, 11.439334f, 0.529939f));
        CHECK(positions[124] == vector3d(5.208778f, 12.707273f, 10.940157f));
    }
}

// To use in loops in order to iterate over files in a specific directory.
struct directory_files_iterator {
    typedef fs::recursive_directory_iterator iterator;
    directory_files_iterator(fs::path p) : p_(p) {}

    iterator begin() { return fs::recursive_directory_iterator(p_); }
    iterator end() { return fs::recursive_directory_iterator(); }

    fs::path p_;
};

TEST_CASE("Errors in XYZ format", "[XYZ]"){
    for (auto entry : directory_files_iterator(XYZDIR"bad/")){
        CHECK_THROWS_AS(
            // We can throw either when creating the trajectory, or when reading
            // the frame, depending on the type of error
            auto file = Trajectory(entry.path().string());
            file.read(),
        FormatError);
    }
}

TEST_CASE("Write files in XYZ format", "[XYZ]"){
    const auto expected_content =
    "4\n"
    "Written by the chemfiles library\n"
    "A 1 2 3\n"
    "B 1 2 3\n"
    "C 1 2 3\n"
    "D 1 2 3\n"
    "6\n"
    "Written by the chemfiles library\n"
    "A 4 5 6\n"
    "B 4 5 6\n"
    "C 4 5 6\n"
    "D 4 5 6\n"
    "E 4 5 6\n"
    "F 4 5 6\n";

    Array3D positions(4);
    for(size_t i=0; i<4; i++)
        positions[i] = vector3d(1, 2, 3);

    Topology topology;
    topology.append(Atom("A"));
    topology.append(Atom("B"));
    topology.append(Atom("C"));
    topology.append(Atom("D"));

    Frame frame;
    frame.set_topology(topology);
    frame.set_positions(positions);

    auto file = Trajectory("test-tmp.xyz", "w");
    file << frame;

    positions.resize(6);
    for(size_t i=0; i<6; i++)
        positions[i] = vector3d(4, 5, 6);

    topology.append(Atom("E"));
    topology.append(Atom("F"));
    frame.set_topology(topology);
    frame.set_positions(positions);

    file << frame;
    file.sync();

    std::ifstream checking("test-tmp.xyz");
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    checking.close();

    CHECK(content == expected_content);

    remove("test-tmp.xyz");
}
