#include <catch.hpp>

#include "chemfiles.hpp"
#include "chemfiles/Atom.hpp"
using namespace chemfiles;


TEST_CASE("Use the Atom type", "[Atoms]"){
    Atom a1("H");
    Atom a2 = Atom();
    Atom a3(Atom::COARSE_GRAINED, "CH4");
    Atom a4("W");
    Atom a5("C", "CB");
    Atom a6("", "Label only");
    Atom a7("", "Na");

    SECTION("Check constructors"){
        CHECK(a1.element() == "H");
        CHECK(a1.label() == "H");
        CHECK(a1.mass() == 1.008);
        CHECK(a1.type() == Atom::ELEMENT);
        CHECK(a1.charge() == 0);

        CHECK(a2.type() == Atom::UNDEFINED);
        CHECK(a2.element() == "");
        CHECK(a2.label() == "");
        CHECK(a2.mass() == 0);
        CHECK(a2.charge() == 0);

        CHECK(a3.type() == Atom::COARSE_GRAINED);
        CHECK(a3.element() == "CH4");
        CHECK(a3.label() == "CH4");
        CHECK(a3.mass() == 0);
        CHECK(a3.charge() == 0);

        CHECK(a5.type() == Atom::ELEMENT);
        CHECK(a5.element() == "C");
        CHECK(a5.label() == "CB");
        CHECK(a5.mass() == 12.011);
        CHECK(a5.charge() == 0);

        CHECK(a6.type() == Atom::COARSE_GRAINED);
        CHECK(a6.element() == "Label only");
        CHECK(a6.label() == "Label only");
        CHECK(a6.mass() == 0);
        CHECK(a6.charge() == 0);

        CHECK(a7.type() == Atom::ELEMENT);
        CHECK(a7.element() == "Na");
        CHECK(a7.label() == "Na");
        CHECK(a7.mass() == 22.98976928);
        CHECK(a7.charge() == 0);
    }

    SECTION("Set and get properties"){
        a1.set_type(Atom::DUMMY);
        CHECK(a1.type() == Atom::DUMMY);

        a1.set_mass(14.789);
        CHECK(a1.mass() == 14.789);

        a1.set_charge(-2);
        CHECK(a1.charge() == -2);

        a1.set_element("foo");
        CHECK(a1.element() == "foo");

        a1.set_label("HE22");
        CHECK(a1.label() == "HE22");

        CHECK(a4.mass() == 183.84);
        CHECK(a4.atomic_number() == 74);
        CHECK(a4.full_name() == "Tungsten");
        CHECK(a4.covalent_radius() == 1.46);
        CHECK(a4.vdw_radius() == 2.1);

        CHECK(a3.atomic_number() == -1);
        CHECK(a3.full_name() == "");
        CHECK(a3.covalent_radius() == -1.0);
        CHECK(a3.vdw_radius() == -1.0);
    }
}
