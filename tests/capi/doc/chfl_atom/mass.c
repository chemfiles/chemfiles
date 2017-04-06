// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

int main() {
    // [example]
    CHFL_ATOM* atom = chfl_atom("Na");

    double mass = 0;
    chfl_atom_mass(atom, &mass);
    assert(fabs(mass - 22.98976928) < 1e-15);

    chfl_atom_free(atom);
    // [example]
    return 0;
}
