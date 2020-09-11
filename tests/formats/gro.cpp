// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
#include <fstream>
using namespace chemfiles;

TEST_CASE("Read files in Gromacs .gro format") {
    SECTION("Simple GRO File") {
        auto file = Trajectory("data/gro/ubiquitin.gro");
        REQUIRE(file.nsteps() == 1);
        Frame frame = file.read();

        CHECK(frame.size() == 1405);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.93, 24.95, 18.87), 1e-2));
        CHECK(approx_eq(positions[1], Vector3D(25.66, 25.37, 18.33), 1e-2));
        CHECK(approx_eq(positions[678], Vector3D(27.57, 32.25, 37.53), 1e-2));

        CHECK(frame[0].name() == "N");
        CHECK(frame[1].name() == "H1");
        CHECK(frame[678].name() == "O");

        CHECK(frame.topology().residues().size() == 134);
        CHECK(frame.topology().residues()[0].name() == "MET");
        CHECK(frame.topology().residues()[75].name() == "GLY");

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {55.68, 58.87, 62.57}, 1e-2));
    }

    SECTION("Triclinic Box") {
        auto file = Trajectory("data/gro/cod_4020641.gro");
        auto frame = file.read();

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::TRICLINIC);
        CHECK(approx_eq(cell.lengths(), {26.2553, 11.3176, 11.8892}, 1e-4));
        CHECK(approx_eq(cell.angles(), {90.0, 112.159, 90.0}, 1e-3));

        file = Trajectory("data/pdb/1vln-triclinic.pdb");
        frame = file.read();
        CHECK(frame.size() == 14520);

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::TRICLINIC);
        CHECK(approx_eq(cell.lengths(), {78.8, 79.3, 133.3}, 1e-12));
        CHECK(approx_eq(cell.angles(), {97.1, 90.2, 97.5}, 1e-12));
    }

    SECTION("Read next step") {
        auto file = Trajectory("data/gro/lysozyme.gro");
        REQUIRE(file.nsteps() == 3);
        Frame frame = file.read();

        CHECK(*frame.get("name") == "LYSOZYME in water NVT");
        CHECK(frame.size() == 1960);
        auto positions = frame.positions();
        auto velocities = *frame.velocities();

        CHECK(approx_eq(positions[0], Vector3D(42.68, 32.61, 22.84), 1e-3));
        CHECK(approx_eq(velocities[0],Vector3D(-00.161,-01.380,-03.884), 1e-3));

        CHECK(approx_eq(positions[1526], Vector3D(27.04, 40.31, 46.51), 1e-3));
        CHECK(approx_eq(velocities[1526],Vector3D(-1.993, -0.378, -4.302), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {70.1008, 70.1008, 70.1008}, 1e-5));

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(*frame.get("name") == "LYSOZYME in water MD");
        CHECK(frame.size() == 1960);
        positions = frame.positions();
        CHECK(frame.velocities());
        velocities = *frame.velocities();

        CHECK(approx_eq(positions[0], Vector3D(35.96, 29.87, 20.63), 1e-3));
        CHECK(approx_eq(velocities[0],Vector3D(3.320, 2.849, -2.494), 1e-3));

        CHECK(approx_eq(positions[1526], Vector3D(29.47, 40.51, 47.43), 1e-3));
        CHECK(approx_eq(velocities[1526],Vector3D(2.073, -0.941, -2.931), 1e-3));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {69.7308, 69.7308, 69.7308}, 1e-5));
    }

    SECTION("Read a specific step") {
        auto file = Trajectory("data/gro/lysozyme.gro");

        auto frame = file.read_step(1);

        CHECK(frame.size() == 1960);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(42.25, 32.32, 22.45), 1e-4));
        CHECK(approx_eq(positions[1526], Vector3D(26.98, 39.97, 46.18), 1e-3));

        frame = file.read_step(0);
        CHECK(frame.size() == 1960);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(42.68, 32.61, 22.84), 1e-3));
        CHECK(approx_eq(positions[1526], Vector3D(27.04, 40.31, 46.51), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {70.1008, 70.1008, 70.1008}, 1e-5));
    }

    SECTION("Read residue information") {
        auto file = Trajectory("data/gro/ubiquitin.gro");
        Frame frame = file.read();

        CHECK(frame.topology().residues().size() == 134);

        REQUIRE(frame.topology().residue_for_atom(1));
        auto residue = *frame.topology().residue_for_atom(1);
        CHECK(residue.size() == 19);
        CHECK(residue.contains(0));
        CHECK(residue.contains(1));
        CHECK(residue.contains(2));
    }
}

TEST_CASE("Write files in GRO format") {
    auto tmpfile = NamedTempPath(".gro");
    const auto EXPECTED_CONTENT =
    "GRO File produced by chemfiles\n"
    "    4\n"
    "    1XXXXX    A    1   0.100   0.200   0.300\n"
    "    2XXXXX    B    2   0.100   0.200   0.300\n"
    "    3XXXXX    C    3   0.100   0.200   0.300\n"
    "    4XXXXX    D    4   0.100   0.200   0.300\n"
    "    2.20000  2.20000  2.20000\n"
    "Second test\n"
    "    7\n"
    "    4XXXXX    A    1   0.100   0.200   0.300  0.0000  0.0000  0.0000\n"
    "    3foo      B    2   0.100   0.200   0.300  0.0000  0.0000  0.0000\n"
    "    3foo      C    3   0.100   0.200   0.300  0.0000  0.0000  0.0000\n"
    "    5barba    D    4   0.100   0.200   0.300  0.0000  0.0000  0.0000\n"
    "    6baz      E    5   0.400   0.500   0.600  0.9000  1.0000  1.1000\n"
    "    7XXXXX    F    6   0.400   0.500   0.600  0.9000  1.0000  1.1000\n"
    "    8XXXXX    G    7   0.400   0.500   0.600  0.9000  1.0000  1.1000\n"
    "    2.20000  1.90526  4.40000 0.0 0.0 -1.10000 0.0  0.00000  0.00000\n";

    auto frame = Frame(UnitCell({22, 22, 22}));
    frame.add_atom(Atom("A"), {1, 2, 3});
    frame.add_atom(Atom("B"), {1, 2, 3});
    frame.add_atom(Atom("C"), {1, 2, 3});
    frame.add_atom(Atom("D"), {1, 2, 3});


    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.set("name", "Second test");
    frame.set_cell(UnitCell({22, 22, 44}, {90, 90, 120}));
    frame.add_velocities();

    frame.add_atom(Atom("E"), {4, 5, 6}, {9, 10, 11});
    frame.add_atom(Atom("F"), {4, 5, 6}, {9, 10, 11});
    frame.add_atom(Atom("G"), {4, 5, 6}, {9, 10, 11});

    Residue residue("foo", 3);
    residue.add_atom(1);
    residue.add_atom(2);
    frame.add_residue(residue);

    residue = Residue("barbar"); // This will be truncated in output
    residue.add_atom(3);
    frame.add_residue(residue);

    residue = Residue("baz", -1);
    residue.add_atom(4);
    frame.add_residue(residue);

    file.write(frame);
    file.close();

    auto check_gro = Trajectory(tmpfile);
    CHECK(check_gro.nsteps() == 2);
    CHECK(check_gro.read().size() == 4);
    CHECK(check_gro.read().size() == 7);
    check_gro.close();

    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == EXPECTED_CONTENT);
}

TEST_CASE("GRO files with big values") {
    SECTION("Unit cell") {
        auto tmpfile = NamedTempPath(".gro");
        auto trajectory = Trajectory(tmpfile, 'w');

        auto frame = Frame();
        frame.resize(1);
        frame.set_cell(UnitCell({1234567890, 1234567890, 1234567890}));
        CHECK_THROWS_WITH(trajectory.write(frame), "value in unit cell is too big for representation in GRO format");

        frame.set_cell(UnitCell({12, 12, 12345678900}, {120, 90, 90}));
        CHECK_THROWS_WITH(trajectory.write(frame), "value in unit cell is too big for representation in GRO format");
    }

    SECTION("Coordinates and velocity") {
        auto tmpfile = NamedTempPath(".gro");
        auto trajectory = Trajectory(tmpfile, 'w');

        auto frame = Frame();
        frame.add_atom(Atom(""), {1234567890, 2, 3});
        CHECK_THROWS_WITH(trajectory.write(frame), "value in atomic position is too big for representation in GRO format");

        frame.resize(0);
        frame.add_velocities();
        frame.add_atom(Atom(""), {1, 2, 3}, {1234567890, 2, 3});
        CHECK_THROWS_WITH(trajectory.write(frame), "value in atomic velocity is too big for representation in GRO format");
    }

    SECTION("Atom counts") {
        if (!is_valgrind_and_travis()) {
            auto tmpfile = NamedTempPath(".gro");

            auto frame = Frame();
            for(size_t i=0; i<100001; i++) {
                frame.add_atom(Atom("A"), {0, 0, 0});
            }
            auto positions = frame.positions();
            positions[9998] = Vector3D(1., 2., 3.);
            positions[99998] = Vector3D(4., 5., 6.);
            positions[99999] = Vector3D(7., 8., 9.);

            Trajectory(tmpfile, 'w').write(frame);

            // Re-read the file we just wrote
            frame = Trajectory(tmpfile, 'r').read();
            positions = frame.positions();

            // If resSeq is has more than 5 characters, coordinates won't be read
            // correctly
            CHECK(approx_eq(positions[9998], Vector3D(1., 2., 3.), 1e-5));
            CHECK(approx_eq(positions[99998],Vector3D(4., 5., 6.), 1e-5));
            CHECK(approx_eq(positions[99999],Vector3D(7., 8., 9.), 1e-5));
        }
    }

    SECTION("User specified residues") {
        if (!is_valgrind_and_travis()) {
            auto tmpfile = NamedTempPath(".gro");

            auto frame = Frame();
            for(size_t i=0; i<100001; i++) {
                frame.add_atom(Atom("A"), {0, 0, 0});
                Residue residue("ANA", static_cast<int64_t>(i + 1));
                residue.add_atom(i);
                frame.add_residue(std::move(residue));
            }
            auto positions = frame.positions();
            positions[9998] = Vector3D(1., 2., 3.);
            positions[99998] = Vector3D(4., 5., 6.);
            positions[99999] = Vector3D(7., 8., 9.);

            Trajectory(tmpfile, 'w').write(frame);

            // Re-read the file we just wrote
            frame = Trajectory(tmpfile, 'r').read();
            positions = frame.positions();

            // If resSeq is has more than 5 characters, coordinates won't be read
            // correctly
            CHECK(approx_eq(positions[9998], Vector3D(1., 2., 3.), 1e-5));
            CHECK(approx_eq(positions[99998],Vector3D(4., 5., 6.), 1e-5));
            CHECK(approx_eq(positions[99999],Vector3D(7., 8., 9.), 1e-5));
        }
    }
}

TEST_CASE("Read and write files in memory") {
    SECTION("Reading from memory") {
        std::ifstream checking("data/gro/ubiquitin.gro");
        std::vector<char> content((std::istreambuf_iterator<char>(checking)),
            std::istreambuf_iterator<char>());

        auto file = Trajectory::memory_reader(content.data(), content.size(), "GRO");
        CHECK(file.nsteps() == 1);

        auto frame = file.read();

        CHECK(frame.size() == 1405);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.93, 24.95, 18.87), 1e-2));
        CHECK(approx_eq(positions[1], Vector3D(25.66, 25.37, 18.33), 1e-2));
        CHECK(approx_eq(positions[678], Vector3D(27.57, 32.25, 37.53), 1e-2));
    }
}
