#include "chemfiles.h"

// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <stdlib.h>

int main() {
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

    CHFL_FRAME* frame = chfl_frame(4);
    assert(!chfl_frame_set_topology(frame, topology));
    assert(!chfl_topology_free(topology));

    bool* matched = malloc(4 * sizeof(bool));
    assert(!chfl_frame_selection(frame, "name O", matched, 4));
    assert(matched[0] == false);
    assert(matched[1] == true);
    assert(matched[2] == true);
    assert(matched[3] == false);

    assert(!chfl_frame_selection(frame, "not index <= 2", matched, 4));
    assert(matched[0] == false);
    assert(matched[1] == false);
    assert(matched[2] == false);
    assert(matched[3] == true);

    free(matched);
    assert(!chfl_frame_free(frame));
    return EXIT_SUCCESS;
}
