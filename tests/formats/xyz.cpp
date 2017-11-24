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


TEST_CASE("Read files in XYZ format") {
    SECTION("Check nsteps") {
        Trajectory file1("data/xyz/trajectory.xyz");
        CHECK(file1.nsteps() == 2);

        Trajectory file2("data/xyz/helium.xyz");
        CHECK(file2.nsteps() == 397);

        Trajectory file3("data/xyz/topology.xyz");
        CHECK(file3.nsteps() == 1);
    }


    SECTION("Read next step") {
        Trajectory file("data/xyz/helium.xyz");
        auto frame = file.read();
        CHECK(frame.size() == 125);
        // Check positions
        auto positions = frame.positions();
        CHECK(positions[0] == Vector3D(0.49053, 8.41351, 0.0777257));
        CHECK(positions[124] == Vector3D(8.57951, 8.65712, 8.06678));
        // Check topology
        auto topology = frame.topology();
        CHECK(topology.size() == 125);
        CHECK(topology[0] == Atom("He"));
    }

    SECTION("Read a specific step") {
        Trajectory file("data/xyz/helium.xyz");
        // Read frame at a specific positions
        auto frame = file.read_step(42);
        CHECK(frame.step() == 42);
        auto positions = frame.positions();
        CHECK(positions[0] == Vector3D(-0.145821, 8.540648, 1.090281));
        CHECK(positions[124] == Vector3D(8.446093, 8.168162, 9.350953));
        auto topology = frame.topology();
        CHECK(topology.size() == 125);
        CHECK(topology[0] == Atom("He"));

        frame = file.read_step(0);
        CHECK(frame.step() == 0);
        positions = frame.positions();
        CHECK(positions[0] == Vector3D(0.49053, 8.41351, 0.0777257));
        CHECK(positions[124] == Vector3D(8.57951, 8.65712, 8.06678));
    }

    SECTION("Read the whole file") {
        Trajectory file("data/xyz/helium.xyz");
        CHECK(file.nsteps() == 397);

        Frame frame;
        while (!file.done()) {
            frame = file.read();
        }
        auto positions = frame.positions();
        CHECK(positions[0] == Vector3D(-1.186037, 11.439334, 0.529939));
        CHECK(positions[124] == Vector3D(5.208778, 12.707273, 10.940157));
    }

    SECTION("Read various files formatting") {
        Trajectory file("data/xyz/spaces.xyz");

        auto frame = file.read();
        auto positions = frame.positions();
        CHECK(positions[10] == Vector3D(0.8336, 0.3006, 0.4968));
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

TEST_CASE("Errors in XYZ format") {
    for (auto entry : directory_files_iterator("data/xyz/bad/")) {
        auto test = [=](){
            // We can throw either when creating the trajectory, or when reading
            // the frame, depending on the type of error
            auto file = Trajectory(entry.path().string());
            file.read();
        };
        CHECK_THROWS_AS(test(), FormatError);
    }
}

TEST_CASE("Write files in XYZ format") {
    auto tmpfile = NamedTempPath(".xyz");
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

    auto topology = Topology();
    topology.add_atom(Atom("A","O"));
    topology.add_atom(Atom("B"));
    topology.add_atom(Atom("C"));
    topology.add_atom(Atom("D"));

    auto frame = Frame(topology);
    auto positions = frame.positions();
    for (size_t i=0; i<4; i++) {
        positions[i] = Vector3D(1, 2, 3);
    }

    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.resize(6);
    positions = frame.positions();
    for (size_t i=0; i<6; i++) {
        positions[i] = Vector3D(4, 5, 6);
    }

    topology.add_atom(Atom("E"));
    topology.add_atom(Atom("F"));
    frame.set_topology(topology);

    file.write(frame);
    file.close();

    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == expected_content);
}
