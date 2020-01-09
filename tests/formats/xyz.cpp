// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;


TEST_CASE("Read files in XYZ format") {
    SECTION("Check nsteps") {
        auto file = Trajectory("data/xyz/trajectory.xyz");
        CHECK(file.nsteps() == 2);

        file = Trajectory("data/xyz/helium.xyz");
        CHECK(file.nsteps() == 397);

        file = Trajectory("data/xyz/topology.xyz");
        CHECK(file.nsteps() == 1);
    }

    SECTION("Read next step") {
        auto file = Trajectory("data/xyz/helium.xyz");
        auto frame = file.read();
        CHECK(frame.size() == 125);
        // Check positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], {0.49053, 8.41351, 0.0777257}, 1e-12));
        CHECK(approx_eq(positions[124], {8.57951, 8.65712, 8.06678}, 1e-12));
        // Check topology
        auto topology = frame.topology();
        CHECK(topology.size() == 125);
        CHECK(topology[0] == Atom("He"));
    }

    SECTION("Read a specific step") {
        auto file = Trajectory("data/xyz/helium.xyz");
        // Read frame at a specific positions
        auto frame = file.read_step(42);
        CHECK(frame.step() == 42);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], {-0.145821, 8.540648, 1.090281}, 1e-12));
        CHECK(approx_eq(positions[124], {8.446093, 8.168162, 9.350953}, 1e-12));
        auto topology = frame.topology();
        CHECK(topology.size() == 125);
        CHECK(topology[0] == Atom("He"));

        frame = file.read_step(0);
        CHECK(frame.step() == 0);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], {0.49053, 8.41351, 0.0777257}, 1e-12));
        CHECK(approx_eq(positions[124], {8.57951, 8.65712, 8.06678}, 1e-12));
    }

    SECTION("Read the whole file") {
        auto file = Trajectory("data/xyz/helium.xyz");
        REQUIRE(file.nsteps() == 397);

        Frame frame;
        while (!file.done()) {
            frame = file.read();
        }
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], {-1.186037, 11.439334, 0.529939}, 1e-12));
        CHECK(approx_eq(positions[124], {5.208778, 12.707273, 10.940157}, 1e-12));
    }

    SECTION("Read various files formatting") {
        auto file = Trajectory("data/xyz/spaces.xyz");

        auto frame = file.read();
        auto positions = frame.positions();
        CHECK(approx_eq(positions[10], {0.8336, 0.3006, 0.4968}, 1e-12));
    }
}

TEST_CASE("Errors in XYZ format") {
    CHECK_THROWS_AS(Trajectory("data/xyz/bad/helium.xyz"), FormatError);
}

TEST_CASE("Write files in XYZ format") {
    auto tmpfile = NamedTempPath(".xyz");
    const auto expected_content =
R"(4
Written by the chemfiles library
A 1 2 3
B 1 2 3
C 1 2 3
D 1 2 3
6
Written by the chemfiles library
A 1 2 3
B 1 2 3
C 1 2 3
D 1 2 3
E 4 5 6
F 4 5 6
)";

    auto frame = Frame();
    frame.add_atom(Atom("A","O"), {1, 2, 3});
    frame.add_atom(Atom("B"), {1, 2, 3});
    frame.add_atom(Atom("C"), {1, 2, 3});
    frame.add_atom(Atom("D"), {1, 2, 3});

    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.add_atom(Atom("E"), {4, 5, 6});
    frame.add_atom(Atom("F"), {4, 5, 6});

    file.write(frame);
    file.close();

    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == expected_content);
}

TEST_CASE("Round-trip read/write") {
    auto tmpfile = NamedTempPath(".xyz");
    auto content =
R"(3
Written by the chemfiles library
O 0.417 8.303 11.737
H 1.32 8.48 12.003
H 0.332 8.726 10.882
)";

    std::ofstream create(tmpfile);
    create << content;
    create.close();

    auto frame = Trajectory(tmpfile).read();
    Trajectory(tmpfile, 'w').write(frame);

    std::ifstream checking(tmpfile);
    std::string actual((std::istreambuf_iterator<char>(checking)),
                        std::istreambuf_iterator<char>());
    CHECK(content == actual);

}
