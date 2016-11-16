// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "chemfiles.h"
#include "helpers.h"

int main(void) {
    silent_crash_handlers();
    CHFL_ATOM* a = chfl_atom("He");
    assert(a != NULL);

    double mass=0, charge=0;
    assert(!chfl_atom_mass(a, &mass));
    assert(fabs(mass - 4.002602) < 1e-6);
    assert(!chfl_atom_charge(a, &charge));
    assert(fabs(charge) < 1e-6);

    char type[32];
    assert(!chfl_atom_type(a, type, sizeof(type)));
    assert(strcmp(type, "He") == 0);

    char name[32];
    assert(!chfl_atom_name(a, name, sizeof(name)));
    assert(strcmp(name, "He") == 0);

    assert(!chfl_atom_set_mass(a, 678));
    assert(!chfl_atom_mass(a, &mass));
    assert(fabs(mass - 678) < 1e-6);

    assert(!chfl_atom_set_charge(a, -1.5));
    assert(!chfl_atom_charge(a, &charge));
    assert(fabs(charge + 1.5) < 1e-6);

    assert(!chfl_atom_set_type(a, "Zn"));
    assert(!chfl_atom_type(a, type, sizeof(type)));
    assert(strcmp(type, "Zn") == 0);

    assert(!chfl_atom_set_name(a, "HB2"));
    assert(!chfl_atom_name(a, name, sizeof(name)));
    assert(strcmp(name, "HB2") == 0);

    assert(!chfl_atom_full_name(a, name, sizeof(name)));
    assert(strcmp(name, "Zinc") == 0);

    double radius=0;
    assert(!chfl_atom_vdw_radius(a, &radius));
    assert(fabs(radius - 2.1) < 1e-6);
    assert(!chfl_atom_covalent_radius(a, &radius));
    assert(fabs(radius - 1.31) < 1e-6);

    int64_t number = 0;
    assert(!chfl_atom_atomic_number(a, &number));
    assert(number == 30);

    assert(!chfl_atom_free(a));

    return EXIT_SUCCESS;
}
