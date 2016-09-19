// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "chemfiles.h"
#include "helpers.h"

#define DATADIR SRCDIR "/data/xyz/"

static void test_read();
static void test_write();

int main() {
    silent_crash_handlers();
    test_read();
    test_write();
    return EXIT_SUCCESS;
}

/******************************************************************************/

static void test_read() {
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

    chfl_vector_t positions_0 = {0.417219, 8.303366, 11.737172};
    chfl_vector_t positions_124 = {5.099554, -0.045104, 14.153846};
    chfl_vector_t* positions = NULL;

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
    char element[32];
    assert(!chfl_atom_element(atom, element, sizeof(element)));
    assert(strcmp(element, "O") == 0);
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

    positions_0[0] = 0.761277;  positions_0[1] = 8.106125;   positions_0[2] = 10.622949;
    positions_124[0] = 5.13242; positions_124[1] = 0.079862; positions_124[2] = 14.194161;

    assert(!chfl_frame_positions(frame, &positions, &natoms));
    assert(natoms == 297);
    for (unsigned i=0; i<3; i++){
        assert(positions[0][i] == positions_0[i]);
        assert(positions[124][i] == positions_124[i]);
    }


    // Get the atom from a frame
    atom = chfl_atom_from_frame(frame, 1);
    assert(!chfl_atom_element(atom, element, sizeof(element)));
    assert(strcmp(element, "H") == 0);
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
    for (unsigned i=0; i<297; i++) {
        assert(!chfl_topology_append(topology, atom));
    }
    assert(!chfl_atom_free(atom));

    assert(!chfl_trajectory_set_topology(file, topology));
    assert(!chfl_topology_free(topology));

    assert(!chfl_trajectory_read_step(file, 10, frame));

    atom = chfl_atom_from_frame(frame, 1);
    assert(!chfl_atom_element(atom, element, sizeof(element)));
    assert(strcmp(element, "Cs") == 0);
    assert(!chfl_atom_free(atom));

    assert(!chfl_trajectory_close(file));
    file = chfl_trajectory_open(DATADIR "trajectory.xyz", 'r');

    // Set the topology associated with a trajectory from a file
    assert(!chfl_trajectory_set_topology_with_format(file, DATADIR "topology.xyz.topology", "XYZ"));
    assert(!chfl_trajectory_read(file, frame));
    atom = chfl_atom_from_frame(frame, 0);
    assert(!chfl_atom_element(atom, element, sizeof(element)));
    assert(strcmp(element, "Zn") == 0);
    assert(!chfl_atom_free(atom));

    assert(!chfl_trajectory_set_topology_file(file, DATADIR "topology.xyz"));
    assert(!chfl_trajectory_read(file, frame));
    atom = chfl_atom_from_frame(frame, 0);
    assert(!chfl_atom_element(atom, element, sizeof(element)));
    assert(strcmp(element, "Zn") == 0);
    assert(!chfl_atom_free(atom));

    assert(!chfl_trajectory_close(file));

    file = chfl_trajectory_with_format(DATADIR "helium.xyz.but.not.really", 'r', "XYZ");
    assert(!chfl_trajectory_read(file, frame));
    assert(!chfl_frame_atoms_count(frame, &natoms));
    assert(natoms == 125);

    assert(!chfl_frame_free(frame));
    assert(!chfl_trajectory_close(file));
}

/******************************************************************************/

const char* expected_content =
"4\n"
"Written by the chemfiles library\n"
"He 1 2 3\n"
"He 1 2 3\n"
"He 1 2 3\n"
"He 1 2 3\n"
"6\n"
"Written by the chemfiles library\n"
"He 4 5 6\n"
"He 4 5 6\n"
"He 4 5 6\n"
"He 4 5 6\n"
"He 4 5 6\n"
"He 4 5 6\n";


static void test_write() {
    CHFL_TOPOLOGY* top = chfl_topology();
    CHFL_ATOM* atom = chfl_atom("He");
    assert(top != NULL);
    assert(atom != NULL);

    for (unsigned i=0; i<4; i++)
        assert(!chfl_topology_append(top, atom));

    CHFL_FRAME* frame = chfl_frame(4);
    assert(frame != NULL);

    chfl_vector_t* positions = NULL;
    size_t natoms = 0;
    assert(!chfl_frame_positions(frame, &positions, &natoms));
    assert(natoms == 4);

    for (unsigned i=0; i<4; i++) {
        for (unsigned j=0; j<3; j++) {
            positions[i][j] = j + 1.0;
        }
    }

    assert(!chfl_frame_set_topology(frame, top));

    CHFL_TRAJECTORY* file = chfl_trajectory_open("test-tmp.xyz", 'w');
    assert(file != NULL);
    assert(!chfl_trajectory_write(file, frame));

    assert(!chfl_frame_resize(frame, 6));
    assert(!chfl_frame_positions(frame, &positions, &natoms));
    assert(natoms == 6);
    for (unsigned i=0; i<6; i++) {
        for (unsigned j=0; j<3; j++) {
            positions[i][j] = j + 4.0;
        }
    }
    assert(!chfl_topology_append(top, atom));
    assert(!chfl_topology_append(top, atom));

    assert(!chfl_frame_set_topology(frame, top));

    assert(!chfl_atom_free(atom));
    assert(!chfl_topology_free(top));

    assert(!chfl_trajectory_write(file, frame));
    assert(!chfl_trajectory_close(file));
    assert(!chfl_frame_free(frame));

    char* content = read_whole_file("test-tmp.xyz");
    assert(strcmp(content, expected_content) == 0);
    free(content);

    remove("test-tmp.xyz");
}
