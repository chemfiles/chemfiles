// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

int main(void) {
    // [example]
    CHFL_ATOM* atom = chfl_atom("H");

    chfl_atom_set_mass(atom, 1.45);

    double mass = 0;
    chfl_atom_mass(atom, &mass);
    assert(fabs(mass - 1.45) < 1e-15);

    chfl_free(atom);
    // [example]
    return 0;
}
