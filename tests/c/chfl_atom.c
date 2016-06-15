// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "chemfiles.h"
#include "crashs.h"

int main() {
    silent_crash_handlers();
    CHFL_ATOM* a = chfl_atom("He");
    assert(a != NULL);

    float mass=0, charge=0;
    assert(!chfl_atom_mass(a, &mass));
    assert(fabs(mass - 4.002602) < 1e-6);
    assert(!chfl_atom_charge(a, &charge));
    assert(fabs(charge) < 1e-6);

    char name[32];
    assert(!chfl_atom_name(a, name, sizeof(name)));
    assert(strcmp(name, "He") == 0);

    assert(!chfl_atom_set_mass(a, 678));
    assert(!chfl_atom_mass(a, &mass));
    assert(fabs(mass - 678) < 1e-6);

    assert(!chfl_atom_set_charge(a, -1.5));
    assert(!chfl_atom_charge(a, &charge));
    assert(fabs(charge + 1.5) < 1e-6);

    assert(!chfl_atom_set_name(a, "Zn"));
    assert(!chfl_atom_name(a, name, sizeof(name)));
    assert(strcmp(name, "Zn") == 0);

    assert(!chfl_atom_full_name(a, name, sizeof(name)));
    assert(strcmp(name, "Zinc") == 0);

    double radius=0;
    assert(!chfl_atom_vdw_radius(a, &radius));
    assert(fabs(radius - 2.1) < 1e-6);
    assert(!chfl_atom_covalent_radius(a, &radius));
    assert(fabs(radius - 1.31) < 1e-6);

    int number = 0;
    assert(!chfl_atom_atomic_number(a, &number));
    assert(number == 30);

    chfl_atom_type_t type;
    assert(!chfl_atom_type(a, &type));
    assert(type == CHFL_ATOM_ELEMENT);

    assert(!chfl_atom_set_type(a, CHFL_ATOM_DUMMY));
    assert(!chfl_atom_type(a, &type));
    assert(type == CHFL_ATOM_DUMMY);

    assert(!chfl_atom_free(a));

    return EXIT_SUCCESS;
}
