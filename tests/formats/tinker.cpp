// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

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

TEST_CASE("Read files in Tinker XYZ format using Molfile") {
    SECTION("water.arc") {
        auto file = Trajectory("data/tinker/water.arc");
        CHECK(file.nsteps() == 1);
        auto frame = file.read();

        CHECK(frame.natoms() == 12);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-12.3637905407, 1.6985027871, 1.2163946648), 1e-6));
        CHECK(approx_eq(positions[10], Vector3D(-9.8283360322, 2.2477303421, 4.0053506840), 1e-6));

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "O");
        CHECK(topology[7].name() == "H");

        CHECK(topology.bonds().size() == 8);

        auto expected = std::vector<Bond>{
            {0, 1}, {0, 2}, {3, 4}, {3, 5}, {6, 7}, {6, 8}, {9, 10}, {9, 11},
        };
        CHECK(topology.bonds() == expected);
    }

    SECTION("nitrogen.arc") {
        auto file = Trajectory("data/tinker/nitrogen.arc");
        CHECK(file.nsteps() == 50);
        auto frame = file.read();

        CHECK(frame.natoms() == 212);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-2.941653, 3.480677, 3.562162), 1e-6));
        CHECK(approx_eq(positions[100], Vector3D(-3.328907, 4.080875, -4.559358), 1e-6));

        CHECK(frame.cell() == UnitCell(18.2736));

        auto& topology = frame.topology();
        CHECK(topology[0].name() == "N");
        CHECK(topology[154].name() == "N");

        CHECK(topology.bonds().size() == 106);
        for (size_t i = 0; i < frame.natoms(); i += 2) {
            CHECK(contains_bond(topology, {i, i + 1}));
        }

        frame = file.read_step(34);
        CHECK(frame.natoms() == 212);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-7.481173, 3.330502, 0.042802), 1e-6));
        CHECK(approx_eq(positions[67], Vector3D(-0.180228, -7.515172, -5.739137), 1e-6));

        CHECK(frame.cell() == UnitCell(18.2736));
    }
}
