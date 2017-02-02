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

/// Create a new empty topology.
///
/// @example{tests/capi/doc/chfl_topology/chfl_topology.c}
/// @return A pointer to the topology, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology(void);

/// Get a copy of the topology of a `frame`.
///
/// @example{tests/capi/doc/chfl_topology/from_frame.c}
/// @return A pointer to the topology, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology_from_frame(
    const CHFL_FRAME* const frame
);

/// Get a copy of a `topology`.
///
/// The caller of this function should free the associated memory using
/// `chfl_topology_free`.
///
/// @example{tests/capi/doc/chfl_topology/copy.c}
/// @return A pointer to the new topology, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology_copy(const CHFL_TOPOLOGY* const topology);

/// Get the number of atoms in the `topology` in the integer pointed to by
/// `natoms`.
///
/// @example{tests/capi/doc/chfl_topology/atoms_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_atoms_count(
    const CHFL_TOPOLOGY* const topology, uint64_t* natoms
);

/// Resize the `topology` to hold `natoms` atoms. If the new number of atoms is
/// bigger than the current number, new atoms will be created with an empty name
/// and type.
///
/// @example{tests/capi/doc/chfl_topology/resize.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_resize(
    CHFL_TOPOLOGY* const topology, uint64_t natoms
);

/// Add a copy of an `atom` at the end of a `topology`.
///
/// @example{tests/capi/doc/chfl_topology/add_atom.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_add_atom(
    CHFL_TOPOLOGY* const topology, const CHFL_ATOM* const atom
);

/// Remove the atom at index `i` from a `topology`.
///
/// This modify all the atoms indexes after `i`.
///
/// @example{tests/capi/doc/chfl_topology/remove.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_remove(
    CHFL_TOPOLOGY* const topology, uint64_t i
);

/// Check if the atoms at indexes `i` and `j` are bonded together, and store
/// the result in `result`.
///
/// @example{tests/capi/doc/chfl_topology/isbond.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_isbond(
    const CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j, bool* result
);

/// Check if the atoms at indexes `i`, `j` and `k` form an angle, and store the
/// result in `result`.
///
/// @example{tests/capi/doc/chfl_topology/isangle.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_isangle(
    const CHFL_TOPOLOGY* const topology,
    uint64_t i,
    uint64_t j,
    uint64_t k,
    bool* result
);

/// Check if the atoms at indexes `i`, `j` and `k` form a dihedral angle, and
/// store the result in `result`.
///
/// @example{tests/capi/doc/chfl_topology/isdihedral.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_isdihedral(
    const CHFL_TOPOLOGY* const topology,
    uint64_t i,
    uint64_t j,
    uint64_t k,
    uint64_t m,
    bool* result
);

/// Get the number of bonds in the `topology` in `nbonds`.
///
/// @example{tests/capi/doc/chfl_topology/bonds_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_bonds_count(
    const CHFL_TOPOLOGY* const topology, uint64_t* nbonds
);

/// Get the number of angles in the `topology` in `nangles`.
///
/// @example{tests/capi/doc/chfl_topology/angles_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_angles_count(
    const CHFL_TOPOLOGY* const topology, uint64_t* nangles
);

/// Get the number of dihedral angles in the `topology` in `ndihedrals`.
///
/// @example{tests/capi/doc/chfl_topology/dihedrals_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_dihedrals_count(
    const CHFL_TOPOLOGY* const topology, uint64_t* ndihedrals
);

/// Get the list of bonds in the `topology` in the pre-allocated array `data`
/// of size `nbonds`.
///
/// `data` size must be passed in the `nbonds` parameter, and be equal to the
/// result of `chfl_topology_bonds_count`.
///
/// @example{tests/capi/doc/chfl_topology/bonds.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_bonds(
    const CHFL_TOPOLOGY* const topology, uint64_t (*data)[2], uint64_t nbonds
);

/// Get the list of angles in the `topology` in the pre-allocated array `data`
/// of size `nangles`.
///
/// `data` size must be passed in the `nangles` parameter, and be equal to the
/// result of `chfl_topology_angles_count`.
///
/// @example{tests/capi/doc/chfl_topology/angles.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_angles(
    const CHFL_TOPOLOGY* const topology, uint64_t (*data)[3], uint64_t nangles
);

/// Get the list of dihedral angles in the `topology` in the pre-allocated array
/// `data` of size `ndihedrals`.
///
/// `data` size must be passed in the `ndihedrals` parameter, and be equal to
/// the result of `chfl_topology_dihedrals_count`.
///
/// @example{tests/capi/doc/chfl_topology/dihedrals.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_dihedrals(
    const CHFL_TOPOLOGY* const topology, uint64_t (*data)[4], uint64_t ndihedrals
);

/// Add a bond between the atoms at indexes `i` and `j` in the `topology`.
///
/// @example{tests/capi/doc/chfl_topology/add_bond.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_add_bond(
    CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j
);

/// Remove any existing bond between the atoms `i` and `j` in the `topology`.
///
/// This function does nothing if there is no bond between `i` and `j`.
///
/// @example{tests/capi/doc/chfl_topology/remove_bond.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_remove_bond(
    CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j
);

/// Get the number of residues in the `topology` in the integer pointed to by
/// `nresidues`.
///
/// @example{tests/capi/doc/chfl_topology/residues_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_residues_count(
    const CHFL_TOPOLOGY* const topology, uint64_t* nresidues
);

/// Add a `residue` to this `topology`.
///
/// The residue id must not already be in the topology, and the residue must
/// contain only atoms that are not already in another residue.
///
/// @example{tests/capi/doc/chfl_topology/add_residue.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_add_residue(
    CHFL_TOPOLOGY* const topology, const CHFL_RESIDUE* const residue
);

/// Check if the two residues `first` and `second` from the `topology` are
/// linked together, *i.e.* if there is a bond between one atom in the first
/// residue and one atom in the second one, and store the result in `result.`
///
/// @example{tests/capi/doc/chfl_topology/residues_linked.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_residues_linked(
    const CHFL_TOPOLOGY* const topology,
    const CHFL_RESIDUE* const first,
    const CHFL_RESIDUE* const second,
    bool* result
);

/// Free the memory associated with a `topology`.
///
/// @example{tests/capi/doc/chfl_topology/chfl_topology.c}
/// @return `CHFL_SUCCESS`
CHFL_EXPORT chfl_status chfl_topology_free(CHFL_TOPOLOGY* const topology);

#ifdef __cplusplus
}
#endif

#endif
