// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"
#include <fstream>
using namespace chemfiles;

// {wrapped, scaled_wrapped, unwrapped, scaled_unwrapped}.lammpstrj
// are based on the same simulation and contain therefore the same unwrapped positions
static void check_pos_representation(Trajectory& file) {
    CHECK(file.nsteps() == 11);

    Frame frame = file.read();
    CHECK(frame.size() == 7751);

    CHECK(frame.cell().shape() == UnitCell::ORTHORHOMBIC);
    CHECK(approx_eq(frame.cell().lengths(), Vector3D(35.7, 35.7, 92.82), 1e-2));
    CHECK(approx_eq(frame.cell().angles(), Vector3D(90.0, 90.0, 90.0), 1e-2));

    auto positions = frame.positions();
    CHECK(approx_eq(positions[5000], Vector3D(12.2614, 7.76219, -13.0444), 1e-3));
    CHECK(approx_eq(positions[7000], Vector3D(15.7755, 15.7059, 20.9502), 1e-3));

    auto velocities = *frame.velocities();
    CHECK(approx_eq(velocities[5000], Vector3D(-0.000273223, 0.000143908, -0.000557713), 1e-7));
    CHECK(approx_eq(velocities[7000], Vector3D(-0.000466344, 0.000701151, 0.000430329), 1e-7));

    CHECK(approx_eq(frame[5000].charge(), 0.5564));
    CHECK(frame[5000].type() == "2");
    CHECK(frame[5000].name() == "C");

    frame = file.read_step(5);
    CHECK(frame.size() == 7751);

    positions = frame.positions();
    CHECK(approx_eq(positions[5000], Vector3D(4.33048, 4.23699, -2.29954), 1e-3));
    CHECK(approx_eq(positions[7000], Vector3D(15.9819, 21.1517, 8.12739), 1e-3));

    velocities = *frame.velocities();
    CHECK(approx_eq(velocities[5000], Vector3D(-0.00404259, -0.000939097, 0.0152453), 1e-7));
    CHECK(approx_eq(velocities[7000], Vector3D(0.00122365, 0.0100476, -0.0167459), 1e-7));

    CHECK_THROWS_AS(file.read_step(11), FileError);
}

TEST_CASE("Read files in LAMMPS Atom format") {
    SECTION("Polymer") {
        auto file = Trajectory("data/lammps/polymer.lammpstrj");
        Frame frame = file.read();
        double eps = 1e-3;

        CHECK(frame.size() == 1714);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(51.8474, 100.348, 116.516), eps));
        // this one has a non zero image index (1 0 0)
        CHECK(approx_eq(positions[1189], Vector3D(116.829, 91.2404, 79.8858), eps));
        // this one has a non zero image index (2 1 -3)
        CHECK(approx_eq(positions[1327], Vector3D(173.311, 87.853, 109.417), eps));
    }

    SECTION("NaCl") {
        auto file = Trajectory("data/lammps/nacl.lammpstrj");
        Frame frame = file.read();

        CHECK(frame.size() == 512);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0, 0.0, 0.0), 1e-3));
        CHECK(approx_eq(positions[222], Vector3D(14.1005, 0.0, 8.4603), 1e-3));

        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[0], Vector3D(-0.00258494, 0.00270859, -0.00314039), 1e-7));
        CHECK(approx_eq(velocities[222], Vector3D(-0.00466812, -0.00196397, -0.000147051), 1e-7));

        frame = file.read_step(5);
        CHECK(frame.size() == 512);
        positions = frame.positions();

        CHECK(approx_eq(positions[0], Vector3D(0.095924, -0.0222584, -0.0152489), 1e-3));
        CHECK(approx_eq(positions[222], Vector3D(14.0788, 0.0954186, 8.56453), 1e-3));

        frame = file.read_step(0); // read a previous step
        CHECK(frame.size() == 512);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0, 0.0, 0.0), 1e-3));
        CHECK(approx_eq(positions[222], Vector3D(14.1005, 0.0, 8.4603), 1e-3));

        CHECK_THROWS_AS(file.read_step(6), FileError);
    }

    SECTION("Wrapped Coordinates") {
        auto file = Trajectory("data/lammps/wrapped.lammpstrj");
        check_pos_representation(file);
    }

    SECTION("Scaled Wrapped Coordinates") {
        auto file = Trajectory("data/lammps/scaled_wrapped.lammpstrj");
        check_pos_representation(file);
    }

    SECTION("Unrapped Coordinates") {
        auto file = Trajectory("data/lammps/unwrapped.lammpstrj");
        check_pos_representation(file);
    }

    SECTION("Scaled Unrapped Coordinates") {
        auto file = Trajectory("data/lammps/scaled_unwrapped.lammpstrj");
        check_pos_representation(file);
    }

    SECTION("Position Representation") {
        auto file = Trajectory("data/lammps/detect_best_pos_repr.lammpstrj");
        CHECK(file.nsteps() == 5);

        Frame frame = file.read();
        CHECK(frame.size() == 854);
        CHECK(frame.step() == 100000);
        CHECK(approx_eq((*frame.get("time")).as_double(), 25e9, 1e-6));
        auto positions = frame.positions();

        CHECK(approx_eq(positions[679], Vector3D(1.47679, -25.2886, 2.38234), 1e-3));
        CHECK(approx_eq(positions[764], Vector3D(-256.58, 117.368, 1.9654), 1e-3));

        frame = file.read();
        CHECK(frame.size() == 854);
        CHECK(frame.step() == 101000);
        CHECK(!frame.get("time"));
        positions = frame.positions();

        CHECK(frame.cell().shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(frame.cell().lengths(), Vector3D(60.0, 60.0, 250.0), 1e-2));
        CHECK(approx_eq(frame.cell().angles(), Vector3D(90.0, 90.0, 90.0), 1e-2));

        CHECK(approx_eq(positions[683], Vector3D(-43.3683, 322.948, 208.063), 1e-3));
        CHECK(approx_eq(positions[828], Vector3D(150.083, -135.113, 189.641), 1e-3));

        frame = file.read();
        CHECK(frame.size() == 856);
        CHECK(frame.step() == 102000);

        positions = frame.positions();
        CHECK(approx_eq(positions[747], Vector3D(-158.317, 142.593, 2.11392), 1e-3));
        CHECK(approx_eq(positions[799], Vector3D(224.784, -167.878, 39.3765), 1e-3));

        frame = file.read();
        CHECK(frame.size() == 856);
        CHECK(frame.step() == 103000);

        positions = frame.positions();
        CHECK(approx_eq(positions[735], Vector3D(67.2657, 30.0627, 2.1141), 1e-3));
        CHECK(approx_eq(positions[775], Vector3D(125.347, -82.3507, 46.611), 1e-3));

        frame = file.read();
        CHECK(frame.size() == 856);
        CHECK(frame.step() == 104000);

        positions = frame.positions();
        CHECK(approx_eq(positions[652], Vector3D(-188.131, 96.0777, 196.23), 1e-3));
        CHECK(approx_eq(positions[838], Vector3D(-33.6068, -50.5113, 209.306), 1e-3));

        CHECK_THROWS_AS(file.read(), FileError);
    }

    SECTION("Atom Properties") {
        auto file = Trajectory("data/lammps/properties.lammpstrj");
        CHECK(file.nsteps() == 4);

        Frame frame = file.read();
        CHECK(frame.size() == 4000);
        CHECK(frame.step() == 0);
        CHECK(!frame.velocities());
        auto positions = frame.positions();

        CHECK(approx_eq(positions[390], Vector3D(10.4004,12.4805, 0.693361), 1e-3));
        CHECK(approx_eq(positions[789], Vector3D(10.4004,13.1739, 1.38672), 1e-3));

        CHECK(approx_eq(frame[390].get("c_stress[6]")->as_double(), -1.38816, 1e-3));
        CHECK(approx_eq(frame[390].get("v_sq_pos")->as_double(), 264.412, 1e-3));
        CHECK(approx_eq(frame[390].get("i_flag")->as_double(), 1.0, 1e-12));
        CHECK(approx_eq(frame[789].get("c_stress[1]")->as_double(), -59.7086, 1e-3));
        CHECK(approx_eq(frame[789].get("v_sq_pos")->as_double(), 283.642, 1e-3));
        CHECK(approx_eq(frame[789].get("i_flag")->as_double(), 0.0, 1e-12));

        frame = file.read_step(3);
        CHECK(frame.size() == 4000);
        CHECK(frame.step() == 300);
        CHECK(!frame.velocities());
        positions = frame.positions();

        CHECK(approx_eq(positions[2988], Vector3D(9.71147, 5.5884, 9.71147), 1e-3));
        CHECK(approx_eq(positions[3905], Vector3D(9.01993, 10.4242, 12.4797), 1e-3));

        CHECK(approx_eq(frame[2988].get("c_stress[5]")->as_double(), 12.9949, 1e-3));
        CHECK(approx_eq(frame[2988].get("v_sq_pos")->as_double(), 219.855, 1e-3));
        CHECK(approx_eq(frame[2988].get("i_flag")->as_double(), 1.0, 1e-12));
        CHECK(approx_eq(frame[3905].get("c_stress[2]")->as_double(), -67.6015, 1e-3));
        CHECK(approx_eq(frame[3905].get("v_sq_pos")->as_double(), 345.766, 1e-3));
        CHECK(approx_eq(frame[3905].get("i_flag")->as_double(), 0.0, 1e-12));

    }

    SECTION("Errors") {
        // ITEM: TIMESTEP issues
        auto file = Trajectory("data/lammps/bad/timestep-no-item.lammpstrj");
        CHECK_THROWS_WITH(file.read(), "can not read next step as LAMMPS format: expected an ITEM entry");

        file = Trajectory("data/lammps/bad/timestep-item-name.lammpstrj");
        CHECK_THROWS_WITH(file.read(), "can not read next step as LAMMPS format: expected 'TIMESTEP' got 'DUMMY'");

        // ITEM: BOX BOUNDS issues
        CHECK_THROWS_WITH(
            Trajectory("data/lammps/bad/box-not-item.lammpstrj"),
            "expected 'ITEM: BOX BOUNDS' after the number of atoms in LAMMPS trajectory, got 'DUMMY'"
        );
        CHECK_THROWS_WITH(
            Trajectory("data/lammps/bad/box-item-name.lammpstrj"),
            "expected 'ITEM: BOX BOUNDS' after the number of atoms in LAMMPS trajectory, got 'ITEM: DUMMY'"
        );

        file = Trajectory("data/lammps/bad/box-not-numbers.lammpstrj");
        CHECK_THROWS_WITH(
            file.read_step(0),
            "can not read box header in LAMMPS format: incomplete box dimensions in LAMMPS format, expected 2 but got 1"
        );
        CHECK_THROWS_WITH(
            file.read_step(1),
            "can not read box header in LAMMPS format: incomplete box dimensions in LAMMPS format, expected 2 but got 1"
        );
        CHECK_THROWS_WITH(
            file.read_step(2),
            "can not read box header in LAMMPS format: incomplete box dimensions in LAMMPS format, expected 2 but got 1"
        );

        file = Trajectory("data/lammps/bad/box-wrong-size.lammpstrj");
        CHECK_THROWS_WITH(
            file.read_step(0),
            "can not read box header in LAMMPS format: incomplete box dimensions in LAMMPS format, expected 3 but got 2"
        );
        CHECK_THROWS_WITH(
            file.read_step(1),
            "can not read box header in LAMMPS format: incomplete box dimensions in LAMMPS format, expected 3 but got 2"
        );
        CHECK_THROWS_WITH(
            file.read_step(2),
            "can not read box header in LAMMPS format: incomplete box dimensions in LAMMPS format, expected 3 but got 2"
        );

        // ITEM: ATOMS issues
        CHECK_THROWS_WITH(
            Trajectory("data/lammps/bad/atom-no-item.lammpstrj"),
            "could not read atom header for LAMMPS trajectory in this line: 'DUMMY'"
        );

        CHECK_THROWS_WITH(
            Trajectory("data/lammps/bad/atom-item-name.lammpstrj"),
            "could not read atom header for LAMMPS trajectory in this line: 'ITEM: DUMMY'"
        );

        file = Trajectory("data/lammps/bad/atom-too-many-fields.lammpstrj");
        CHECK_THROWS_WITH(
            file.read(),
            "LAMMPS atom line has wrong number of fields: expected 5 got 6"
        );

        file = Trajectory("data/lammps/bad/atom-duplicated-id.lammpstrj");
        CHECK_THROWS_WITH(
            file.read(),
            "found atoms with the same ID in LAMMPS format: 2 is already present"
        );

        CHECK_THROWS_WITH(
            Trajectory("data/lammps/bad/items-after-atoms.lammpstrj"),
            "could not find 'ITEM: NUMBER OF ATOMS' in LAMMPS trajectory"
        );
    }
}

TEST_CASE("Write files in LAMMPS Atom format") {
    SECTION("Single frame") {
        auto tmpfile = NamedTempPath(".lammpstrj");
        const auto EXPECTED_CONTENT = "ITEM: UNITS\n"
                                      "real\n"
                                      "ITEM: TIMESTEP\n"
                                      "0\n"
                                      "ITEM: NUMBER OF ATOMS\n"
                                      "2\n"
                                      "ITEM: BOX BOUNDS pp pp pp\n"
                                      "0.0000000000000000e+00 0.0000000000000000e+00\n"
                                      "0.0000000000000000e+00 0.0000000000000000e+00\n"
                                      "0.0000000000000000e+00 0.0000000000000000e+00\n"
                                      "ITEM: ATOMS id xu yu zu type element mass q\n"
                                      "1 1 2 3 1 A 0 0\n"
                                      "2 1 2 3 2 B 10.81 0\n";

        auto frame = Frame(UnitCell());
        frame.add_atom(Atom("A"), {1, 2, 3});
        frame.add_atom(Atom("B"), {1, 2, 3});
        auto file = Trajectory(tmpfile, 'w');
        file.write(frame);
        file.close();

        auto check_traj = Trajectory(tmpfile);
        CHECK(check_traj.nsteps() == 1);
        frame = check_traj.read();
        CHECK(frame.size() == 2);
        CHECK(approx_eq(frame.positions()[1], Vector3D(1.0, 2.0, 3.0), 1e-3));
        CHECK(frame[0].type() == "1"); // generated type should start at 1
        CHECK(frame[1].type() == "2");
        CHECK(frame.cell().shape() == UnitCell::INFINITE);
        check_traj.close();

        std::ifstream checking(tmpfile);
        std::string content((std::istreambuf_iterator<char>(checking)),
                            std::istreambuf_iterator<char>());
        CHECK(content == EXPECTED_CONTENT);
    }

    SECTION("Multiple frames") {
        auto tmpfile = NamedTempPath(".lammpstrj");
        const auto EXPECTED_CONTENT =
            "ITEM: UNITS\n"
            "real\n"
            "ITEM: TIMESTEP\n"
            "0\n"
            "ITEM: NUMBER OF ATOMS\n"
            "4\n"
            "ITEM: BOX BOUNDS pp pp pp\n"
            "0.0000000000000000e+00 2.2000000000000000e+01\n"
            "0.0000000000000000e+00 2.2000000000000000e+01\n"
            "0.0000000000000000e+00 2.2000000000000000e+01\n"
            "ITEM: ATOMS id xu yu zu type element mass q\n"
            "1 1 2 3 1 A 0 0\n"
            "2 1 2 3 2 B 10.81 0\n"
            "3 1 2 3 10 C 12.011 0\n"
            "4 1 2 3 11 D 0 0\n"
            "ITEM: UNITS\n"
            "cgs\n"
            "ITEM: TIME\n"
            "235.67\n"
            "ITEM: TIMESTEP\n"
            "0\n"
            "ITEM: NUMBER OF ATOMS\n"
            "7\n"
            "ITEM: BOX BOUNDS xy xz yz pp pp pp\n"
            "0.0000000000000000e+00 2.2000000000000000e+01 -1.0999999999999995e+01\n"
            "0.0000000000000000e+00 1.9052558883257653e+01 -2.1999999999999989e+01\n"
            "0.0000000000000000e+00 3.0209918110637730e+01 2.3224143638648190e+01\n"
            "ITEM: ATOMS id xu yu zu type element mass q vx vy vz\n"
            "1 1 2 3 1 A 0 -1 0 0 0\n"
            "2 1 2 3 2 B 10.81 2.5 0 0 0\n"
            "3 1 2 3 12 C 12.011 0 0 0 0\n"
            "4 1 2 3 11 D 0 0 0 0 0\n"
            "5 4 5 6 13 E 0 0 9 10 11\n"
            "6 4 5 6 14 F 18.9984 0 9 10 11\n"
            "7 4 5 6 15 G 0 0 9 10 11\n";

        auto frame = Frame(UnitCell({22, 22, 22}));
        frame.add_atom(Atom("A"), {1, 2, 3});
        frame.add_atom(Atom("B"), {1, 2, 3});
        frame.add_atom(Atom("C"), {1, 2, 3});
        frame.add_atom(Atom("D"), {1, 2, 3});
        // set the first types to valid LAMMPS types
        frame[0].set_type("1");
        frame[1].set_type("2");
        frame[2].set_type("10");

        auto file = Trajectory(tmpfile, 'w');
        file.write(frame);

        frame[2].set_type(""); // empty type

        frame.set("lammps_units", "cgs");
        frame.set("time", 235.67);
        frame.set_cell(UnitCell({22, 22, 44}, {45, 120, 120}));
        frame.add_velocities();

        frame.add_atom(Atom("E"), {4, 5, 6}, {9, 10, 11});
        frame.add_atom(Atom("F"), {4, 5, 6}, {9, 10, 11});
        frame.add_atom(Atom("G"), {4, 5, 6}, {9, 10, 11});

        frame[0].set_charge(-1.0);
        frame[1].set_charge(2.5);

        file.write(frame);
        file.close();

        auto check_traj = Trajectory(tmpfile);
        CHECK(check_traj.nsteps() == 2);
        CHECK(check_traj.read().size() == 4);
        frame = check_traj.read();
        CHECK(frame.size() == 7);
        CHECK(approx_eq(frame.positions()[0], Vector3D(1.0, 2.0, 3.0), 1e-3));
        CHECK(frame.cell().shape() == UnitCell::TRICLINIC);
        CHECK(approx_eq(frame.cell().lengths(), Vector3D(22.0, 22.0, 44.0), 1e-2));
        CHECK(approx_eq(frame.cell().angles(), Vector3D(45.0, 120.0, 120.0), 1e-2));
        check_traj.close();

        std::ifstream checking(tmpfile);
        std::string content((std::istreambuf_iterator<char>(checking)),
                            std::istreambuf_iterator<char>());
        CHECK(content == EXPECTED_CONTENT);
    }
}

TEST_CASE("Read files in memory") {
    SECTION("Reading from memory") {
        std::string content(R"(ITEM: TIMESTEP
0
ITEM: NUMBER OF ATOMS
2
ITEM: BOX BOUNDS pp pp pp
0.0000000000000000e+00 2.0000000000000000e+01
0.0000000000000000e+00 3.0000000000000000e+01
0.0000000000000000e+00 4.0000000000000000e+01
ITEM: ATOMS id type x y z
1 1 5 5 5
2 5 6.5 6.5 6.5
)");

        auto file = Trajectory::memory_reader(content.data(), content.size(), "LAMMPS");
        CHECK(file.nsteps() == 1);

        auto frame = file.read();
        CHECK(frame.size() == 2);
        CHECK(frame.cell().shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(frame.cell().lengths(), Vector3D(20.0, 30.0, 40.0), 1e-2));
        CHECK(approx_eq(frame.cell().angles(), Vector3D(90.0, 90.0, 90.0), 1e-2));
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(5.0, 5.0, 5.0), 1e-2));
        CHECK(approx_eq(positions[1], Vector3D(6.5, 6.5, 6.5), 1e-2));
        CHECK(frame[0].type() == "1");
        CHECK(frame[0].name() == "");
        CHECK(frame[1].type() == "5");
    }

    SECTION("Frame properties") {
        std::string content(R"(ITEM: UNITS
lj
ITEM: TIME
250.5
ITEM: TIMESTEP
5
ITEM: NUMBER OF ATOMS
0
ITEM: BOX BOUNDS pp pp pp
0.0000000000000000e+00 1.0000000000000000e+01
0.0000000000000000e+00 1.0000000000000000e+01
0.0000000000000000e+00 1.0000000000000000e+01
ITEM: ATOMS id type x y z
ITEM: UNITS
metal
ITEM: TIMESTEP
15
ITEM: NUMBER OF ATOMS
3
ITEM: BOX BOUNDS pp pp pp
0.0000000000000000e+00 1.0000000000000000e+01
0.0000000000000000e+00 1.0000000000000000e+01
0.0000000000000000e+00 1.0000000000000000e+01
ITEM: ATOMS id type x y z
1 1 5 5 5
2 1 5 5 5
3 1 5 5 5
ITEM: TIME
335.678
ITEM: TIMESTEP
20
ITEM: NUMBER OF ATOMS
0
ITEM: BOX BOUNDS pp pp pp
0.0000000000000000e+00 1.0000000000000000e+01
0.0000000000000000e+00 1.0000000000000000e+01
0.0000000000000000e+00 1.0000000000000000e+01
ITEM: ATOMS id type x y z
)");

        auto file = Trajectory::memory_reader(content.data(), content.size(), "LAMMPS");
        CHECK(file.nsteps() == 3);

        auto frame = file.read();
        CHECK(frame.size() == 0);
        CHECK(*frame.get("lammps_units") == "lj");
        CHECK(approx_eq((*frame.get("time")).as_double(), 250.5, 1e-6));
        CHECK(frame.step() == 5);

        frame = file.read();
        CHECK(frame.size() == 3);
        CHECK(*frame.get("lammps_units") == "metal");
        CHECK(!frame.get("time"));
        CHECK(frame.step() == 15);

        frame = file.read();
        CHECK(frame.size() == 0);
        CHECK(!frame.get("lammps_units"));
        CHECK(approx_eq((*frame.get("time")).as_double(), 335.678, 1e-6));
        CHECK(frame.step() == 20);
    }

    SECTION("Atom properties") {
        std::string content(R"(ITEM: TIMESTEP
7
ITEM: NUMBER OF ATOMS
2
ITEM: BOX BOUNDS pp pp pp
-1.5000000000000000e+00 2.0000000000000000e+01
-2.6000000000000000e+00 3.0000000000000000e+01
-3.7000000000000000e+00 4.0000000000000000e+01
ITEM: ATOMS type element z mass y x vy vz q id
32 Ge -1.234 72.6 50.432 1.555 -2.345 6.456 2.5 2
87 Fr 7 223.0 6 5 8 9 -1 1
)"); // column order very messed up

        auto file = Trajectory::memory_reader(content.data(), content.size(), "LAMMPS");
        CHECK(file.nsteps() == 1);

        auto frame = file.read();
        CHECK(frame.size() == 2);
        CHECK(approx_eq(frame.cell().lengths(), Vector3D(21.5, 32.6, 43.7), 1e-2));
        CHECK(approx_eq(frame.cell().angles(), Vector3D(90.0, 90.0, 90.0), 1e-2));
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(5.0, 6.0, 7.0), 1e-2));
        CHECK(approx_eq(positions[1], Vector3D(1.555, 50.432, -1.234), 1e-2));
        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[0], Vector3D(0.0, 8.0, 9.0), 1e-6));
        CHECK(approx_eq(velocities[1], Vector3D(0.0, -2.345, 6.456), 1e-6));
        CHECK(frame.step() == 7);
        CHECK(frame[0].type() == "87");
        CHECK(frame[1].type() == "32");
        CHECK(frame[0].name() == "Fr");
        CHECK(frame[1].name() == "Ge");
        CHECK(approx_eq(frame[0].mass(), 223.0, 1e-6));
        CHECK(approx_eq(frame[1].mass(), 72.6, 1e-6));
        CHECK(approx_eq(frame[0].charge(), -1, 1e-6));
        CHECK(approx_eq(frame[1].charge(), 2.5, 1e-6));
    }

    SECTION("Best position representation") {
        std::string content(R"(ITEM: TIMESTEP
0
ITEM: NUMBER OF ATOMS
1
ITEM: BOX BOUNDS pp pp pp
0.0000000000000000e+00 2.0000000000000000e+01
0.0000000000000000e+00 3.0000000000000000e+01
0.0000000000000000e+00 4.0000000000000000e+01
ITEM: ATOMS id type y z xs ys zs
1 1 -1 -1 0.5 0.5 0.5
ITEM: TIMESTEP
1
ITEM: NUMBER OF ATOMS
1
ITEM: BOX BOUNDS pp pp pp
0.0000000000000000e+00 2.0000000000000000e+01
0.0000000000000000e+00 3.0000000000000000e+01
0.0000000000000000e+00 4.0000000000000000e+01
ITEM: ATOMS id type x y z xu yu zu xus yus zus
1 1 -1 -1 -1 150.5 160.6 170.7 -1 -1 -1
ITEM: TIMESTEP
2
ITEM: NUMBER OF ATOMS
1
ITEM: BOX BOUNDS pp pp pp
0.0000000000000000e+00 2.0000000000000000e+01
0.0000000000000000e+00 3.0000000000000000e+01
0.0000000000000000e+00 4.0000000000000000e+01
ITEM: ATOMS id type
1 1
)");

        auto file = Trajectory::memory_reader(content.data(), content.size(), "LAMMPS");
        CHECK(file.nsteps() == 3);

        auto frame = file.read();
        CHECK(frame.size() == 1);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(10.0, 15.0, 20.0), 1e-2));

        frame = file.read();
        CHECK(frame.size() == 1);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(150.5, 160.6, 170.7), 1e-2));

        frame = file.read();
        CHECK(frame.size() == 1);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0, 0.0, 0.0), 1e-2));
    }

    SECTION("Triclinic boxes") {
        std::string content(R"(ITEM: TIMESTEP
0
ITEM: NUMBER OF ATOMS
1
ITEM: BOX BOUNDS pp pp pp xy xz yz
-4.0000000000000000e+00 6.0000000000000000e+00 5.0000000000000000e+00
0.0000000000000000e+00 2.0000000000000000e+01 4.0000000000000000e+00
-1.0000000000000000e+00 1.0000000000000000e+01 3.5000000000000000e+00
ITEM: ATOMS id type x y z
1 1 5 5 5
ITEM: TIMESTEP
1
ITEM: NUMBER OF ATOMS
1
ITEM: BOX BOUNDS xy xz yz pp pp pp
-4.0000000000000000e+00 6.0000000000000000e+00 5.0000000000000000e+00
0.0000000000000000e+00 2.0000000000000000e+01 4.0000000000000000e+00
-1.0000000000000000e+00 1.0000000000000000e+01 3.5000000000000000e+00
ITEM: ATOMS id type xs ys zs ix iy iz
1 1 0.604545 0.154545 0.545455 3 1 1
)"); // in older LAMMPS versions (pre Apr 2011 [f7ce527]) the boundary flags come before 'xy xz yz'

        auto file = Trajectory::memory_reader(content.data(), content.size(), "LAMMPS");
        CHECK(file.nsteps() == 2);

        auto frame = file.read();
        CHECK(frame.size() == 1);
        CHECK(frame.cell().shape() == UnitCell::TRICLINIC);
        CHECK(approx_eq(frame.cell().lengths(), Vector3D(10.0, 20.616, 12.217), 1e-3));
        CHECK(approx_eq(frame.cell().angles(), Vector3D(69.063, 70.888, 75.964), 1e-3));
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(5.0, 5.0, 5.0), 1e-2));

        frame = file.read();
        CHECK(frame.size() == 1);
        CHECK(frame.cell().shape() == UnitCell::TRICLINIC);
        CHECK(approx_eq(frame.cell().lengths(), Vector3D(10.0, 20.616, 12.217), 1e-3));
        CHECK(approx_eq(frame.cell().angles(), Vector3D(69.063, 70.888, 75.964), 1e-3));
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(44.0, 28.5, 16.0), 1e-3));
    }
}
