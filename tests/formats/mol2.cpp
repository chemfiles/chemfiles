// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"

using namespace chemfiles;

static bool contains_bond(const Topology& topology, Bond bond) {
    for (auto& exist: topology.bonds()) {
        if (bond == exist) {
            return true;
        }
    }
    return false;
}

TEST_CASE("Read files in mol2 format") {

    SECTION("Various files") {
        // Just checking that we can read them without error
        auto file = Trajectory("data/mol2/lysozyme-ligand-tripos.mol2");
        REQUIRE(file.nsteps() == 1);
        auto frame = file.read();
        CHECK(frame.size() == 18);

        file = Trajectory("data/mol2/zinc_856218.mol2");
        REQUIRE(file.nsteps() == 1);
        frame = file.read();
        CHECK(frame.size() == 45);

        file = Trajectory("data/mol2/adp.mol2");
        REQUIRE(file.nsteps() == 1);
        frame = file.read();
        CHECK(frame.size() == 39);

        file = Trajectory("data/mol2/li.mol2");
        REQUIRE(file.nsteps() == 1);
        frame = file.read();
        CHECK(frame.size() == 1);
        CHECK(frame.cell().shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(frame.cell().lengths(), {10, 10, 10}, 1e-4));

        file = Trajectory("data/mol2/status-bits.mol2");
        REQUIRE(file.nsteps() == 1);
        frame = file.read();
        CHECK(frame.size() == 18);
    }

    SECTION("imatinib.mol2") {
        auto file = Trajectory("data/mol2/imatinib.mol2");
        REQUIRE(file.nsteps() == 1);

        auto frame = file.read();
        CHECK(frame.size() == 68);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[3], Vector3D(-0.1070, -1.8330, -0.2330), 1e-4));
        CHECK(approx_eq(positions[61], Vector3D(-5.5050, -4.7850, -0.1660), 1e-4));

        auto& topology = frame.topology();
        CHECK(topology[3].name() == "N2");
        CHECK(topology[3].type() == "n");
        CHECK(approx_eq(topology[3].charge(), -0.471100, 1e-5));
        CHECK(topology[61].name() == "H24");
        CHECK(topology[61].type() == "H");
        CHECK(approx_eq(topology[61].charge(), 0.044367, 1e-5));

        CHECK(topology.bonds().size() == 72);
        CHECK(contains_bond(topology, {12, 14}));
        CHECK(contains_bond(topology, {34, 65}));

        CHECK(frame.topology().residues().size() == 1);

        REQUIRE(frame.topology().residue_for_atom(33));
        auto residue = *frame.topology().residue_for_atom(33);
        CHECK(residue.size() == 68);
        CHECK(residue.contains(0));
        CHECK(residue.contains(1));
        CHECK(residue.contains(2));

        CHECK(topology.bond_order(0, 20) == Bond::DOUBLE);
        CHECK(topology.bond_order(5, 28) == Bond::AROMATIC);
    }

    SECTION("Molecules.mol2") {
        auto file = Trajectory("data/mol2/Molecules.mol2");
        REQUIRE(file.nsteps() == 200);

        auto frame = file.read();
        CHECK(frame.size() == 49);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(6.8420, 9.9900, 22.7430), 1e-4));
        CHECK(approx_eq(positions[33], Vector3D(4.5540, 11.1000, 22.5880), 1e-4));

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "N1");
        CHECK(topology[0].type() == "N");
        CHECK(topology[0].get("sybyl")->as_string() == "N.am");
        CHECK(approx_eq(topology[0].charge(), -0.8960, 1e-4));
        CHECK(topology[33].name() == "H131");
        CHECK(topology[33].type() == "H");
        CHECK(approx_eq(topology[33].charge(), 0.0720, 1e-4));

        CHECK(topology.bonds().size() == 51);
        CHECK(contains_bond(topology, {7, 34}));
        CHECK(contains_bond(topology, {13, 19}));
    }

    SECTION("Read next step") {
        auto file = Trajectory("data/mol2/Molecules.mol2");
        file.read();
        auto frame = file.read();

        CHECK(frame.size() == 49);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(6.6710, 9.9330, 22.9940), 1e-4));
        CHECK(approx_eq(positions[33], Vector3D(4.1880, 9.4540, 22.6900), 1e-4));

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "N1");
        CHECK(topology[0].type() == "N");
        CHECK(topology[0].get("sybyl")->as_string() == "N.am");
        CHECK(approx_eq(topology[0].charge(), -0.8960, 1e-4));

        CHECK(topology[33].name() == "H131");
        CHECK(topology[33].type() == "H");
        CHECK(approx_eq(topology[33].charge(), 0.0720, 1e-4));

        CHECK(topology.bonds().size() == 51);
        CHECK(contains_bond(topology, {7, 35}));
        CHECK(contains_bond(topology, {13, 15}));
    }

    SECTION("Read a specific step") {
        auto file = Trajectory("data/mol2/Molecules.mol2");
        auto frame = file.read_step(1);

        CHECK(frame.size() == 49);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(6.6710, 9.9330, 22.9940), 1e-4));
        CHECK(approx_eq(positions[33], Vector3D(4.1880, 9.4540, 22.6900), 1e-4));

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "N1");
        CHECK(topology[0].type() == "N");
        CHECK(topology[0].get("sybyl")->as_string() == "N.am");

        CHECK(approx_eq(topology[0].charge(), -0.8960, 1e-4));
        CHECK(topology[33].name() == "H131");
        CHECK(topology[33].type() == "H");
        CHECK(approx_eq(topology[33].charge(), 0.0720, 1e-4));

        CHECK(topology.bonds().size() == 51);
        CHECK(contains_bond(topology, {7, 35}));
        CHECK(contains_bond(topology, {13, 15}));
    }
}

TEST_CASE("Write files in mol2 format") {
    auto tmpfile = NamedTempPath(".mol2");
    const auto EXPECTED_CONTENT =
    "@<TRIPOS>MOLECULE\n"
    "\n"
    "   4     1    1    0    0\n"
    "SMALL\n"
    "USER_CHARGES\n\n"
    "@<TRIPOS>ATOM\n"
    "   1 C     1.000000 2.000000 3.000000 C.2 1 XXX 0.000000\n"
    "   2 B     1.123456 2.123457 10000000.123456 B 2 XXX 0.000000\n"
    "   3 C     1.000000 2.000000 3.000000 C 3 XXX 0.000000\n"
    "   4 D     1.000000 2.000000 3.000000 D 4 XXX 0.000000\n"
    "@<TRIPOS>BOND\n"
    "   1     1     2    1\n"
    "@<TRIPOS>SUBSTRUCTURE\n"
    "   1 ****        1 TEMP                        0 ****  **** 0 ROOT\n\n"
    "@<TRIPOS>MOLECULE\n"
    "test\n"
    "   7     8    1    0    0\n"
    "SMALL\n"
    "USER_CHARGES\n\n"
    "@<TRIPOS>ATOM\n"
    "   1 C     1.000000 2.000000 3.000000 C.2 4 XXX 0.000000\n"
    "   2 B     1.123456 2.123457 10000000.123456 B 3 foo 0.000000\n"
    "   3 C     1.000000 2.000000 3.000000 C 3 foo 0.000000\n"
    "   4 D     1.000000 2.000000 3.000000 D 5 barbar 0.000000\n"
    "   5 E     4.000000 5.000000 6.000000 E 6 XXX 0.000000\n"
    "   6 F     4.000000 5.000000 6.000000 F 7 XXX 0.000000\n"
    "   7 G     4.000000 5.000000 6.000000 G 8 XXX 0.000000\n"
    "@<TRIPOS>BOND\n"
    "   1     1     2    1\n"
    "   2     1     7    1\n"
    "   3     2     7    2\n"
    "   4     3     7    3\n"
    "   5     4     7    ar\n"
    "   6     5     6    am\n"
    "   7     5     7    du\n"
    "   8     6     7    du\n"
    "@<TRIPOS>CRYSIN\n"
    "   22.0000   22.0000   22.0000   90.0000   90.0000   90.0000 1 1\n"
    "@<TRIPOS>SUBSTRUCTURE\n"
    "   1 ****        1 TEMP                        0 ****  **** 0 ROOT\n\n";

    auto frame = Frame();
    frame.add_atom(Atom("C"), {1, 2, 3});
    frame[0].set("sybyl", "C.2");
    frame.add_atom(Atom("B"), {1.123456, 2.123456789, 10000000.123456});
    frame.add_atom(Atom("C"), {1, 2, 3});
    frame.add_atom(Atom("D"), {1, 2, 3});
    frame.add_bond(0, 1, Bond::SINGLE);

    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.set_cell(UnitCell({22, 22, 22}));
    frame.set("name", "test");
    frame.add_atom(Atom("E"), {4, 5, 6});
    frame.add_atom(Atom("F"), {4, 5, 6});
    frame.add_atom(Atom("G"), {4, 5, 6});
    frame.add_bond(4, 5, Bond::AMIDE);
    frame.add_bond(0, 6, Bond::SINGLE);
    frame.add_bond(1, 6, Bond::DOUBLE);
    frame.add_bond(2, 6, Bond::TRIPLE);
    frame.add_bond(3, 6, Bond::AROMATIC);
    frame.add_bond(4, 6, Bond::UNKNOWN);
    frame.add_bond(5, 6);

    Residue residue("foo", 3);
    residue.add_atom(1);
    residue.add_atom(2);
    frame.add_residue(residue);

    residue = Residue("barbar"); // This will be truncated in output
    residue.add_atom(3);
    frame.add_residue(residue);

    file.write(frame);
    file.close();

    auto check_pdb = Trajectory(tmpfile);
    CHECK(check_pdb.nsteps() == 2);
    CHECK(check_pdb.read().size() == 4);
    CHECK(check_pdb.read().size() == 7);
    check_pdb.close();

    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == EXPECTED_CONTENT);
}


TEST_CASE("Read and write files in memory") {
    SECTION("Reading from memory") {
        std::ifstream checking("data/mol2/Molecules.mol2");
        std::vector<char> content((std::istreambuf_iterator<char>(checking)),
            std::istreambuf_iterator<char>());

        auto file = Trajectory::memory_reader(content.data(), content.size(), "MOL2");
        auto frame = file.read_step(1);

        CHECK(frame.size() == 49);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(6.6710, 9.9330, 22.9940), 1e-4));
        CHECK(approx_eq(positions[33], Vector3D(4.1880, 9.4540, 22.6900), 1e-4));

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "N1");
        CHECK(topology[0].type() == "N");
        CHECK(topology[0].get("sybyl")->as_string() == "N.am");
    }
}
