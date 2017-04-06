// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

int main() {
    // [example]
    CHFL_ATOM* atom = chfl_atom("Na");

    double radius = 0;
    chfl_atom_covalent_radius(atom, &radius);
    assert(fabs(radius - 1.54) < 1e-15);

    chfl_atom_free(atom);
    // [example]
    return 0;
}
