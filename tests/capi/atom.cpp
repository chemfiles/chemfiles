// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

TEST_CASE("chfl_atom") {
    SECTION("Name") {
        CHFL_ATOM* atom = chfl_atom("He");
        REQUIRE(atom);

        char name[32];
        CHECK_STATUS(chfl_atom_name(atom, name, sizeof(name)));
        CHECK(name == std::string("He"));

        CHECK_STATUS(chfl_atom_set_name(atom, "H5"));
        CHECK_STATUS(chfl_atom_name(atom, name, sizeof(name)));
        CHECK(name == std::string("H5"));

        CHECK_STATUS(chfl_atom_free(atom));
    }

    SECTION("Type") {
        CHFL_ATOM* atom = chfl_atom("He");
        REQUIRE(atom);

        char type[32];
        CHECK_STATUS(chfl_atom_type(atom, type, sizeof(type)));
        CHECK(type == std::string("He"));

        char name[32];
        CHECK_STATUS(chfl_atom_full_name(atom, name, sizeof(name)));
        CHECK(name == std::string("Helium"));

        CHECK_STATUS(chfl_atom_set_type(atom, "Zn"));
        CHECK_STATUS(chfl_atom_type(atom, type, sizeof(type)));
        CHECK(type == std::string("Zn"));

        CHECK_STATUS(chfl_atom_full_name(atom, name, sizeof(name)));
        CHECK(name == std::string("Zinc"));

        CHECK_STATUS(chfl_atom_free(atom));
    }

    SECTION("Mass") {
        CHFL_ATOM* atom = chfl_atom("He");
        REQUIRE(atom);

        double mass = 0;
        CHECK_STATUS(chfl_atom_mass(atom, &mass));
        CHECK(mass == 4.002602);

        CHECK_STATUS(chfl_atom_set_mass(atom, 678));
        CHECK_STATUS(chfl_atom_mass(atom, &mass));
        CHECK(mass == 678);

        CHECK_STATUS(chfl_atom_free(atom));
    }

    SECTION("Charge") {
        CHFL_ATOM* atom = chfl_atom("He");
        REQUIRE(atom);

        double charge = 0;
        CHECK_STATUS(chfl_atom_charge(atom, &charge));
        CHECK(charge == 0.0);

        CHECK_STATUS(chfl_atom_set_charge(atom, -1.8));
        CHECK_STATUS(chfl_atom_charge(atom, &charge));
        CHECK(charge == -1.8);

        CHECK_STATUS(chfl_atom_free(atom));
    }

    SECTION("Radius") {
        CHFL_ATOM* atom = chfl_atom("Zn");
        REQUIRE(atom);

        double radius = 0;
        CHECK_STATUS(chfl_atom_vdw_radius(atom, &radius));
        CHECK(radius == 2.1);

        CHECK_STATUS(chfl_atom_covalent_radius(atom, &radius));
        CHECK(radius == 1.31);

        CHECK_STATUS(chfl_atom_free(atom));

        atom = chfl_atom("");
        REQUIRE(atom);

        CHECK_STATUS(chfl_atom_vdw_radius(atom, &radius));
        CHECK(radius == 0.0);

        radius = 10;
        CHECK_STATUS(chfl_atom_covalent_radius(atom, &radius));
        CHECK(radius == 0.0);

        CHECK_STATUS(chfl_atom_free(atom));
    }

    SECTION("Number") {
        CHFL_ATOM* atom = chfl_atom("Zn");
        REQUIRE(atom);

        uint64_t number = 0;
        CHECK_STATUS(chfl_atom_atomic_number(atom, &number));
        CHECK(number == 30);

        CHECK_STATUS(chfl_atom_free(atom));

        atom = chfl_atom("");
        REQUIRE(atom);

        CHECK_STATUS(chfl_atom_atomic_number(atom, &number));
        CHECK(number == 0);

        CHECK_STATUS(chfl_atom_free(atom));
    }

    SECTION("Property") {
        CHFL_ATOM* atom = chfl_atom("Zn");
        REQUIRE(atom);

        CHFL_PROPERTY* property = chfl_property_double(-23);
        REQUIRE(property);

        CHECK_STATUS(chfl_atom_set_property(atom, "this", property));
        CHECK_STATUS(chfl_property_free(property));
        property = nullptr;

        property = chfl_atom_get_property(atom, "this");
        double value = 0;
        CHECK_STATUS(chfl_property_get_double(property, &value));
        CHECK(value == -23);

        CHECK_FALSE(chfl_atom_get_property(atom, "that"));

        CHECK_STATUS(chfl_property_free(property));
        CHECK_STATUS(chfl_atom_free(atom));
    }
}
