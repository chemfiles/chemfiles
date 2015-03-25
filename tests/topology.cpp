#include "catch.hpp"

#define private public
#include "Topology.hpp"
#undef private

#include "Chemharp.hpp"
using namespace harp;

template <class T>
bool find(const std::vector<T>& vec, const T& val){
    for (auto content : vec){
        if (val == content)
            return true;
    }
    return false;
}

TEST_CASE("Use the Topology class", "[Topology]"){

    SECTION("Basic usage"){
        auto topo = Topology(42);
        CHECK(topo._atoms.capacity() == 42);

        topo.append(Atom("H"));
        CHECK(topo[0].type() == Atom::ELEMENT);
        CHECK(topo[0].name() == "H");

        topo.resize(100);
        CHECK(topo._atoms.size() == 100);
    }

    SECTION("Dummy topology provider"){
        auto topo = dummy_topology(42);
        CHECK(topo.natoms() == 42);

        CHECK(topo.natom_types() == 1);
        CHECK(topo._templates[0] == Atom(Atom::UNDEFINED));
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

        CHECK(topo.natom_types() == 2);

        auto bonds = topo.bonds();
        CHECK(bonds.size() == 4);
        CHECK(find(bonds, bond(0, 4)));
        CHECK(find(bonds, bond(1, 4)));
        CHECK(find(bonds, bond(2, 5)));
        CHECK(find(bonds, bond(3, 5)));

        auto angles = topo.angles();
        CHECK(angles.size() == 2);
        CHECK(find(angles, angle(0, 4, 1)));
        CHECK(find(angles, angle(2, 5, 3)));

        topo.append(Atom("O"));
        topo.add_bond(3, 6);
        CHECK(topo.bonds().size() == 5);
        auto dihedrals = topo.dihedrals();
        CHECK(dihedrals.size() == 1);
        CHECK(find(dihedrals, dihedral(2, 5, 3, 6)));
    }
}
