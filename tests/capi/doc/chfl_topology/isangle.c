#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    chfl_topology_add_bond(topology, 0, 1);
    chfl_topology_add_bond(topology, 1, 2);

    bool result = false;
    chfl_topology_isangle(topology, 0, 1, 2, &result);
    assert(result == true);

    chfl_topology_isangle(topology, 0, 2, 1, &result);
    assert(result == false);

    chfl_topology_free(topology);
    // [example]
    return 0;
}
