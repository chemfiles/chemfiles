#include "chemfiles.h"

// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    CHFL_FRAME* frame = chfl_frame(5);

    size_t natoms=0, step=0;
    assert(!chfl_frame_atoms_count(frame, &natoms));
    assert(natoms == 5);

    assert(!chfl_frame_step(frame, &step));
    assert(step == 0);

    assert(!chfl_frame_set_step(frame, 42));
    assert(!chfl_frame_step(frame, &step));
    assert(step == 42);

    float data[4][3];
    float pos[4][3];
    for (unsigned i=0; i<4; i++)
        for (unsigned j=0; j<3; j++)
            data[i][j] = i*j;

    assert(!chfl_frame_set_positions(frame, data, 4));
    assert(!chfl_frame_atoms_count(frame, &natoms));
    assert(natoms == 4);

    assert(!chfl_frame_positions(frame, pos, 4));
    for (unsigned i=0; i<4; i++)
        for (unsigned j=0; j<3; j++)
            assert(fabs(data[i][j] - pos[i][j]) < 1e-9);


    bool has_vel = true;
    assert(!chfl_frame_has_velocities(frame, &has_vel));
    assert(has_vel == false);

    assert(!chfl_frame_set_velocities(frame, data, 4));
    float vel[4][3];
    assert(!chfl_frame_velocities(frame, vel, 4));
    for (unsigned i=0; i<4; i++)
        for (unsigned j=0; j<3; j++)
            assert(fabs(data[i][j] - vel[i][j]) < 1e-9);

    assert(!chfl_frame_has_velocities(frame, &has_vel));
    assert(has_vel == true);

    /*********************/
    CHFL_CELL* cell = chfl_cell(3, 4, 5);
    assert(!chfl_frame_set_cell(frame, cell));
    chfl_cell_free(cell);
    cell = chfl_cell_from_frame(frame);
    double a, b, c;
    assert(!chfl_cell_lengths(cell, &a, &b, &c));
    assert(fabs(a - 3.0) < 1e-9);
    assert(fabs(b - 4.0) < 1e-9);
    assert(fabs(c - 5.0) < 1e-9);
    chfl_cell_free(cell);
    /*********************/

    /*********************/
    CHFL_TOPOLOGY* top = chfl_topology();
    CHFL_ATOM* Zn = chfl_atom("Zn");
    CHFL_ATOM* Ar = chfl_atom("Ar");
    assert(!chfl_topology_append(top, Zn));
    assert(!chfl_topology_append(top, Ar));

    assert(!chfl_frame_set_topology(frame, top));
    chfl_topology_free(top);
    chfl_atom_free(Zn);
    chfl_atom_free(Ar);
    /*********************/

    top = chfl_topology_from_frame(frame);
    CHFL_ATOM* atom = chfl_atom_from_topology(top, 0);
    char name[32];
    assert(!chfl_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Zn") == 0);

    atom = chfl_atom_from_frame(frame, 1);
    assert(!chfl_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Ar") == 0);
    chfl_atom_free(atom);

    assert(!chfl_frame_free(frame));

    return EXIT_SUCCESS;
}
