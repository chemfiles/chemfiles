// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

TEST_CASE("Property") {
    SECTION("Constructors errors") {
        fail_next_allocation();
        CHECK(chfl_property_bool(false) == nullptr);

        fail_next_allocation();
        CHECK(chfl_property_double(42) == nullptr);

        fail_next_allocation();
        CHECK(chfl_property_string("foo") == nullptr);

        chfl_vector3d initial = {1, 3, 4};
        fail_next_allocation();
        CHECK(chfl_property_vector3d(initial) == nullptr);
    }

    SECTION("Bool") {
        CHFL_PROPERTY* property = chfl_property_bool(false);
        REQUIRE(property);

        bool value = true;
        CHECK_STATUS(chfl_property_get_bool(property, &value));
        CHECK(value == false);

        char dummy_char = '\0';
        CHECK(chfl_property_get_string(property, &dummy_char, 0) == CHFL_PROPERTY_ERROR);
        double dummy_double = 0;
        CHECK(chfl_property_get_double(property, &dummy_double) == CHFL_PROPERTY_ERROR);
        chfl_vector3d dummy_vector = {0};
        CHECK(chfl_property_get_vector3d(property, dummy_vector) == CHFL_PROPERTY_ERROR);

        chfl_property_kind kind;
        CHECK_STATUS(chfl_property_get_kind(property, &kind));
        CHECK(kind == CHFL_PROPERTY_BOOL);

        chfl_free(property);
    }

    SECTION("Double") {
        CHFL_PROPERTY* property = chfl_property_double(42);
        REQUIRE(property);

        double value = 0;
        CHECK_STATUS(chfl_property_get_double(property, &value));
        CHECK(value == 42.0);

        char dummy_char = '\0';
        CHECK(chfl_property_get_string(property, &dummy_char, 0) == CHFL_PROPERTY_ERROR);
        bool dummy_bool = false;
        CHECK(chfl_property_get_bool(property, &dummy_bool) == CHFL_PROPERTY_ERROR);
        chfl_vector3d dummy_vector = {0};
        CHECK(chfl_property_get_vector3d(property, dummy_vector) == CHFL_PROPERTY_ERROR);

        chfl_property_kind kind;
        CHECK_STATUS(chfl_property_get_kind(property, &kind));
        CHECK(kind == CHFL_PROPERTY_DOUBLE);

        chfl_free(property);
    }

    SECTION("String") {
        CHFL_PROPERTY* property = chfl_property_string("foobar");
        REQUIRE(property);

        char value[32];
        CHECK_STATUS(chfl_property_get_string(property, value, sizeof(value)));
        CHECK(value == std::string("foobar"));

        double dummy_double = 0;
        CHECK(chfl_property_get_double(property, &dummy_double) == CHFL_PROPERTY_ERROR);
        bool dummy_bool = false;
        CHECK(chfl_property_get_bool(property, &dummy_bool) == CHFL_PROPERTY_ERROR);
        chfl_vector3d dummy_vector = {0};
        CHECK(chfl_property_get_vector3d(property, dummy_vector) == CHFL_PROPERTY_ERROR);

        chfl_property_kind kind;
        CHECK_STATUS(chfl_property_get_kind(property, &kind));
        CHECK(kind == CHFL_PROPERTY_STRING);

        chfl_free(property);
    }

    SECTION("Vector3D") {
        chfl_vector3d initial = {1, 3, 4};
        CHFL_PROPERTY* property = chfl_property_vector3d(initial);
        REQUIRE(property);

        chfl_vector3d value = {0};
        CHECK_STATUS(chfl_property_get_vector3d(property, value));
        CHECK(value[0] == 1.0);
        CHECK(value[1] == 3.0);
        CHECK(value[2] == 4.0);

        char dummy_char = '\0';
        CHECK(chfl_property_get_string(property, &dummy_char, 0) == CHFL_PROPERTY_ERROR);
        bool dummy_bool = false;
        CHECK(chfl_property_get_bool(property, &dummy_bool) == CHFL_PROPERTY_ERROR);
        double dummy_double = 0;
        CHECK(chfl_property_get_double(property, &dummy_double) == CHFL_PROPERTY_ERROR);

        chfl_property_kind kind;
        CHECK_STATUS(chfl_property_get_kind(property, &kind));
        CHECK(kind == CHFL_PROPERTY_VECTOR3D);

        chfl_free(property);
    }
}
