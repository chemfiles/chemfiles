// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in SDF format") {
    SECTION("Check nsteps") {
        auto file = Trajectory("data/sdf/aspirin.sdf");
        CHECK(file.size() == 1);

        file = Trajectory("data/sdf/kinases.sdf");
        CHECK(file.size() == 6);
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
        auto frame = file.read_at(3);
        CHECK(frame.index() == 3);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-0.8276, 0.2486, -1.0418), 1e-3));
        CHECK(approx_eq(positions[67], Vector3D(-1.1356, 5.2260, 1.3726), 1e-3));
        auto topology = frame.topology();
        CHECK(topology.size() == 68);
        CHECK(topology[0] == Atom("O"));

        frame = file.read_at(0);
        CHECK(frame.index() == 0);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(4.9955, -2.6277, 0.2047), 1e-3));
        CHECK(approx_eq(positions[46], Vector3D(-8.5180, 0.2962, 2.1406), 1e-3));
    }

    SECTION("Read the whole file") {
        auto file = Trajectory("data/sdf/kinases.sdf");
        REQUIRE(file.size() == 6);

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
    auto file = Trajectory("data/sdf/bad/bad-atom-line.sdf");
    CHECK_THROWS_WITH(file.read(), "atom line is too small for SDF: '    3.7320   -0.0600'");

    CHECK_THROWS_WITH(
        Trajectory("data/sdf/bad/count-line-not-numbers.sdf"),
        "could not parse counts line in SDF file: ' 21aaa           '"
    );

    CHECK_THROWS_WITH(
        Trajectory("data/sdf/bad/count-line-too-short.sdf"),
        "counts line must have at least 10 characters in SFD file, it has 6: '  0  0'"
    );
}

TEST_CASE("Write files in SDF format") {
    auto tmpfile = NamedTempPath(".sdf");
    const auto* EXPECTED_CONTENT = R"(

created by chemfiles
  4  3  0     0  0  0  0  0  0999 V2000
    1.0000    2.0000    3.0000 O   0  0  0  0  0  0  0  0  0  0  0  0
    1.0000    2.0000    3.0000 N   0  0  0  0  0  0  0  0  0  0  0  0
    1.0000    2.0000    3.0000 C   0  0  0  0  0  0  0  0  0  0  0  0
    1.0000    2.0000    3.0000 F   0  0  0  0  0  0  0  0  0  0  0  0
  1  3  1  0  0  0  0
  2  3  2  0  0  0  0
  3  4  3  0  0  0  0
M  END
> <string-property>
prop1

$$$$
TEST

created by chemfiles
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
> <float property>
1.23000

> <string-property>
prop1

$$$$
TEST

created by chemfiles
  1  0  0     0  0  0  0  0  0999 V2000
    1.0000    2.0000    3.0000 O   0  0  0  0  0  0  0  0  0  0  0  0
M  END
> <bool property>
false

> <float property>
1.23000

> <string-property>
prop1

$$$$
TEST

created by chemfiles
  1  0  0     0  0  0  0  0  0999 V2000
    1.0000    2.0000    3.0000 O   0  0  0  0  0  0  0  0  0  0  0  0
M  END
> <bool property>
false

> <float property>
1.23000

> <string-property>
prop1

> <vector property>
1.00000 2.00000 3.00000

$$$$
abc dfe ghi jkl mno pqr stu vwx yz 123 456 789 ABC DFE GHI JKL MNO PQR STU VWX Y

created by chemfiles
  0  0  0     0  0  0  0  0  0999 V2000
M  END
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
    frame.set("string-property", Property("prop1"));

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

    frame.set("name", "TEST");
    frame.set("float property", 1.23);

    file.write(frame);

    frame.clear_bonds();
    frame.resize(1);

    frame.set("bool property", false);
    file.write(frame);

    frame.set("vector property", Vector3D{1.0, 2.0, 3.0});
    file.write(frame);

    // name is too long for SDF specification
    frame = Frame();
    frame.set("name", "abc dfe ghi jkl mno pqr stu vwx yz 123 456 789 ABC DFE GHI JKL MNO PQR STU VWX YZ 123 456 789");
    file.write(frame);

    file.close();

    auto content = read_text_file(tmpfile);
    CHECK(content == EXPECTED_CONTENT);
}

TEST_CASE("Read and write files in memory") {
    SECTION("Reading from memory") {
        auto content = read_text_file("data/sdf/kinases.sdf");

        auto file = Trajectory::memory_reader(content.data(), content.size(), "SDF");
        REQUIRE(file.size() == 6);

        Frame frame;
        while (!file.done()) {
            frame = file.read();
        }
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(3.1149, -1.1207, 3.0606), 1e-3));
        CHECK(approx_eq(positions[49], Vector3D(-7.4890, -0.0147, -2.1114), 1e-3));
    }
}
