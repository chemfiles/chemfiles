// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"
using namespace chemfiles;

TEST_CASE("Read files in XTC format") {
    SECTION("Read trajectory") {
        auto file = Trajectory("data/xtc/ubiquitin.xtc");
        CHECK(file.nsteps() == 251);
        auto frame = file.read();

        CHECK(frame.step() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(frame.get("xtc_precision")->as_double() == 1000);
        CHECK(frame.size() == 20455);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.8300, 24.6600, 18.8100), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(23.7700, 24.5600, 21.4700), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {55.6800, 58.8700, 62.5700}, 1e-4));

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.step() == 200);
        CHECK(approx_eq(frame.get("time")->as_double(), 0.4, 1e-4));
        CHECK(frame.get("xtc_precision")->as_double() == 1000);
        CHECK(frame.size() == 20455);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.7100, 24.5700, 18.4500), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(23.6700, 24.4800, 21.5200), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {55.6800, 58.8700, 62.5700}, 1e-4));

        frame = file.read_step(230); // skip forward

        CHECK(frame.step() == 23000);
        CHECK(approx_eq(frame.get("time")->as_double(), 46.0));
        CHECK(frame.get("xtc_precision")->as_double() == 1000);
        CHECK(frame.size() == 20455);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.6300, 24.6700, 18.5000), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(23.6800, 24.0700, 21.3100), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {55.6800, 58.8700, 62.5700}, 1e-4));

        frame = file.read_step(50); // skip behind previous step

        CHECK(frame.step() == 5000);
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
        frame.set_step(100);
        frame.add_atom(Atom("A"), {4, 5, 6});
        frame.add_atom(Atom("B"), {7, 8, 9});
        frame.add_atom(Atom("C"), {1, 2, 3});

        file.write(frame);
        CHECK(file.nsteps() == 2);
        file.close();

        // now append one frame
        file = Trajectory(tmpfile, 'a');

        frame = Frame(UnitCell({30, 31, 32}));
        frame.set_step(200);
        frame.set("time", 20);
        frame.add_atom(Atom("A"), {7, 8, 9});
        frame.add_atom(Atom("B"), {1, 2, 3});
        frame.add_atom(Atom("C"), {4, 5, 6});

        file.write(frame);
        CHECK(file.nsteps() == 3);
        file.close();

        // now read every thing back and check
        file = Trajectory(tmpfile, 'r');

        frame = file.read();

        CHECK(frame.step() == 0); // default step
        CHECK(approx_eq(frame.get("time")->as_double(), 19.376, 1e-4));
        CHECK(frame.size() == 3);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(1.999, 2.888, 3.777), 1e-4));
        CHECK(approx_eq(positions[2], Vector3D(7, 8, 9), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.lengths(), {10.111, 11.222, 12.333}, 1e-4));

        frame = file.read();

        CHECK(frame.step() == 100);
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

        CHECK(frame.step() == 200);
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
        CHECK(file.nsteps() == 1);
        file.close();

        // now read every thing back and check
        file = Trajectory(tmpfile, 'r');

        frame = file.read();

        CHECK(frame.step() == 0); // default step
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
        frame.set_step(100);
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
        CHECK(file.nsteps() == 2);
        file.close();

        // now append one frame
        file = Trajectory(tmpfile, 'a');

        frame = Frame(UnitCell({30, 31, 32}));
        frame.set_step(200);
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
        CHECK(file.nsteps() == 3);
        file.close();

        // now read every thing back and check
        file = Trajectory(tmpfile, 'r');

        frame = file.read();

        CHECK(frame.step() == 0); // default step
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

        CHECK(frame.step() == 100);
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

        CHECK(frame.step() == 200);
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
        CHECK(file.nsteps() == 1);
        file.close();

        // now read every thing back and check
        file = Trajectory(tmpfile, 'r');

        frame = file.read();

        CHECK(frame.step() == 0); // default step
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
