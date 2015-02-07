#include "catch.hpp"

#include "Harp.hpp"
#include "Atom.hpp"
using namespace harp;


TEST_CASE("Use the Atom type", "[Atoms]"){
    Atom a1("H");
    Atom a2 = Atom();
    Atom a3(Atom::CORSE_GRAIN, "CH4");

    SECTION("Check constructors"){
        CHECK(a1.name() == "H");
        //TODO: CHECK(a1.mass() == 1.01);
        CHECK(a1.type() == Atom::ELEMENT);
        CHECK(a1.charge() == 0);

        CHECK(a2.type() == Atom::UNDEFINED);
        CHECK(a2.name() == "");
        CHECK(a2.mass() == 0);
        CHECK(a2.charge() == 0);

        CHECK(a3.type() == Atom::CORSE_GRAIN);
        CHECK(a3.name() == "CH4");
        //TODO: CHECK(a3.mass() == 0);
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
    }
}
