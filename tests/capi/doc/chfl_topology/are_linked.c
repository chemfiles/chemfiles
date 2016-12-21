#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example] [no-run]
    CHFL_TOPOLOGY* topology = chfl_topology();

    // Build the topology ...

    CHFL_RESIDUE* first = chfl_residue_from_topology(topology, 0);
    CHFL_RESIDUE* second = chfl_residue_from_topology(topology, 1);

    bool linked = false;
    chfl_topology_are_linked(topology, first, second, &linked);

    chfl_residue_free(first);
    chfl_residue_free(second);

    chfl_topology_free(topology);
    // [example]
    return 0;
}
