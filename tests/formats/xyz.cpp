// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include <iostream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;


TEST_CASE("Read files in XYZ format") {
    SECTION("Check nsteps") {
        auto file = Trajectory("data/xyz/trajectory.xyz");
        CHECK(file.nsteps() == 2);

        file = Trajectory("data/xyz/helium.xyz");
        CHECK(file.nsteps() == 397);

        file = Trajectory("data/xyz/topology.xyz");
        CHECK(file.nsteps() == 1);
    }

    SECTION("Read next step") {
        auto file = Trajectory("data/xyz/helium.xyz");
        auto frame = file.read();
        CHECK(frame.size() == 125);
        // Check positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], {0.49053, 8.41351, 0.0777257}, 1e-12));
        CHECK(approx_eq(positions[124], {8.57951, 8.65712, 8.06678}, 1e-12));
        // Check topology
        auto topology = frame.topology();
        CHECK(topology.size() == 125);
        CHECK(topology[0] == Atom("He"));
    }

    SECTION("Read a specific step") {
        auto file = Trajectory("data/xyz/helium.xyz");
        // Read frame at a specific positions
        auto frame = file.read_step(42);
        CHECK(frame.step() == 42);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], {-0.145821, 8.540648, 1.090281}, 1e-12));
        CHECK(approx_eq(positions[124], {8.446093, 8.168162, 9.350953}, 1e-12));
        auto topology = frame.topology();
        CHECK(topology.size() == 125);
        CHECK(topology[0] == Atom("He"));

        frame = file.read_step(0);
        CHECK(frame.step() == 0);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], {0.49053, 8.41351, 0.0777257}, 1e-12));
        CHECK(approx_eq(positions[124], {8.57951, 8.65712, 8.06678}, 1e-12));
    }

    SECTION("Read the whole file") {
        auto file = Trajectory("data/xyz/helium.xyz");
        REQUIRE(file.nsteps() == 397);

        Frame frame;
        while (!file.done()) {
            frame = file.read();
        }
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], {-1.186037, 11.439334, 0.529939}, 1e-12));
        CHECK(approx_eq(positions[124], {5.208778, 12.707273, 10.940157}, 1e-12));
    }

    SECTION("Read various files formatting") {
        auto file = Trajectory("data/xyz/spaces.xyz");

        auto frame = file.read();
        auto positions = frame.positions();
        CHECK(approx_eq(positions[10], {0.8336, 0.3006, 0.4968}, 1e-12));
    }

    SECTION("Extended XYZ") {
        auto file = Trajectory("data/xyz/extended.xyz");
        CHECK(file.nsteps() == 3);

        auto frame = file.read();
        CHECK(frame.size() == 192);

        // Reading the unit cell
        auto expected = UnitCell(
            {8.43116035, 14.50510613, 15.60911468}, 
            {73.31699212, 85.70200582, 89.37501529}
        );
        CHECK(approx_eq(frame.cell().matrix(), expected.matrix(), 1e-6));

        // frame level properties
        CHECK(frame.get("ENERGY")->as_string() == "-2069.84934116");
        CHECK(frame.get("Natoms")->as_string() == "192");
        CHECK(frame.get("NAME")->as_string() == "COBHUW");
        CHECK(frame.get("IsStrange")->as_bool() == true);

        // Atom level properties
        CHECK(approx_eq(frame.positions()[0], {2.33827271799, 4.55315540425, 11.5841360926}, 1e-12));
        CHECK(frame[0].get("CS_0")->as_double() == 24.10);
        CHECK(frame[0].get("CS_1")->as_double() == 31.34);

        CHECK(frame[51].get("CS_0")->as_double() == -73.98);
        CHECK(frame[51].get("CS_1")->as_double() == -81.85);

        // different types
        frame = file.read();
        CHECK(frame.size() == 62);
        CHECK(approx_eq(frame[0].get("CS")->as_vector3d(), {198.20, 202.27, 202.27}, 1e-12));

        // Different syntaxes for bool values
        frame = file.read();
        CHECK(frame.size() == 8);
        CHECK(frame[0].get("bool")->as_bool() == true);
        CHECK(frame[1].get("bool")->as_bool() == true);
        CHECK(frame[2].get("bool")->as_bool() == true);
        CHECK(frame[3].get("bool")->as_bool() == true);
        CHECK(frame[4].get("bool")->as_bool() == false);
        CHECK(frame[5].get("bool")->as_bool() == false);
        CHECK(frame[6].get("bool")->as_bool() == false);
        CHECK(frame[7].get("bool")->as_bool() == false);

        CHECK(frame[0].get("int")->as_double() == 33.0);
        CHECK(frame[0].get("strings_0")->as_string() == "bar");
        CHECK(frame[0].get("strings_1")->as_string() == "\"test\"");
    }
}

static void check_bad_properties_still_read_frame(const Frame& frame) {
    CHECK(frame.size() == 1);
    CHECK(frame[0].name() == "H");
    CHECK(frame.positions()[0] == Vector3D(1, 4, 2.3));
}

TEST_CASE("Errors in XYZ format") {
    SECTION("bad files") {
        CHECK_THROWS_WITH(Trajectory("data/xyz/bad/helium.xyz"),
            "XYZ format: not enough lines at step 0 (expected 10, got 7)"
        );

        auto file = Trajectory("data/xyz/bad/extended.xyz");
        CHECK_THROWS_WITH(file.read_step(0),
            "error while reading '': expected 1 values, found 0"
        );

        CHECK_THROWS_WITH(file.read_step(1),
            "error while reading ' ff': can not parse 'ff' as a double"
        );

        CHECK_THROWS_WITH(file.read_step(2),
            "error while reading '': expected 1 values, found 0"
        );

        CHECK_THROWS_WITH(file.read_step(3),
            "error while reading ' ze': can not parse 'ze' as a double"
        );

        CHECK_THROWS_WITH(file.read_step(4),
            "error while reading ' 3 4': expected 3 values, found 2"
        );

        CHECK_THROWS_WITH(file.read_step(5),
            "error while reading ' 3 4 ff': can not parse 'ff' as a double"
        );

        CHECK_THROWS_WITH(file.read_step(6),
            "error while reading '': expected 1 values, found 0"
        );

        CHECK_THROWS_WITH(file.read_step(7),
            "invalid value for boolean 'ok'"
        );

        CHECK_THROWS_WITH(file.read_step(8),
            "error while reading '': expected 1 values, found 0"
        );
    }

    SECTION("Invalid extended XYZ properties") {
        // capture warnings
        std::string WARNINGS;
        chemfiles::set_warning_callback([&](const std::string& message) {
            WARNINGS = message;
        });

        auto file = Trajectory("data/xyz/bad/extended-bad-properties.xyz");
        REQUIRE(file.nsteps() == 5);

        auto frame = file.read();
        check_bad_properties_still_read_frame(frame);
        // This one is not even recognized as an extended XYZ file
        CHECK(WARNINGS == "");
        CHECK_FALSE(frame.get("invalid"));
        WARNINGS.clear();

        frame = file.read();
        check_bad_properties_still_read_frame(frame);
        CHECK(WARNINGS == "Extended XYZ: ignoring invalid Properties='species:S:1:pos:R:3:bad:R:'");
        WARNINGS.clear();

        frame = file.read();
        check_bad_properties_still_read_frame(frame);
        CHECK(WARNINGS == "Extended XYZ: invalid type name for bad in Properties='species:S:1:pos:R:3:bad:F:1'");
        WARNINGS.clear();

        frame = file.read();
        check_bad_properties_still_read_frame(frame);
        CHECK(WARNINGS == "Extended XYZ: invalid type repeat for bad in Properties='species:S:1:pos:R:3:bad:R:ff'");
        WARNINGS.clear();

        frame = file.read();
        check_bad_properties_still_read_frame(frame);
        CHECK(WARNINGS == "Extended XYZ: invalid type repeat for bad in Properties='species:S:1:pos:R:3:bad:R:0'");
        WARNINGS.clear();

        // reset default warning handle
        chemfiles::set_warning_callback([&](const std::string& message) {
            std::cerr << "[chemfiles] " << message << std::endl;
        });
    }
}

TEST_CASE("Write files in XYZ format") {
    auto tmpfile = NamedTempPath(".xyz");
    const auto expected_content =
R"(4
Properties=species:S:1:pos:R:3 name="Test"
A 1 2 3
B 1 2 3
C 1 2 3
D 1 2 3
6
Properties=species:S:1:pos:R:3 Lattice="12 0 0 0 13 0 0 0 14" direction="1 0 2" is_open=F name="Test" 'quotes"'=T "quotes'"=T speed=33.4 "with space"=T
A 1 2 3
B 1 2 3
C 1 2 3
D 1 2 3
E 4 5 6
F 4 5 6
)";

    auto frame = Frame();
    frame.set("name", "Test");
    frame.add_atom(Atom("A","O"), {1, 2, 3});
    frame.add_atom(Atom("B"), {1, 2, 3});
    frame.add_atom(Atom("C"), {1, 2, 3});
    frame.add_atom(Atom("D"), {1, 2, 3});

    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.set_cell(UnitCell({12, 13, 14}));
    frame.set("is_open", false);
    frame.set("speed", 33.4);
    frame.set("direction", Vector3D(1, 0, 2));
    frame.set("with space", true);
    frame.set("quotes'", true);
    frame.set("quotes\"", true);

    // properties with two type of quotes are skipped
    frame.set("all_quotes'\"", true);

    frame.add_atom(Atom("E"), {4, 5, 6});
    frame.add_atom(Atom("F"), {4, 5, 6});

    file.write(frame);
    file.close();

    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == expected_content);
}

TEST_CASE("Read and write files in memory") {
    SECTION("Reading from memory") {
        std::ifstream checking("data/xyz/topology.xyz");
        std::vector<char> content((std::istreambuf_iterator<char>(checking)),
            std::istreambuf_iterator<char>());

        auto file = Trajectory::memory_reader(content.data(), content.size(), "XYZ");
        CHECK(file.nsteps() == 1);

        auto frame = file.read();
    }

    SECTION("Writing to memory") {
        const auto expected_content =
R"(4
Properties=species:S:1:pos:R:3
A 1 2 3
B 1 2 3
C 1 2 3
D 1 2 3
6
Properties=species:S:1:pos:R:3
A 1 2 3
B 1 2 3
C 1 2 3
D 1 2 3
E 4 5 6
F 4 5 6
)";

        auto frame = Frame();
        frame.add_atom(Atom("A", "O"), { 1, 2, 3 });
        frame.add_atom(Atom("B"), { 1, 2, 3 });
        frame.add_atom(Atom("C"), { 1, 2, 3 });
        frame.add_atom(Atom("D"), { 1, 2, 3 });

        auto file = Trajectory::memory_writer("XYZ");
        file.write(frame);

        frame.add_atom(Atom("E"), { 4, 5, 6 });
        frame.add_atom(Atom("F"), { 4, 5, 6 });

        file.write(frame);
        file.close();

        auto result = *file.memory_buffer();
        auto result_str = std::string(result.data(), result.size());
        CHECK(result_str == expected_content);
    }
}

TEST_CASE("Round-trip read/write") {
    auto tmpfile = NamedTempPath(".xyz");
    auto content =
R"(3
Properties=species:S:1:pos:R:3
O 0.417 8.303 11.737
H 1.32 8.48 12.003
H 0.332 8.726 10.882
)";

    std::ofstream create(tmpfile);
    create << content;
    create.close();

    auto frame = Trajectory(tmpfile).read();
    Trajectory(tmpfile, 'w').write(frame);

    std::ifstream checking(tmpfile);
    std::string actual((std::istreambuf_iterator<char>(checking)),
                        std::istreambuf_iterator<char>());
    CHECK(content == actual);
}
