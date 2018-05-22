// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;


TEST_CASE("Read files in CSSR format") {
    SECTION("Water") {
        // This is the first frame of data/xyz/water.xyz, converted to CSSR
        // with open babel
        Trajectory file("data/cssr/water.cssr");
        auto frame = file.read();

        CHECK(frame.size() == 297);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.417, 8.303, 11.737), 1e-3));
        CHECK(approx_eq(positions[296], Vector3D(6.664, 11.6148, 12.961), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(fabs(cell.a() - 1.0) < 1e-5);
        CHECK(fabs(cell.b() - 1.0) < 1e-5);
        CHECK(fabs(cell.c() - 1.0) < 1e-5);

        // bonds comes from open babel, which do not use PBC to guess them
        CHECK(frame.topology().bonds().size() == 186);

        CHECK(frame[0].name() == "O1");
        CHECK(frame[10].name() == "H7");
        CHECK(frame[15].name() == "O6");
        CHECK(frame[15].type() == "O");
    }

    SECTION("EDI zeolite") {
        Trajectory file("data/cssr/EDI.cssr");
        auto frame = file.read();

        CHECK(frame.size() == 15);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.31455, 0.0, 2.27555), 1e-5));
        CHECK(approx_eq(positions[11], Vector3D(0.0, 5.07052, 5.65106), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(fabs(cell.a() - 6.926) < 1e-5);
        CHECK(fabs(cell.b() - 6.926) < 1e-5);
        CHECK(fabs(cell.c() - 6.410) < 1e-5);

        CHECK(frame[0].name() == "O");
        CHECK(frame[0].type() == "O");
        CHECK(frame[11].name() == "Si");
        CHECK(frame[11].type() == "Si");
    }
}

TEST_CASE("Write files in CSSR format") {
    auto tmpfile = NamedTempPath(".cssr");
    const auto expected_content =
R"( REFERENCE STRUCTURE = 00000   A,B,C =  10.000  10.000  12.000
   ALPHA,BETA,GAMMA =  90.000  90.000  90.000    SPGR =  1 P1
   4   0
 file created with chemfiles
1    A       0.10000   0.20000   0.25000   3   0   0   0   0   0   0   0   0.000
2    B       0.10000   0.20000   0.25000   4   0   0   0   0   0   0   0   0.000
3    C       0.10000   0.20000   0.25000   1   0   0   0   0   0   0   0 -42.000
4    D       0.10000   0.20000   0.25000   2   0   0   0   0   0   0   0   0.000
)";

    auto frame = Frame();
    frame.add_atom(Atom("A"), Vector3D(1, 2, 3));
    frame.add_atom(Atom("B"), Vector3D(1, 2, 3));
    frame.add_atom(Atom("C"), Vector3D(1, 2, 3));
    frame.add_atom(Atom("D"), Vector3D(1, 2, 3));

    frame[2].set_charge(-42);

    frame.add_bond(0, 2);
    frame.add_bond(1, 3);

    frame.set_cell(UnitCell(10, 10, 12));
    auto t = Trajectory(tmpfile, 'w');
    t.write(frame);
    t.close();

    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == expected_content);
}
