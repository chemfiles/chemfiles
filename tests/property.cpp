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
        CHECK(property.kind() == Property::BOOL);
        CHECK(property == Property(false));

        CHECK(property != Property(0));
        CHECK(property != Property("false"));

        CHECK_THROWS_AS(property.as_double(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);
    }

    SECTION("Double") {
        auto property = Property(42.0);

        CHECK(property.as_double() == 42.0);
        CHECK(property.kind() == Property::DOUBLE);

        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(23);
        CHECK(property.as_double() == 23.0);
        CHECK(property.kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(24l);
        CHECK(property.as_double() == 24.0);
        CHECK(property.kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(25ll);
        CHECK(property.as_double() == 25.0);
        CHECK(property.kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(26u);
        CHECK(property.as_double() == 26.0);
        CHECK(property.kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(27ul);
        CHECK(property.as_double() == 27.0);
        CHECK(property.kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property(28ull);
        CHECK(property.as_double() == 28.0);
        CHECK(property.kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        short val_short = 29;
        property = Property(val_short);
        CHECK(property.as_double() == 29.0);
        CHECK(property.kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        char val_char = 30;
        property = Property(val_char);
        CHECK(property.as_double() == 30.0);
        CHECK(property.kind() == Property::DOUBLE);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);
        CHECK(property == Property(30.0));
    }

    SECTION("String") {
        auto property = Property(std::string("test"));
        CHECK(property.as_string() == "test");
        CHECK(property.kind() == Property::STRING);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_double(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        property = Property("test-2");
        CHECK(property.as_string() == "test-2");
        CHECK(property.kind() == Property::STRING);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_double(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        char data[] = {'e', 'm', 'p', 't', 'y', '\0'};
        property = Property(data);
        CHECK(property.as_string() == "empty");
        CHECK(property.kind() == Property::STRING);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_double(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);

        char* val_char = data;
        property = Property(val_char);
        CHECK(property.as_string() == "empty");
        CHECK(property.kind() == Property::STRING);
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_double(), PropertyError);
        CHECK_THROWS_AS(property.as_vector3d(), PropertyError);
        CHECK(property == Property("empty"));
    }

    SECTION("Vector3D") {
        auto property = Property(Vector3D(0.0, 1.1, 2.2));

        CHECK(property.as_vector3d() == Vector3D(0.0, 1.1, 2.2));
        CHECK(property.kind() == Property::VECTOR3D);
        CHECK(property == Property(Vector3D(0.0, 1.1, 2.2)));
        CHECK(property != Property(Vector3D(0.1, 1.1, 2.2)));
        CHECK_THROWS_AS(property.as_bool(), PropertyError);
        CHECK_THROWS_AS(property.as_string(), PropertyError);
        CHECK_THROWS_AS(property.as_double(), PropertyError);

        // Checking copy and move constructors
        property = Property(Vector3D(0, 0, 0));
        CHECK(property.as_vector3d() == Vector3D(0, 0, 0));

        auto new_prop = property;
        CHECK(new_prop.as_vector3d() == Vector3D(0, 0, 0));
    }
}

TEST_CASE("Property map") {
    auto map = property_map();
    map.set("foo", 33);
    map.set("bar", "barbar");

    auto property = map.get("foo");
    CHECK(property);
    CHECK(property->kind() == Property::DOUBLE);
    CHECK(property->as_double() == 33.0);

    CHECK(map.get<Property::DOUBLE>("foo").value() == 33.0);
    CHECK(map.get<Property::STRING>("bar").value() == "barbar");

    CHECK_FALSE(map.get<Property::BOOL>("bar"));
    CHECK_FALSE(map.get<Property::DOUBLE>("bar"));
    CHECK_FALSE(map.get<Property::VECTOR3D>("bar"));
}
