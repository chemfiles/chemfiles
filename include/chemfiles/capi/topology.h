// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CHFL_TOPOLOGY_H
#define CHEMFILES_CHFL_TOPOLOGY_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// @brief Create a new empty topology
/// @return A pointer to the new topology, or NULL in case of error
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology(void);

/// @brief Get a copy of the topology of a frame
/// @param frame The frame
/// @return A pointer to the new topology, or NULL in case of error
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology_from_frame(
    const CHFL_FRAME* const frame
);

/// @brief Get the current number of atoms in the topology.
/// @param topology The topology to analyse
/// @param natoms Will contain the number of atoms in the frame
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_atoms_count(
    const CHFL_TOPOLOGY* const topology, uint64_t* natoms
);

/// @brief Resize the topology to hold `natoms` atoms.
///
/// @param topology The topology
/// @param natoms The new number of atoms.
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_resize(
    CHFL_TOPOLOGY* const topology, uint64_t natoms
);

/// @brief Add an atom at the end of a topology
/// @param topology The topology
/// @param atom The atom to be added
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_append(
    CHFL_TOPOLOGY* const topology, const CHFL_ATOM* const atom
);

/// @brief Remove an atom from a topology by index. This modify all the other
/// atoms indexes.
/// @param topology The topology
/// @param i The atomic index
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_remove(
    CHFL_TOPOLOGY* const topology, uint64_t i
);

/// @brief Tell if the atoms `i` and `j` are bonded together
/// @param topology The topology
/// @param i index of the first atom in the topology
/// @param j index of the second atom in the topology
/// @param result true if the atoms are bonded, false otherwise
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_isbond(
    const CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j, bool* result
);

/// @brief Tell if the atoms `i`, `j` and `k` constitues an angle
/// @param topology The topology
/// @param i index of the first atom in the topology
/// @param j index of the second atom in the topology
/// @param k index of the third atom in the topology
/// @param result true if the atoms constitues an angle, false otherwise
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_isangle(
    const CHFL_TOPOLOGY* const topology,
    uint64_t i,
    uint64_t j,
    uint64_t k,
    bool* result
);

/// @brief Tell if the atoms `i`, `j`, `k` and `m` constitues a dihedral angle
/// @param topology The topology
/// @param i index of the first atom in the topology
/// @param j index of the second atom in the topology
/// @param k index of the third atom in the topology
/// @param m index of the fourth atom in the topology
/// @param result true if the atoms constitues a dihedral angle, false otherwise
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_isdihedral(
    const CHFL_TOPOLOGY* const topology,
    uint64_t i,
    uint64_t j,
    uint64_t k,
    uint64_t m,
    bool* result
);

/// @brief Get the number of bonds in the system
/// @param topology The topology
/// @param nbonds After the call, contains the number of bond
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_bonds_count(
    const CHFL_TOPOLOGY* const topology, uint64_t* nbonds
);

/// @brief Get the number of angles in the system
/// @param topology The topology
/// @param nangles After the call, contains the number of angles
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_angles_count(
    const CHFL_TOPOLOGY* const topology, uint64_t* nangles
);

/// @brief Get the number of dihedral angles in the system
/// @param topology The topology
/// @param ndihedrals After the call, contains the number of dihedral angles
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_dihedrals_count(
    const CHFL_TOPOLOGY* const topology, uint64_t* ndihedrals
);

/// @brief Get the list of bonds in the system
/// @param topology The topology
/// @param data A nbonds x 2 array to be filled with the bonds in the system
/// @param nbonds The size of the array. This should equal the value given by the
///               `chfl_topology_bonds_count` function
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_bonds(
    const CHFL_TOPOLOGY* const topology, uint64_t (*data)[2], uint64_t nbonds
);

/// @brief Get the list of angles in the system
/// @param topology The topology
/// @param data A nangles x 3 array to be filled with the angles in the system
/// @param nangles The size of the array. This should equal the value given by the
///               `chfl_topology_angles_count` function
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_angles(
    const CHFL_TOPOLOGY* const topology, uint64_t (*data)[3], uint64_t nangles
);

/// @brief Get the list of dihedral angles in the system
/// @param topology The topology
/// @param data A ndihedrals x 4 array to be filled with the dihedral angles in
/// the system
/// @param ndihedrals The size of the array. This should equal the value given by
/// the `chfl_topology_dihedrals_count` function
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_dihedrals(
    const CHFL_TOPOLOGY* const topology, uint64_t (*data)[4], uint64_t ndihedrals
);

/// @brief Add a bond between the atoms `i` and `j` in the system
/// @param topology The topology
/// @param i index of the first atom in the topology
/// @param j index of the second atom in the topology
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_add_bond(
    CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j
);

/// @brief Remove any existing bond between the atoms `i` and `j` in the system
/// @param topology The topology
/// @param i index of the first atom in the topology
/// @param j index of the second atom in the topology
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_remove_bond(
    CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j
);

/// @brief Get the number of residues in the system
/// @param topology The topology
/// @param residues The number of residues
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_residues_count(
    const CHFL_TOPOLOGY* const topology, uint64_t* residues
);

/// @brief Add a residue to this topology
/// @param topology The topology
/// @param residue The new residue
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_add_residue(
    CHFL_TOPOLOGY* const topology, const CHFL_RESIDUE* const residue
);

/// @brief Check if two residues are linked together, i.e. if there is a bond
///        between one atom in the first residue and one atom in the second one.
/// @param topology The topology
/// @param first The first residue
/// @param second The second residue
/// @param result true if the residues are linked, false otherwise
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_are_linked(
    CHFL_TOPOLOGY* const topology,
    const CHFL_RESIDUE* const first,
    const CHFL_RESIDUE* const second,
    bool* result
);

/// @brief Destroy a topology, and free the associated memory
/// @param topology The topology to destroy
/// @return The status code
CHFL_EXPORT chfl_status chfl_topology_free(CHFL_TOPOLOGY* const topology);

#ifdef __cplusplus
}
#endif

#endif
