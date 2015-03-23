#include <streambuf>
#include <fstream>

#include "catch.hpp"

#include "Chemharp.hpp"
using namespace harp;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;

#define XYZDIR SRCDIR "/files/xyz/"

TEST_CASE("Read files in XYZ format", "[XYZ]"){
    auto file = Trajectory(XYZDIR"helium.xyz");
    Frame frame;

    SECTION("Stream style reading"){
        file >> frame;
        CHECK(frame.natoms() == 125);
        // Check positions
        auto positions = frame.positions();
        CHECK(positions[0] == Vector3D(0.49053f, 8.41351f, 0.0777257f));
        CHECK(positions[124] == Vector3D(8.57951f, 8.65712f, 8.06678f));
        // Check topology
        auto topology = frame.topology();
        CHECK(topology.natom_types() == 1);
        CHECK(topology.natoms() == 125);
        CHECK(topology[0] == Atom("He"));
    }

    SECTION("Method style reading"){
        frame = file.read_next_step();
        CHECK(frame.natoms() == 125);
        // Check positions
        auto positions = frame.positions();
        CHECK(positions[0] == Vector3D(0.49053f, 8.41351f, 0.0777257f));
        CHECK(positions[124] == Vector3D(8.57951f, 8.65712f, 8.06678f));
        // Check topology
        auto topology = frame.topology();
        CHECK(topology.natom_types() == 1);
        CHECK(topology.natoms() == 125);
        CHECK(topology[0] == Atom("He"));
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
        auto file = Trajectory(entry.path().string());
        CHECK_THROWS_AS(file.read_next_step(), FormatError);
    }
}

TEST_CASE("Write files in XYZ format", "[XYZ]"){
    const auto expected_content =
    "4\n"
    "Written by Chemharp\n"
    "X 1 2 3\n"
    "X 1 2 3\n"
    "X 1 2 3\n"
    "X 1 2 3\n";

    Frame frame(0);
    frame.topology(dummy_topology(4));

    Array3D positions;
    for(int i=0; i<4; i++)
        positions.push_back(Vector3D(1, 2, 3));

    frame.positions(positions);

    auto file = Trajectory("test-tmp.xyz", "w");
    file << frame;
    file.close();

    std::ifstream checking("test-tmp.xyz");
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    checking.close();

    CHECK(content == expected_content);

    unlink("test-tmp.xyz");
}
