#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    chfl_topology_add_bond(topology, 0, 1);
    chfl_topology_add_bond(topology, 1, 2);
    chfl_topology_add_bond(topology, 2, 3);

    uint64_t dihedrals[1][4] = {{0}};
    chfl_topology_dihedrals(topology, dihedrals, 1);
    assert(dihedrals[0][0] == 0);
    assert(dihedrals[0][1] == 1);
    assert(dihedrals[0][2] == 2);
    assert(dihedrals[0][3] == 3);

    chfl_topology_free(topology);
    // [example]
    return 0;
}
