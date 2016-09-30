#include "catch.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#define PDBDIR SRCDIR "/data/pdb/"

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in PDB format", "[Molfile]"){
    SECTION("Read trajectory") {
        Trajectory file(PDBDIR"water.pdb");
        Frame frame = file.read();

        CHECK(frame.natoms() == 297);
        auto positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(0.417f, 8.303f, 11.737f), 1e-3));
        CHECK(roughly(positions[296], vector3d(6.664f, 11.6148f, 12.961f), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.type() == UnitCell::ORTHORHOMBIC);
        CHECK(fabs(cell.a() - 15.0) < 1e-5);

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.natoms() == 297);
        positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(0.299f, 8.310f, 11.721f), 1e-4));
        CHECK(roughly(positions[296], vector3d(6.798f, 11.509f, 12.704f), 1e-4));
    }

    SECTION("Read bonds") {
        Trajectory file(PDBDIR"MOF-5.pdb");
        Frame frame = file.read();

        auto topology = frame.topology();

        CHECK(topology.natoms() == 65);

        CHECK(topology[0].element() == "ZN");
        CHECK(topology[1].element() == "O");

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
        Trajectory file(PDBDIR "cryst1.pdb");
        Frame frame = file.read();
    }

    SECTION("Read residue information") {
        Trajectory file(PDBDIR "water.pdb");
        Frame frame = file.read();

        CHECK(frame.topology().residues().size() == 99);

        REQUIRE(frame.topology().residue(1));
        auto residue = (*frame.topology().residue(1));
        CHECK(residue.size() == 3);
        CHECK(residue.contains(0));
        CHECK(residue.contains(1));
        CHECK(residue.contains(2));

        file = Trajectory(PDBDIR "MOF-5.pdb");
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
    "HETATM    0   A    X   0    1.000   2.000   3.000  0.00  0.00 A\n"
    "HETATM    1   B    X   1    1.000   2.000   3.000  0.00  0.00 B\n"
    "HETATM    2   C    X   2    1.000   2.000   3.000  0.00  0.00 C\n"
    "HETATM    3   D    X   3    1.000   2.000   3.000  0.00  0.00 D\n"
    "CONECT    0    1\n"
    "CONECT    1    0\n"
    "END\n"
    "CRYST1   22.000   22.000   22.000  90.00  90.00  90.00 P 1           1\n"
    "HETATM    0   A    X   0    4.000   5.000   6.000  0.00  0.00 A\n"
    "HETATM    1   B    X   1    4.000   5.000   6.000  0.00  0.00 B\n"
    "HETATM    2   C    X   2    4.000   5.000   6.000  0.00  0.00 C\n"
    "HETATM    3   D    X   3    4.000   5.000   6.000  0.00  0.00 D\n"
    "HETATM    4   E    X   4    4.000   5.000   6.000  0.00  0.00 E\n"
    "HETATM    5   F    X   5    4.000   5.000   6.000  0.00  0.00 F\n"
    "CONECT    0    1\n"
    "CONECT    1    0\n"
    "CONECT    4    5\n"
    "CONECT    5    4\n"
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
