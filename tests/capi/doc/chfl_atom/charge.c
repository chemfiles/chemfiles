#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_ATOM* atom = chfl_atom("Na");

    double charge = 0;
    chfl_atom_charge(atom, &charge);
    assert(charge == 0);

    chfl_atom_free(atom);
    // [example]
    return 0;
}
