#include "chemharp.h"

// Force NDEBUG to be undefined
#undef NDEBUG

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define XYZFILE DATADIR "/xyz/helium.xyz"

// Some data
static float FIRST_FRAME[2][3] = {
    {0.49053, 8.41351, 0.0777257},
    {8.57951, 8.65712, 8.06678}
};

static float LAST_FRAME[2][3] = {
    {-1.186037, 11.439334, 0.529939},
    {5.208778, 12.707273, 10.940157}
};

static float MIDLE_FRAME[2][3] = {
    {-0.145821, 8.540648, 1.090281},
    {8.446093, 8.168162, 9.350953}
};

// Some helper functions
void check_frame(CHRP_FRAME* frame, float reference[2][3]);
void check_topology(CHRP_TOPOLOGY* topology);
void check_cell(CHRP_CELL* cell);

int main(int argc, char** argv){
    CHRP_TRAJECTORY* traj = chrp_open(XYZFILE, "r");
    CHRP_FRAME* frame = chrp_frame(0);

    assert(traj != NULL);
    assert(!chrp_trajectory_read(traj, frame));
    check_frame(frame, FIRST_FRAME);

    CHRP_TOPOLOGY* topology = chrp_topology_from_frame(frame);

    assert(topology != NULL);
    check_topology(topology);

    CHRP_CELL* cell = chrp_cell_from_frame(frame);

    assert(cell != NULL);
    check_cell(cell);

    size_t nsteps = 0;
    assert(!chrp_trajectory_nsteps(traj, &nsteps));

    for (size_t i=1; i<nsteps; i++) {
        assert(!chrp_trajectory_read(traj, frame));
    }
    check_frame(frame, LAST_FRAME);

    assert(!chrp_trajectory_read_at(traj, 42, frame));
    check_frame(frame, MIDLE_FRAME);

	return 0;
}

void check_frame(CHRP_FRAME* frame, float reference[2][3]) {
    bool has_velocities = true;
    size_t natoms = 0;

    chrp_frame_size(frame, &natoms);
    assert(natoms == 125);

    assert(!chrp_frame_has_velocities(frame, &has_velocities));
    assert(!has_velocities);

    float (*positions)[3] = (float(*)[3])malloc(sizeof(float[natoms][3]));
    assert(positions != NULL);

    assert(!chrp_frame_positions(frame, positions, natoms));

    for(size_t i=0; i<3; i++){
        assert(fabs(positions[0][i] - reference[0][i]) < 1e-5);
        assert(fabs(positions[124][i] - reference[1][i]) < 1e-5);
    }

    free(positions);
}


void check_topology(CHRP_TOPOLOGY* topology){
    size_t natoms;

    assert(!chrp_topology_size(topology, &natoms));
    assert(natoms == 125);

    CHRP_ATOM* atom = chrp_atom_from_topology(topology, 3);
    assert(atom != NULL);

    char name[5];
    assert(!chrp_atom_name(atom, name, 5));
    assert(strcmp(name, "He") == 0);
}

void check_cell(CHRP_CELL* cell){
    double a = 0, b = 0, c = 0;
    double alpha = 0, beta = 0, gamma = 0;

    assert(!chrp_cell_lengths(cell, &a, &b, &c));
    assert(a == 0.0);
    assert(b == 0.0);
    assert(c == 0.0);

    assert(!chrp_cell_angles(cell, &alpha, &beta, &gamma));
    assert(alpha == 90.0);
    assert(beta == 90.0);
    assert(gamma == 90.0);

    chrp_cell_type_t type;

    assert(!chrp_cell_type(cell, &type));
    assert(type == INFINITE);
}
