#include <catch.hpp>

#define private public
#include "chemfiles/Topology.hpp"
#undef private

#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Use the Topology class", "[Topology]"){

    SECTION("Basic usage"){
        auto topo = Topology(42);
        CHECK(topo.atoms_.capacity() == 42);

        topo.append(Atom("H"));
        CHECK(topo[0].type() == Atom::ELEMENT);
        CHECK(topo[0].name() == "H");

        topo.resize(100);
        CHECK(topo.atoms_.size() == 100);
    }

    SECTION("Dummy topology provider"){
        auto topo = dummy_topology(42);
        CHECK(topo.natoms() == 42);

        CHECK(topo.natom_types() == 1);
        CHECK(topo.templates_[0] == Atom(Atom::UNDEFINED));
        CHECK(topo[10] == Atom(Atom::UNDEFINED));
    }

    SECTION("Add and remove items in the topology"){
        auto topo = Topology();

        for (int i=0; i<4; i++)
            topo.append(Atom("H"));
        topo.append(Atom("O"));
        topo.append(Atom("O"));

        topo.add_bond(0, 4);
        topo.add_bond(1, 4);
        topo.add_bond(2, 5);
        topo.add_bond(3, 5);

        CHECK(topo.natom_types() == 2);

        CHECK(topo.bonds().size() == 4);
        CHECK(topo.isbond(0, 4));
        CHECK(topo.isbond(1, 4));
        CHECK(topo.isbond(2, 5));
        CHECK(topo.isbond(3, 5));

        CHECK(topo.angles().size() == 2);
        CHECK(topo.isangle(0, 4, 1));
        CHECK(topo.isangle(2, 5, 3));

        topo.append(Atom("O"));
        topo.add_bond(3, 6);
        CHECK(topo.bonds().size() == 5);
        CHECK(topo.dihedrals().size() == 1);
        CHECK(topo.isdihedral(2, 5, 3, 6));

        topo.remove(6);
        CHECK(topo.natoms() == 6);
        CHECK(topo.bonds().size() == 4);

        topo.remove_bond(0, 4);
        topo.remove_bond(1, 4);
        CHECK(topo.natoms() == 6);
        CHECK(topo.bonds().size() == 2);

        CHECK_FALSE(topo.isbond(0, 4));
        CHECK_FALSE(topo.isbond(1, 4));
        CHECK_FALSE(topo.isangle(0, 4, 1));
    }
}
