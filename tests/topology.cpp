#include <catch.hpp>

#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Connectivity elements", "[Topology]") {
    SECTION("Bonds") {
        CHECK(Bond(2, 3) == Bond(3, 2));

        auto bond = Bond(45, 8);
        CHECK(bond[0] == 8);
        CHECK(bond[1] == 45);

        CHECK_THROWS_AS(Bond(2, 2), Error);

        CHECK(Bond(1, 3) < Bond(2, 3));
        CHECK(Bond(1, 2) < Bond(1, 3));
        CHECK_FALSE(Bond(1, 2) < Bond(1, 2));

        CHECK(Bond(2, 3) > Bond(1, 3));
        CHECK(Bond(1, 3) > Bond(1, 2));
        CHECK_FALSE(Bond(1, 2) > Bond(1, 2));
    }

    SECTION("Angles") {
        CHECK(Angle(2, 3, 4) == Angle(4, 3, 2));

        auto angle = Angle(4, 5, 8);
        CHECK(angle[0] == 4);
        CHECK(angle[1] == 5);
        CHECK(angle[2] == 8);

        CHECK_THROWS_AS(Angle(2, 2, 3), Error);
        CHECK_THROWS_AS(Angle(2, 3, 2), Error);
        CHECK_THROWS_AS(Angle(3, 2, 2), Error);

        CHECK(Angle(1, 3, 4) < Angle(2, 3, 4));
        CHECK(Angle(1, 2, 4) < Angle(1, 3, 4));
        CHECK(Angle(1, 2, 3) < Angle(1, 2, 4));
        CHECK_FALSE(Angle(1, 2, 3) < Angle(1, 2, 3));

        CHECK(Angle(2, 3, 4) > Angle(1, 3, 4));
        CHECK(Angle(1, 3, 4) > Angle(1, 2, 4));
        CHECK(Angle(1, 2, 4) > Angle(1, 2, 3));
        CHECK_FALSE(Angle(1, 2, 3) > Angle(1, 2, 3));
    }

    SECTION("Dihedral") {
        CHECK(Dihedral(2, 3, 4, 5) == Dihedral(5, 4, 3, 2));

        auto dihedral = Dihedral(6, 7, 5, 8);
        CHECK(dihedral[0] == 6);
        CHECK(dihedral[1] == 7);
        CHECK(dihedral[2] == 5);
        CHECK(dihedral[3] == 8);

        CHECK_THROWS_AS(Dihedral(2, 2, 3, 4), Error);
        CHECK_THROWS_AS(Dihedral(1, 2, 2, 4), Error);
        CHECK_THROWS_AS(Dihedral(1, 2, 3, 3), Error);
        CHECK_THROWS_AS(Dihedral(2, 3, 2, 4), Error);
        CHECK_THROWS_AS(Dihedral(1, 2, 3, 2), Error);
        CHECK_THROWS_AS(Dihedral(1, 2, 3, 1), Error);

        CHECK(Dihedral(1, 3, 4, 5) < Dihedral(2, 3, 4, 5));
        CHECK(Dihedral(1, 2, 4, 5) < Dihedral(1, 3, 4, 5));
        CHECK(Dihedral(1, 2, 3, 5) < Dihedral(1, 2, 4, 5));
        CHECK(Dihedral(1, 2, 3, 4) < Dihedral(1, 2, 3, 5));
        CHECK_FALSE(Dihedral(1, 2, 3, 4) < Dihedral(1, 2, 3, 4));

        CHECK(Dihedral(2, 3, 4, 5) > Dihedral(1, 3, 4, 5));
        CHECK(Dihedral(1, 3, 4, 5) > Dihedral(1, 2, 4, 5));
        CHECK(Dihedral(1, 2, 4, 5) > Dihedral(1, 2, 3, 5));
        CHECK(Dihedral(1, 2, 3, 5) > Dihedral(1, 2, 3, 4));
        CHECK_FALSE(Dihedral(1, 2, 3, 4) > Dihedral(1, 2, 3, 4));
    }
}

TEST_CASE("Use the Topology class", "[Topology]") {
    auto topology = Topology();

    topology.append(Atom("H"));
    CHECK(topology[0].name() == "H");
    CHECK(topology[0].type() == "H");

    topology.append(Atom("H"));
    CHECK(topology[1].name() == "H");
    CHECK(topology[1].type() == "H");

    topology.add_bond(0, 1);
    CHECK(topology.bonds().size() == 1);
    CHECK(topology.bonds()[0] == Bond(0, 1));
}

TEST_CASE("Angles and dihedral detection", "[Topology]") {

    SECTION("Angles detection") {
        auto topology = Topology();
        CHECK(topology.angles().size() == 0);

        topology.add_bond(0, 1);
        topology.add_bond(1, 2);
        auto angles = std::vector<Angle>{{0, 1, 2}};
        CHECK(topology.angles() == angles);

        topology.add_bond(17, 13);
        topology.add_bond(19, 17);
        angles.push_back({13, 17, 19});
        topology.add_bond(22, 21);
        topology.add_bond(26, 21);
        angles.push_back({22, 21, 26});
        CHECK(topology.angles() == angles);
    }

    SECTION("Dihedral angles") {
        auto topology = Topology();
        CHECK(topology.dihedrals().size() == 0);

        topology.add_bond(0, 1);
        topology.add_bond(1, 2);
        topology.add_bond(2, 3);
        auto dihedrals = std::vector<Dihedral>{{0, 1, 2, 3}};
        CHECK(topology.dihedrals() == dihedrals);

        topology.add_bond(12, 19);
        topology.add_bond(19, 18);
        topology.add_bond(16, 18);
        dihedrals.push_back({12, 19, 18, 16});
        CHECK(topology.dihedrals() == dihedrals);
    }
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

    CHECK_FALSE(topo.isbond(0, 0));

    CHECK(topo.angles().size() == 2);
    CHECK(topo.isangle(0, 4, 1));
    CHECK(topo.isangle(2, 5, 3));

    CHECK_FALSE(topo.isangle(0, 0, 1));

    topo.append(Atom("O"));
    topo.add_bond(3, 6);
    CHECK(topo.bonds().size() == 5);
    CHECK(topo.dihedrals().size() == 1);
    CHECK(topo.isdihedral(2, 5, 3, 6));

    CHECK_FALSE(topo.isdihedral(0, 0, 1, 2));
    CHECK_FALSE(topo.isdihedral(0, 1, 1, 2));
    CHECK_FALSE(topo.isdihedral(0, 1, 2, 2));

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

    auto first = topo.residue(0);
    CHECK(first);
    auto second = topo.residue(2);
    CHECK(second);

    CHECK_FALSE(topo.residue(7));

    CHECK_FALSE(topo.are_linked(*first, *second));
    topo.add_bond(6, 9);
    CHECK(topo.are_linked(*first, *second));

    // A residue is linked to itself
    second = topo.residue(0);
    CHECK(topo.are_linked(*first, *second));
}
