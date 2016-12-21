#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_RESIDUE* residue = chfl_residue("water", 3);

    uint64_t id = 0;
    chfl_residue_id(residue, &id);
    assert(id == 3);

    chfl_residue_free(residue);
    // [example]
    return 0;
}
