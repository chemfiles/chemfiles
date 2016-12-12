#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_ATOM* atom = chfl_atom("Na");

    int64_t number = 0;
    chfl_atom_atomic_number(atom, &number);
    assert(number == 11);

    chfl_atom_free(atom);
    // [example]
    return 0;
}
