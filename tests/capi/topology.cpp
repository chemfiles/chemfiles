// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

static CHFL_TOPOLOGY* testing_topology();

TEST_CASE("chfl_topology") {
    SECTION("Constructors errors") {
        fail_next_allocation();
        CHECK(chfl_topology() == nullptr);

        CHFL_TOPOLOGY* topology = chfl_topology();
        REQUIRE(topology);

        fail_next_allocation();
        CHECK(chfl_topology_copy(topology) == nullptr);

        chfl_free(topology);
    }

    SECTION("copy") {
        CHFL_TOPOLOGY* topology = chfl_topology();
        REQUIRE(topology);
        CHECK_STATUS(chfl_topology_resize(topology, 4));

        CHFL_TOPOLOGY* copy = chfl_topology_copy(topology);
        REQUIRE(copy);

        uint64_t natoms = 0;
        CHECK_STATUS(chfl_topology_atoms_count(topology, &natoms));
        CHECK(natoms == 4);

        CHECK_STATUS(chfl_topology_atoms_count(copy, &natoms));
        CHECK(natoms == 4);

        CHECK_STATUS(chfl_topology_resize(topology, 22));

        CHECK_STATUS(chfl_topology_atoms_count(topology, &natoms));
        CHECK(natoms == 22);

        CHECK_STATUS(chfl_topology_atoms_count(copy, &natoms));
        CHECK(natoms == 4);

        chfl_free(copy);
        chfl_free(topology);
    }

    SECTION("Size") {
        CHFL_TOPOLOGY* topology = chfl_topology();
        REQUIRE(topology);

        uint64_t natoms = 100;
        CHECK_STATUS(chfl_topology_atoms_count(topology, &natoms));
        CHECK(natoms == 0);

        CHECK_STATUS(chfl_topology_resize(topology, 42));
        CHECK_STATUS(chfl_topology_atoms_count(topology, &natoms));
        CHECK(natoms == 42);

        chfl_free(topology);
    }

    SECTION("Atoms") {
        CHFL_TOPOLOGY* topology = testing_topology();
        REQUIRE(topology);

        uint64_t natoms = 0;
        CHECK_STATUS(chfl_topology_atoms_count(topology, &natoms));
        CHECK(natoms == 4);

        CHECK_STATUS(chfl_topology_remove(topology, 3));
        CHECK_STATUS(chfl_topology_atoms_count(topology, &natoms));
        CHECK(natoms == 3);

        CHFL_ATOM* atom = chfl_atom_from_topology(topology, 0);
        REQUIRE(atom);

        char name[32];
        CHECK_STATUS(chfl_atom_type(atom, name, sizeof(name)));
        CHECK(name == std::string("H"));
        chfl_free(atom);

        // Out of bound access
        atom = chfl_atom_from_topology(topology, 10000);
        CHECK_FALSE(atom);

        chfl_free(topology);
    }

    SECTION("Bonds") {
        CHFL_TOPOLOGY* topology = testing_topology();
        REQUIRE(topology);

        uint64_t n = 0;
        CHECK_STATUS(chfl_topology_bonds_count(topology, &n));
        CHECK(n == 3);

        uint64_t expected[3][2] = {{0, 1}, {1, 2}, {2, 3}};
        uint64_t bonds[3][2] = {{0}};
        CHECK_STATUS(chfl_topology_bonds(topology, bonds, 3));
        for (unsigned i=0; i<3; i++) {
            for (unsigned j=0; j<2; j++) {
                CHECK(bonds[i][j] == expected[i][j]);
            }
        }

        CHECK_STATUS(chfl_topology_remove_bond(topology, 2, 3));
        CHECK_STATUS(chfl_topology_bonds_count(topology, &n));
        CHECK(n == 2);

        CHECK_STATUS(chfl_topology_clear_bonds(topology));
        CHECK_STATUS(chfl_topology_bonds_count(topology, &n));
        CHECK(n == 0);

        chfl_free(topology);
    }

    SECTION("Angles") {
        CHFL_TOPOLOGY* topology = testing_topology();
        REQUIRE(topology);

        uint64_t n = 0;
        CHECK_STATUS(chfl_topology_angles_count(topology, &n));
        CHECK(n == 2);

        uint64_t expected[2][3] = {{0, 1, 2}, {1, 2, 3}};
        uint64_t angles[2][3] = {{0}};
        CHECK_STATUS(chfl_topology_angles(topology, angles, 2));
        for (unsigned i=0; i<2; i++) {
            for (unsigned j=0; j<3; j++) {
                CHECK(angles[i][j] == expected[i][j]);
            }
        }

        CHECK_STATUS(chfl_topology_remove_bond(topology, 2, 3));
        CHECK_STATUS(chfl_topology_angles_count(topology, &n));
        CHECK(n == 1);

        chfl_free(topology);
    }

    SECTION("Dihedrals") {
        CHFL_TOPOLOGY* topology = testing_topology();
        REQUIRE(topology);

        uint64_t n = 0;
        CHECK_STATUS(chfl_topology_dihedrals_count(topology, &n));
        CHECK(n == 1);

        uint64_t expected[1][4] = {{0, 1, 2, 3}};
        uint64_t dihedrals[1][4] = {{0}};
        CHECK_STATUS(chfl_topology_dihedrals(topology, dihedrals, 1));
        for (unsigned j=0; j<4; j++) {
            CHECK(dihedrals[0][j] == expected[0][j]);
        }

        CHECK_STATUS(chfl_topology_remove_bond(topology, 2, 3));
        CHECK_STATUS(chfl_topology_dihedrals_count(topology, &n));
        CHECK(n == 0);

        chfl_free(topology);
    }

    SECTION("Impropers") {
        CHFL_TOPOLOGY* topology = testing_topology();
        REQUIRE(topology);

        CHFL_ATOM* H = chfl_atom("H");
        REQUIRE(H);
        CHECK_STATUS(chfl_topology_add_atom(topology, H));
        CHECK_STATUS(chfl_topology_add_bond(topology, 2, 4));
        chfl_free(H);

        uint64_t n = 0;
        CHECK_STATUS(chfl_topology_impropers_count(topology, &n));
        CHECK(n == 1);

        uint64_t expected[1][4] = {{1, 2, 3, 4}};
        uint64_t impropers[1][4] = {{0}};
        CHECK_STATUS(chfl_topology_impropers(topology, impropers, 1));
        for (unsigned j=0; j<4; j++) {
            CHECK(impropers[0][j] == expected[0][j]);
        }

        CHECK_STATUS(chfl_topology_remove_bond(topology, 2, 4));
        CHECK_STATUS(chfl_topology_impropers_count(topology, &n));
        CHECK(n == 0);

        chfl_free(topology);
    }

    SECTION("Residues") {
        CHFL_TOPOLOGY* topology = chfl_topology();
        REQUIRE(topology);

        CHFL_ATOM* atom = chfl_atom("X");
        for (uint64_t i=0; i<10; i++) {
            CHECK_STATUS(chfl_topology_add_atom(topology, atom));
        }
        chfl_free(atom);

        uint64_t residues[3][3] = {{2, 3, 6}, {0, 1, 9}, {4, 5, 8}};
        for (uint64_t i=0; i<3; i++) {
            CHFL_RESIDUE* residue = chfl_residue("X");
            REQUIRE(residue);
            for (uint64_t j=0; j<3; j++) {
                CHECK_STATUS(chfl_residue_add_atom(residue, residues[i][j]));
            }

            CHECK_STATUS(chfl_topology_add_residue(topology, residue));
            chfl_free(residue);
        }

        uint64_t count = 0;
        CHECK_STATUS(chfl_topology_residues_count(topology, &count));
        CHECK(count == 3);

        const CHFL_RESIDUE* first = chfl_residue_for_atom(topology, 2);
        const CHFL_RESIDUE* second = chfl_residue_for_atom(topology, 0);
        REQUIRE(first);
        REQUIRE(second);

        const CHFL_RESIDUE* out_of_bounds = chfl_residue_for_atom(topology, 7);
        CHECK_FALSE(out_of_bounds);

        bool linked = true;
        CHECK_STATUS(chfl_topology_residues_linked(topology, first, second, &linked));
        CHECK(linked == false);

        CHECK_STATUS(chfl_topology_add_bond(topology, 6, 9));
        CHECK_STATUS(chfl_topology_residues_linked(topology, first, second, &linked));
        CHECK(linked == true);

        chfl_free(first);
        chfl_free(second);
        chfl_free(topology);
    }
}

static CHFL_TOPOLOGY* testing_topology() {
    CHFL_TOPOLOGY* topology = chfl_topology();
    REQUIRE(topology);

    CHFL_ATOM* O = chfl_atom("O");
    CHFL_ATOM* H = chfl_atom("H");
    REQUIRE(O);
    REQUIRE(H);

    CHECK_STATUS(chfl_topology_add_atom(topology, H));
    CHECK_STATUS(chfl_topology_add_atom(topology, O));
    CHECK_STATUS(chfl_topology_add_atom(topology, O));
    CHECK_STATUS(chfl_topology_add_atom(topology, H));

    chfl_free(O);
    chfl_free(H);

    CHECK_STATUS(chfl_topology_add_bond(topology, 0, 1));
    CHECK_STATUS(chfl_topology_add_bond(topology, 1, 2));
    CHECK_STATUS(chfl_topology_add_bond(topology, 2, 3));

    return topology;
}
