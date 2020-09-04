// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <streambuf>
#include <fstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Read files in CML format") {
    SECTION("Check nsteps") {
        auto file = Trajectory("data/cml/ethane.cml");
        CHECK(file.nsteps() == 1);

        file = Trajectory("data/cml/drugs.cml");
        CHECK(file.nsteps() == 4);

        file = Trajectory("data/cml/properties.cml");
        CHECK(file.nsteps() == 1);
    }

    SECTION("Read next step") {
        auto file = Trajectory("data/cml/drugs.cml");
        auto frame = file.read();
        CHECK(frame.size() == 17);
        CHECK(frame.get("title")->as_string() == "naproxen");

        // Check positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0000, -4.6200, 0.0000), 1e-3));
        CHECK(approx_eq(positions[14], Vector3D(9.3358, -0.7700, 0.0000), 1e-3));

        // Check topology
        auto topology = frame.topology();
        CHECK(topology.size() == 17);
        CHECK(topology[0].type() == "C");
    }

    SECTION("Read a specific step") {
        auto file = Trajectory("data/cml/drugs.cml");
        // Read frame at a specific positions
        auto frame = file.read_step(2);
        CHECK(frame.step() == 2);
        CHECK(frame.get("title")->as_string() == "tylenol");
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0000, 6.1600, 0.0000), 1e-3));
        auto topology = frame.topology();
        CHECK(topology.size() == 11);
        CHECK(topology[0].type() == "C");

        frame = file.read_step(0);
        CHECK(frame.step() == 0);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(0.0000, -4.6200, 0.0000), 1e-3));
        CHECK(approx_eq(positions[14], Vector3D(9.3358, -0.7700, 0.0000), 1e-3));
    }

    SECTION("Read fractional coordinates") {
        auto file = Trajectory("data/cml/ethane.cml");
        auto frame = file.read();

        auto cell = frame.cell();
        CHECK(approx_eq(cell.lengths(), {3.0, 3.0, 3.0}, 1e-12));
        CHECK(approx_eq(cell.angles(), {90.0, 90.0, 120.0}, 1e-12));

        auto positions = frame.positions();
        auto fract0 = frame.cell().matrix().invert() * positions[0];
        CHECK(approx_eq(fract0, Vector3D(-1.77493, 0.980333, 0.0000), 1e-3));
    }

    SECTION("Read properties") {
        auto file = Trajectory("data/cml/properties.cml");
        auto frame = file.read();
        CHECK(frame.get("num_c")->as_double() == 10.0);
        CHECK(frame.get("is_organic")->as_bool() == true);
        CHECK(frame[6].get("r")->as_string() == "tButyl");
    }
}

TEST_CASE("Write CML file") {
    auto tmpfile = NamedTempPath(".cml");
    auto frame = Frame();
    frame.add_atom(Atom("A"), {1, 2, 3});
    frame.add_atom(Atom("B"), {1, 2, 3});
    frame.add_atom(Atom("C"), {1, 2, 3});
    frame.add_atom(Atom("D"), {1, 2, 3});

    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.positions()[0] = {4.0, 5.0, 6.0};
    frame.set_cell(UnitCell({22, 22, 22}));

    frame.set("is_organic", false);
    frame.set("name", "test");

    frame[1].set_mass(12);

    frame[2].set("force", Vector3D{1., 2., 3.});
    frame[2].set("num_c", 1.0);

    frame[3].set_charge(1.0);
    frame[3].set("hydrogen_count", 3);

    frame.add_bond(0, 1, Bond::UNKNOWN);
    frame.add_bond(0, 2, Bond::SINGLE);
    frame.add_bond(1, 2, Bond::DOUBLE);
    frame.add_bond(1, 3, Bond::TRIPLE);
    frame.add_bond(2, 3, Bond::AROMATIC);

    file.write(frame);

    file.close();

    // We can't compare the files directly as the properties may be in any order
    // because property_map is based on an unordered_map. Therefore, we should
    // just try to reload the file and see if everything is as it should be.

    auto check_cml = Trajectory(tmpfile);
    CHECK(check_cml.nsteps() == 2);

    auto frame1 = check_cml.read();
    CHECK(frame1.size() == 4);
    CHECK(frame1.topology().bonds().size() == 0);

    auto frame2 = check_cml.read();
    CHECK(frame2.size() == 4);
    CHECK(frame2.cell() == UnitCell({22, 22, 22}));

    auto& orders = frame2.topology().bond_orders();
    CHECK(orders[0] == Bond::UNKNOWN);
    CHECK(orders[1] == Bond::SINGLE);
    CHECK(orders[2] == Bond::DOUBLE);
    CHECK(orders[3] == Bond::TRIPLE);
    CHECK(orders[4] == Bond::AROMATIC);

    CHECK(frame2.get("is_organic")->as_bool() == false);
    CHECK(frame2.get("name")->as_string() == "test");

    CHECK(frame2[1].mass() == 12);

    CHECK(frame2[2].get("num_c")->as_double() == 1.0);
    CHECK(frame2[2].get("force")->as_vector3d() == Vector3D{1., 2., 3.});

    CHECK(frame2[3].charge() == 1);
    CHECK(frame2[3].get("hydrogen_count")->as_double() == 3);
}

TEST_CASE("Append CML file") {
    auto tmpfile = NamedTempPath(".cml");
    const auto EXPECTED_CONTENT =
    R"(<molecule title="appended">
  <propertyList />
  <atomArray>
    <atom id="a1" elementType="A" x3="1" y3="2" z3="3">
      <vector3 title="velocity">4.000000 5.000000 6.000000</vector3>
    </atom>
    <atom id="a2" elementType="B" x3="1" y3="2" z3="3">
      <vector3 title="velocity">0.000000 0.000000 0.000000</vector3>
    </atom>
    <atom id="a3" elementType="C" x3="1" y3="2" z3="3">
      <vector3 title="velocity">0.000000 0.000000 0.000000</vector3>
    </atom>
    <atom id="a4" elementType="D" x3="1" y3="2" z3="3">
      <vector3 title="velocity">0.000000 0.000000 0.000000</vector3>
    </atom>
  </atomArray>
</molecule>
)";

    auto frame = Frame();
    frame.add_velocities();
    frame.add_atom(Atom("", "A"), {1, 2, 3}, {4, 5, 6});
    frame.add_atom(Atom("", "B"), {1, 2, 3});
    frame.add_atom(Atom("", "C"), {1, 2, 3});
    frame.add_atom(Atom("", "D"), {1, 2, 3});
    frame.set("title", "appended");

    auto file = Trajectory(tmpfile, 'a');
    file.write(frame);
    file.close();

    std::ifstream checking(tmpfile, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());

    CHECK(EXPECTED_CONTENT == content);
}

TEST_CASE("Read and write files in memory") {
    SECTION("Reading from memory") {
        std::ifstream checking("data/cml/drugs.cml");
        std::vector<char> content((std::istreambuf_iterator<char>(checking)),
            std::istreambuf_iterator<char>());

        auto file = Trajectory::memory_reader(content.data(), content.size(), "CML");
        CHECK(file.nsteps() == 4);
    }
}
