// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_TOPOLOGY_H
#define CHEMFILES_CHFL_TOPOLOGY_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// Create a new empty topology.
///
/// The caller of this function should free the associated memory using
/// `chfl_topology_free`.
///
/// @example{tests/capi/doc/chfl_topology/chfl_topology.c}
/// @return A pointer to the topology, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology(void);

/// Get a copy of the topology of a `frame`.
///
/// The caller of this function should free the associated memory using
/// `chfl_topology_free`.
///
/// @example{tests/capi/doc/chfl_topology/from_frame.c}
/// @return A pointer to the topology, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology_from_frame(const CHFL_FRAME* frame);

/// Get a copy of a `topology`.
///
/// The caller of this function should free the associated memory using
/// `chfl_topology_free`.
///
/// @example{tests/capi/doc/chfl_topology/copy.c}
/// @return A pointer to the new topology, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology_copy(const CHFL_TOPOLOGY* topology);

/// Get the number of atoms in the `topology` in the integer pointed to by
/// `size`.
///
/// @example{tests/capi/doc/chfl_topology/atoms_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_atoms_count(
    const CHFL_TOPOLOGY* topology, uint64_t* size
);

/// Resize the `topology` to hold `natoms` atoms. If the new number of atoms is
/// bigger than the current number, new atoms will be created with an empty name
/// and type. If it is lower than the current number of atoms, the last atoms
/// will be removed, together with the associated bonds, angles and dihedrals.
///
/// @example{tests/capi/doc/chfl_topology/resize.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_resize(CHFL_TOPOLOGY* topology, uint64_t natoms);

/// Add a copy of an `atom` at the end of a `topology`.
///
/// @example{tests/capi/doc/chfl_topology/add_atom.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_add_atom(
    CHFL_TOPOLOGY* topology, const CHFL_ATOM* atom
);

/// Remove the atom at index `i` from a `topology`.
///
/// This shifts all the atoms indexes after `i` by 1 (n becomes n-1).
///
/// @example{tests/capi/doc/chfl_topology/remove.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_remove(
    CHFL_TOPOLOGY* topology, uint64_t i
);

/// Get the number of bonds in the `topology` in `nbonds`.
///
/// @example{tests/capi/doc/chfl_topology/bonds_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_bonds_count(
    const CHFL_TOPOLOGY* topology, uint64_t* nbonds
);

/// Get the number of angles in the `topology` in `nangles`.
///
/// @example{tests/capi/doc/chfl_topology/angles_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_angles_count(
    const CHFL_TOPOLOGY* topology, uint64_t* nangles
);

/// Get the number of dihedral angles in the `topology` in `ndihedrals`.
///
/// @example{tests/capi/doc/chfl_topology/dihedrals_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_dihedrals_count(
    const CHFL_TOPOLOGY* topology, uint64_t* ndihedrals
);

/// Get the number of improper dihedral angles in the `topology` in `nimpropers`.
///
/// @example{tests/capi/doc/chfl_topology/impropers_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_impropers_count(
    const CHFL_TOPOLOGY* topology, uint64_t* nimpropers
);

/// Get the list of bonds in the `topology` in the pre-allocated array `data`
/// of size `nbonds`.
///
/// `data` size must be passed in the `nbonds` parameter, and be equal to the
/// result of `chfl_topology_bonds_count`. The bonds are sorted in the array.
///
/// @example{tests/capi/doc/chfl_topology/bonds.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_bonds(
    const CHFL_TOPOLOGY* topology, uint64_t (*data)[2], uint64_t nbonds
);

/// Get the list of angles in the `topology` in the pre-allocated array `data`
/// of size `nangles`.
///
/// `data` size must be passed in the `nangles` parameter, and be equal to the
/// result of `chfl_topology_angles_count`. The angles are sorted in the array.
///
/// @example{tests/capi/doc/chfl_topology/angles.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_angles(
    const CHFL_TOPOLOGY* topology, uint64_t (*data)[3], uint64_t nangles
);

/// Get the list of dihedral angles in the `topology` in the pre-allocated array
/// `data` of size `ndihedrals`.
///
/// `data` size must be passed in the `ndihedrals` parameter, and be equal to
/// the result of `chfl_topology_dihedrals_count`. The dihedrals are sorted in
/// the array.
///
/// @example{tests/capi/doc/chfl_topology/dihedrals.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_dihedrals(
    const CHFL_TOPOLOGY* topology, uint64_t (*data)[4], uint64_t ndihedrals
);

/// Get the list of improper dihedral angles in the `topology` in the
/// pre-allocated array `data` of size `nimpropers`.
///
/// `data` size must be passed in the `nimpropers` parameter, and be equal to
/// the result of `chfl_topology_impropers_count`. The impropers are sorted in
/// the array.
///
/// @example{tests/capi/doc/chfl_topology/impropers.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_impropers(
    const CHFL_TOPOLOGY* topology, uint64_t (*data)[4], uint64_t nimpropers
);

/// Add a bond between the atoms at indexes `i` and `j` in the `topology`.
///
/// @example{tests/capi/doc/chfl_topology/add_bond.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_add_bond(
    CHFL_TOPOLOGY* topology, uint64_t i, uint64_t j
);

/// Remove any existing bond between the atoms at indexes `i` and `j` in the
/// `topology`.
///
/// This function does nothing if there is no bond between `i` and `j`.
///
/// @example{tests/capi/doc/chfl_topology/remove_bond.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_remove_bond(
    CHFL_TOPOLOGY* topology, uint64_t i, uint64_t j
);

/// Get the number of residues in the `topology` in the integer pointed to by
/// `residues`.
///
/// @example{tests/capi/doc/chfl_topology/residues_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_residues_count(
    const CHFL_TOPOLOGY* topology, uint64_t* residues
);

/// Add a copy of `residue` to this `topology`.
///
/// The residue id must not already be in the topology, and the residue must
/// contain only atoms that are not already in another residue.
///
/// @example{tests/capi/doc/chfl_topology/add_residue.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_add_residue(
    CHFL_TOPOLOGY* topology, const CHFL_RESIDUE* residue
);

/// Check if the two residues `first` and `second` from the `topology` are
/// linked together, *i.e.* if there is a bond between one atom in the first
/// residue and one atom in the second one, and store the result in `result.`
///
/// @example{tests/capi/doc/chfl_topology/residues_linked.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_topology_residues_linked(
    const CHFL_TOPOLOGY* topology,
    const CHFL_RESIDUE* first,
    const CHFL_RESIDUE* second,
    bool* result
);

/// Free the memory associated with a `topology`.
///
/// @example{tests/capi/doc/chfl_topology/chfl_topology.c}
/// @return `CHFL_SUCCESS`
CHFL_EXPORT chfl_status chfl_topology_free(CHFL_TOPOLOGY* topology);

#ifdef __cplusplus
}
#endif

#endif
