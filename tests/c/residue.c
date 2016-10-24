// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "chemfiles.h"
#include "helpers.h"

int main() {
    silent_crash_handlers();

    CHFL_RESIDUE* residue = chfl_residue("Foo", 56);
    assert(residue != NULL);

    char name[32] = {0};
    assert(!chfl_residue_name(residue, name, 32));
    assert(strcmp(name, "Foo") == 0);

    uint64_t resid = 0;
    assert(!chfl_residue_id(residue, &resid));
    assert(resid == 56);

    uint64_t size = 10;
    assert(!chfl_residue_atoms_count(residue, &size));
    assert(size == 0);

    assert(!chfl_residue_add_atom(residue, 0));
    assert(!chfl_residue_add_atom(residue, 1));
    assert(!chfl_residue_add_atom(residue, 2));

    assert(!chfl_residue_atoms_count(residue, &size));
    assert(size == 3);

    bool contains = false;
    assert(!chfl_residue_contains(residue, 1, &contains));
    assert(contains == true);
    assert(!chfl_residue_contains(residue, 16, &contains));
    assert(contains == false);

    CHFL_TOPOLOGY* topology = chfl_topology();

    assert(!chfl_topology_residues_count(topology, &size));
    assert(size == 0);

    assert(!chfl_topology_add_residue(topology, residue));
    assert(!chfl_residue_free(residue));

    assert(!chfl_topology_residues_count(topology, &size));
    assert(size == 1);

    residue = chfl_residue_from_topology(topology, 0);
    resid = 0;
    assert(!chfl_residue_id(residue, &resid));
    assert(resid == 56);
    assert(!chfl_residue_free(residue));

    residue = chfl_residue_from_topology(topology, 10);
    assert(residue == NULL);

    residue = chfl_residue_for_atom(topology, 2);
    resid = 0;
    assert(!chfl_residue_id(residue, &resid));
    assert(resid == 56);
    assert(!chfl_residue_free(residue));

    residue = chfl_residue_for_atom(topology, 10);
    assert(residue == NULL);

    assert(!chfl_topology_free(topology));

    return EXIT_SUCCESS;
}
