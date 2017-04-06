// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

TEST_CASE("Residue", "[CAPI]") {
    SECTION("Name") {
        CHFL_RESIDUE* residue = chfl_residue("Foo", 0);
        REQUIRE(residue != NULL);

        char name[32] = {0};
        CHECK_STATUS(chfl_residue_name(residue, name, sizeof(name)));
        CHECK(name == std::string("Foo"));

        CHECK_STATUS(chfl_residue_free(residue));
    }

    SECTION("Id") {
        CHFL_RESIDUE* residue = chfl_residue("", 5426);
        REQUIRE(residue != NULL);

        uint64_t resid = 0;
        CHECK_STATUS(chfl_residue_id(residue, &resid));
        CHECK(resid == 5426);

        CHECK_STATUS(chfl_residue_free(residue));
    }

    SECTION("Atoms") {
        CHFL_RESIDUE* residue = chfl_residue("", 0);
        REQUIRE(residue != NULL);

        uint64_t size = 10;
        CHECK_STATUS(chfl_residue_atoms_count(residue, &size));
        CHECK(size == 0);

        CHECK_STATUS(chfl_residue_add_atom(residue, 0));
        CHECK_STATUS(chfl_residue_add_atom(residue, 1));
        CHECK_STATUS(chfl_residue_add_atom(residue, 20));

        CHECK_STATUS(chfl_residue_atoms_count(residue, &size));
        CHECK(size == 3);

        bool contains = false;
        CHECK_STATUS(chfl_residue_contains(residue, 1, &contains));
        CHECK(contains == true);
        CHECK_STATUS(chfl_residue_contains(residue, 16, &contains));
        CHECK(contains == false);

        CHECK_STATUS(chfl_residue_free(residue));
    }

    SECTION("Topology") {
        CHFL_RESIDUE* residue = chfl_residue("", 56);
        REQUIRE(residue != NULL);
        CHECK_STATUS(chfl_residue_add_atom(residue, 0));
        CHECK_STATUS(chfl_residue_add_atom(residue, 1));
        CHECK_STATUS(chfl_residue_add_atom(residue, 2));

        CHFL_TOPOLOGY* topology = chfl_topology();
        REQUIRE(topology != NULL);

        uint64_t size = 10;
        CHECK_STATUS(chfl_topology_residues_count(topology, &size));
        CHECK(size == 0);

        CHECK_STATUS(chfl_topology_add_residue(topology, residue));
        CHECK_STATUS(chfl_residue_free(residue));

        CHECK_STATUS(chfl_topology_residues_count(topology, &size));
        CHECK(size == 1);

        residue = chfl_residue_from_topology(topology, 0);
        REQUIRE(residue != NULL);
        uint64_t resid = 0;
        CHECK_STATUS(chfl_residue_id(residue, &resid));
        CHECK(resid == 56);
        CHECK_STATUS(chfl_residue_free(residue));

        residue = chfl_residue_from_topology(topology, 10);
        CHECK(residue == NULL);

        residue = chfl_residue_for_atom(topology, 2);
        REQUIRE(residue != NULL);

        resid = 0;
        CHECK_STATUS(chfl_residue_id(residue, &resid));
        CHECK(resid == 56);
        CHECK_STATUS(chfl_residue_free(residue));

        residue = chfl_residue_for_atom(topology, 10);
        REQUIRE(residue == NULL);

        CHECK_STATUS(chfl_topology_free(topology));
        CHECK_STATUS(chfl_residue_free(residue));
    }
}
