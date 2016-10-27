#include <catch.hpp>

#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Use the Topology class", "[Topology]") {
    auto topology = Topology();

    topology.append(Atom("H"));
    CHECK(topology[0].element() == "H");

    topology.append(Atom("H"));
    CHECK(topology[1].element() == "H");

    topology.add_bond(0, 1);
    CHECK(topology.bonds().size() == 1);
    CHECK(topology.bonds()[0] == Bond(0, 1));
}

TEST_CASE("Add and remove items in the topology", "[Topology]") {
    auto topo = Topology();

    for (unsigned i=0; i<4; i++) {
        topo.append(Atom("H"));
    }
    topo.append(Atom("O"));
    topo.append(Atom("O"));

    topo.add_bond(0, 4);
    topo.add_bond(1, 4);
    topo.add_bond(2, 5);
    topo.add_bond(3, 5);

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

TEST_CASE("Residues in topologies", "[Topology]") {
    auto topo = Topology();
    for (unsigned i=0; i<10; i++) {
        topo.append(Atom("X"));
    }

    size_t residues[3][3] = {{2, 3, 6}, {0, 1, 9}, {4, 5, 8}};
    for (auto& atoms: residues) {
        auto residue = Residue("X");
        for (auto i: atoms) {
            residue.add_atom(i);
        }
        topo.add_residue(std::move(residue));
    }

    CHECK(topo.residues().size() == 3);


    auto residue = Residue("X");
    residue.add_atom(2);
    CHECK_THROWS_AS(topo.add_residue(residue), Error);

    auto res_1 = topo.residue(2);
    CHECK(res_1);
    auto res_2 = topo.residue(0);
    CHECK(res_2);

    CHECK_FALSE(topo.residue(7));

    CHECK_FALSE(topo.are_linked(*res_1, *res_2));
    topo.add_bond(6, 9);
    CHECK(topo.are_linked(*res_1, *res_2));
}
