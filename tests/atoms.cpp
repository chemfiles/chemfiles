// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Use the Atom type") {
    SECTION("Constructors") {
        auto atom = Atom("H");
        CHECK(atom.name() == "H");
        CHECK(atom.type() == "H");
        CHECK(atom.mass() == 1.008);
        CHECK(atom.charge() == 0);

        atom = Atom();
        CHECK(atom.name() == "");
        CHECK(atom.type() == "");
        CHECK(atom.mass() == 0);
        CHECK(atom.charge() == 0);

        atom = Atom("C1", "C");
        CHECK(atom.name() == "C1");
        CHECK(atom.type() == "C");
        CHECK(atom.mass() == 12.011);
        CHECK(atom.charge() == 0);

        atom = Atom("name only", "");
        CHECK(atom.name() == "name only");
        CHECK(atom.type() == "");
        CHECK(atom.mass() == 0);
        CHECK(atom.charge() == 0);
    }

    SECTION("Set and get properties") {
        Atom atom;
        CHECK(atom.mass() == 0);
        atom.set_mass(14.789);
        CHECK(atom.mass() == 14.789);

        CHECK(atom.charge() == 0);
        atom.set_charge(-2);
        CHECK(atom.charge() == -2);

        CHECK(atom.name() == "");
        atom.set_name("HE22");
        CHECK(atom.name() == "HE22");

        CHECK(atom.type() == "");
        atom.set_type("foo");
        CHECK(atom.type() == "foo");
    }

    SECTION("Elements properties") {
        auto atom = Atom("H");
        CHECK(atom.atomic_number().value() == 1);
        CHECK(atom.full_name().value() == "Hydrogen");
        CHECK(atom.covalent_radius() == 0.37);
        CHECK(atom.vdw_radius() == 1.2);

        atom = Atom();
        CHECK_FALSE(atom.atomic_number());
        CHECK_FALSE(atom.full_name());
        CHECK_FALSE(atom.covalent_radius());
        CHECK_FALSE(atom.vdw_radius());

        atom = Atom("C2");
        CHECK_FALSE(atom.atomic_number());
        CHECK_FALSE(atom.full_name());
        CHECK_FALSE(atom.covalent_radius());
        CHECK_FALSE(atom.vdw_radius());

        atom = Atom("name only", "");
        CHECK_FALSE(atom.atomic_number());
        CHECK_FALSE(atom.full_name());
        CHECK_FALSE(atom.covalent_radius());
        CHECK_FALSE(atom.vdw_radius());

        atom = Atom("ZN");
        CHECK(atom.atomic_number().value() == 30);
        CHECK(atom.full_name().value() == "Zinc");
        CHECK(atom.covalent_radius().value() == 1.31);
        CHECK(atom.vdw_radius().value() == 2.1);

        atom = Atom("zn");
        CHECK(atom.atomic_number().value() == 30);
        CHECK(atom.full_name().value() == "Zinc");
        CHECK(atom.covalent_radius().value() == 1.31);
        CHECK(atom.vdw_radius().value() == 2.1);

        atom = Atom("zN");
        CHECK(atom.atomic_number().value() == 30);
        CHECK(atom.full_name().value() == "Zinc");
        CHECK(atom.covalent_radius().value() == 1.31);
        CHECK(atom.vdw_radius().value() == 2.1);

        atom = Atom("c");
        CHECK(atom.atomic_number().value() == 6);
        CHECK(atom.full_name().value() == "Carbon");
        CHECK(atom.covalent_radius().value() == 0.77);
        CHECK(atom.vdw_radius().value() == 1.7);
    }

    SECTION("Properties") {
        auto atom = Atom("H");
        atom.set("foo", 35);
        atom.set("bar", false);

        CHECK(atom.get("foo")->as_double() == 35.0);
        CHECK(atom.get("bar")->as_bool() == false);

        atom.set("foo", "test");
        CHECK(atom.get("foo")->as_string() == "test");
        CHECK_FALSE(atom.get("not here"));

        // Iterate over all properties
        atom.set("buzz", 22);
        atom.set("fizz", Vector3D(1, 2, 3));

        auto expected = std::vector<std::tuple<std::string, Property>>{
            std::tuple<std::string, Property>{"bar", false},
            std::tuple<std::string, Property>{"buzz", 22},
            std::tuple<std::string, Property>{"fizz", Vector3D(1, 2, 3)},
            std::tuple<std::string, Property>{"foo", "test"},
        };
        size_t i = 0;
        for(auto it: atom.properties()) {
            CHECK(it.first == std::get<0>(expected[i]));
            CHECK(it.second == std::get<1>(expected[i]));
            i += 1;
        }

        // Typed access to properties
        CHECK(atom.get<Property::BOOL>("bar").value() == false);
        CHECK_FALSE(atom.get<Property::STRING>("bar"));
        CHECK_FALSE(atom.get<Property::DOUBLE>("bar"));
        CHECK_FALSE(atom.get<Property::VECTOR3D>("bar"));

        CHECK(atom.get<Property::STRING>("foo").value() == "test");
        CHECK_FALSE(atom.get<Property::BOOL>("foo"));
        CHECK_FALSE(atom.get<Property::DOUBLE>("foo"));
        CHECK_FALSE(atom.get<Property::VECTOR3D>("foo"));

        CHECK(atom.get<Property::DOUBLE>("buzz").value() == 22);
        CHECK_FALSE(atom.get<Property::BOOL>("buzz"));
        CHECK_FALSE(atom.get<Property::STRING>("buzz"));
        CHECK_FALSE(atom.get<Property::VECTOR3D>("buzz"));

        CHECK(atom.get<Property::VECTOR3D>("fizz").value() == Vector3D(1, 2, 3));
        CHECK_FALSE(atom.get<Property::BOOL>("fizz"));
        CHECK_FALSE(atom.get<Property::STRING>("fizz"));
        CHECK_FALSE(atom.get<Property::DOUBLE>("fizz"));
    }
}
