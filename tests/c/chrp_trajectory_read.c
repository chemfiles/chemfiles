#include "chemharp.h"

// Force NDEBUG to be undefined
#undef NDEBUG

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
    CHRP_FRAME* frame = chrp_frame(0);
    CHRP_TRAJECTORY* file = chrp_trajectory_open(DATADIR "/xyz/water.xyz", "r");

    // Read the first frame
    assert(!chrp_trajectory_read(file, frame));

    size_t natoms=0;
    assert(!chrp_frame_atoms_count(frame, &natoms));
    assert(natoms == 297);

    float pos_0[3] = {0.417219, 8.303366, 11.737172};
    float pos_124[3] = {5.099554, -0.045104, 14.153846};
    float pos[297][3];

    // Check positions in the first frame
    assert(!chrp_frame_positions(frame, pos, 297));
    for (unsigned i=0; i<3; i++){
        assert(pos[0][i] == pos_0[i]);
        assert(pos[124][i] == pos_124[i]);
    }

    // Check topology in the first frame
    CHRP_TOPOLOGY* topology = chrp_topology_from_frame(frame);
    assert(!chrp_topology_atoms_count(topology, &natoms));
    assert(natoms == 297);
    size_t n = 0;
    assert(!chrp_topology_bonds_count(topology, &n));
    assert(n == 0);

    CHRP_ATOM* atom = chrp_atom_from_topology(topology, 0);
    char name[32];
    assert(!chrp_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "O") == 0);
    assert(!chrp_atom_free(atom));
    assert(!chrp_topology_free(topology));

    // Set the cell associated with a trajectory
    CHRP_CELL* cell = chrp_cell(30, 30, 30);
    assert(!chrp_trajectory_set_cell(file, cell));
    assert(!chrp_cell_free(cell));

    // Check reading a specific step
    assert(!chrp_trajectory_read_step(file, 41, frame));

    // Check that the cell was set
    cell = chrp_cell_from_frame(frame);
    double a=0, b=0, c=0;
    assert(!chrp_cell_lengths(cell, &a, &b, &c));
    assert(a == 30.0);
    assert(b == 30.0);
    assert(c == 30.0);
    assert(!chrp_cell_free(cell));

    pos_0[0] = 0.761277;  pos_0[1] = 8.106125;   pos_0[2] = 10.622949;
    pos_124[0] = 5.13242; pos_124[1] = 0.079862; pos_124[2] = 14.194161;

    assert(!chrp_frame_positions(frame, pos, 297));
    for (unsigned i=0; i<3; i++){
        assert(pos[0][i] == pos_0[i]);
        assert(pos[124][i] == pos_124[i]);
    }


    // Get the atom from a frame
    atom = chrp_atom_from_frame(frame, 1);
    assert(!chrp_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "H") == 0);
    assert(!chrp_atom_free(atom));

    // Guess the system topology
    assert(!chrp_frame_guess_topology(frame, true));
    topology = chrp_topology_from_frame(frame);
    assert(!chrp_topology_bonds_count(topology, &n));
    assert(n == 181);
    assert(!chrp_topology_angles_count(topology, &n));
    assert(n == 87);
    assert(!chrp_topology_free(topology));

    // Set the topology associated with a trajectory by hand
    topology = chrp_topology();
    atom = chrp_atom("Cs");
    for (unsigned i=0; i<3; i++)
        assert(!chrp_topology_append(topology, atom));

    assert(!chrp_trajectory_set_topology(file, topology));
    assert(!chrp_topology_free(topology));

    assert(!chrp_trajectory_read_step(file, 10, frame));

    atom = chrp_atom_from_frame(frame, 1);
    assert(!chrp_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Cs") == 0);
    assert(!chrp_atom_free(atom));

    // Set the topology associated with a trajectory from a file
    assert(!chrp_trajectory_set_topology_file(file, DATADIR "/xyz/topology.xyz"));
    assert(!chrp_trajectory_read(file, frame));
    atom = chrp_atom_from_frame(frame, 0);
    assert(!chrp_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Zn") == 0);
    assert(!chrp_atom_free(atom));

    assert(!chrp_trajectory_close(file));

    file = chrp_trajectory_with_format(DATADIR "xyz/helium.xyz.but.not.really", "r", "XYZ");
    assert(!chrp_trajectory_read(file, frame));
    assert(!chrp_frame_atoms_count(frame, &natoms));
    assert(natoms == 125);

    assert(!chrp_frame_free(frame));
    assert(!chrp_trajectory_close(file));

    return EXIT_SUCCESS;
}
