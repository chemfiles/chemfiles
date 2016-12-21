#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    chfl_topology_add_bond(topology, 0, 1);
    chfl_topology_add_bond(topology, 2, 3);

    uint64_t bonds[2][2] = {{0}};
    chfl_topology_bonds(topology, bonds, 2);
    assert(bonds[0][0] == 0);
    assert(bonds[0][1] == 1);

    assert(bonds[1][0] == 2);
    assert(bonds[1][1] == 3);

    chfl_topology_free(topology);
    // [example]
    return 0;
}
