// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

TEST_CASE("chfl_residue") {
    SECTION("Constructors errors") {
        fail_next_allocation();
        CHECK(chfl_residue("ALA") == nullptr);

        fail_next_allocation();
        CHECK(chfl_residue_with_id("ALA", 22) == nullptr);

        CHFL_RESIDUE* residue = chfl_residue("ALA");
        REQUIRE(residue);

        fail_next_allocation();
        CHECK(chfl_residue_copy(residue) == nullptr);

        CHFL_TOPOLOGY* topology = chfl_topology();
        REQUIRE(topology);

        CHECK(chfl_residue_from_topology(topology, 0) == nullptr);
        CHECK(chfl_residue_for_atom(topology, 0) == nullptr);

        chfl_free(residue);
        chfl_free(topology);
    }

    SECTION("copy") {
        CHFL_RESIDUE* residue = chfl_residue("ALA");
        REQUIRE(residue);

        CHECK_STATUS(chfl_residue_add_atom(residue, 33));
        CHECK_STATUS(chfl_residue_add_atom(residue, 22));

        CHFL_RESIDUE* copy = chfl_residue_copy(residue);
        REQUIRE(copy);

        uint64_t size = 0;
        CHECK_STATUS(chfl_residue_atoms_count(residue, &size));
        CHECK(size == 2);

        CHECK_STATUS(chfl_residue_atoms_count(copy, &size));
        CHECK(size == 2);

        CHECK_STATUS(chfl_residue_add_atom(residue, 11));
        CHECK_STATUS(chfl_residue_add_atom(residue, 44));

        CHECK_STATUS(chfl_residue_atoms_count(residue, &size));
        CHECK(size == 4);

        CHECK_STATUS(chfl_residue_atoms_count(copy, &size));
        CHECK(size == 2);

        chfl_free(copy);
        chfl_free(residue);
    }

    SECTION("Name") {
        CHFL_RESIDUE* residue = chfl_residue("Foo");
        REQUIRE(residue);

        char name[32] = {0};
        CHECK_STATUS(chfl_residue_name(residue, name, sizeof(name)));
        CHECK(name == std::string("Foo"));

        chfl_free(residue);
    }

    SECTION("Id") {
        CHFL_RESIDUE* residue = chfl_residue_with_id("", 5426);
        REQUIRE(residue);

        int64_t resid = 0;
        CHECK_STATUS(chfl_residue_id(residue, &resid));
        CHECK(resid == 5426);

        chfl_free(residue);

        residue = chfl_residue("");
        REQUIRE(residue);

        CHECK(chfl_residue_id(residue, &resid) == CHFL_GENERIC_ERROR);

        chfl_free(residue);
    }

    SECTION("Atoms") {
        CHFL_RESIDUE* residue = chfl_residue("");
        REQUIRE(residue);

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

        uint64_t atoms[3] = {0};
        CHECK_STATUS(chfl_residue_atoms(residue, atoms, 3));
        CHECK(atoms[0] == 0);
        CHECK(atoms[1] == 1);
        CHECK(atoms[2] == 20);

        chfl_free(residue);
    }

    SECTION("Topology") {
        CHFL_RESIDUE* residue = chfl_residue_with_id("", 56);
        REQUIRE(residue);
        CHECK_STATUS(chfl_residue_add_atom(residue, 0));
        CHECK_STATUS(chfl_residue_add_atom(residue, 1));
        CHECK_STATUS(chfl_residue_add_atom(residue, 2));

        CHFL_TOPOLOGY* topology = chfl_topology();
        REQUIRE(topology);

        uint64_t size = 10;
        CHECK_STATUS(chfl_topology_residues_count(topology, &size));
        CHECK(size == 0);

        CHECK_STATUS(chfl_topology_add_residue(topology, residue));
        chfl_free(residue);

        CHECK_STATUS(chfl_topology_residues_count(topology, &size));
        CHECK(size == 1);

        const CHFL_RESIDUE* checking_residue = chfl_residue_from_topology(topology, 0);
        REQUIRE(checking_residue);
        int64_t resid = 0;
        CHECK_STATUS(chfl_residue_id(checking_residue, &resid));
        CHECK(resid == 56);
        chfl_free(checking_residue);

        checking_residue = chfl_residue_from_topology(topology, 10);
        CHECK_FALSE(checking_residue);

        checking_residue = chfl_residue_for_atom(topology, 2);
        REQUIRE(checking_residue);

        resid = 0;
        CHECK_STATUS(chfl_residue_id(checking_residue, &resid));
        CHECK(resid == 56);
        chfl_free(checking_residue);

        checking_residue = chfl_residue_for_atom(topology, 10);
        CHECK_FALSE(checking_residue);

        chfl_free(topology);
        chfl_free(checking_residue);
    }

    SECTION("Property") {
        CHFL_RESIDUE* residue = chfl_residue("ALA");
        REQUIRE(residue);

        CHFL_PROPERTY* property = chfl_property_double(-23);
        REQUIRE(property);

        CHECK_STATUS(chfl_residue_set_property(residue, "this", property));
        chfl_free(property);
        property = nullptr;

        property = chfl_residue_get_property(residue, "this");
        double value = 0;
        CHECK_STATUS(chfl_property_get_double(property, &value));
        CHECK(value == -23);
        chfl_free(property);

        CHECK_FALSE(chfl_residue_get_property(residue, "that"));

        property = chfl_property_bool(false);
        REQUIRE(property);

        CHECK_STATUS(chfl_residue_set_property(residue, "that", property));
        chfl_free(property);

        uint64_t count = 0;
        CHECK_STATUS(chfl_residue_properties_count(residue, &count));
        CHECK(count == 2);

        const char* names[2] = {nullptr};
        CHECK_STATUS(chfl_residue_list_properties(residue, names, count));
        // There are no guarantee of ordering
        if (names[0] == std::string("this")) {
            CHECK(names[1] == std::string("that"));
        } else {
            CHECK(names[0] == std::string("that"));
            CHECK(names[1] == std::string("this"));
        }

        chfl_free(residue);
    }
}
