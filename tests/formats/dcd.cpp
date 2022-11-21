// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"

#include "chemfiles/error_fmt.hpp"
using namespace chemfiles;

static Vector3D vector3d_float(float x, float y, float z) {
    return {
        static_cast<double>(x),
        static_cast<double>(y),
        static_cast<double>(z),
    };
}

static bool is_little_endian() {
    int16_t value = 1;
    const int8_t* bytes = reinterpret_cast<int8_t*>(&value);
    return bytes[0] == 1;
}

TEST_CASE("Read files in DCD format") {
    auto file = Trajectory("data/dcd/water.dcd");
    CHECK(file.nsteps() == 100);

    auto frame = file.read();
    CHECK(frame.size() == 297);
    CHECK(frame.get<Property::DOUBLE>("time") == 0.0);
    CHECK(frame.get<Property::STRING>("title").value() == "Created by DCD plugin\nREMARKS Created 30 May, 2015 at 19:24\n");

    auto positions = frame.positions();
    CHECK(positions[0] == vector3d_float(0.41721907f, 8.303366f, 11.737172f));
    CHECK(positions[296] == vector3d_float(6.664049f, 11.614183f, 12.961486f));

    auto cell = frame.cell();
    CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
    CHECK(cell.lengths() == vector3d_float(15.0, 15.0, 15.0));

    frame = file.read_step(2);
    CHECK(frame.size() == 297);
    CHECK(frame.get<Property::DOUBLE>("time") == 2.0);

    positions = frame.positions();
    CHECK(positions[0] == vector3d_float(0.29909524f, 8.31003f, 11.721462f));
    CHECK(positions[296] == vector3d_float(6.797599f, 11.50882f, 12.704233f));
}


TEST_CASE("Read unit cell in DCD files") {
    SECTION("No unit cell") {
        auto file = Trajectory("data/dcd/nopbc.dcd");
        auto frame = file.read();
        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::INFINITE);
        CHECK(cell.lengths() == Vector3D(0.0, 0.0, 0.0));
    }


    SECTION("Orthorhombic cell") {
        auto file = Trajectory("data/dcd/withpbc.dcd");
        auto frame = file.read();
        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(cell.lengths() == Vector3D(100.0, 100.0, 100.0));
    }

    SECTION("Triclinic cell, cell vectors") {
        // this is used by modern version of CHARMM
        auto file = Trajectory("data/dcd/triclinic-octane-vectors.dcd");
        CHECK(file.nsteps() == 10);

        auto frame = file.read();
        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::TRICLINIC);

        CHECK(approx_eq(cell.lengths(), {4.1594858, 4.749700, 11.000093}, 1e-6));
        CHECK(approx_eq(cell.angles(), {94.804658, 84.486392, 105.108346}, 1e-6));

        auto EXPECTED = std::string(
"* OCTANE CRYSTAL                                                                "
"* FOR TESTING CRYST BUILDING CODE IN CHARMM22 AND CHARMM25                      "
"* USING X-CRYST FRACTIONAL COOR AS STARTING STRUCT                              "
"* H. MATHISEN AND N. NORMAN ACTA CHEMICA SCANDINAVICA 15,1961,1747              "
"*  DATE:     5/15/22     20: 4:28      CREATED BY USER: guillaume               "
        );

        CHECK(frame.get<Property::STRING>("title").value() == EXPECTED);
    }

    SECTION("Triclinic cell, angles as cosine") {
        // this is the same file as triclinic-octane-vectors.dcd, with the
        // CHARMM version set to 24.
        auto file = Trajectory("data/dcd/triclinic-octane-cos.dcd");
        CHECK(file.nsteps() == 10);

        auto frame = file.read();
        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::TRICLINIC);

        CHECK(approx_eq(cell.lengths(), {4.109898, 4.707060, 10.993230}, 1e-6));
        CHECK(approx_eq(cell.angles(), {93.892818, 85.922246, 98.783338}, 1e-6));
    }

    SECTION("Triclinic cell, direct angles") {
        // this is the result of reading triclinic-octane-vectors.dcd and
        // writting it back with MDAnalysis
        auto file = Trajectory("data/dcd/triclinic-octane-direct.dcd");
        CHECK(file.nsteps() == 10);

        auto frame = file.read();
        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::TRICLINIC);

        CHECK(approx_eq(cell.lengths(), {4.109898, 4.707060, 10.993230}, 1e-6));
        CHECK(approx_eq(cell.angles(), {105.571273, 73.688987, 125.133354}, 1e-6));
    }
}

TEST_CASE("4D DCD files") {
    auto file = Trajectory("data/dcd/4d-dynamic.dcd");
    CHECK(file.nsteps() == 5);

    auto frame = file.read();
    CHECK(frame.cell().shape() == UnitCell::INFINITE);

    CHECK(frame.size() == 27);
    auto positions = frame.positions();
    CHECK(positions[5] == vector3d_float(-1.5822195f, 0.6511365f, 1.3911803f));
    CHECK(positions[15] == vector3d_float(2.2381972f, -0.5173331f, -0.4879273f));


    frame = file.read_step(3);
    CHECK(frame.cell().shape() == UnitCell::INFINITE);
    CHECK(frame.size() == 27);
    positions = frame.positions();
    CHECK(positions[5] == vector3d_float(-1.5833939f, 0.70485264f, 1.3606575f));
    CHECK(positions[15] == vector3d_float(2.230041f, -0.5253474f, -0.50111574f));
}


TEST_CASE("fixed atoms") {
    // atoms 0 to 5 are fixed, the rest are free
    auto file = Trajectory("data/dcd/fixed-atoms.dcd");
    CHECK(file.nsteps() == 10);

    auto frame = file.read();
    CHECK(frame.size() == 12);
    CHECK(frame.cell().shape() == UnitCell::INFINITE);

    auto positions = frame.positions();
    CHECK(positions[2] == vector3d_float(-1.0220516f, -1.0135641f, 0.0));
    CHECK(positions[10] == vector3d_float(1.820057f, -1.3015488f, 10.0f));

    frame = file.read();
    positions = frame.positions();
    CHECK(positions[2] == vector3d_float(-1.0220516f, -1.0135641f, 0.0));
    CHECK(positions[10] == vector3d_float(1.8200468f, -1.3015325f, 10.0f));

    frame = file.read_step(5);
    CHECK(frame.size() == 12);
    CHECK(frame.cell().shape() == UnitCell::INFINITE);
    positions = frame.positions();
    CHECK(positions[2] == vector3d_float(-1.0220516f, -1.0135641f, 0.0));
    CHECK(positions[10] == vector3d_float(1.8199368f, -1.3013588f, 10.0f));
}

TEST_CASE("reading uncommon files") {
    // the most common case is 32-bit fortran markers, little endian.
    // all the files above are using this variant

    SECTION("64-bit fortran markers, little endian") {
        auto file = Trajectory("data/dcd/mrmd_h2so4-64bit-le.dcd");
        CHECK(file.nsteps() == 50);

        auto frame = file.read_step(23);
        CHECK(frame.size() == 7);
        CHECK(frame.cell().shape() == UnitCell::INFINITE);
        auto positions = frame.positions();
        CHECK(positions[2] == vector3d_float(0.6486294f, 0.062248673f, -1.5570515f));
        CHECK(positions[4] == vector3d_float(-1.3111109f, 0.35563222f, 0.9946163f));
    }

    SECTION("32-bit fortran markers, big endian") {
        auto file = Trajectory("data/dcd/mrmd_h2so4-32bit-be.dcd");
        CHECK(file.nsteps() == 50);

        auto frame = file.read_step(23);
        CHECK(frame.size() == 7);
        CHECK(frame.cell().shape() == UnitCell::INFINITE);
        auto positions = frame.positions();
        CHECK(positions[2] == vector3d_float(0.6486294f, 0.062248673f, -1.5570515f));
        CHECK(positions[4] == vector3d_float(-1.3111109f, 0.35563222f, 0.9946163f));
    }

    SECTION("64-bit fortran markers, big endian") {
        auto file = Trajectory("data/dcd/mrmd_h2so4-64bit-be.dcd");
        CHECK(file.nsteps() == 50);

        auto frame = file.read_step(23);
        CHECK(frame.size() == 7);
        CHECK(frame.cell().shape() == UnitCell::INFINITE);
        auto positions = frame.positions();
        CHECK(positions[2] == vector3d_float(0.6486294f, 0.062248673f, -1.5570515f));
        CHECK(positions[4] == vector3d_float(-1.3111109f, 0.35563222f, 0.9946163f));
    }
}


TEST_CASE("write DCD files") {
    auto tmpfile = NamedTempPath(".dcd");

    {
        auto file = Trajectory(tmpfile, 'w');

        auto cell = UnitCell({10, 12, 11}, {90, 90, 90});
        auto frame = Frame(cell);
        frame.add_atom(Atom("N"), {1, 2, 3});
        frame.add_atom(Atom("B"), {0, 0, 0});
        frame.add_atom(Atom("N"), {0, 0, 0});
        frame.set("title", "hello DCD!");
        file.write(frame);

        cell = UnitCell({10, 12, 11}, {90, 80, 120});
        frame = Frame(cell);
        frame.add_atom(Atom("N"), {1000, 20000, 300000});
        frame.add_atom(Atom("B"), {4, 5, 7.5});
        frame.add_atom(Atom("N"), {-3, 0, 0});
        file.write(frame);
    }

    if (is_little_endian()) {
        auto expected = std::vector<uint8_t>{
            // header
            0x54, 0x0, 0x0, 0x0,
            'C', 'O', 'R', 'D',
            0x2, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,    0x1, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x9, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x1, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x18, 0x0, 0x0, 0x0,
            0x54, 0x0, 0x0, 0x0,
            // title
            0x54, 0x0, 0x0, 0x0,
            0x1, 0x0, 0x0, 0x0,
            'h', 'e', 'l', 'l', 'o', ' ', 'D', 'C', 'D', '!', 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x54, 0x0, 0x0, 0x0,
            // n_atoms
            0x4, 0x0, 0x0, 0x0,
            0x3, 0x0, 0x0, 0x0,
            0x4, 0x0, 0x0, 0x0,
            // cell
            0x30, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x24, 0x40,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x56, 0x40,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x28, 0x40,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x56, 0x40,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x56, 0x40,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x26, 0x40,
            0x30, 0x0, 0x0, 0x0,
            // x
            0xc, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x80, 0x3f,   0x0, 0x0, 0x0, 0x0,   0x0, 0x0, 0x0, 0x0,
            0xc, 0x0, 0x0, 0x0,
            // y
            0xc, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x40,    0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0xc, 0x0, 0x0, 0x0,
            // z
            0xc, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x40, 0x40,   0x0, 0x0, 0x0, 0x0,   0x0, 0x0, 0x0, 0x0,
            0xc, 0x0, 0x0, 0x0,
            // step 2, cell
            0x30, 0x0, 0x0, 0x0,
            // the unit cell values are tested below. We remove them from this
            // test since there can be small numerical differences here between
            // different libm implementations
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            //
            0x30, 0x0, 0x0, 0x0,
            // x
            0xc, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x7a, 0x44,    0x0, 0x0, 0x80, 0x40,    0x0, 0x0, 0x40, 0xc0,
            0xc, 0x0, 0x0, 0x0,
            // y
            0xc, 0x0, 0x0, 0x0,
            0x0, 0x40, 0x9c, 0x46,   0x0, 0x0, 0xa0, 0x40,    0x0, 0x0, 0x0, 0x0,
            0xc, 0x0, 0x0, 0x0,
            // z
            0xc, 0x0, 0x0, 0x0,
            0x0, 0x7c, 0x92, 0x48,    0x0, 0x0, 0xf0, 0x40,    0x0, 0x0, 0x0, 0x0,
            0xc, 0x0, 0x0, 0x0,
        };

        auto content = read_binary_file(tmpfile);
        // overwrite the second frame unit cell
        std::memset(content.data() + 316, 0xaa, 48);
        CHECK(content == expected);
    }

    auto check = Trajectory(tmpfile);
    CHECK(check.nsteps() == 2);

    auto check_frame = check.read_step(1);
    CHECK(check_frame.size() == 3);
    CHECK(approx_eq(check_frame.cell().lengths(), {10, 12, 11}, 1e-12));
    CHECK(approx_eq(check_frame.cell().angles(), {90, 80, 120}, 1e-12));

    auto positions = check_frame.positions();
    CHECK(approx_eq(positions[0], {1000, 20000, 300000}, 1e-12));
    CHECK(approx_eq(positions[1], {4, 5, 7.5}, 1e-12));
    CHECK(approx_eq(positions[2], {-3, 0, 0}, 1e-12));

    auto cell = check_frame.cell();
    CHECK(approx_eq(cell.lengths(), {10, 12, 11}, 1e-12));
    CHECK(approx_eq(cell.angles(), {90, 80, 120}, 1e-12));


    tmpfile = NamedTempPath(".dcd");
    {
        auto file = Trajectory(tmpfile, 'w');

        auto frame = Frame(UnitCell());
        frame.add_atom(Atom("N"), {1, 2, 3});
        frame.add_atom(Atom("B"), {0, 0, 0});
        frame.add_atom(Atom("N"), {0, 0, 0});
        file.write(frame);
    }

    if (is_little_endian()) {
        auto expected = std::vector<uint8_t>{
            // header
            0x54, 0x0, 0x0, 0x0,
            'C', 'O', 'R', 'D',
            0x1, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,    0x1, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x9, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,    0x18, 0x0, 0x0, 0x0,
            0x54, 0x0, 0x0, 0x0,
            // title
            0x4, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x0,
            0x4, 0x0, 0x0, 0x0,
            // n_atoms
            0x4, 0x0, 0x0, 0x0,
            0x3, 0x0, 0x0, 0x0,
            0x4, 0x0, 0x0, 0x0,
            // x
            0xc, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x80, 0x3f,   0x0, 0x0, 0x0, 0x0,   0x0, 0x0, 0x0, 0x0,
            0xc, 0x0, 0x0, 0x0,
            // y
            0xc, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x0, 0x40,    0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
            0xc, 0x0, 0x0, 0x0,
            // z
            0xc, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x40, 0x40,   0x0, 0x0, 0x0, 0x0,   0x0, 0x0, 0x0, 0x0,
            0xc, 0x0, 0x0, 0x0,
        };

        auto content = read_binary_file(tmpfile);
        CHECK(content == expected);
    }
}


TEST_CASE("Append to DCD files") {
    SECTION("pre-existing file") {
        auto tmpfile = NamedTempPath(".dcd");

        {
            auto file = Trajectory(tmpfile, 'w');

            auto cell = UnitCell({10, 12, 11}, {90, 90, 90});
            auto frame = Frame(cell);
            frame.add_atom(Atom("N"), {1, 2, 3});
            frame.add_atom(Atom("B"), {0, 0, 0});
            frame.add_atom(Atom("N"), {0, 0, 0});
            frame.set("title", "hello DCD!");
            file.write(frame);
        }

        {
            auto file = Trajectory(tmpfile, 'a');

            auto cell = UnitCell({10, 12, 11}, {90, 80, 120});
            auto frame = Frame(cell);
            frame.add_atom(Atom("N"), {1000, 20000, 300000});
            frame.add_atom(Atom("B"), {4, 5, 7.5});
            frame.add_atom(Atom("N"), {-3, 0, 0});
            file.write(frame);
        }

        auto check = Trajectory(tmpfile);
        CHECK(check.nsteps() == 2);

        auto frame = check.read_step(0);
        CHECK(frame.size() == 3);
        CHECK(approx_eq(frame.cell().lengths(), {10, 12, 11}, 1e-12));
        CHECK(approx_eq(frame.cell().angles(), {90, 90, 90}, 1e-12));

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], {1, 2, 3}, 1e-12));
        CHECK(approx_eq(positions[1], {0, 0, 0}, 1e-12));
        CHECK(approx_eq(positions[2], {0, 0, 0}, 1e-12));

        frame = check.read_step(1);
        CHECK(frame.size() == 3);
        CHECK(approx_eq(frame.cell().lengths(), {10, 12, 11}, 1e-12));
        CHECK(approx_eq(frame.cell().angles(), {90, 80, 120}, 1e-12));

        positions = frame.positions();
        CHECK(approx_eq(positions[0], {1000, 20000, 300000}, 1e-12));
        CHECK(approx_eq(positions[1], {4, 5, 7.5}, 1e-12));
        CHECK(approx_eq(positions[2], {-3, 0, 0}, 1e-12));
    }

    SECTION("new file") {
        auto tmpfile = NamedTempPath(".dcd");

        {
            auto file = Trajectory(tmpfile, 'a');

            auto cell = UnitCell({10, 12, 11}, {90, 80, 120});
            auto frame = Frame(cell);
            frame.add_atom(Atom("N"), {1000, 20000, 300000});
            frame.add_atom(Atom("B"), {4, 5, 7.5});
            frame.add_atom(Atom("N"), {-3, 0, 0});
            file.write(frame);
        }

        auto check = Trajectory(tmpfile);
        CHECK(check.nsteps() == 1);

        auto frame = check.read();
        CHECK(frame.size() == 3);
        CHECK(approx_eq(frame.cell().lengths(), {10, 12, 11}, 1e-12));
        CHECK(approx_eq(frame.cell().angles(), {90, 80, 120}, 1e-12));

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], {1000, 20000, 300000}, 1e-12));
        CHECK(approx_eq(positions[1], {4, 5, 7.5}, 1e-12));
        CHECK(approx_eq(positions[2], {-3, 0, 0}, 1e-12));
    }
}
