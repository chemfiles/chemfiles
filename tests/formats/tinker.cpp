// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

static bool contains_bond(const Topology& topology, Bond bond) {
    for (auto& exist: topology.bonds()) {
        if (bond == exist) {
            return true;
        }
    }
    return false;
}

TEST_CASE("Read files in Tinker XYZ format") {
    SECTION("water.arc") {
        auto file = Trajectory("data/tinker/water.arc");
        REQUIRE(file.nsteps() == 1);
        auto frame = file.read();

        CHECK(frame.size() == 12);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-12.3637905407, 1.6985027871, 1.2163946648), 1e-6));
        CHECK(approx_eq(positions[10], Vector3D(-9.8283360322, 2.2477303421, 4.0053506840), 1e-6));

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "O");
        CHECK(topology[7].name() == "H");

        CHECK(topology[0].get("atom_type")->as_double() == 1);
        CHECK(topology[7].get("atom_type")->as_double() == 2);

        CHECK(topology.bonds().size() == 8);

        auto expected = std::vector<Bond>{
            {0, 1}, {0, 2}, {3, 4}, {3, 5}, {6, 7}, {6, 8}, {9, 10}, {9, 11},
        };
        CHECK(topology.bonds() == expected);
    }

    SECTION("nitrogen.arc") {
        auto file = Trajectory("data/tinker/nitrogen.arc");
        REQUIRE(file.nsteps() == 50);
        auto frame = file.read();

        CHECK(frame.size() == 212);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-2.941653, 3.480677, 3.562162), 1e-6));
        CHECK(approx_eq(positions[100], Vector3D(-3.328907, 4.080875, -4.559358), 1e-6));

        CHECK(frame.cell() == UnitCell({18.2736, 18.2736, 18.2736}));

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "N");
        CHECK(topology[154].name() == "N");

        CHECK(topology[0].get("atom_type")->as_double() == 24);
        CHECK(topology[154].get("atom_type")->as_double() == 24);
		
        CHECK(topology.bonds().size() == 106);
        for (size_t i = 0; i < frame.size(); i += 2) {
            CHECK(contains_bond(topology, {i, i + 1}));
        }

        frame = file.read_step(34);
        CHECK(frame.size() == 212);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-7.481173, 3.330502, 0.042802), 1e-6));
        CHECK(approx_eq(positions[67], Vector3D(-0.180228, -7.515172, -5.739137), 1e-6));

        CHECK(frame.cell() == UnitCell({18.2736, 18.2736, 18.2736}));
    }
}

TEST_CASE("Write files in Tinker XYZ format") {
    auto tmpfile = NamedTempPath(".arc");
    const auto expected_content =
R"(4 written by the chemfiles library
0.0 0.0 0.0 90.0 90.0 90.0
1 A 1.0 2.0 3.0 1 2 3
2 A 1.0 2.0 3.0 1 1
3 B 1.0 2.0 3.0 2 1
4 B 1.0 2.0 3.0 3
6 written by the chemfiles library
22.0 33.0 44.0 90.0 90.0 90.0
1 A 1.0 2.0 3.0 1 2 3
2 A 1.0 2.0 3.0 1 1
3 B 1.0 2.0 3.0 2 1
4 B 1.0 2.0 3.0 3
5 E 4.0 5.0 6.0 4
6 F 4.0 5.0 6.0 5
)";

    auto frame = Frame();
    frame.add_atom(Atom("A"), {1, 2, 3});
    frame.add_atom(Atom("A"), {1, 2, 3});
    frame.add_atom(Atom("B"), {1, 2, 3});
    frame.add_atom(Atom("B", "C"), {1, 2, 3});
    frame.add_bond(0, 1);
    frame.add_bond(0, 2);


    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.add_atom(Atom("E"), {4, 5, 6});
    frame.add_atom(Atom("F"), {4, 5, 6});
    frame.set_cell(UnitCell({22, 33, 44}));

    file.write(frame);
    file.close();

    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == expected_content);
}


TEST_CASE("Read and write files in memory") {
    SECTION("Reading from memory") {
        std::ifstream checking("data/tinker/nitrogen.arc");
        std::vector<char> content((std::istreambuf_iterator<char>(checking)),
            std::istreambuf_iterator<char>());

        auto file = Trajectory::memory_reader(content.data(), content.size(), "Tinker");
        REQUIRE(file.nsteps() == 50);
        auto frame = file.read();

        CHECK(frame.size() == 212);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-2.941653, 3.480677, 3.562162), 1e-6));
        CHECK(approx_eq(positions[100], Vector3D(-3.328907, 4.080875, -4.559358), 1e-6));

        CHECK(frame.cell() == UnitCell({18.2736, 18.2736, 18.2736}));

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "N");
        CHECK(topology[154].name() == "N");
		
        CHECK(topology[0].get("atom_type")->as_double() == 24);
        CHECK(topology[154].get("atom_type")->as_double() == 24);

        CHECK(topology.bonds().size() == 106);
        for (size_t i = 0; i < frame.size(); i += 2) {
            CHECK(contains_bond(topology, {i, i + 1}));
        }

        frame = file.read_step(34);
        CHECK(frame.size() == 212);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-7.481173, 3.330502, 0.042802), 1e-6));
        CHECK(approx_eq(positions[67], Vector3D(-0.180228, -7.515172, -5.739137), 1e-6));

        CHECK(frame.cell() == UnitCell({18.2736, 18.2736, 18.2736}));
    }
}
