#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    // We have two angles: 0-1-2 and 1-2-3
    chfl_topology_add_bond(topology, 0, 1);
    chfl_topology_add_bond(topology, 1, 2);
    chfl_topology_add_bond(topology, 2, 3);

    uint64_t angles = 0;
    chfl_topology_angles_count(topology, &angles);
    assert(angles == 2);

    chfl_topology_free(topology);
    // [example]
    return 0;
}
