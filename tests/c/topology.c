// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "chemfiles.h"
#include "helpers.h"

static void test_bonds();
static void test_size();
static void test_residues();

int main() {
    silent_crash_handlers();
    test_size();
    test_bonds();
    test_residues();
    return EXIT_SUCCESS;
}

static void test_bonds() {
    CHFL_TOPOLOGY* topology = chfl_topology();
    assert(topology != NULL);

    size_t natoms=100, n=0;
    assert(!chfl_topology_atoms_count(topology, &natoms));
    assert(natoms == 0);

    // Creating some H2O2
    CHFL_ATOM* O = chfl_atom("O");
    CHFL_ATOM* H = chfl_atom("H");
    assert(!chfl_topology_append(topology, H));
    assert(!chfl_topology_append(topology, O));
    assert(!chfl_topology_append(topology, O));
    assert(!chfl_topology_append(topology, H));
    assert(!chfl_topology_atoms_count(topology, &natoms));
    assert(natoms == 4);
    assert(!chfl_atom_free(O));
    assert(!chfl_atom_free(H));

    assert(!chfl_topology_bonds_count(topology, &n));
    assert(n == 0);
    assert(!chfl_topology_angles_count(topology, &n));
    assert(n == 0);
    assert(!chfl_topology_dihedrals_count(topology, &n));
    assert(n == 0);

    assert(!chfl_topology_add_bond(topology, 0, 1));
    assert(!chfl_topology_add_bond(topology, 1, 2));
    assert(!chfl_topology_add_bond(topology, 2, 3));

    assert(!chfl_topology_bonds_count(topology, &n));
    assert(n == 3);
    assert(!chfl_topology_angles_count(topology, &n));
    assert(n == 2);
    assert(!chfl_topology_dihedrals_count(topology, &n));
    assert(n == 1);

    bool res = false;
    assert(!chfl_topology_isbond(topology, 0, 1, &res));
    assert(res == true);
    assert(!chfl_topology_isbond(topology, 0, 3, &res));
    assert(res == false);

    assert(!chfl_topology_isangle(topology, 0, 1, 2, &res));
    assert(res == true);
    assert(!chfl_topology_isangle(topology, 0, 1, 3, &res));
    assert(res == false);

    assert(!chfl_topology_isdihedral(topology, 0, 1, 2, 3, &res));
    assert(res == true);
    assert(!chfl_topology_isdihedral(topology, 0, 1, 3, 2, &res));
    assert(res == false);

    size_t top_bonds[3][2] = {{2, 3}, {1, 2}, {0, 1}};
    size_t bonds[3][2];
    assert(!chfl_topology_bonds(topology, bonds, 3));
    for (unsigned i=0; i<3; i++) {
        for (unsigned j=0; j<2; j++) {
            assert(bonds[i][j] == top_bonds[i][j]);
        }
    }

    size_t top_angles[2][3] = {{0, 1, 2}, {1, 2, 3}};
    size_t angles[2][3];
    assert(!chfl_topology_angles(topology, angles, 2));
    for (unsigned i=0; i<2; i++) {
        for (unsigned j=0; j<3; j++) {
            assert(angles[i][j] == top_angles[i][j]);
        }
    }

    size_t top_dihedrals[1][4] = {{0, 1, 2, 3}};
    size_t dihedrals[1][4];
    assert(!chfl_topology_dihedrals(topology, dihedrals, 1));
    for (unsigned j=0; j<4; j++) {
        assert(dihedrals[0][j] == top_dihedrals[0][j]);
    }

    assert(!chfl_topology_remove_bond(topology, 2, 3));
    assert(!chfl_topology_bonds_count(topology, &n));
    assert(n == 2);
    assert(!chfl_topology_angles_count(topology, &n));
    assert(n == 1);
    assert(!chfl_topology_dihedrals_count(topology, &n));
    assert(n == 0);

    assert(!chfl_topology_remove(topology, 3));
    assert(!chfl_topology_atoms_count(topology, &natoms));
    assert(natoms == 3);

    assert(!chfl_topology_free(topology));
}

static void test_size() {
    CHFL_TOPOLOGY* topology = chfl_topology();
    assert(topology != NULL);

    size_t natoms = 100;
    assert(!chfl_topology_atoms_count(topology, &natoms));
    assert(natoms == 0);

    assert(!chfl_topology_resize(topology, 42));
    assert(!chfl_topology_atoms_count(topology, &natoms));
    assert(natoms == 42);

    assert(!chfl_topology_free(topology));
}

static void test_residues() {
    CHFL_TOPOLOGY* topology = chfl_topology();
    assert(topology != NULL);

    CHFL_ATOM* atom = chfl_atom("X");
    for (size_t i=0; i<10; i++) {
        assert(!chfl_topology_append(topology, atom));
    }
    chfl_atom_free(atom);

    size_t residues[3][3] = {{2, 3, 6}, {0, 1, 9}, {4, 5, 8}};
    for (size_t i=0; i<3; i++) {
        CHFL_RESIDUE* residue = chfl_residue("X", (size_t)-1);
        assert(residue != NULL);
        for (size_t j=0; j<3; j++) {
            assert(!chfl_residue_add_atom(residue, residues[i][j]));
        }

        assert(!chfl_topology_add_residue(topology, residue));
        chfl_residue_free(residue);
    }

    size_t count = 0;
    assert(!chfl_topology_residues_count(topology, &count));
    assert(count == 3);

    CHFL_RESIDUE* residue_1 = chfl_residue_for_atom(topology, 2);
    CHFL_RESIDUE* residue_2 = chfl_residue_for_atom(topology, 0);
    assert(residue_1 != NULL);
    assert(residue_2 != NULL);

    CHFL_RESIDUE* residue_3 = chfl_residue_for_atom(topology, 7);
    assert(residue_3 == NULL);

    bool result = true;
    assert(!chfl_topology_are_linked(topology, residue_1, residue_2, &result));
    assert(result == false);

    assert(!chfl_topology_add_bond(topology, 6, 9));
    assert(!chfl_topology_are_linked(topology, residue_1, residue_2, &result));
    assert(result == true);

    assert(!chfl_residue_free(residue_1));
    assert(!chfl_residue_free(residue_2));
    assert(!chfl_topology_free(topology));
}
