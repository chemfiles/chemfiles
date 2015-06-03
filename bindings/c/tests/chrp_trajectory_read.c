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
    CHRP_TRAJECTORY* file = chrp_open(DATADIR "/xyz/water.xyz", "r");

    assert(!chrp_trajectory_read(file, frame));

    size_t natoms=0;
    assert(!chrp_frame_size(frame, &natoms));
    assert(natoms == 297);

    float pos_0[3] = {0.417219, 8.303366, 11.737172};
    float pos_124[3] = {5.099554, -0.045104, 14.153846};
    float pos[297][3];

    assert(!chrp_frame_positions(frame, pos, 297));
    for (unsigned i=0; i<3; i++){
        assert(pos[0][i] == pos_0[i]);
        assert(pos[124][i] == pos_124[i]);
    }

    CHRP_TOPOLOGY* topology = chrp_topology_from_frame(frame);
    assert(!chrp_topology_size(topology, &natoms));
    assert(natoms == 297);


    CHRP_ATOM* atom = chrp_atom_from_topology(topology, 0);
    char name[32];
    assert(!chrp_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "O") == 0);
    assert(!chrp_atom_free(atom));
    assert(!chrp_topology_free(topology));


/*
set_cell!(file, UnitCell(30, 30, 30))
frame = read_step(file, 41)

@fact lengths(UnitCell(frame)) => (30.0, 30.0, 30.0)
*/
    assert(!chrp_trajectory_read_step(file, 41, frame));
    pos_0[0] = 0.761277; pos_0[1] = 8.106125; pos_0[2] = 10.622949;
    pos_124[0] = 5.13242; pos_124[1] = 0.079862; pos_124[2] = 14.194161;

    assert(!chrp_frame_positions(frame, pos, 297));
    for (unsigned i=0; i<3; i++){
        assert(pos[0][i] == pos_0[i]);
        assert(pos[124][i] == pos_124[i]);
    }

    topology = chrp_topology_from_frame(frame);
    assert(!chrp_topology_size(topology, &natoms));
    assert(natoms == 297);

    size_t n=10;
    assert(!chrp_topology_bonds_count(topology, &n));
    assert(n == 0);
    assert(!chrp_topology_free(topology));

    atom = chrp_atom_from_frame(frame, 0);
    assert(!chrp_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "O") == 0);
    assert(!chrp_atom_free(atom));
    atom = chrp_atom_from_frame(frame, 1);
    assert(!chrp_atom_name(atom, name, sizeof(name)));
    assert(strcmp(name, "H") == 0);
    assert(!chrp_atom_free(atom));


    assert(!chrp_frame_guess_topology(frame, true));
    topology = chrp_topology_from_frame(frame);
    assert(!chrp_topology_bonds_count(topology, &n));
    assert(n == 181);
    assert(!chrp_topology_angles_count(topology, &n));
    assert(n == 87);
    assert(!chrp_topology_free(topology));


    topology = chrp_topology();
    atom = chrp_atom("Cs");
    for (unsigned i=0; i<3; i++)
        assert(!chrp_topology_append(topology, atom));

    assert(!chrp_trajectory_set_topology(file, topology));
/*
        set_topology!(file, topology)
        frame = read_step(file, 10)
        @fact name(Atom(frame, 10)) => "Cs"

        set_topology!(file, joinpath(DATAPATH, "topology.xyz"))
        frame = read(file)
        @fact name(Atom(frame, 100)) => "Rd"
*/
    assert(!chrp_frame_free(frame));
    assert(!chrp_trajectory_close(file));
    return EXIT_SUCCESS;
}
