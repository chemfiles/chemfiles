#include "chemharp.h"

// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    CHRP_FRAME* frame = chrp_frame(5);

    size_t natoms=0, step=0;
    assert(!chrp_frame_size(frame, &natoms));
    assert(natoms == 5);

    assert(!chrp_frame_step(frame, &step));
    assert(step == 0);

    assert(!chrp_frame_set_step(frame, 42));
    assert(!chrp_frame_step(frame, &step));
    assert(step == 42);

    float data[4][3];
    float pos[4][3];
    for (unsigned i=0; i<4; i++)
        for (unsigned j=0; j<3; j++)
            data[i][j] = i*j;

    assert(!chrp_frame_set_positions(frame, data, 4));
    assert(!chrp_frame_size(frame, &natoms));
    assert(natoms == 4);

    assert(!chrp_frame_positions(frame, pos, 4));
    for (unsigned i=0; i<4; i++)
        for (unsigned j=0; j<3; j++)
            assert(fabs(data[i][j] - pos[i][j]) < 1e-9);


    bool has_vel = true;
    assert(!chrp_frame_has_velocities(frame, &has_vel));
    assert(has_vel == false);

    assert(!chrp_frame_set_velocities(frame, data, 4));
    float vel[4][3];
    assert(!chrp_frame_velocities(frame, vel, 4));
    for (unsigned i=0; i<4; i++)
        for (unsigned j=0; j<3; j++)
            assert(fabs(data[i][j] - vel[i][j]) < 1e-9);

    assert(!chrp_frame_has_velocities(frame, &has_vel));
    assert(has_vel == true);

    /*********************/
    CHRP_CELL* cell = chrp_cell(3, 4, 5);
    assert(!chrp_frame_set_cell(frame, cell));
    chrp_cell_free(cell);
    cell = chrp_cell_from_frame(frame);
    double a, b, c;
    assert(!chrp_cell_lengths(cell, &a, &b, &c));
    assert(fabs(a - 3.0) < 1e-9);
    assert(fabs(b - 4.0) < 1e-9);
    assert(fabs(c - 5.0) < 1e-9);
    chrp_cell_free(cell);
    /*********************/

    /*********************/
    CHRP_TOPOLOGY* top = chrp_topology();
    CHRP_ATOM* Zn = chrp_atom("Zn");
    CHRP_ATOM* Ar = chrp_atom("Ar");
    assert(!chrp_topology_append(top, Zn));
    assert(!chrp_topology_append(top, Ar));

    assert(!chrp_frame_set_topology(frame, top));
    chrp_topology_free(top);
    chrp_atom_free(Zn);
    chrp_atom_free(Ar);
    /*********************/

    top = chrp_topology_from_frame(frame);
    CHRP_ATOM* atom = chrp_atom_from_topology(top, 0);
    char name[32];
    assert(!chrp_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Zn") == 0);

    atom = chrp_atom_from_topology(top, 1);
    assert(!chrp_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Ar") == 0);
    chrp_atom_free(atom);

    assert(!chrp_frame_free(frame));

    return EXIT_SUCCESS;
}
