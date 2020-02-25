// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <streambuf>
#include <fstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in SDF format") {
    SECTION("Check nsteps") {
        auto file = Trajectory("data/sdf/aspirin.sdf");
        CHECK(file.nsteps() == 1);

        file = Trajectory("data/sdf/kinases.sdf");
        CHECK(file.nsteps() == 6);
    }

    SECTION("Read next step") {
        auto file = Trajectory("data/sdf/kinases.sdf");
        auto frame = file.read();
        CHECK(frame.size() == 47);

        // Check positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(4.9955, -2.6277, 0.2047), 1e-3));
        CHECK(approx_eq(positions[46], Vector3D(-8.5180, 0.2962, 2.1406), 1e-3));

        // Check topology
        auto topology = frame.topology();
        CHECK(topology.size() == 47);
        CHECK(topology[0] == Atom("O"));
    }

    SECTION("Read a specific step") {
        auto file = Trajectory("data/sdf/kinases.sdf");
        // Read frame at a specific positions
        auto frame = file.read_step(3);
        CHECK(frame.step() == 3);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-0.8276, 0.2486, -1.0418), 1e-3));
        CHECK(approx_eq(positions[67], Vector3D(-1.1356, 5.2260, 1.3726), 1e-3));
        auto topology = frame.topology();
        CHECK(topology.size() == 68);
        CHECK(topology[0] == Atom("O"));

        frame = file.read_step(0);
        CHECK(frame.step() == 0);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(4.9955, -2.6277, 0.2047), 1e-3));
        CHECK(approx_eq(positions[46], Vector3D(-8.5180, 0.2962, 2.1406), 1e-3));
    }

    SECTION("Read the whole file") {
        auto file = Trajectory("data/sdf/kinases.sdf");
        REQUIRE(file.nsteps() == 6);

        Frame frame;
        while (!file.done()) {
            frame = file.read();
        }
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(3.1149, -1.1207, 3.0606), 1e-3));
        CHECK(approx_eq(positions[49], Vector3D(-7.4890, -0.0147, -2.1114), 1e-3));
    }

    SECTION("Read various file properties") {
        auto file = Trajectory("data/sdf/aspirin.sdf");

        auto frame = file.read();
        auto prop  = frame.get("PUBCHEM_COMPOUND_CID");
        CHECK(prop->as_string() == "2244");

        auto prop2 = frame.get("PUBCHEM_MOLECULAR_FORMULA");
        CHECK(prop2->as_string() == "C9H8O4");
    }

    SECTION("Read charges") {
        auto file = Trajectory("data/sdf/aspirin_charged.sdf");

        auto frame = file.read();
        CHECK(approx_eq(frame[0].charge(), 0.0));
        CHECK(approx_eq(frame[1].charge(), 3.0));
        CHECK(approx_eq(frame[2].charge(), 2.0));
        CHECK(approx_eq(frame[3].charge(), 1.0));
        CHECK(approx_eq(frame[4].charge(), 0.0));
        CHECK(approx_eq(frame[5].charge(),-1.0));
        CHECK(approx_eq(frame[6].charge(),-2.0));
        CHECK(approx_eq(frame[7].charge(),-3.0));
        CHECK(approx_eq(frame[8].charge(), 0.0));
        CHECK(approx_eq(frame[9].charge(), 0.0));
        CHECK(approx_eq(frame[10].charge(), 0.0));
    }
}

TEST_CASE("Errors in SDF format") {
    auto file = Trajectory("data/sdf/bad/bad_atom_line.sdf");
    CHECK_THROWS_WITH(file.read(), "atom line is too small for SDF: '    3.7320   -0.0600'");

    CHECK_THROWS_WITH(
        Trajectory("data/sdf/bad/bad_counts_line.sdf"),
        "could not parse counts line: ' 21aaa           '"
    );

    CHECK_THROWS_WITH(
        Trajectory("data/sdf/bad/bad_counts_line2.sdf"),
        "could not parse counts line: '  0  0'"
    );

    CHECK_THROWS_WITH(
        Trajectory("data/sdf/bad/blank.sdf"),
        "could not parse counts line: 'asdf'"
    );
}

TEST_CASE("Write files in SDF format") {
    auto tmpfile = NamedTempPath(".sdf");
    const auto expected_content =
R"(NONAME
 chemfiles-lib

  4  3  0     0  0  0  0  0  0999 V2000
    1.0000    2.0000    3.0000 O   0  0  0  0  0  0  0  0  0  0  0  0
    1.0000    2.0000    3.0000 N   0  0  0  0  0  0  0  0  0  0  0  0
    1.0000    2.0000    3.0000 C   0  0  0  0  0  0  0  0  0  0  0  0
    1.0000    2.0000    3.0000 F   0  0  0  0  0  0  0  0  0  0  0  0
  1  3  1  0  0  0  0
  2  3  2  0  0  0  0
  3  4  3  0  0  0  0
M  END
> <prop>
prop1

$$$$
TEST
 chemfiles-lib

 11  5  0     0  0  0  0  0  0999 V2000
    1.0000    2.0000    3.0000 O   0  0  0  0  0  0  0  0  0  0  0  0
    1.0000    2.0000    3.0000 N   0  3  0  0  0  0  0  0  0  0  0  0
    1.0000    2.0000    3.0000 C   0  2  0  0  0  0  0  0  0  0  0  0
    1.0000    2.0000    3.0000 F   0  1  0  0  0  0  0  0  0  0  0  0
    4.0000    5.0000    6.0000 E   0  0  0  0  0  0  0  0  0  0  0  0
    4.0000    5.0000    6.0000 D   0  5  0  0  0  0  0  0  0  0  0  0
    0.0000    0.0000    0.0000 G   0  6  0  0  0  0  0  0  0  0  0  0
    0.0000    0.0000    0.0000 H   0  7  0  0  0  0  0  0  0  0  0  0
    0.0000    0.0000    0.0000 I   0  0  0  0  0  0  0  0  0  0  0  0
    0.0000    0.0000    0.0000 J   0  0  0  0  0  0  0  0  0  0  0  0
    0.0000    0.0000    0.0000 Xxx 0  0  0  0  0  0  0  0  0  0  0  0
  1  3  1  0  0  0  0
  2  3  2  0  0  0  0
  3  4  3  0  0  0  0
  9 10  8  0  0  0  0
 10 11  4  0  0  0  0
M  END
> <prop>
1.23

$$$$
TEST
 chemfiles-lib

  1  0  0     0  0  0  0  0  0999 V2000
    1.0000    2.0000    3.0000 O   0  0  0  0  0  0  0  0  0  0  0  0
M  END
> <prop>
false

$$$$
TEST
 chemfiles-lib

  1  0  0     0  0  0  0  0  0999 V2000
    1.0000    2.0000    3.0000 O   0  0  0  0  0  0  0  0  0  0  0  0
M  END
> <prop>
1.0 2.0 3.0

$$$$
)";

    auto frame = Frame();
    frame.add_atom(Atom("A","O"), {1, 2, 3});
    frame.add_atom(Atom("B","N"), {1, 2, 3});
    frame.add_atom(Atom("C"), {1, 2, 3});
    frame.add_atom(Atom("F"), {1, 2, 3});
    frame.add_bond(0, 2, Bond::SINGLE);
    frame.add_bond(1, 2, Bond::DOUBLE);
    frame.add_bond(2, 3, Bond::TRIPLE);
    frame.set("prop", Property("prop1"));

    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.add_atom(Atom("E"), {4, 5, 6});
    frame.add_atom(Atom("D"), {4, 5, 6});
    frame.add_atom(Atom("G"), {0, 0, 0});
    frame.add_atom(Atom("H"), {0, 0, 0});
    frame.add_atom(Atom("I"), {0, 0, 0});
    frame.add_atom(Atom("J"), {0, 0, 0});
    frame.add_atom(Atom(""), {0, 0, 0});

    frame[0].set_charge(0.05);
    frame[1].set_charge(1.0);
    frame[2].set_charge(2.0);
    frame[3].set_charge(3.0);
    frame[4].set_charge(4.0);
    frame[5].set_charge(-1.0);
    frame[6].set_charge(-2.0);
    frame[7].set_charge(-3.0);

    frame.add_bond(9, 10, Bond::AROMATIC);
    frame.add_bond(8, 9, Bond::UNKNOWN);

    frame.set("name", Property("TEST"));
    frame.set("prop", Property(1.23));

    file.write(frame);

    frame.clear_bonds();
    frame.resize(1);

    frame.set("prop", Property(false));
    file.write(frame);

    frame.set("prop", Property(Vector3D{1.0, 2.0, 3.0}));
    file.write(frame);

    file.close();

    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == expected_content);
}

TEST_CASE("Read and write files in memory") {
    SECTION("Reading from memory") {
        std::ifstream checking("data/sdf/kinases.sdf");
        std::vector<char> content((std::istreambuf_iterator<char>(checking)),
            std::istreambuf_iterator<char>());

        auto file = Trajectory::memory_reader(content.data(), content.size(), "SDF");
        REQUIRE(file.nsteps() == 6);

        Frame frame;
        while (!file.done()) {
            frame = file.read();
        }
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(3.1149, -1.1207, 3.0606), 1e-3));
        CHECK(approx_eq(positions[49], Vector3D(-7.4890, -0.0147, -2.1114), 1e-3));
    }
}
