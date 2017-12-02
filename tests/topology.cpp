// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Connectivity elements") {
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

        CHECK_THROWS_AS(Bond(2, 1)[2], OutOfBounds);
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

        CHECK_THROWS_AS(Angle(3, 2, 1)[3], OutOfBounds);
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

        CHECK_THROWS_AS(Dihedral(2, 1, 4, 6)[4], OutOfBounds);
    }

    SECTION("Improper") {
        CHECK(Improper(2, 3, 4, 5) == Improper(5, 3, 2, 4));
        CHECK(Improper(2, 3, 4, 5) == Improper(2, 3, 5, 4));

        auto improper = Improper(6, 7, 5, 8);
        CHECK(improper[0] == 5);
        CHECK(improper[1] == 7);
        CHECK(improper[2] == 6);
        CHECK(improper[3] == 8);

        CHECK_THROWS_AS(Improper(2, 2, 3, 4), Error);
        CHECK_THROWS_AS(Improper(1, 2, 2, 4), Error);
        CHECK_THROWS_AS(Improper(1, 2, 3, 3), Error);
        CHECK_THROWS_AS(Improper(2, 3, 2, 4), Error);
        CHECK_THROWS_AS(Improper(1, 2, 3, 2), Error);
        CHECK_THROWS_AS(Improper(1, 2, 3, 1), Error);

        CHECK_THROWS_AS(Improper(2, 1, 4, 6)[4], OutOfBounds);
    }
}

TEST_CASE("Use the Topology class") {
    auto topology = Topology();

    topology.add_atom(Atom("H"));
    CHECK(topology[0].name() == "H");
    CHECK(topology[0].type() == "H");

    topology.add_atom(Atom("H"));
    CHECK(topology[1].name() == "H");
    CHECK(topology[1].type() == "H");

    topology.add_bond(0, 1);
    CHECK(topology.bonds().size() == 1);
    CHECK(topology.bonds()[0] == Bond(0, 1));
}

TEST_CASE("Connectivity detection") {
    SECTION("Angles detection") {
        auto topology = Topology();
        for (size_t i=0; i<30; i++) {
            topology.add_atom(Atom());
        }

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
        for (size_t i=0; i<30; i++) {
            topology.add_atom(Atom());
        }

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

    SECTION("Improper angles") {
        auto topology = Topology();
        for (size_t i=0; i<30; i++) {
            topology.add_atom(Atom());
        }

        CHECK(topology.impropers().size() == 0);

        topology.add_bond(0, 1);
        topology.add_bond(0, 2);
        topology.add_bond(0, 3);
        auto impropers = std::vector<Improper>{{1, 0, 2, 3}};
        CHECK(topology.impropers() == impropers);

        topology.add_bond(12, 19);
        topology.add_bond(19, 18);
        topology.add_bond(16, 19);
        impropers.push_back({12, 19, 16, 18});
        CHECK(topology.impropers() == impropers);
    }
}

TEST_CASE("Out of bounds errors") {
    auto topology = Topology();
    topology.add_atom(Atom());
    topology.add_atom(Atom());
    topology.add_atom(Atom());

    CHECK_THROWS_AS(topology[25], OutOfBounds);

    CHECK_THROWS_AS(topology.add_bond(0, 25), OutOfBounds);
    CHECK_THROWS_AS(topology.add_bond(25, 0), OutOfBounds);

    CHECK_THROWS_AS(topology.remove_bond(0, 25), OutOfBounds);
    CHECK_THROWS_AS(topology.remove_bond(25, 0), OutOfBounds);

    CHECK_THROWS_AS(topology.remove(25), OutOfBounds);
}

TEST_CASE("Add and remove items in the topology") {
    SECTION("Atoms") {
        auto topology = Topology();
        for (unsigned i=0; i<4; i++) {
            topology.add_atom(Atom("H"));
        }
        topology.add_atom(Atom("O"));
        topology.add_atom(Atom("O"));

        CHECK(topology.size() == 6);

        topology.remove(4);
        CHECK(topology.size() == 5);
    }

    SECTION("Bonds") {
        auto topology = Topology();
        for (unsigned i=0; i<4; i++) {
            topology.add_atom(Atom("H"));
        }
        topology.add_atom(Atom("O"));
        topology.add_atom(Atom("O"));

        topology.add_bond(0, 4);
        topology.add_bond(1, 4);
        topology.add_bond(2, 5);
        topology.add_bond(3, 5);

        CHECK(topology.bonds() == (std::vector<Bond>{{0, 4}, {1, 4}, {2, 5}, {3, 5}}));
        CHECK(topology.angles() == (std::vector<Angle>{{0, 4, 1}, {2, 5, 3}}));
        CHECK(topology.dihedrals() == (std::vector<Dihedral>{}));

        topology.add_atom(Atom("O"));
        topology.add_bond(3, 6);
        CHECK(topology.bonds().size() == 5);
        CHECK(topology.dihedrals()[0] == Dihedral(2, 5, 3, 6));

        topology.remove(6);
        CHECK(topology.bonds().size() == 4);

        // we can not resize while there are bonds betweena atoms to remove
        CHECK_THROWS_AS(topology.resize(5), Error);

        topology.remove_bond(2, 5);
        topology.remove_bond(3, 5);
        CHECK(topology.size() == 6);
        CHECK(topology.bonds().size() == 2);

        // Now that the bonds are gone, we can resize
        topology.resize(5);
    }

    SECTION("Bonds and atoms") {
        auto topology = Topology();
        for (unsigned i=0; i<4; i++) {
            topology.add_atom(Atom(""));
        }

        topology.add_bond(0, 2);
        topology.add_bond(1, 3);

        CHECK(topology.bonds() == (std::vector<Bond>{{0, 2}, {1, 3}}));

        // Checking that the stored bonds indexes are updated
        topology.remove(2);
        CHECK(topology.bonds() == (std::vector<Bond>{{1, 2}}));
    }
}

TEST_CASE("Residues in topologies") {
    auto topology = Topology();
    for (unsigned i=0; i<10; i++) {
        topology.add_atom(Atom("X"));
    }

    size_t residues[3][3] = {{2, 3, 6}, {0, 1, 9}, {4, 5, 8}};
    for (auto& atoms: residues) {
        auto residue = Residue("X");
        for (auto i: atoms) {
            residue.add_atom(i);
        }
        topology.add_residue(std::move(residue));
    }

    CHECK(topology.residues().size() == 3);


    auto residue = Residue("X");
    residue.add_atom(2);
    CHECK_THROWS_AS(topology.add_residue(residue), Error);

    auto first = topology.residue_for_atom(0);
    CHECK(first);
    auto second = topology.residue_for_atom(2);
    CHECK(second);

    CHECK_FALSE(topology.residue_for_atom(7));

    CHECK_FALSE(topology.are_linked(*first, *second));
    topology.add_bond(6, 9);
    CHECK(topology.are_linked(*first, *second));

    // A residue is linked to itself
    second = topology.residue_for_atom(0);
    CHECK(topology.are_linked(*first, *second));
}
