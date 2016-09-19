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
    CHFL_FRAME* frame = chfl_frame(5);
    assert(frame != NULL);

    size_t natoms=0, step=0;
    assert(!chfl_frame_atoms_count(frame, &natoms));
    assert(natoms == 5);

    assert(!chfl_frame_step(frame, &step));
    assert(step == 0);

    assert(!chfl_frame_set_step(frame, 42));
    assert(!chfl_frame_step(frame, &step));
    assert(step == 42);

    assert(!chfl_frame_resize(frame, 4));
    assert(!chfl_frame_atoms_count(frame, &natoms));
    assert(natoms == 4);

    chfl_vector_t* data = NULL;
    assert(!chfl_frame_positions(frame, &data, &natoms));
    assert(natoms == 4);

    for (unsigned i=0; i<4; i++) {
        for (unsigned j=0; j<3; j++) {
            data[i][j] = (float)(i * j);
        }
    }

    assert(!chfl_frame_positions(frame, &data, &natoms));
    for (unsigned i=0; i<4; i++) {
        for (unsigned j=0; j<3; j++) {
            assert(fabs(data[i][j] - (float)(i * j)) < 1e-9);
        }
    }

    bool has_vel = true;
    assert(!chfl_frame_has_velocities(frame, &has_vel));
    assert(has_vel == false);
    assert(!chfl_frame_add_velocities(frame));
    assert(!chfl_frame_has_velocities(frame, &has_vel));
    assert(has_vel == true);

    assert(!chfl_frame_velocities(frame, &data, &natoms));
    assert(natoms == 4);

    for (unsigned i=0; i<4; i++) {
        for (unsigned j=0; j<3; j++) {
            data[i][j] = (float)(i * j);
        }
    }

    assert(!chfl_frame_velocities(frame, &data, &natoms));
    for (unsigned i=0; i<4; i++) {
        for (unsigned j=0; j<3; j++) {
            assert(fabs(data[i][j] - (float)(i * j)) < 1e-9);
        }
    }

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
    CHFL_TOPOLOGY* topology = chfl_topology();
    CHFL_ATOM* Zn = chfl_atom("Zn");
    CHFL_ATOM* Ar = chfl_atom("Ar");
    assert(!chfl_topology_append(topology, Zn));
    assert(!chfl_topology_append(topology, Ar));
    assert(!chfl_topology_append(topology, Zn));
    assert(!chfl_topology_append(topology, Ar));

    assert(!chfl_frame_set_topology(frame, topology));
    chfl_topology_free(topology);
    chfl_atom_free(Zn);
    chfl_atom_free(Ar);
    /*********************/

    topology = chfl_topology_from_frame(frame);
    CHFL_ATOM* atom = chfl_atom_from_topology(topology, 0);
    char name[32];
    assert(!chfl_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Zn") == 0);
    assert(!chfl_atom_free(atom));

    atom = chfl_atom_from_topology(topology, 10000);
    assert(atom == NULL);
    assert(!chfl_topology_free(topology));

    atom = chfl_atom_from_frame(frame, 1);
    assert(!chfl_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Ar") == 0);
    chfl_atom_free(atom);

    atom = chfl_atom_from_frame(frame, 10000);
    assert(atom == NULL);

    assert(!chfl_frame_free(frame));

    return EXIT_SUCCESS;
}
