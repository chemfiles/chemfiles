#include "catch.hpp"

#include "Chemharp.hpp"
#include "chemharp/Atom.hpp"
using namespace harp;


TEST_CASE("Use the Atom type", "[Atoms]"){
    Atom a1("H");
    Atom a2 = Atom();
    Atom a3(Atom::CORSE_GRAIN, "CH4");
    Atom a4("W");

    SECTION("Check constructors"){
        CHECK(a1.name() == "H");
        CHECK(a1.mass() == 1.008f);
        CHECK(a1.type() == Atom::ELEMENT);
        CHECK(a1.charge() == 0);

        CHECK(a2.type() == Atom::UNDEFINED);
        CHECK(a2.name() == "");
        CHECK(a2.mass() == 0);
        CHECK(a2.charge() == 0);

        CHECK(a3.type() == Atom::CORSE_GRAIN);
        CHECK(a3.name() == "CH4");
        CHECK(a3.mass() == 0);
        CHECK(a3.charge() == 0);
    }

    SECTION("Set and get properties"){
        a1.type(Atom::DUMMY);
        CHECK(a1.type() == Atom::DUMMY);

        a1.mass(14.789f);
        CHECK(a1.mass() == 14.789f);

        a1.charge(-2);
        CHECK(a1.charge() == -2);

        a1.name("foo");
        CHECK(a1.name() == "foo");

        CHECK(a4.mass() == 183.84f);
        CHECK(a4.atomic_number() == 74);
        CHECK(a4.full_name() == "Tungsten");
        CHECK(a4.covalent_radius() == 1.46f);
        CHECK(a4.vdw_radius() == 2.1f);

        CHECK(a3.atomic_number() == -1);
        CHECK(a3.full_name() == "");
        CHECK(a3.covalent_radius() == -1.0f);
        CHECK(a3.vdw_radius() == -1.0f);
    }
}
