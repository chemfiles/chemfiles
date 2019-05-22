// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

TEST_CASE("chfl_atom") {
    SECTION("Constructors errors") {
        fail_next_allocation();
        CHECK(chfl_atom("He") == nullptr);

        CHFL_ATOM* atom = chfl_atom("He");
        REQUIRE(atom);

        fail_next_allocation();
        CHECK(chfl_atom_copy(atom) == nullptr);

        chfl_free(atom);
    }

    SECTION("copy") {
        CHFL_ATOM* atom = chfl_atom("He");
        REQUIRE(atom);

        CHFL_ATOM* copy = chfl_atom_copy(atom);
        REQUIRE(copy);

        char name[32];
        CHECK_STATUS(chfl_atom_name(atom, name, sizeof(name)));
        CHECK(name == std::string("He"));

        CHECK_STATUS(chfl_atom_name(copy, name, sizeof(name)));
        CHECK(name == std::string("He"));

        CHECK_STATUS(chfl_atom_set_name(atom, "Zr"));

        CHECK_STATUS(chfl_atom_name(atom, name, sizeof(name)));
        CHECK(name == std::string("Zr"));

        CHECK_STATUS(chfl_atom_name(copy, name, sizeof(name)));
        CHECK(name == std::string("He"));

        chfl_free(copy);
        chfl_free(atom);
    }

    SECTION("Name") {
        CHFL_ATOM* atom = chfl_atom("He");
        REQUIRE(atom);

        char name[32];
        CHECK_STATUS(chfl_atom_name(atom, name, sizeof(name)));
        CHECK(name == std::string("He"));

        CHECK_STATUS(chfl_atom_set_name(atom, "H5"));
        CHECK_STATUS(chfl_atom_name(atom, name, sizeof(name)));
        CHECK(name == std::string("H5"));

        chfl_free(atom);
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

        chfl_free(atom);
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

        chfl_free(atom);
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

        chfl_free(atom);
    }

    SECTION("Radius") {
        CHFL_ATOM* atom = chfl_atom("Zn");
        REQUIRE(atom);

        double radius = 0;
        CHECK_STATUS(chfl_atom_vdw_radius(atom, &radius));
        CHECK(radius == 2.1);

        CHECK_STATUS(chfl_atom_covalent_radius(atom, &radius));
        CHECK(radius == 1.31);

        chfl_free(atom);

        atom = chfl_atom("");
        REQUIRE(atom);

        CHECK_STATUS(chfl_atom_vdw_radius(atom, &radius));
        CHECK(radius == 0.0);

        radius = 10;
        CHECK_STATUS(chfl_atom_covalent_radius(atom, &radius));
        CHECK(radius == 0.0);

        chfl_free(atom);
    }

    SECTION("Number") {
        CHFL_ATOM* atom = chfl_atom("Zn");
        REQUIRE(atom);

        uint64_t number = 0;
        CHECK_STATUS(chfl_atom_atomic_number(atom, &number));
        CHECK(number == 30);

        chfl_free(atom);

        atom = chfl_atom("");
        REQUIRE(atom);

        CHECK_STATUS(chfl_atom_atomic_number(atom, &number));
        CHECK(number == 0);

        chfl_free(atom);
    }

    SECTION("Property") {
        CHFL_ATOM* atom = chfl_atom("Zn");
        REQUIRE(atom);

        CHFL_PROPERTY* property = chfl_property_double(-23);
        REQUIRE(property);

        CHECK_STATUS(chfl_atom_set_property(atom, "this", property));
        chfl_free(property);
        property = nullptr;

        property = chfl_atom_get_property(atom, "this");
        double value = 0;
        CHECK_STATUS(chfl_property_get_double(property, &value));
        CHECK(value == -23);
        chfl_free(property);
        property = nullptr;

        CHECK_FALSE(chfl_atom_get_property(atom, "that"));

        property = chfl_property_bool(false);
        REQUIRE(property);

        CHECK_STATUS(chfl_atom_set_property(atom, "that", property));
        chfl_free(property);

        uint64_t count = 0;
        CHECK_STATUS(chfl_atom_properties_count(atom, &count));
        CHECK(count == 2);

        const char* names[2] = {nullptr};
        CHECK_STATUS(chfl_atom_list_properties(atom, names, count));
        // There are no guarantee of ordering
        if (names[0] == std::string("this")) {
            CHECK(names[1] == std::string("that"));
        } else {
            CHECK(names[0] == std::string("that"));
            CHECK(names[1] == std::string("this"));
        }

        chfl_free(atom);
    }
}
