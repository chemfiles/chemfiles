#include "catch.hpp"

#include "Chemharp.hpp"
#include "Topology.hpp"
using namespace harp;

TEST_CASE("Use the Topology class", "[Topology]"){

    SECTION("Basic usage"){
        auto topo = Topology(42);
        CHECK(topo.atom_list().capacity() == 42);

        topo.append(Atom("H"));
        CHECK(topo[0].type() == Atom::ELEMENT);
        CHECK(topo[0].name() == "H");

        topo.reserve(100);
        CHECK(topo.atom_list().capacity() == 100);
    }

    SECTION("Dummy topology provider"){
        auto topo = dummy_topology(42);
        CHECK(topo.atom_list().capacity() == 42);
        CHECK(topo.atom_list().size() == 42);

        CHECK(topo.atom_types().size() == 1);
        CHECK(topo.atom_types()[0] == Atom(Atom::UNDEFINED));
        CHECK(topo[10] == Atom(Atom::UNDEFINED));
    }

    SECTION("Add atoms and liaisons"){
        auto topo = Topology(6);

        for (int i=0; i<4; i++)
            topo.append(Atom("H"));
        topo.append(Atom("O"));
        topo.append(Atom("O"));

        topo.add_bond(0, 4);
        topo.add_bond(1, 4);
        topo.add_bond(2, 5);
        topo.add_bond(3, 5);

        CHECK(topo.atom_types().size() == 2);
        CHECK(topo.bonds().size() == 8);

        CHECK(topo.bonds()[0] == bond(0, 4));
        CHECK(topo.bonds()[3] == bond(3, 5));
        for (size_t i=0; i<4; i++) {
            CHECK(topo.bonds()[i][0] == topo.bonds()[i + 4][1]);
            CHECK(topo.bonds()[i][1] == topo.bonds()[i + 4][0]);
        }

        CHECK(topo.angles().size() == 4);
        CHECK(topo.angles()[0] == angle(0, 4, 1));
        CHECK(topo.angles()[1] == angle(1, 4, 0));
        CHECK(topo.angles()[2] == angle(2, 5, 3));
        CHECK(topo.angles()[3] == angle(3, 5, 2));

        topo.append(Atom("O"));
        topo.add_bond(3, 6);
        CHECK(topo.bonds().size() == 10);
        CHECK(topo.dihedrals().size() == 2);
        CHECK(topo.dihedrals()[0] == dihedral(2, 5, 3, 6));
        CHECK(topo.dihedrals()[1] == dihedral(6, 3, 5, 2));


        topo.add_bond(5, 5); // This should do nothing
        CHECK(topo.bonds().size() == 10);

        for (size_t i=0; i<6; i++) {
            CHECK(topo.atom(i) == topo[i]);
        }
    }
}
