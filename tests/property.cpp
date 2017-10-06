// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "helpers.hpp"
#include "chemfiles/Property.hpp"
using namespace chemfiles;

TEST_CASE("Property") {
    SECTION("Bool") {
        auto property = Property(false);

        CHECK(property.as_bool() == false);
        CHECK(property.get_kind() == Property::BOOL);

        CHECK_THROWS_AS(property.as_double(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);
    }

    SECTION("Double") {
        auto property = Property(42.0);

        CHECK(property.as_double() == 42.0);
        CHECK(property.get_kind() == Property::DOUBLE);

        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(23.0f);
        CHECK(property.as_double() == 23.0);
        CHECK(property.get_kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(23);
        CHECK(property.as_double() == 23.0);
        CHECK(property.get_kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(24l);
        CHECK(property.as_double() == 24.0);
        CHECK(property.get_kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(25ll);
        CHECK(property.as_double() == 25.0);
        CHECK(property.get_kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(26u);
        CHECK(property.as_double() == 26.0);
        CHECK(property.get_kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(27ul);
        CHECK(property.as_double() == 27.0);
        CHECK(property.get_kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(28ull);
        CHECK(property.as_double() == 28.0);
        CHECK(property.get_kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        short val_short = 29;
        property = Property(val_short);
        CHECK(property.as_double() == 29.0);
        CHECK(property.get_kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        char val_char = 30;
        property = Property(val_char);
        CHECK(property.as_double() == 30.0);
        CHECK(property.get_kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);
    }

    SECTION("String") {
        auto property = Property(std::string("test"));
        CHECK(property.as_string() == "test");
        CHECK(property.get_kind() == Property::STRING);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_double(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property("test-2");
        CHECK(property.as_string() == "test-2");
        CHECK(property.get_kind() == Property::STRING);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_double(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        char data[] = {'e', 'm', 'p', 't', 'y', '\0'};
        property = Property(data);
        CHECK(property.as_string() == "empty");
        CHECK(property.get_kind() == Property::STRING);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_double(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        char* val_char = data;
        property = Property(val_char);
        CHECK(property.as_string() == "empty");
        CHECK(property.get_kind() == Property::STRING);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_double(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);
    }

    SECTION("Vector3D") {
        auto property = Property(Vector3D(0.0, 1.1, 2.2));

        CHECK(property.as_vector3d() == Vector3D(0.0, 1.1, 2.2));
        CHECK(property.get_kind() == Property::VECTOR3D);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_double(), PropertyError);
    }
}
