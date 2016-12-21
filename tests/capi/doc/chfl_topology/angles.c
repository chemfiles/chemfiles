#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    chfl_topology_add_bond(topology, 0, 1);
    chfl_topology_add_bond(topology, 1, 2);
    chfl_topology_add_bond(topology, 2, 3);

    uint64_t angles[2][3] = {{0}};
    chfl_topology_angles(topology, angles, 2);
    assert(angles[0][0] == 0);
    assert(angles[0][1] == 1);
    assert(angles[0][2] == 2);

    assert(angles[1][0] == 1);
    assert(angles[1][1] == 2);
    assert(angles[1][2] == 3);

    chfl_topology_free(topology);
    // [example]
    return 0;
}
