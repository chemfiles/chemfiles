// Chemfiles, a modern library for chemistry file readiconng and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <streambuf>
#include <fstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;


TEST_CASE("Read files in SMI format") {
    SECTION("Check nsteps") {
        Trajectory file1("data/smi/test.smi");
        CHECK(file1.nsteps() == 3);
    }

    SECTION("Read next frame") {
        Trajectory file("data/smi/test.smi");

        //Check to make sure things aren't exploding...
        auto frame1 = file.read();
        CHECK(frame1.size() == 6);
        auto topol1 = frame1.topology();
        CHECK(topol1.bonds().size() == 7);

        auto frame2 = file.read();
        CHECK(frame2.size() == 6);
        auto topol2 = frame2.topology();
        CHECK(topol2.bonds().size() == 6);

        auto frame3 = file.read();
        CHECK(frame3.size() == 4);
        auto topol3 = frame3.topology();
        auto bonds3 = topol3.bonds();
        CHECK(bonds3.size() == 3);
        CHECK((bonds3[0][0] == 0 && bonds3[0][1] == 1));
        CHECK((bonds3[1][0] == 0 && bonds3[1][1] == 2));
        CHECK((bonds3[2][0] == 0 && bonds3[2][1] == 3));
        CHECK(frame3[0].type() == "C");
        CHECK(frame3[1].type() == "Cl");
        CHECK(frame3[2].type() == "Cl");
        CHECK(frame3[3].type() == "Cl");
    }

    SECTION("Read a specific step") {
        Trajectory file("data/smi/test.smi");
        auto frame = file.read_step(1);
        CHECK(frame.size() == 6);
        auto topol = frame.topology();
        CHECK(topol.bonds().size() == 6);
    }

    SECTION("Read entire file") {
        Trajectory file("data/smi/rdkit_problems.smi");
        CHECK(file.nsteps() == 70);

        Frame frame;
        while (!file.done()) {
            frame = file.read();
        }
        CHECK(frame.size() == 14);
        CHECK(frame[0].type() == "Db");
        CHECK(frame[13].type()== "Og");
    }
}

TEST_CASE("Check parsing results") {
    SECTION("Details") {
        Trajectory file("data/smi/details.smi");
        CHECK(file.nsteps() == 1);

        auto frame = file.read();
        CHECK(frame.size() == 5);
        CHECK(frame[0].charge() == 0);
        CHECK(frame[0].type() == "O");
        CHECK(frame[4].charge() == -1);
        CHECK(frame[4].type() == "O");
    }

    SECTION("Ugly SMILES strings") {
        Trajectory file("data/smi/ugly.smi");
        CHECK(file.nsteps() == 3);

        // C1(CC1CC1CC1)
        auto frame = file.read();
        CHECK(frame.size() == 7);
        auto bonds = frame.topology().bonds();
        CHECK(bonds.size() == 8);
        CHECK((bonds[0][0] == 0 && bonds[0][1] == 1));
        CHECK((bonds[1][0] == 0 && bonds[1][1] == 2));
        CHECK((bonds[2][0] == 1 && bonds[2][1] == 2));
        CHECK((bonds[3][0] == 2 && bonds[3][1] == 3));
        CHECK((bonds[4][0] == 3 && bonds[4][1] == 4));
        CHECK((bonds[5][0] == 4 && bonds[5][1] == 5));
        CHECK((bonds[6][0] == 4 && bonds[6][1] == 6));
        CHECK((bonds[7][0] == 5 && bonds[7][1] == 6));

        // C1.C1CC1CC1
        frame = file.read();
        CHECK(frame.size() == 6);
        bonds = frame.topology().bonds();
        CHECK(bonds.size() == 6);
        CHECK((bonds[0][0] == 0 && bonds[0][1] == 1));
        CHECK((bonds[1][0] == 1 && bonds[1][1] == 2));
        CHECK((bonds[2][0] == 2 && bonds[2][1] == 3));
        CHECK((bonds[3][0] == 3 && bonds[3][1] == 4));
        CHECK((bonds[4][0] == 3 && bonds[4][1] == 5));
        CHECK((bonds[5][0] == 4 && bonds[5][1] == 5));
        CHECK(frame.topology().residues().size() == 2);
        CHECK(frame.topology().are_linked(
                frame.topology().residues()[0],
                frame.topology().residues()[1]));

        // C1CC11CC1
        frame = file.read();
        CHECK(frame.size() == 5);
        bonds = frame.topology().bonds();
        CHECK(bonds.size() == 6);
        CHECK((bonds[0][0] == 0 && bonds[0][1] == 1));
        CHECK((bonds[1][0] == 0 && bonds[1][1] == 2));
        CHECK((bonds[2][0] == 1 && bonds[2][1] == 2));
        CHECK((bonds[3][0] == 2 && bonds[3][1] == 3));
        CHECK((bonds[4][0] == 2 && bonds[4][1] == 4));
        CHECK((bonds[5][0] == 3 && bonds[5][1] == 4));
    }

    SECTION("RDKit problems") {
        Trajectory file("data/smi/rdkit_problems.smi");
        CHECK(file.nsteps() == 70);

        // C1CC2C1CC2
        Frame frame = file.read();
        CHECK(frame.size() == 6);
        auto bonds = frame.topology().bonds();
        CHECK(bonds.size() == 7);
        CHECK((bonds[0][0] == 0 && bonds[0][1] == 1));
        CHECK((bonds[1][0] == 0 && bonds[1][1] == 3));
        CHECK((bonds[2][0] == 1 && bonds[2][1] == 2));
        CHECK((bonds[3][0] == 2 && bonds[3][1] == 3));
        CHECK((bonds[4][0] == 2 && bonds[4][1] == 5));
        CHECK((bonds[5][0] == 3 && bonds[5][1] == 4));
        CHECK((bonds[6][0] == 4 && bonds[6][1] == 5));

        // [CH2+]C[CH+2]
        frame = file.read_step(6);
        CHECK(frame[0].get<Property::DOUBLE>("explicit_hydrogens") == 2.0);
        CHECK(frame[0].charge() == 1.0);
        CHECK(frame[2].get<Property::DOUBLE>("explicit_hydrogens") == 1.0);
        CHECK(frame[2].charge() == 2.0);

        // C1CC=1
        frame = file.read_step(8);
        auto bond_orders = frame.topology().bond_orders();
        CHECK(bond_orders[0] == Bond::SINGLE);
        CHECK(bond_orders[1] == Bond::DOUBLE);

        // C=1CC1
        frame = file.read_step(9);
        bond_orders = frame.topology().bond_orders();
        CHECK(bond_orders[0] == Bond::SINGLE);
        CHECK(bond_orders[1] == Bond::DOUBLE);
    }
}

TEST_CASE("Write SMI File") {
    auto tmpfile = NamedTempPath(".smi");
    const auto EXPECTED_CONTENT =
R"(C
CN
CN(P)O
CN(P(F)B)O
C1N(P(F1)B)O
C12N(P(F1)B2)O
)";

    Frame frame;
    frame.add_atom(Atom("C"), {0, 0, 0});

    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.add_atom(Atom("N"), {0, 0, 0});
    frame.add_bond(0, 1);
    file.write(frame);

    frame.add_atom(Atom("P"), {0, 0, 0});
    frame.add_atom(Atom("O"), {0, 0, 0});
    frame.add_bond(1, 2);
    frame.add_bond(1, 3);
    file.write(frame);

    frame.add_atom(Atom("F"), {0, 0, 0});
    frame.add_atom(Atom("B"), {0, 0, 0});
    frame.add_bond(2, 4);
    frame.add_bond(2, 5);
    file.write(frame);

    frame.add_bond(0, 4);
    file.write(frame);

    frame.add_bond(0, 5);
    file.write(frame);

    file.close();
    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == EXPECTED_CONTENT);
}
