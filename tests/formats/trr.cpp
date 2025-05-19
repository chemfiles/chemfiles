// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"
using namespace chemfiles;

static void check_ubiquitin(Trajectory& file) {
    CHECK(file.size() == 2);
    auto frame = file.read_at(0);

    CHECK(frame.index() == 0);
    CHECK(frame.get("simulation_step")->as_double() == 0);
    CHECK(approx_eq(frame.get("time")->as_double(), 0));
    CHECK(frame.get("has_positions")->as_bool());
    CHECK(frame.size() == 20455);
    CHECK(frame.velocities());

    auto positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(24.8277, 24.6620, 18.8104), 1e-4));
    CHECK(approx_eq(positions[11], Vector3D(23.7713, 24.5589, 21.4702), 1e-4));

    auto velocities = *frame.velocities();
    CHECK(approx_eq(velocities[100], Vector3D(-2.8750, 2.8159, 1.2047), 1e-4));
    CHECK(approx_eq(velocities[111], Vector3D(-3.0103, 3.3177, -0.8265), 1e-4));

    CHECK(approx_eq(frame[120].get("force")->as_vector3d(), Vector3D(-2.26034, 8.17245, 6.43913),
                    1e-4));
    CHECK(approx_eq(frame[131].get("force")->as_vector3d(),
                    Vector3D(5.63974e1, 5.40824e1, 4.20528e1), 1e-4));

    auto cell = frame.cell();
    CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
    CHECK(approx_eq(cell.lengths(), {55.6800, 58.8700, 62.5700}, 1e-4));

    frame = file.read_at(1);

    CHECK(frame.index() == 1);
    CHECK(frame.get("simulation_step")->as_double() == 25000);
    CHECK(approx_eq(frame.get("time")->as_double(), 50));
    CHECK(frame.get("has_positions")->as_bool());
    CHECK(frame.size() == 20455);
    CHECK(frame.velocities());

    positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(24.8625, 25.0285, 18.5973), 1e-4));
    CHECK(approx_eq(positions[11], Vector3D(23.7971, 24.2192, 21.1569), 1e-4));

    velocities = *frame.velocities();
    CHECK(approx_eq(velocities[100], Vector3D(-5.3413, -1.2646, 1.0216), 1e-4));
    CHECK(approx_eq(velocities[111], Vector3D(-1.7052, 1.0418, 5.3836), 1e-4));

    CHECK(approx_eq(frame[120].get("force")->as_vector3d(), Vector3D(-2.65996, 2.72049, 5.21425),
                    1e-4));
    CHECK(approx_eq(frame[131].get("force")->as_vector3d(),
                    Vector3D(-5.06721e1, 1.10391e2, 4.86442e1), 1e-4));

    cell = frame.cell();
    CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
    CHECK(approx_eq(cell.lengths(), {55.6800, 58.8700, 62.5700}, 1e-4));
}

static void check_different_cell_shapes(Trajectory& file) {
    CHECK(file.size() == 3);

    auto frame = file.read();
    CHECK(frame.size() == 10);
    auto cell = frame.cell();
    CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
    CHECK(approx_eq(cell.lengths(), {11.23, 22.34, 33.45}, 1e-4));
    CHECK(approx_eq(cell.angles(), {90.0, 90.0, 90.0}, 1e-4));
    CHECK(approx_eq(frame.positions()[9], Vector3D(9.0000, 90.0000, 900.0000), 1e-4));

    frame = file.read();
    CHECK(frame.size() == 10);
    cell = frame.cell();
    CHECK(cell.shape() == UnitCell::TRICLINIC);
    CHECK(approx_eq(cell.lengths(), {11.23, 22.34, 33.45}, 1e-4));
    CHECK(approx_eq(cell.angles(), {33.45, 44.56, 55.67}, 1e-4));
    CHECK(approx_eq(frame.positions()[9], Vector3D(9.0000, 90.0000, 900.0000), 1e-4));

    frame = file.read();
    CHECK(frame.size() == 10);
    cell = frame.cell();
    CHECK(cell.shape() == UnitCell::INFINITE);
    CHECK(approx_eq(cell.lengths(), {0.0, 0.0, 0.0}, 1e-4));
    CHECK(approx_eq(cell.angles(), {90.0, 90.0, 90.0}, 1e-4));
    CHECK(approx_eq(frame.positions()[9], Vector3D(9.0000, 90.0000, 900.0000), 1e-4));
}

TEST_CASE("Read files in TRR format") {
    SECTION("Read trajectory: Ubiquitin") {
        auto file = Trajectory("data/trr/ubiquitin.trr");
        check_ubiquitin(file);
    }

    SECTION("Read double precision trajectory: Ubiquitin") {
        auto file = Trajectory("data/trr/ubiquitin_d.trr");
        check_ubiquitin(file);
    }

    SECTION("Read trajectory: Water") {
        auto file = Trajectory("data/trr/water.trr");
        CHECK(file.size() == 100);
        auto frame = file.read();

        CHECK(frame.index() == 0);
        CHECK(frame.get("simulation_step")->as_double() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 297);
        CHECK(!frame.velocities());

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.4172, 8.3034, 11.7372), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(10.4311, 10.1225, 4.1827), 1e-4));

        CHECK(!frame[120].get("force"));
        CHECK(!frame[131].get("force"));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {15.0, 15.0, 15.0}, 1e-4));

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.index() == 2);
        CHECK(frame.get("simulation_step")->as_double() == 2);
        CHECK(approx_eq(frame.get("time")->as_double(), 0.2, 1e-4));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 297);
        CHECK(!frame.velocities());

        positions = frame.positions();
        CHECK(approx_eq(positions[100], Vector3D(11.8583, 12.2360, 1.3922), 1e-4));
        CHECK(approx_eq(positions[111], Vector3D(4.2604, 12.1676, 7.6417), 1e-4));

        CHECK(!frame[120].get("force"));
        CHECK(!frame[131].get("force"));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {15.0, 15.0, 15.0}, 1e-4));

        frame = file.read_at(75); // skip forward

        CHECK(frame.index() == 75);
        CHECK(frame.get("simulation_step")->as_double() == 75);
        CHECK(approx_eq(frame.get("time")->as_double(), 7.5));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 297);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.8856, 8.9006, 11.4770), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(9.5294, 9.4827, 5.2602), 1e-4));

        CHECK(!frame[120].get("force"));
        CHECK(!frame[131].get("force"));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {15.0, 15.0, 15.0}, 1e-4));

        frame = file.read_at(50); // skip behind previous step

        CHECK(frame.index() == 50);
        CHECK(frame.get("simulation_step")->as_double() == 50);
        CHECK(approx_eq(frame.get("time")->as_double(), 5.0));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 297);

        positions = frame.positions();
        CHECK(approx_eq(positions[100], Vector3D(12.9620, 12.5275, 0.6565), 1e-4));
        CHECK(approx_eq(positions[111], Vector3D(4.5618, 12.8612, 8.5790), 1e-4));

        CHECK(!frame[120].get("force"));
        CHECK(!frame[131].get("force"));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {15.0, 15.0, 15.0}, 1e-4));
    }

    SECTION("Read trajectory: 1AKI") {
        auto file = Trajectory("data/trr/1aki.trr");
        CHECK(file.size() == 6);
        auto frame = file.read();

        CHECK(frame.index() == 0);
        CHECK(frame.get("simulation_step")->as_double() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 38376);
        CHECK(frame.velocities());

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(44.2396, 34.0603, 24.6208), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(46.3897, 31.1119, 26.4815), 1e-4));

        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[0], Vector3D(-2.5418, 3.0469, 0.8539), 1e-4));
        CHECK(approx_eq(velocities[11], Vector3D(21.9733, 5.7779, -4.5049), 1e-4));

        CHECK(!frame[120].get("force"));
        CHECK(!frame[131].get("force"));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {73.3925, 73.3925, 73.3925}, 1e-4));

        frame = file.read_at(5); // skip forward

        CHECK(frame.index() == 5);
        CHECK(frame.get("simulation_step")->as_double() == 50);
        CHECK(approx_eq(frame.get("time")->as_double(), 0.1, 1e-4));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 38376);
        CHECK(frame.velocities());

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(44.3276, 33.9384, 24.4364), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(47.0237, 31.4641, 25.9076), 1e-4));

        velocities = *frame.velocities();
        CHECK(approx_eq(velocities[0], Vector3D(-0.5480, 1.1550, 2.5022), 1e-4));
        CHECK(approx_eq(velocities[11], Vector3D(-6.2324, 5.5043, -6.6486), 1e-4));

        CHECK(!frame[120].get("force"));
        CHECK(!frame[131].get("force"));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {73.3925, 73.3925, 73.3925}, 1e-4));
    }

    SECTION("Read different cell shapes") {
        auto file = Trajectory("data/trr/cell_shapes.trr");
        check_different_cell_shapes(file);
    }

    SECTION("Read different cell shapes in double precision") {
        auto file = Trajectory("data/trr/cell_shapes_d.trr");
        check_different_cell_shapes(file);
    }
}

TEST_CASE("Write and append files in TRR format") {
    SECTION("Write and append trajectory") {
        // Write two frames to a file
        auto tmpfile = NamedTempPath(".trr");

        auto frame = Frame(UnitCell({10.111, 11.222, 12.333}));
        frame.add_velocities();
        frame.set("time", 19.376);
        frame.add_atom(Atom("A"), {1.999, 2.888, 3.777}, {3, 2, 1});
        frame.add_atom(Atom("B"), {4, 5, 6}, {9, 8, 7});
        frame.add_atom(Atom("C"), {7, 8, 9}, {6.777, 5.666, 4.555});
        frame[1].set("force", Vector3D(8, 6, 3));

        auto file = Trajectory(tmpfile, 'w');
        file.write(frame);

        frame = Frame(UnitCell({20, 21, 22}, {33.333, 44.444, 55.555}));
        frame.set("simulation_step", 100);
        frame.set("trr_lambda", 0.345);
        frame.add_atom(Atom("A"), {4, 5, 6});
        frame.add_atom(Atom("B"), {7, 8, 9});
        frame.add_atom(Atom("C"), {1, 2, 3});
        file.write(frame);

        frame = Frame();
        frame.add_velocities();
        frame.set("has_positions", false);
        frame.add_atom(Atom("A"), {0, 0, 0}, {-7, 8, 9});
        frame.add_atom(Atom("B"), {0, 0, 0}, {4, 5, 6});
        frame.add_atom(Atom("C"), {0, 0, 0}, {1.222, -2.333, -3.444});
        frame[0].set("force", Vector3D(3, 9, 2));
        frame[1].set("force", Vector3D(8, 5, 8));
        frame[2].set("force", Vector3D(5, 3, 2));
        file.write(frame);

        CHECK(file.size() == 3);
        file.close();

        // now append one frame
        file = Trajectory(tmpfile, 'a');

        frame = Frame(UnitCell({30, 31, 32}));
        frame.set_index(200);
        frame.set("time", 20);
        frame.add_atom(Atom("A"), {7, 8, 9});
        frame.add_atom(Atom("B"), {1, 2, 3});
        frame.add_atom(Atom("C"), {4, 5, 6});
        frame[2].set("force", Vector3D(1.713, 3.577, 9.119));

        file.write(frame);
        CHECK(file.size() == 4);
        file.close();

        // now read every thing back and check
        file = Trajectory(tmpfile, 'r');

        frame = file.read();

        CHECK(frame.index() == 0);
        CHECK(frame.get("simulation_step")->as_double() == 0); // default step
        CHECK(approx_eq(frame.get("time")->as_double(), 19.376, 1e-4));
        CHECK(approx_eq(frame.get("trr_lambda")->as_double(), 0)); // default lambda
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 3);
        CHECK(frame.velocities());

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.999, 2.888, 3.777), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(7, 8, 9), 1e-4));

        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[0], Vector3D(3, 2, 1), 1e-4));
        CHECK(approx_eq(velocities[2], Vector3D(6.777, 5.666, 4.555), 1e-4));

        CHECK(approx_eq(frame[0].get("force")->as_vector3d(), Vector3D(0, 0, 0), 1e-4));
        CHECK(approx_eq(frame[1].get("force")->as_vector3d(), Vector3D(8, 6, 3), 1e-4));
        CHECK(approx_eq(frame[2].get("force")->as_vector3d(), Vector3D(0, 0, 0), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {10.111, 11.222, 12.333}, 1e-4));

        frame = file.read();

        CHECK(frame.index() == 1);
        CHECK(frame.get("simulation_step")->as_double() == 100);
        CHECK(approx_eq(frame.get("time")->as_double(), 0)); // default time
        CHECK(approx_eq(frame.get("trr_lambda")->as_double(), 0.345, 1e-4));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 3);
        CHECK(!frame.velocities());

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(4, 5, 6), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(1, 2, 3), 1e-4));

        CHECK(!frame[0].get("force"));
        CHECK(!frame[1].get("force"));
        CHECK(!frame[2].get("force"));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::TRICLINIC);
        CHECK(approx_eq(cell.lengths(), {20, 21, 22}, 1e-4));
        CHECK(approx_eq(cell.angles(), {33.333, 44.444, 55.555}, 1e-4));

        frame = file.read();

        CHECK(frame.index() == 2);
        CHECK(frame.get("simulation_step")->as_double() == 0);     // default step
        CHECK(approx_eq(frame.get("time")->as_double(), 0));       // default time
        CHECK(approx_eq(frame.get("trr_lambda")->as_double(), 0)); // default lambda
        CHECK(!frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 3);
        CHECK(frame.velocities());

        velocities = *frame.velocities();
        CHECK(approx_eq(velocities[0], Vector3D(-7, 8, 9), 1e-4));
        CHECK(approx_eq(velocities[2], Vector3D(1.222, -2.333, -3.444), 1e-4));

        CHECK(approx_eq(frame[0].get("force")->as_vector3d(), Vector3D(3, 9, 2), 1e-4));
        CHECK(approx_eq(frame[1].get("force")->as_vector3d(), Vector3D(8, 5, 8), 1e-4));
        CHECK(approx_eq(frame[2].get("force")->as_vector3d(), Vector3D(5, 3, 2), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::INFINITE);

        frame = file.read();

        CHECK(frame.index() == 3);
        CHECK(frame.get("simulation_step")->as_double() == 200);
        CHECK(approx_eq(frame.get("time")->as_double(), 20));
        CHECK(approx_eq(frame.get("trr_lambda")->as_double(), 0)); // default lambda
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 3);
        CHECK(!frame.velocities());

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(7, 8, 9), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(4, 5, 6), 1e-4));

        CHECK(approx_eq(frame[0].get("force")->as_vector3d(), Vector3D(0, 0, 0), 1e-4));
        CHECK(approx_eq(frame[1].get("force")->as_vector3d(), Vector3D(0, 0, 0), 1e-4));
        CHECK(approx_eq(frame[2].get("force")->as_vector3d(), Vector3D(1.713, 3.577, 9.119), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {30, 31, 32}, 1e-4));
    }

    SECTION("Append on new trajectory") {
        auto tmpfile = NamedTempPath(".trr");

        auto frame = Frame(UnitCell({10.111, 11.222, 12.333}));
        frame.add_velocities();
        frame.set("time", 19.376);
        frame.set("trr_lambda", 0.753);
        frame.add_atom(Atom("A"), {1.999, 2.888, 3.777}, {0, -1, -2});
        frame.add_atom(Atom("B"), {4, 5, 6}, {1, 3, 5});
        frame.add_atom(Atom("C"), {7, 8, 9}, {0.001, 1.002, -2.333});

        auto file = Trajectory(tmpfile, 'a');
        file.write(frame);
        CHECK(file.size() == 1);
        file.close();

        // now read every thing back and check
        file = Trajectory(tmpfile, 'r');

        frame = file.read();

        CHECK(frame.index() == 0);
        CHECK(frame.get("simulation_step")->as_double() == 0); // default step
        CHECK(approx_eq(frame.get("time")->as_double(), 19.376, 1e-4));
        CHECK(approx_eq(frame.get("trr_lambda")->as_double(), 0.753, 1e-4));
        CHECK(frame.get("has_positions")->as_bool());
        CHECK(frame.size() == 3);
        CHECK(frame.velocities());

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.999, 2.888, 3.777), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(7, 8, 9), 1e-4));

        auto velocities = *frame.velocities();
        CHECK(approx_eq(velocities[0], Vector3D(0, -1, -2), 1e-4));
        CHECK(approx_eq(velocities[2], Vector3D(0.001, 1.002, -2.333), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {10.111, 11.222, 12.333}, 1e-4));
    }
}

TEST_CASE("Check Errors") {
    auto tmpfile = NamedTempPath(".trr");
    auto file = Trajectory(tmpfile, 'w');

    auto frame = Frame();
    frame.add_atom(Atom("A"), {1, 2, 3});
    file.write(frame);

    frame = Frame();
    frame.add_atom(Atom("A"), {1, 2, 3});
    frame.add_atom(Atom("B"), {4, 5, 6});
    CHECK_THROWS_WITH(
        file.write(frame),
        "TRR format does not support varying numbers of atoms: expected 1, but got 2");
    file.close();

    file = Trajectory(tmpfile, 'a');
    frame = Frame();
    frame.add_atom(Atom("A"), {1, 2, 3});
    frame.add_atom(Atom("B"), {4, 5, 6});
    CHECK_THROWS_WITH(
        file.write(frame),
        "TRR format does not support varying numbers of atoms: expected 1, but got 2");
}
