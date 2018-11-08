// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Residue class usage") {
    SECTION("name") {
        auto residue = Residue("ALA", 4);
        CHECK(residue.name() == "ALA");

        residue = Residue("GUA");
        CHECK(residue.name() == "GUA");
    }

    SECTION("id") {
        auto residue = Residue("ALA", 4);
        CHECK(residue.id().value() == 4);

        residue = Residue("GUA");
        CHECK_FALSE(residue.id());
    }

    SECTION("Atoms") {
        auto residue = Residue("ALA", 4);

        CHECK(residue.size() == 0);
        residue.add_atom(0);
        residue.add_atom(56);
        residue.add_atom(30);
        CHECK(residue.size() == 3);

        residue.add_atom(56);
        CHECK(residue.size() == 3);

        auto atoms = std::vector<size_t>(residue.begin(), residue.end());
        auto expected = std::vector<size_t>{0, 30, 56};
        CHECK(atoms == expected);

        CHECK(residue.contains(56));
    }

    SECTION("Properties") {
        auto residue = Residue("foo");
        residue.set("foo", 35);
        residue.set("bar", false);

        CHECK(residue.get("foo")->as_double() == 35.0);
        CHECK(residue.get("bar")->as_bool() == false);

        residue.set("foo", "test");
        CHECK(residue.get("foo")->as_string() == "test");
        CHECK_FALSE(residue.get("not here"));

        // Iterate over all properties
        residue.set("buzz", 22);
        residue.set("fizz", Vector3D(1, 2, 3));
        for(auto it: residue.properties()) {
            auto name = it.first;
            if (name == "bar") {
                CHECK(it.second.as_bool() == false);
            } else if (name == "foo") {
                CHECK(it.second.as_string() == "test");
            } else if (name == "buzz") {
                CHECK(it.second.as_double() == 22);
            } else if (name == "fizz") {
                CHECK(it.second.as_vector3d() == Vector3D(1, 2, 3));
            } else {
                CHECK(false);  // all case should have been covered
            }
        }

        // Typed access to properties
        CHECK(residue.get<Property::BOOL>("bar").value() == false);
        CHECK_FALSE(residue.get<Property::STRING>("bar"));
        CHECK_FALSE(residue.get<Property::DOUBLE>("bar"));
        CHECK_FALSE(residue.get<Property::VECTOR3D>("bar"));

        CHECK(residue.get<Property::STRING>("foo").value() == "test");
        CHECK_FALSE(residue.get<Property::BOOL>("foo"));
        CHECK_FALSE(residue.get<Property::DOUBLE>("foo"));
        CHECK_FALSE(residue.get<Property::VECTOR3D>("foo"));

        CHECK(residue.get<Property::DOUBLE>("buzz").value() == 22);
        CHECK_FALSE(residue.get<Property::BOOL>("buzz"));
        CHECK_FALSE(residue.get<Property::STRING>("buzz"));
        CHECK_FALSE(residue.get<Property::VECTOR3D>("buzz"));

        CHECK(residue.get<Property::VECTOR3D>("fizz").value() == Vector3D(1, 2, 3));
        CHECK_FALSE(residue.get<Property::BOOL>("fizz"));
        CHECK_FALSE(residue.get<Property::STRING>("fizz"));
        CHECK_FALSE(residue.get<Property::DOUBLE>("fizz"));
    }
}
