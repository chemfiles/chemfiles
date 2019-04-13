// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <streambuf>
#include <fstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;

#include <iostream>

TEST_CASE("Read files in CML format") {
    SECTION("Check nsteps") {
        Trajectory file1("data/cml/ethane.cml");
        CHECK(file1.nsteps() == 1);

        Trajectory file2("data/cml/drugs.cml");
        CHECK(file2.nsteps() == 4);

        Trajectory file3("data/cml/properties.cml");
        CHECK(file3.nsteps() == 1);
    }

    SECTION("Read next step") {
        Trajectory file("data/cml/drugs.cml");
        auto frame = file.read();
        CHECK(frame.size() == 17);
        CHECK(frame.get("name")->as_string() == "naproxen");

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
        Trajectory file("data/cml/drugs.cml");
        // Read frame at a specific positions
        auto frame = file.read_step(2);
        CHECK(frame.step() == 2);
        CHECK(frame.get("name")->as_string() == "tylenol");
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
        Trajectory file("data/cml/ethane.cml");
        auto frame = file.read();

        auto cell = frame.cell();
        CHECK(cell.a() == 3.0);
        CHECK(cell.b() == 3.0);
        CHECK(cell.c() == 3.0);
        CHECK(cell.alpha() == 90.0);
        CHECK(cell.beta() == 90.0);
        CHECK(cell.gamma() == 120.0);

        auto positions = frame.positions();
        auto fract0 = frame.cell().matrix().invert() * positions[0];
        CHECK(approx_eq(fract0, Vector3D(-1.77493, 0.980333, 0.0000), 1e-3));
    }

    SECTION("Read properties") {
        Trajectory file("data/cml/properties.cml");
        auto frame = file.read();
        CHECK(frame.get("num_c")->as_double() == 10.0);
        CHECK(frame.get("is_organic")->as_bool() == true);
        CHECK(frame[6].get("r")->as_string() == "tButyl");
    }
}

TEST_CASE("Write CML file") {
    auto tmpfile = NamedTempPath(".cml");
    const auto EXPECTED_CONTENT =
    "<?xml version=\"1.0\"?>\n"
    "<cml xmlns=\"http://www.xml-cml.org/schema\" "
    "xmlns:cml=\"http://www.xml-cml.org/dict/cml\" "
    "xmlns:units=\"http://www.xml-cml.org/units/units\" "
    "xmlns:convention=\"http://www.xml-cml.org/convention\" "
    "convention=\"convention:molecular\" "
    "xmlns:iupac=\"http://www.iupac.org\">\n"
    "  <molecule id=\"m1\">\n"
    "    <atomArray>\n"
    "      <atom id=\"a1\" elementType=\"A\" x3=\"1\" y3=\"2\" z3=\"3\" />\n"
    "      <atom id=\"a2\" elementType=\"B\" x3=\"1\" y3=\"2\" z3=\"3\" />\n"
    "      <atom id=\"a3\" elementType=\"C\" x3=\"1\" y3=\"2\" z3=\"3\" />\n"
    "      <atom id=\"a4\" elementType=\"D\" x3=\"1\" y3=\"2\" z3=\"3\" />\n"
    "    </atomArray>\n"
    "  </molecule>\n"
    "  <molecule id=\"m2\">\n"
    "    <crystal>\n"
    "      <scalar units=\"units:angstrom\" title=\"a\">22</scalar>\n"
    "      <scalar units=\"units:angstrom\" title=\"b\">22</scalar>\n"
    "      <scalar units=\"units:angstrom\" title=\"c\">22</scalar>\n"
    "      <scalar units=\"units:degree\" title=\"alpha\">90</scalar>\n"
    "      <scalar units=\"units:degree\" title=\"beta\">90</scalar>\n"
    "      <scalar units=\"units:degree\" title=\"gamma\">90</scalar>\n"
    "    </crystal>\n"
    "    <propertyList>\n"
    "      <property title=\"name\">\n"
    "        <scalar dataType=\"xsd:string\">test</scalar>\n"
    "      </property>\n"
    "      <property title=\"is_organic\">\n"
    "        <scalar dataType=\"xsd:boolean\">false</scalar>\n"
    "      </property>\n"
    "    </propertyList>\n"
    "    <atomArray>\n"
    "      <atom id=\"a1\" elementType=\"A\" x3=\"4\" y3=\"5\" z3=\"6\" />\n"
    "      <atom id=\"a2\" elementType=\"B\" x3=\"1\" y3=\"2\" z3=\"3\" />\n"
    "      <atom id=\"a3\" elementType=\"C\" x3=\"1\" y3=\"2\" z3=\"3\">\n"
    "        <scalar title=\"num_c\" dataType=\"xsd:double\">1</scalar>\n"
    "        <scalar title=\"force\" dataType=\"xsd:string\">1.000000 2.000000 3.000000</scalar>\n"
    "      </atom>\n"
    "      <atom id=\"a4\" elementType=\"D\" x3=\"1\" y3=\"2\" z3=\"3\" />\n"
    "    </atomArray>\n"
    "    <bondArray>\n"
    "      <bond atomRefs2=\"a1 a2\" order=\"1\" />\n"
    "      <bond atomRefs2=\"a1 a3\" />\n"
    "      <bond atomRefs2=\"a1 a4\" order=\"a\" />\n"
    "      <bond atomRefs2=\"a2 a3\" order=\"3\" />\n"
    "      <bond atomRefs2=\"a3 a4\" order=\"2\" />\n"
    "    </bondArray>\n"
    "  </molecule>\n"
    "</cml>\n";

    auto frame = Frame();
    frame.add_atom(Atom("A"), {1, 2, 3});
    frame.add_atom(Atom("B"), {1, 2, 3});
    frame.add_atom(Atom("C"), {1, 2, 3});
    frame.add_atom(Atom("D"), {1, 2, 3});

    auto file = Trajectory(tmpfile, 'w');
    file.write(frame);

    frame.positions()[0] = {4.0, 5.0, 6.0};
    frame.set_cell(UnitCell(22));

    frame.set("is_organic", false);
    frame.set("name", "test");

    frame[2].set("force", Vector3D{1., 2., 3.});
    frame[2].set("num_c", 1.0);

    frame.add_bond(0, 1, Bond::SINGLE);
    frame.add_bond(1, 2, Bond::TRIPLE);
    frame.add_bond(2, 3, Bond::DOUBLE);
    frame.add_bond(3, 0, Bond::AROMATIC);
    frame.add_bond(0, 2, Bond::UNKNOWN);

    file.write(frame);

    file.close();
    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(EXPECTED_CONTENT == content);
}

TEST_CASE("Append CML file") {
    auto tmpfile = NamedTempPath(".cml");
    const auto EXPECTED_CONTENT = // note the lack of indentation
    "<molecule>\n"
    "<atomArray>\n"
    "<atom id=\"a1\" elementType=\"A\" x3=\"1\" y3=\"2\" z3=\"3\" />\n"
    "<atom id=\"a2\" elementType=\"B\" x3=\"1\" y3=\"2\" z3=\"3\" />\n"
    "<atom id=\"a3\" elementType=\"C\" x3=\"1\" y3=\"2\" z3=\"3\" />\n"
    "<atom id=\"a4\" elementType=\"D\" x3=\"1\" y3=\"2\" z3=\"3\" />\n"
    "</atomArray>\n"
    "</molecule>\n";

    auto frame = Frame();
    frame.add_atom(Atom("A"), {1, 2, 3});
    frame.add_atom(Atom("B"), {1, 2, 3});
    frame.add_atom(Atom("C"), {1, 2, 3});
    frame.add_atom(Atom("D"), {1, 2, 3});

    auto file = Trajectory(tmpfile, 'a');
    file.write(frame);
    file.close();

    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());

    CHECK(EXPECTED_CONTENT == content);
}
