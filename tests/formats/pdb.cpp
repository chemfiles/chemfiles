#include <fstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in PDB format", "[Molfile]"){
    SECTION("Read trajectory") {
        Trajectory file("data/pdb/water.pdb");
        Frame frame = file.read();

        CHECK(frame.natoms() == 297);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], vector3d(0.417, 8.303, 11.737), 1e-3));
        CHECK(approx_eq(positions[296], vector3d(6.664, 11.6148, 12.961), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(fabs(cell.a() - 15.0) < 1e-5);

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.natoms() == 297);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], vector3d(0.299, 8.310, 11.721), 1e-4));
        CHECK(approx_eq(positions[296], vector3d(6.798, 11.509, 12.704), 1e-4));
    }

    SECTION("Read bonds") {
        Trajectory file("data/pdb/MOF-5.pdb");
        Frame frame = file.read();

        auto topology = frame.topology();

        CHECK(topology.natoms() == 65);

        CHECK(topology[0].type() == "Zn");
        CHECK(topology[1].type() == "O");

        CHECK(topology[0].name() == "ZN");
        CHECK(topology[1].name() == "O");

        CHECK(topology.bonds().size() == 68);

        CHECK(topology.isbond(9, 38));
        CHECK(topology.isbond(58, 62));
        CHECK(topology.isbond(37, 24));
        CHECK(topology.isbond(27, 31));

        CHECK(topology.isangle(20, 21, 23));
        CHECK(topology.isangle(9, 38, 44));

        CHECK(topology.isdihedral(64, 62, 58, 53));
        CHECK(topology.isdihedral(22, 21, 23, 33));
    }

    SECTION("Support short records") {
        Trajectory file("data/pdb/cryst1.pdb");
        Frame frame = file.read();
    }

    SECTION("Read residue information") {
        Trajectory file("data/pdb/water.pdb");
        Frame frame = file.read();

        CHECK(frame.topology().residues().size() == 99);

        REQUIRE(frame.topology().residue(1));
        auto residue = (*frame.topology().residue(1));
        CHECK(residue.size() == 3);
        CHECK(residue.contains(0));
        CHECK(residue.contains(1));
        CHECK(residue.contains(2));

        file = Trajectory("data/pdb/MOF-5.pdb");
        frame = file.read();

        CHECK(frame.topology().residues().size() == 1);
        residue = frame.topology().residues()[0];
        CHECK(residue.size() == frame.natoms());
        CHECK(residue.name() == "LIG");
    }
}

TEST_CASE("Write files in PDB format", "[PDB]"){
    const auto EXPECTED_CONTENT =
    "CRYST1   22.000   22.000   22.000  90.00  90.00  90.00 P 1           1\n"
    "HETATM    1    A RES X   1       1.000   2.000   3.000  1.00  0.00           A\n"
    "HETATM    2    B RES X   2       1.000   2.000   3.000  1.00  0.00           B\n"
    "HETATM    3    C RES X   3       1.000   2.000   3.000  1.00  0.00           C\n"
    "HETATM    4    D RES X   4       1.000   2.000   3.000  1.00  0.00           D\n"
    "CONECT    1    2\n"
    "CONECT    2    1\n"
    "END\n"
    "CRYST1   22.000   22.000   22.000  90.00  90.00  90.00 P 1           1\n"
    "HETATM    1    A RES X   4       4.000   5.000   6.000  1.00  0.00           A\n"
    "HETATM    2    B foo X   3       4.000   5.000   6.000  1.00  0.00           B\n"
    "HETATM    3    C foo X   3       4.000   5.000   6.000  1.00  0.00           C\n"
    "HETATM    4    D bar X  -1       4.000   5.000   6.000  1.00  0.00           D\n"
    "HETATM    5    E RES X   5       4.000   5.000   6.000  1.00  0.00           E\n"
    "HETATM    6    F RES X   6       4.000   5.000   6.000  1.00  0.00           F\n"
    "CONECT    1    2\n"
    "CONECT    2    1\n"
    "CONECT    5    6\n"
    "CONECT    6    5\n"
    "END\n";

    Topology topology;
    topology.append(Atom("A"));
    topology.append(Atom("B"));
    topology.append(Atom("C"));
    topology.append(Atom("D"));
    topology.add_bond(0, 1);

    Frame frame(topology);
    frame.set_cell(UnitCell(22));

    auto positions = frame.positions();
    for(size_t i=0; i<4; i++) {
        positions[i] = vector3d(1, 2, 3);
    }

    {
        auto file = Trajectory("test-tmp.pdb", 'w');
        file.write(frame);
    }

    frame.resize(6);
    positions = frame.positions();
    for(size_t i=0; i<6; i++)
        positions[i] = vector3d(4, 5, 6);

    topology.append(Atom("E"));
    topology.append(Atom("F"));
    topology.add_bond(4, 5);

    Residue residue("foo", 3);
    residue.add_atom(1);
    residue.add_atom(2);
    topology.add_residue(residue);

    residue = Residue("barbar"); // This will be truncated in output
    residue.add_atom(3);
    topology.add_residue(residue);

    frame.set_topology(topology);

    {
        auto file = Trajectory("test-tmp.pdb", 'a');
        file.write(frame);
    }

    std::ifstream checking("test-tmp.pdb");
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    checking.close();

    CHECK(content == EXPECTED_CONTENT);
    remove("test-tmp.pdb");
}
