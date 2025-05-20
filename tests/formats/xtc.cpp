// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"
using namespace chemfiles;

static void check_traj_ubiquitin(const char* path) {
    auto file = Trajectory(path);
    CHECK(file.size() == 251);
    auto frame = file.read_at(0);

    CHECK(frame.index() == 0);
    CHECK(frame.get("simulation_step")->as_double() == 0);
    CHECK(approx_eq(frame.get("time")->as_double(), 0));
    CHECK(frame.get("xtc_precision")->as_double() == 1000);
    CHECK(frame.size() == 20455);

    auto positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(24.8300, 24.6600, 18.8100), 1e-4));
    CHECK(approx_eq(positions[11], Vector3D(23.7700, 24.5600, 21.4700), 1e-4));

    auto cell = frame.cell();
    CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
    CHECK(approx_eq(cell.lengths(), {55.6800, 58.8700, 62.5700}, 1e-4));

    frame = file.read_at(1); // Skip a frame
    CHECK(frame.index() == 1);
    CHECK(frame.get("simulation_step")->as_double() == 100);

    frame = file.read();

    CHECK(frame.index() == 2);
    CHECK(frame.get("simulation_step")->as_double() == 200);
    CHECK(approx_eq(frame.get("time")->as_double(), 0.4, 1e-4));
    CHECK(frame.get("xtc_precision")->as_double() == 1000);
    CHECK(frame.size() == 20455);

    positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(24.7100, 24.5700, 18.4500), 1e-4));
    CHECK(approx_eq(positions[11], Vector3D(23.6700, 24.4800, 21.5200), 1e-4));

    cell = frame.cell();
    CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
    CHECK(approx_eq(cell.lengths(), {55.6800, 58.8700, 62.5700}, 1e-4));

    frame = file.read_at(230); // skip forward

    CHECK(frame.index() == 230);
    CHECK(frame.get("simulation_step")->as_double() == 23000);
    CHECK(approx_eq(frame.get("time")->as_double(), 46.0));
    CHECK(frame.get("xtc_precision")->as_double() == 1000);
    CHECK(frame.size() == 20455);

    positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(24.6300, 24.6700, 18.5000), 1e-4));
    CHECK(approx_eq(positions[11], Vector3D(23.6800, 24.0700, 21.3100), 1e-4));

    cell = frame.cell();
    CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
    CHECK(approx_eq(cell.lengths(), {55.6800, 58.8700, 62.5700}, 1e-4));

    frame = file.read_at(50); // skip behind previous step

    CHECK(frame.index() == 50);
    CHECK(frame.get("simulation_step")->as_double() == 5000);
    CHECK(approx_eq(frame.get("time")->as_double(), 10.0));
    CHECK(frame.get("xtc_precision")->as_double() == 1000);
    CHECK(frame.size() == 20455);

    positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(24.5100, 24.5300, 18.7800), 1e-4));
    CHECK(approx_eq(positions[11], Vector3D(23.5300, 24.0900, 21.3100), 1e-4));

    cell = frame.cell();
    CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
    CHECK(approx_eq(cell.lengths(), {55.6800, 58.8700, 62.5700}, 1e-4));
}

TEST_CASE("Read files in XTC format") {
    SECTION("Read trajectory") {
        check_traj_ubiquitin("data/xtc/ubiquitin.xtc");
    }

    SECTION("Read trajectory with gigantic system") {
        check_traj_ubiquitin("data/xtc/ubiquitin_faux2023magic.xtc");
    }

    SECTION("Read different cell shapes") {
        auto file = Trajectory("data/xtc/cell_shapes.xtc");
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
}

TEST_CASE("Write and append files in uncompressed XTC format") {
    // Do not test `xtc_precision` here, because it is not stored in uncompressed data

    SECTION("Write and append trajectory") {
        // Write two frames to a file
        auto tmpfile = NamedTempPath(".xtc");

        auto frame = Frame(UnitCell({10.111, 11.222, 12.333}));
        frame.set("time", 19.376);
        frame.add_atom(Atom("A"), {1.999, 2.888, 3.777});
        frame.add_atom(Atom("B"), {4, 5, 6});
        frame.add_atom(Atom("C"), {7, 8, 9});

        auto file = Trajectory(tmpfile, 'w');
        file.write(frame);

        frame = Frame(UnitCell({20, 21, 22}, {33.333, 44.444, 55.555}));
        frame.set_index(100);
        frame.add_atom(Atom("A"), {4, 5, 6});
        frame.add_atom(Atom("B"), {7, 8, 9});
        frame.add_atom(Atom("C"), {1, 2, 3});

        file.write(frame);
        CHECK(file.size() == 2);
        file.close();

        // now append one frame
        file = Trajectory(tmpfile, 'a');

        frame = Frame(UnitCell({30, 31, 32}));
        frame.set("simulation_step", 200);
        frame.set("time", 20);
        frame.add_atom(Atom("A"), {7, 8, 9});
        frame.add_atom(Atom("B"), {1, 2, 3});
        frame.add_atom(Atom("C"), {4, 5, 6});

        file.write(frame);
        CHECK(file.size() == 3);
        file.close();

        // now read every thing back and check
        file = Trajectory(tmpfile, 'r');

        frame = file.read_at(0);

        CHECK(frame.index() == 0);
        CHECK(frame.get("simulation_step")->as_double() == 0); // default step
        CHECK(approx_eq(frame.get("time")->as_double(), 19.376, 1e-4));
        CHECK(frame.size() == 3);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.999, 2.888, 3.777), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(7, 8, 9), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {10.111, 11.222, 12.333}, 1e-4));

        frame = file.read_at(1);

        CHECK(frame.index() == 1);
        CHECK(frame.get("simulation_step")->as_double() == 100);
        CHECK(approx_eq(frame.get("time")->as_double(), 0)); // default time
        CHECK(frame.size() == 3);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(4, 5, 6), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(1, 2, 3), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::TRICLINIC);
        CHECK(approx_eq(cell.lengths(), {20, 21, 22}, 1e-4));
        CHECK(approx_eq(cell.angles(), {33.333, 44.444, 55.555}, 1e-4));

        frame = file.read();

        CHECK(frame.index() == 2);
        CHECK(frame.get("simulation_step")->as_double() == 200);
        CHECK(approx_eq(frame.get("time")->as_double(), 20));
        CHECK(frame.size() == 3);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(7, 8, 9), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(4, 5, 6), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {30, 31, 32}, 1e-4));
    }

    SECTION("Append on new trajectory") {
        auto tmpfile = NamedTempPath(".xtc");

        auto frame = Frame(UnitCell({10.111, 11.222, 12.333}));
        frame.set("time", 19.376);
        frame.add_atom(Atom("A"), {1.999, 2.888, 3.777});
        frame.add_atom(Atom("B"), {4, 5, 6});
        frame.add_atom(Atom("C"), {7, 8, 9});

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
        CHECK(frame.size() == 3);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.999, 2.888, 3.777), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(7, 8, 9), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {10.111, 11.222, 12.333}, 1e-4));
    }
}

TEST_CASE("Write and append files in compressed XTC format") {
    SECTION("Write and append trajectory") {
        // Write two frames to a file
        auto tmpfile = NamedTempPath(".xtc");

        auto frame = Frame(UnitCell({10.111, 11.222, 12.333}));
        frame.set("time", 19.376);
        frame.set("xtc_precision", 10000); // higher precision for more decimal places
        frame.add_atom(Atom("A"), {1.999, 2.888, 3.777});
        frame.add_atom(Atom("B"), {4, 5, 6});
        frame.add_atom(Atom("C"), {7, 8, 9});
        frame.add_atom(Atom("D"), {10, 11, 12});
        frame.add_atom(Atom("E"), {13, 14, 15});
        frame.add_atom(Atom("F"), {16, 17, 18});
        frame.add_atom(Atom("G"), {19, 20, 21});
        frame.add_atom(Atom("H"), {22, 23, 24});
        frame.add_atom(Atom("I"), {25, 26, 27});
        frame.add_atom(Atom("J"), {28, 29, 30});

        auto file = Trajectory(tmpfile, 'w');
        file.write(frame);

        frame = Frame(UnitCell({20, 21, 22}, {33.333, 44.444, 55.555}));
        frame.set("simulation_step", 100);
        frame.set("xtc_precision", 1000.01);
        frame.add_atom(Atom("A"), {4, 5, 6});
        frame.add_atom(Atom("B"), {7, 8, 9});
        frame.add_atom(Atom("C"), {1, 2, 3});
        frame.add_atom(Atom("D"), {10, 11, 12});
        frame.add_atom(Atom("E"), {13, 14, 15});
        frame.add_atom(Atom("F"), {16, 17, 18});
        frame.add_atom(Atom("G"), {19, 20, 21});
        frame.add_atom(Atom("H"), {22, 23, 24});
        frame.add_atom(Atom("I"), {25, 26, 27});
        frame.add_atom(Atom("J"), {28, 29, 30});

        file.write(frame);
        CHECK(file.size() == 2);
        file.close();

        // now append one frame
        file = Trajectory(tmpfile, 'a');

        frame = Frame(UnitCell({30, 31, 32}));
        frame.set_index(200);
        frame.set("time", 20);
        frame.add_atom(Atom("A"), {7, 8, 9});
        frame.add_atom(Atom("B"), {1, 2, 3});
        frame.add_atom(Atom("C"), {4, 5, 6});
        frame.add_atom(Atom("D"), {10, 11, 12});
        frame.add_atom(Atom("E"), {13, 14, 15});
        frame.add_atom(Atom("F"), {16, 17, 18});
        frame.add_atom(Atom("G"), {19, 20, 21});
        frame.add_atom(Atom("H"), {22, 23, 24});
        frame.add_atom(Atom("I"), {25, 26, 27});
        frame.add_atom(Atom("J"), {28, 29, 30});

        file.write(frame);
        CHECK(file.size() == 3);
        file.close();

        // now read every thing back and check
        file = Trajectory(tmpfile, 'r');

        frame = file.read();

        CHECK(frame.index() == 0);
        CHECK(frame.get("simulation_step")->as_double() == 0); // default step
        CHECK(approx_eq(frame.get("time")->as_double(), 19.376, 1e-4));
        CHECK(approx_eq(frame.get("xtc_precision")->as_double(), 10000));
        CHECK(frame.size() == 10);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.999, 2.888, 3.777), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(7, 8, 9), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {10.111, 11.222, 12.333}, 1e-4));

        frame = file.read();

        CHECK(frame.index() == 1);
        CHECK(frame.get("simulation_step")->as_double() == 100);
        CHECK(approx_eq(frame.get("time")->as_double(), 0)); // default time
        CHECK(approx_eq(frame.get("xtc_precision")->as_double(), 1000.01, 1e-4));
        CHECK(frame.size() == 10);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(4, 5, 6), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(1, 2, 3), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::TRICLINIC);
        CHECK(approx_eq(cell.lengths(), {20, 21, 22}, 1e-4));
        CHECK(approx_eq(cell.angles(), {33.333, 44.444, 55.555}, 1e-4));

        frame = file.read();

        CHECK(frame.index() == 2);
        CHECK(frame.get("simulation_step")->as_double() == 200);
        CHECK(approx_eq(frame.get("time")->as_double(), 20));
        CHECK(approx_eq(frame.get("xtc_precision")->as_double(), 1000)); // default precision
        CHECK(frame.size() == 10);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(7, 8, 9), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(4, 5, 6), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {30, 31, 32}, 1e-4));
    }

    SECTION("Append on new trajectory") {
        auto tmpfile = NamedTempPath(".xtc");

        auto frame = Frame(UnitCell({10.111, 11.222, 12.333}));
        frame.set("time", 19.376);
        frame.set("xtc_precision", 10000); // higher precision for more decimal places
        frame.add_atom(Atom("A"), {1.999, 2.888, 3.777});
        frame.add_atom(Atom("B"), {4, 5, 6});
        frame.add_atom(Atom("C"), {7, 8, 9});
        frame.add_atom(Atom("D"), {10, 11, 12});
        frame.add_atom(Atom("E"), {13, 14, 15});
        frame.add_atom(Atom("F"), {16, 17, 18});
        frame.add_atom(Atom("G"), {19, 20, 21});
        frame.add_atom(Atom("H"), {22, 23, 24});
        frame.add_atom(Atom("I"), {25, 26, 27});
        frame.add_atom(Atom("J"), {28, 29, 30});

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
        CHECK(approx_eq(frame.get("xtc_precision")->as_double(), 10000));
        CHECK(frame.size() == 10);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.999, 2.888, 3.777), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(7, 8, 9), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {10.111, 11.222, 12.333}, 1e-4));
    }
}

TEST_CASE("Check Errors") {
    auto tmpfile = NamedTempPath(".xtc");
    auto file = Trajectory(tmpfile, 'w');

    auto frame = Frame();
    frame.add_atom(Atom("A"), {1, 2, 3});
    file.write(frame);

    frame = Frame();
    frame.add_atom(Atom("A"), {1, 2, 3});
    frame.add_atom(Atom("B"), {4, 5, 6});
    CHECK_THROWS_WITH(
        file.write(frame),
        "XTC format does not support varying numbers of atoms: expected 1, but got 2");
    file.close();

    file = Trajectory(tmpfile, 'a');
    frame = Frame();
    frame.add_atom(Atom("A"), {1, 2, 3});
    frame.add_atom(Atom("B"), {4, 5, 6});
    CHECK_THROWS_WITH(
        file.write(frame),
        "XTC format does not support varying numbers of atoms: expected 1, but got 2");
}

TEST_CASE("Large Numbers") {
    SECTION("Read large numbers") {
        auto file = Trajectory("data/xtc/large_diff.xtc", 'r');

        CHECK(file.size() == 4);
        auto frame = file.read();

        CHECK(frame.index() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(frame.get("xtc_precision")->as_double() == 1000);
        CHECK(frame.size() == 10);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0, 0.0, 0.0), 1e-4));
        CHECK(approx_eq(positions[5], Vector3D(5.0, 50.0, 500.0), 1e-4));
        // compression error gets pretty large
        CHECK(approx_eq(positions[9], Vector3D(16777216.0, 0.0, 0.0), 0.3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {16777220.0, 10.0, 10.0}, 1e-4));

        frame = file.read_at(3);

        CHECK(frame.index() == 3);
        CHECK(frame.get("simulation_step")->as_double() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(frame.get("xtc_precision")->as_double() == 1000);
        CHECK(frame.size() == 10);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0, 0.0, 0.0), 1e-4));
        CHECK(approx_eq(positions[5], Vector3D(5.0, 50.0, 500.0), 1e-4));
        CHECK(approx_eq(positions[9], Vector3D(16777216.0, 16777216.0, 16777216.0), 0.3));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {16777220.0, 16777220.0, 16777220.0}, 1e-4));
    }

    SECTION("Write large numbers") {
        auto tmpfile = NamedTempPath(".xtc");
        auto file = Trajectory(tmpfile, 'w');

        // check large numbers close together
        auto frame = Frame(UnitCell({16777220, 16777220, 16777220}));
        frame.add_atom(Atom("A"), {0.0, 0.0, 0.0});
        for (size_t i = 0; i < 9; ++i) {
            frame.add_atom(Atom("A"), {-16777216.0, 16777216.0, 16777216.0});
        }
        file.write(frame);

        // check large difference to previous atom
        frame = Frame(UnitCell({16777220, 16777220, 16777220}));
        for (size_t i = 0; i < 5; ++i) {
            frame.add_atom(Atom("A"), {16777216.0, 16777216.0, 16777216.0});
            frame.add_atom(Atom("A"), {0.0, 0.0, 0.0});
        }
        file.write(frame);

        // this must be at the end because it writes some bytes
        frame.set("xtc_precision", 10000);
        CHECK_THROWS_WITH(file.write(frame), "internal overflow compressing XTC coordinates");
        file.close();

        // now read every thing back and check
        file = Trajectory(tmpfile, 'r');
        CHECK(file.size() == 2);

        frame = file.read();

        CHECK(frame.index() == 0);
        CHECK(frame.get("simulation_step")->as_double() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(approx_eq(frame.get("xtc_precision")->as_double(), 1000));
        CHECK(frame.size() == 10);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0, 0.0, 0.0), 1e-4));
        CHECK(approx_eq(positions[9], Vector3D(-16777216.0, 16777216.0, 16777216.0), 0.3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {16777220, 16777220, 16777220}, 1e-4));

        frame = file.read();

        CHECK(frame.index() == 1);
        CHECK(frame.get("simulation_step")->as_double() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(approx_eq(frame.get("xtc_precision")->as_double(), 1000));
        CHECK(frame.size() == 10);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(16777216.0, 16777216.0, 16777216.0), 0.3));
        CHECK(approx_eq(positions[9], Vector3D(0.0, 0.0, 0.0), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {16777220, 16777220, 16777220}, 1e-4));
    }
}
