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
        CHECK(file1.nsteps() == 8);
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
        CHECK(frame[0].get<Property::DOUBLE>("hydrogen_count") == 2.0);
        CHECK(frame[0].charge() == 1.0);
        CHECK(frame[2].get<Property::DOUBLE>("hydrogen_count") == 1.0);
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

    SECTION("Chirality") {
        Trajectory file("data/smi/chiral.smi");

        auto frame = file.read();
        CHECK(frame[1].get("chirality")->as_string() == "CCW TB1");
        frame = file.read();
        CHECK(frame[1].get("chirality")->as_string() == "CCW TB15");
        frame = file.read();
        CHECK(frame[1].get("chirality")->as_string() == "CW");
        frame = file.read();
        CHECK(frame[1].get("chirality")->as_string() == "CCW OH15");
        frame = file.read();
        CHECK(frame[1].get("chirality")->as_string() == "CW");
        frame = file.read();
        CHECK(frame[1].get("chirality")->as_string() == "CCW");
    }

    SECTION("Other tests") {
        Trajectory file("data/smi/test.smi");
        auto frame = file.read();
        frame = file.read();
        CHECK(frame[0].get("is_aromatic")->as_bool());
        CHECK(frame.get("name")->as_string() == "Benzene");

        while (!file.done()) {
            frame = file.read();
        }
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

TEST_CASE("Errors in SMI format") {
    for (auto entry : directory_files_iterator("data/smi/bad/")) {
        auto test = [=](){
            // We can throw either when creating the trajectory, or when reading
            // the frame, depending on the type of error
            auto file = Trajectory(entry.path().string());
            file.read();
        };
        CHECK_THROWS(test());
    }
}

TEST_CASE("Write SMI File") {
    auto tmpfile = NamedTempPath(".smi");
    const auto EXPECTED_CONTENT =
R"(C
C~N
C~N(P)=O
C~N(P(#F)$B)=O
C1~N(P(#F:1)$B)=O
C12~N(P(#F:1)$B/2)=O	test
[WH5+3].[35Cl-]->[c:1@H]<-[te@SP3]\[C@@]
O.O.O
)";

    Frame frame;
    frame.add_atom(Atom("C"), {0, 0, 0});

    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.add_atom(Atom("N"), {0, 0, 0});
    frame.add_bond(0, 1, Bond::UNKNOWN);
    file.write(frame);

    frame.add_atom(Atom("P"), {0, 0, 0});
    frame.add_atom(Atom("O"), {0, 0, 0});
    frame.add_bond(1, 2, Bond::SINGLE);
    frame.add_bond(1, 3, Bond::DOUBLE);
    file.write(frame);

    frame.add_atom(Atom("F"), {0, 0, 0});
    frame.add_atom(Atom("B"), {0, 0, 0});
    frame.add_bond(2, 4, Bond::TRIPLE);
    frame.add_bond(2, 5, Bond::QUADRUPLE);
    file.write(frame);

    frame.add_bond(0, 4, Bond::AROMATIC);
    file.write(frame);

    frame.add_bond(0, 5, Bond::UP);
    frame.set("name", "test");
    file.write(frame);

    // Reinitialize
    frame = Frame();
    frame.add_atom(Atom("W"), {0, 0, 0});
    frame[0].set_charge(3);
    frame[0].set("hydrogen_count", 5);
    frame[0].set("chirality", "CCW TX99");

    frame.add_atom(Atom("Cl"), {0, 0, 0});
    frame[1].set_charge(-1);
    frame[1].set_mass(35);
    frame[1].set("hydrogen_count", -1); // warning
    frame[1].set("smiles_class", "35-chloride"); // warning
    frame[1].set("chirality","CXX"); // warning

    frame.add_atom(Atom("C"), {0, 0, 0});
    frame[2].set("is_aromatic", true);
    frame[2].set("smiles_class", 1);
    frame[2].set("hydrogen_count", 1);
    frame[2].set("chirality", "CCW");

    frame.add_atom(Atom("Te"), {0, 0, 0});
    frame[3].set("is_aromatic", true);
    frame[3].set("chirality", "CCW SP3");

    frame.add_atom(Atom("C"), {0, 0, 0});
    frame[4].set("chirality", "CW");

    frame.add_bond(1, 2, Bond::DATIVE_R);
    frame.add_bond(2, 3, Bond::DATIVE_L);
    frame.add_bond(3, 4, Bond::DOWN);

    file.write(frame);

    // Reinitialize and test for discrete molecules
    frame = Frame();
    frame.add_atom(Atom("O"), {0, 0, 0});
    frame.add_atom(Atom("O"), {0, 0, 0});
    frame.add_atom(Atom("O"), {0, 0, 0});
    file.write(frame);

    file.close();
    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == EXPECTED_CONTENT);
}
