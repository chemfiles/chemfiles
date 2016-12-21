#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_RESIDUE* residue = chfl_residue("water", 3);

    chfl_residue_add_atom(residue, 0);
    chfl_residue_add_atom(residue, 32);
    chfl_residue_add_atom(residue, 28);

    chfl_residue_free(residue);
    // [example]
    return 0;
}
