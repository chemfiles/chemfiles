#include "chemfiles.h"

// Force NDEBUG to be undefined
#undef NDEBUG

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define DATADIR SRCDIR "/data/xyz/"

int main(){
    CHFL_FRAME* frame = chfl_frame(0);
    CHFL_TRAJECTORY* file = chfl_trajectory_open(DATADIR "water.xyz", 'r');
    assert(frame != NULL);
    assert(file != NULL);

    size_t nsteps = 0;
    assert(!chfl_trajectory_nsteps(file, &nsteps));
    assert(nsteps == 100);

    // Read the first frame
    assert(!chfl_trajectory_read(file, frame));

    size_t natoms = 0;
    assert(!chfl_frame_atoms_count(frame, &natoms));
    assert(natoms == 297);

    float positions_0[3] = {0.417219f, 8.303366f, 11.737172f};
    float positions_124[3] = {5.099554f, -0.045104f, 14.153846f};
    float (*positions)[3] = NULL;

    // Check for the error when requesting non-existent velocities
    assert(chfl_frame_velocities(frame, &positions, &natoms) != CHFL_SUCCESS);

    // Check positions in the first frame
    assert(!chfl_frame_positions(frame, &positions, &natoms));
    assert(natoms == 297);
    for (unsigned i=0; i<3; i++){
        assert(positions[0][i] == positions_0[i]);
        assert(positions[124][i] == positions_124[i]);
    }

    // Check topology in the first frame
    CHFL_TOPOLOGY* topology = chfl_topology_from_frame(frame);
    assert(!chfl_topology_atoms_count(topology, &natoms));
    assert(natoms == 297);
    size_t n = 0;
    assert(!chfl_topology_bonds_count(topology, &n));
    assert(n == 0);

    CHFL_ATOM* atom = chfl_atom_from_topology(topology, 0);
    char name[32];
    assert(!chfl_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "O") == 0);
    assert(!chfl_atom_free(atom));
    assert(!chfl_topology_free(topology));

    // Set the cell associated with a trajectory
    CHFL_CELL* cell = chfl_cell(30, 30, 30);
    assert(!chfl_trajectory_set_cell(file, cell));
    assert(!chfl_cell_free(cell));

    // Check reading a specific step
    assert(!chfl_trajectory_read_step(file, 41, frame));

    // Check that the cell was set
    cell = chfl_cell_from_frame(frame);
    double a=0, b=0, c=0;
    assert(!chfl_cell_lengths(cell, &a, &b, &c));
    assert(a == 30.0);
    assert(b == 30.0);
    assert(c == 30.0);
    assert(!chfl_cell_free(cell));

    positions_0[0] = 0.761277f;  positions_0[1] = 8.106125f;   positions_0[2] = 10.622949f;
    positions_124[0] = 5.13242f; positions_124[1] = 0.079862f; positions_124[2] = 14.194161f;

    assert(!chfl_frame_positions(frame, &positions, &natoms));
    assert(natoms == 297);
    for (unsigned i=0; i<3; i++){
        assert(positions[0][i] == positions_0[i]);
        assert(positions[124][i] == positions_124[i]);
    }


    // Get the atom from a frame
    atom = chfl_atom_from_frame(frame, 1);
    assert(!chfl_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "H") == 0);
    assert(!chfl_atom_free(atom));

    // Guess the system topology
    assert(!chfl_frame_guess_topology(frame));
    topology = chfl_topology_from_frame(frame);
    assert(!chfl_topology_bonds_count(topology, &n));
    assert(n == 181);
    assert(!chfl_topology_angles_count(topology, &n));
    assert(n == 87);
    assert(!chfl_topology_free(topology));

    // Set the topology associated with a trajectory by hand
    topology = chfl_topology();
    atom = chfl_atom("Cs");
    for (unsigned i=0; i<297; i++)
        assert(!chfl_topology_append(topology, atom));

    assert(!chfl_trajectory_set_topology(file, topology));
    assert(!chfl_topology_free(topology));

    assert(!chfl_trajectory_read_step(file, 10, frame));

    atom = chfl_atom_from_frame(frame, 1);
    assert(!chfl_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Cs") == 0);
    assert(!chfl_atom_free(atom));

    assert(!chfl_trajectory_close(file));
    file = chfl_trajectory_open(DATADIR "trajectory.xyz", 'r');

    // Set the topology associated with a trajectory from a file
    assert(!chfl_trajectory_set_topology_with_format(file, DATADIR "topology.xyz.topology", "XYZ"));
    assert(!chfl_trajectory_read(file, frame));
    atom = chfl_atom_from_frame(frame, 0);
    assert(!chfl_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Zn") == 0);
    assert(!chfl_atom_free(atom));

    assert(!chfl_trajectory_set_topology_file(file, DATADIR "topology.xyz"));
    assert(!chfl_trajectory_read(file, frame));
    atom = chfl_atom_from_frame(frame, 0);
    assert(!chfl_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "Zn") == 0);
    assert(!chfl_atom_free(atom));

    assert(!chfl_trajectory_close(file));

    file = chfl_trajectory_with_format(DATADIR "helium.xyz.but.not.really", 'r', "XYZ");
    assert(!chfl_trajectory_read(file, frame));
    assert(!chfl_frame_atoms_count(frame, &natoms));
    assert(natoms == 125);

    assert(!chfl_frame_free(frame));
    assert(!chfl_trajectory_close(file));

    return EXIT_SUCCESS;
}
