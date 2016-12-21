#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();
    CHFL_RESIDUE* residue = chfl_residue("res", 1);

    chfl_topology_add_residue(topology, residue);

    chfl_residue_free(residue);
    chfl_topology_free(topology);
    // [example]
    return 0;
}
