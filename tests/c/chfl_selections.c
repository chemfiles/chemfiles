#include "chemfiles.h"

// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <stdlib.h>

CHFL_FRAME* testing_frame();

int main() {
    CHFL_FRAME* frame = testing_frame();
    CHFL_SELECTION* selection = chfl_selection("name O");

    assert(frame != NULL);
    assert(selection != NULL);

    size_t size = 0;
    assert(!chfl_selection_size(selection, &size));
    assert(size == 1);

    size_t n_matches = 0;
    assert(!chfl_selection_evalutate(selection, frame, &n_matches));
    assert(n_matches == 2);

    chfl_match_t *matches = malloc(n_matches * sizeof(chfl_match_t));
    assert(matches);
    assert(!chfl_selection_matches(selection, matches, n_matches));
    assert(matches[0].size == 1);
    assert(matches[1].size == 1);
    assert(matches[0].atoms[0] == 1);
    assert(matches[1].atoms[0] == 2);

    // Check "out of bound" values
    assert(matches[1].atoms[3] == (size_t)-1);

    free(matches);
    assert(!chfl_selection_free(selection));
    selection = NULL;
    selection = chfl_selection("not index <= 2");
    assert(selection != NULL);

    assert(!chfl_selection_size(selection, &size));
    assert(size == 1);

    assert(!chfl_selection_evalutate(selection, frame, &n_matches));
    assert(n_matches == 1);

    matches = malloc(n_matches * sizeof(chfl_match_t));
    assert(matches);
    assert(!chfl_selection_matches(selection, matches, n_matches));
    assert(matches[0].atoms[0] == 3);

    free(matches);
    assert(!chfl_selection_free(selection));

    selection = NULL;
    selection = chfl_selection("angles: all");
    assert(selection != NULL);

    assert(!chfl_selection_size(selection, &size));
    assert(size == 3);

    assert(!chfl_selection_evalutate(selection, frame, &n_matches));
    assert(n_matches == 2);

    matches = malloc(n_matches * sizeof(chfl_match_t));
    assert(matches);
    assert(!chfl_selection_matches(selection, matches, n_matches));
    assert(matches[0].size == 3);
    assert(matches[1].size == 3);
    assert(matches[0].atoms[0] == 0);
    assert(matches[0].atoms[1] == 1);
    assert(matches[0].atoms[2] == 2);
    assert(matches[1].atoms[0] == 1);
    assert(matches[1].atoms[1] == 2);
    assert(matches[1].atoms[2] == 3);

    free(matches);
    assert(!chfl_selection_free(selection));

    assert(!chfl_frame_free(frame));
    return EXIT_SUCCESS;
}

CHFL_FRAME* testing_frame() {
    CHFL_TOPOLOGY* topology = chfl_topology();
    CHFL_ATOM* O = chfl_atom("O");
    CHFL_ATOM* H = chfl_atom("H");
    assert(topology != NULL);
    assert(H != NULL);
    assert(O != NULL);

    assert(!chfl_topology_append(topology, H));
    assert(!chfl_topology_append(topology, O));
    assert(!chfl_topology_append(topology, O));
    assert(!chfl_topology_append(topology, H));
    assert(!chfl_atom_free(O));
    assert(!chfl_atom_free(H));

    assert(!chfl_topology_add_bond(topology, 0, 1));
    assert(!chfl_topology_add_bond(topology, 1, 2));
    assert(!chfl_topology_add_bond(topology, 2, 3));

    CHFL_FRAME* frame = chfl_frame(4);
    assert(frame != NULL);
    assert(!chfl_frame_set_topology(frame, topology));
    assert(!chfl_topology_free(topology));
    return frame;
}
