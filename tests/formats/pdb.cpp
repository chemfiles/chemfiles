// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

template <typename T>
static bool contains(const std::vector<T> haystack, const T& needle) {
    return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
}

TEST_CASE("Read files in PDB format") {
    SECTION("Read next step") {
        Trajectory file("data/pdb/water.pdb");
        Frame frame = file.read();

        CHECK(frame.size() == 297);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.417, 8.303, 11.737), 1e-3));
        CHECK(approx_eq(positions[296], Vector3D(6.664, 11.6148, 12.961), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(fabs(cell.a() - 15.0) < 1e-5);

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.size() == 297);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.299, 8.310, 11.721), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(6.798, 11.509, 12.704), 1e-4));
    }

    SECTION("Read a specific step") {
        Trajectory file("data/pdb/water.pdb");

        auto frame = file.read_step(2);
        CHECK(frame.size() == 297);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.299, 8.310, 11.721), 1e-4));
        CHECK(approx_eq(positions[296], Vector3D(6.798, 11.509, 12.704), 1e-4));

        frame = file.read_step(0);
        CHECK(frame.size() == 297);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.417, 8.303, 11.737), 1e-3));
        CHECK(approx_eq(positions[296], Vector3D(6.664, 11.6148, 12.961), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(fabs(cell.a() - 15.0) < 1e-5);
    }

    SECTION("Read bonds") {
        Trajectory file("data/pdb/MOF-5.pdb");
        Frame frame = file.read();

        auto topology = frame.topology();

        CHECK(topology.size() == 65);

        CHECK(topology[0].type() == "Zn");
        CHECK(topology[1].type() == "O");

        CHECK(topology[0].name() == "ZN");
        CHECK(topology[1].name() == "O");

        CHECK(topology.bonds().size() == 68);

        CHECK(contains(topology.bonds(), Bond(9, 38)));
        CHECK(contains(topology.bonds(), Bond(58, 62)));
        CHECK(contains(topology.bonds(), Bond(37, 24)));
        CHECK(contains(topology.bonds(), Bond(27, 31)));

        CHECK(contains(topology.angles(), Angle(20, 21, 23)));
        CHECK(contains(topology.angles(), Angle(9, 38, 44)));

        CHECK(contains(topology.dihedrals(), Dihedral(64, 62, 58, 53)));
        CHECK(contains(topology.dihedrals(), Dihedral(22, 21, 23, 33)));
    }

    SECTION("Support short records") {
        Trajectory file("data/pdb/cryst1.pdb");
        Frame frame = file.read();
    }

    SECTION("Read residue information") {
        Trajectory file("data/pdb/water.pdb");
        Frame frame = file.read();

        CHECK(frame.topology().residues().size() == 99);

        REQUIRE(frame.topology().residue_for_atom(1));
        auto residue = *frame.topology().residue_for_atom(1);
        CHECK(residue.size() == 3);
        CHECK(residue.contains(0));
        CHECK(residue.contains(1));
        CHECK(residue.contains(2));

        file = Trajectory("data/pdb/MOF-5.pdb");
        frame = file.read();

        CHECK(frame.topology().residues().size() == 1);
        residue = frame.topology().residues()[0];
        CHECK(residue.size() == frame.size());
        CHECK(residue.name() == "LIG");
    }

    SECTION("Read ATOM/HETATM information") {
        Trajectory file("data/pdb/hemo.pdb");
        Frame frame = file.read();

        for (size_t i = 0; i < 73; i++) {
            CHECK(frame.topology()[i].get("is_hetatm")->as_bool());
        }

        for (size_t i = 73; i < 522; i++) {
            CHECK(frame.topology()[i].get("is_hetatm")->as_bool() == false);
        }
    }

    SECTION("Handle multiple END records") {
        Trajectory file("data/pdb/end-endmdl.pdb");
        CHECK(file.nsteps() == 2);

        auto frame = file.read();
        CHECK(frame.size() == 4);

        frame = file.read();
        CHECK(frame.size() == 7);
    }

    SECTION("Handle multiple MODEL without END") {
        Trajectory file("data/pdb/model.pdb");
        CHECK(file.nsteps() == 2);

        auto frame = file.read();
        CHECK(frame.size() == 2223);

        frame = file.read();
        CHECK(frame.size() == 2223);
    }
}

TEST_CASE("Write files in PDB format") {
    auto tmpfile = NamedTempPath(".pdb");
    const auto EXPECTED_CONTENT =
    "MODEL    1\n"
    "CRYST1   22.000   22.000   22.000  90.00  90.00  90.00 P 1           1\n"
    "ATOM      1 A    XXX X   1       1.000   2.000   3.000  1.00  0.00           A\n"
    "HETATM    2 B    XXX X   2       1.000   2.000   3.000  1.00  0.00           B\n"
    "ATOM      3 C    XXX X   3       1.000   2.000   3.000  1.00  0.00           C\n"
    "HETATM    4 D    XXX X   4       1.000   2.000   3.000  1.00  0.00           D\n"
    "CONECT    1    2\n"
    "CONECT    2    1\n"
    "ENDMDL\n"
    "MODEL    2\n"
    "CRYST1   22.000   22.000   22.000  90.00  90.00  90.00 P 1           1\n"
    "ATOM      1 A    XXX X   4       4.000   5.000   6.000  1.00  0.00           A\n"
    "HETATM    2 B    foo X   3       4.000   5.000   6.000  1.00  0.00           B\n"
    "ATOM      3 C    foo X   3       4.000   5.000   6.000  1.00  0.00           C\n"
    "HETATM    4 D    bar X  -1       4.000   5.000   6.000  1.00  0.00           D\n"
    "HETATM    5 E    XXX X   5       4.000   5.000   6.000  1.00  0.00           E\n"
    "HETATM    6 F    XXX X   6       4.000   5.000   6.000  1.00  0.00           F\n"
    "HETATM    7 G    XXX X   7       4.000   5.000   6.000  1.00  0.00           G\n"
    "CONECT    1    2    7\n"
    "CONECT    2    1    7\n"
    "CONECT    3    7\n"
    "CONECT    4    7\n"
    "CONECT    5    6    7\n"
    "CONECT    6    5    7\n"
    "CONECT    7    1    2    3    4\n"
    "CONECT    7    5    6\n"
    "ENDMDL\n"
    "END\n";

    Topology topology;
    topology.add_atom(Atom("A"));
    topology.add_atom(Atom("B"));
    topology.add_atom(Atom("C"));
    topology.add_atom(Atom("D"));
    topology.add_bond(0, 1);
    topology[0].set("is_hetatm", false);
    topology[1].set("is_hetatm", true);
    topology[2].set("is_hetatm", false);
    topology[3].set("is_hetatm", true);

    Frame frame(topology);
    frame.set_cell(UnitCell(22));

    auto positions = frame.positions();
    for(size_t i=0; i<4; i++) {
        positions[i] = Vector3D(1, 2, 3);
    }

    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.resize(7);
    positions = frame.positions();
    for(size_t i=0; i<7; i++) {
        positions[i] = Vector3D(4, 5, 6);
    }
    topology.add_atom(Atom("E"));
    topology.add_atom(Atom("F"));
    topology.add_atom(Atom("G"));
    topology.add_bond(4, 5);
    topology.add_bond(0, 6);
    topology.add_bond(1, 6);
    topology.add_bond(2, 6);
    topology.add_bond(3, 6);
    topology.add_bond(4, 6);
    topology.add_bond(5, 6);

    Residue residue("foo", 3);
    residue.add_atom(1);
    residue.add_atom(2);
    topology.add_residue(residue);

    residue = Residue("barbar"); // This will be truncated in output
    residue.add_atom(3);
    topology.add_residue(residue);

    frame.set_topology(topology);

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

TEST_CASE("PDB files with big values") {
    SECTION("Unit cell and Coordinates") {
        auto tmpfile = NamedTempPath(".pdb");
        auto trajectory = Trajectory(tmpfile, 'w');

        auto frame = Frame();
        frame.resize(1);
        frame.set_cell(UnitCell(1234567890));
        CHECK_THROWS_AS(trajectory.write(frame), FormatError);

        frame.set_cell(UnitCell(12));
        frame.positions()[0] = Vector3D(123456789, 2, 3);
        CHECK_THROWS_AS(trajectory.write(frame), FormatError);
    }

    SECTION("Default residues") {
        auto tmpfile = NamedTempPath(".pdb");

        Topology topology;
        for(size_t i=0; i<10001; i++) {
            topology.add_atom(Atom("A"));
        }
        Frame frame(topology);
        auto positions = frame.positions();
        positions[998] = Vector3D(1., 2., 3.);
        positions[9998] = Vector3D(4., 5., 6.);
        positions[9999] = Vector3D(7., 8., 9.);

        Trajectory(tmpfile, 'w').write(frame);

        // Re-read the file we just wrote
        frame = Trajectory(tmpfile, 'r').read();
        positions = frame.positions();

        // If resSeq is has more than 4 characters, coordinates won't be read
        // correctly
        CHECK(positions[998] == Vector3D(1., 2., 3.));
        CHECK(positions[9998] == Vector3D(4., 5., 6.));
        CHECK(positions[9999] == Vector3D(7., 8., 9.));
    }

    SECTION("User specified residues") {
        auto tmpfile = NamedTempPath(".pdb");

        Topology topology;
        for(size_t i=0; i<10001; i++) {
            Atom atom("A");
            topology.add_atom(atom);
            Residue residue("ANA", i + 1);
            residue.add_atom(i);
            topology.add_residue(residue);
        }
        Frame frame(topology);
        auto positions = frame.positions();
        positions[998] = Vector3D(1., 2., 3.);
        positions[9998] = Vector3D(4., 5., 6.);
        positions[9999] = Vector3D(7., 8., 9.);

        Trajectory(tmpfile, 'w').write(frame);

        // Re-read the file we just wrote
        frame = Trajectory(tmpfile, 'r').read();
        positions = frame.positions();

        // If resSeq is has more than 4 characters, coordinates won't be read
        // correctly
        CHECK(positions[998] == Vector3D(1., 2., 3.));
        CHECK(positions[9998] == Vector3D(4., 5., 6.));
        CHECK(positions[9999] == Vector3D(7., 8., 9.));
    }

    SECTION("CONNECT with too many atoms") {
        auto tmpfile = NamedTempPath(".pdb");

        auto frame = Frame();
        for(size_t i=0; i<110000; i++) {
            frame.add_atom(Atom("A"), {0.0, 0.0, 0.0});
        }
        frame.add_bond(101000, 101008);
        Trajectory(tmpfile, 'w').write(frame);

        // Re-read the file we just wrote
        frame = Trajectory(tmpfile, 'r').read();
        CHECK(frame.topology().bonds().empty());
    }
}
