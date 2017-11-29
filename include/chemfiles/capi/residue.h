// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_RESIDUE_H
#define CHEMFILES_CHFL_RESIDUE_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// Create a new residue with the given `name`.
///
/// The caller of this function should free the allocated memory using
/// `chfl_residue_free`.
///
/// @example{tests/capi/doc/chfl_residue/chfl_residue.c}
/// @return A pointer to the residue, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_RESIDUE* chfl_residue(const char* name);

/// Create a new residue with the given `name` and residue identifier `resid`.
///
/// The caller of this function should free the allocated memory using
/// `chfl_residue_free`.
///
/// @example{tests/capi/doc/chfl_residue/with_id.c}
/// @return A pointer to the residue, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_RESIDUE* chfl_residue_with_id(const char* name, uint64_t resid);

/// Get a copy of the residue at index `i` from a `topology`.
///
/// If `i` is bigger than the result of `chfl_topology_residues_count`, this
/// function will return `NULL`.
///
/// The residue index in the topology is not always the same as the residue
/// `id`.
///
/// The caller of this function should free the allocated memory using
/// `chfl_residue_free`.
///
/// @example{tests/capi/doc/chfl_residue/from_topology.c}
/// @return A pointer to the residue, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_RESIDUE* chfl_residue_from_topology(
    const CHFL_TOPOLOGY* topology, uint64_t i
);

/// Get a copy of the residue containing the atom at index `i` in the
/// `topology`.
///
/// This function will return `NULL` if the atom is not in a residue, or if the
/// index `i` is bigger than `chfl_topology_atoms_count`.
///
/// The caller of this function should free the allocated memory using
/// `chfl_residue_free`.
///
/// @example{tests/capi/doc/chfl_residue/for_atom.c}
/// @return A pointer to the residue, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_RESIDUE* chfl_residue_for_atom(
    const CHFL_TOPOLOGY* topology, uint64_t i
);

/// Get a copy of a `residue`.
///
/// The caller of this function should free the associated memory using
/// `chfl_residue_free`.
///
/// @example{tests/capi/doc/chfl_residue/copy.c}
/// @return A pointer to the new residue, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_RESIDUE* chfl_residue_copy(const CHFL_RESIDUE* residue);

/// Get the number of atoms in a `residue` in the integer pointed to by `size`.
///
/// @example{tests/capi/doc/chfl_residue/atoms_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_residue_atoms_count(
    const CHFL_RESIDUE* residue, uint64_t* size
);

/// Get the list of atoms in the `residue` in the pre-allocated array `atoms`
/// of size `natoms`.
///
/// The `atoms` array size must be passed in the `natoms` parameter, and be
/// equal to the result of `chfl_residue_atoms_count`. The `atoms` array is
/// sorted.
///
/// @example{tests/capi/doc/chfl_residue/atoms.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_residue_atoms(
    const CHFL_RESIDUE* residue, uint64_t atoms[], uint64_t natoms
);

/// Get the identifier of a `residue` in the initial topology file in the
/// integer pointed to by `id`.
///
/// This function will return `CHFL_GENERIC_ERROR` if this residue does not
/// have an associated identifier.
///
/// @example{tests/capi/doc/chfl_residue/id.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_residue_id(
    const CHFL_RESIDUE* residue, uint64_t* id
);

/// Get the name of a `residue` in the string buffer `name`.
///
/// The buffer size must be passed in `buffsize`. This function will truncate
/// the residue name to fit in the buffer.
///
/// @example{tests/capi/doc/chfl_residue/name.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_residue_name(
    const CHFL_RESIDUE* residue, char* name, uint64_t buffsize
);

/// Add the atom at index `i` in the `residue`.
///
/// @example{tests/capi/doc/chfl_residue/add_atom.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_residue_add_atom(
    CHFL_RESIDUE* residue, uint64_t i
);

/// Check if the atom at index `i` is in the `residue`, and store the result in
/// `result`.
///
/// @example{tests/capi/doc/chfl_residue/contains.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_residue_contains(
    const CHFL_RESIDUE* residue, uint64_t i, bool* result
);

/// Free the memory associated with a `residue`.
///
/// @example{tests/capi/doc/chfl_residue/chfl_residue.c}
/// @return `CHFL_SUCCESS`
CHFL_EXPORT chfl_status chfl_residue_free(CHFL_RESIDUE* residue);

#ifdef __cplusplus
}
#endif

#endif
