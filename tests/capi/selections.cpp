#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

static CHFL_FRAME* testing_frame(void);
static bool find_match(const chfl_match_t* matches, uint64_t n_matches, chfl_match_t match);

TEST_CASE("Selections", "[CAPI]") {
    SECTION("Selection string") {
        CHFL_SELECTION* selection = chfl_selection("name O");
        REQUIRE(selection != NULL);

        char buffer[32] = {0};
        CHECK_STATUS(chfl_selection_string(selection, buffer, sizeof(buffer)));
        CHECK(buffer == std::string("name O"));

        CHECK_STATUS(chfl_selection_free(selection));

        selection = chfl_selection("angles: all");
        REQUIRE(selection != NULL);

        CHECK_STATUS(chfl_selection_string(selection, buffer, sizeof(buffer)));
        CHECK(buffer == std::string("angles: all"));

        CHECK_STATUS(chfl_selection_free(selection));
    }

    SECTION("Size") {
        CHFL_SELECTION* selection = chfl_selection("name O");
        REQUIRE(selection != NULL);

        uint64_t size = 0;
        CHECK_STATUS(chfl_selection_size(selection, &size));
        CHECK(size == 1);

        CHECK_STATUS(chfl_selection_free(selection));

        selection = chfl_selection("angles: all");
        REQUIRE(selection != NULL);

        CHECK_STATUS(chfl_selection_size(selection, &size));
        CHECK(size == 3);

        CHECK_STATUS(chfl_selection_free(selection));
    }

    SECTION("Evaluate") {
        CHFL_SELECTION* selection = chfl_selection("name O");
        REQUIRE(selection != NULL);

        CHFL_FRAME* frame = testing_frame();
        REQUIRE(frame != NULL);

        uint64_t matches_count = 0;
        CHECK_STATUS(chfl_selection_evalutate(selection, frame, &matches_count));
        CHECK(matches_count == 2);

        chfl_match_t* matches = new chfl_match_t[matches_count];

        REQUIRE(matches != 0);
        CHECK_STATUS(chfl_selection_matches(selection, matches, matches_count));
        CHECK(matches[0].size == 1);
        CHECK(matches[1].size == 1);
        CHECK(matches[0].atoms[0] == 1);
        CHECK(matches[1].atoms[0] == 2);

        // Check "out of bound" values
        CHECK(matches[1].atoms[3] == static_cast<uint64_t>(-1));
        delete[] matches;

        CHECK_STATUS(chfl_selection_free(selection));

        selection = chfl_selection("angles: all");
        REQUIRE(selection != NULL);

        CHECK_STATUS(chfl_selection_evalutate(selection, frame, &matches_count));
        CHECK(matches_count == 2);

        matches = new chfl_match_t[matches_count];
        REQUIRE(matches != NULL);

        CHECK_STATUS(chfl_selection_matches(selection, matches, matches_count));
        CHECK(matches[0].size == 3);
        CHECK(matches[1].size == 3);
        chfl_match_t match_1 = {3, {0, 1, 2, static_cast<uint64_t>(-1)}};
        CHECK(find_match(matches, matches_count, match_1));
        chfl_match_t match_2 = {3, {1, 2, 3, static_cast<uint64_t>(-1)}};
        CHECK(find_match(matches, matches_count, match_2));
        delete[] matches;

        CHECK_STATUS(chfl_selection_free(selection));
        CHECK_STATUS(chfl_frame_free(frame));
    }
}

static CHFL_FRAME* testing_frame(void) {
    CHFL_TOPOLOGY* topology = chfl_topology();
    CHFL_ATOM* O = chfl_atom("O");
    CHFL_ATOM* H = chfl_atom("H");
    REQUIRE(topology != NULL);
    REQUIRE(H != NULL);
    REQUIRE(O != NULL);

    CHECK_STATUS(chfl_topology_append(topology, H));
    CHECK_STATUS(chfl_topology_append(topology, O));
    CHECK_STATUS(chfl_topology_append(topology, O));
    CHECK_STATUS(chfl_topology_append(topology, H));
    CHECK_STATUS(chfl_atom_free(O));
    CHECK_STATUS(chfl_atom_free(H));

    CHECK_STATUS(chfl_topology_add_bond(topology, 0, 1));
    CHECK_STATUS(chfl_topology_add_bond(topology, 1, 2));
    CHECK_STATUS(chfl_topology_add_bond(topology, 2, 3));

    CHFL_FRAME* frame = chfl_frame(4);
    REQUIRE(frame != NULL);
    CHECK_STATUS(chfl_frame_set_topology(frame, topology));
    CHECK_STATUS(chfl_topology_free(topology));
    return frame;
}

static bool find_match(const chfl_match_t* matches, uint64_t n_matches, chfl_match_t match) {
    REQUIRE(matches != NULL);
    for (uint64_t i=0; i<n_matches; i++) {
        CHECK(matches[i].size == match.size);
        if (matches[i].atoms[0] == match.atoms[0] &&
            matches[i].atoms[1] == match.atoms[1] &&
            matches[i].atoms[2] == match.atoms[2] &&
            matches[i].atoms[3] == match.atoms[3]) {
            return true;
        }
    }
    return false;
}
