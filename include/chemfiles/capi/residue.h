// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CHFL_RESIDUE_H
#define CHEMFILES_CHFL_RESIDUE_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// @brief Create a new residue
/// @param name The residue name
/// @param resid The residue identifier, or `uint64_t(-1)` if the residue do not
///              have an identifier
/// @return The status code
CHFL_EXPORT CHFL_RESIDUE* chfl_residue(const char* name, uint64_t resid);

/// @brief Get a residue from a topology
/// @param topology The topology
/// @param i The residue index in the topology. This is not always the same as
///          the `resid`. This value should be between 0 and the result of
///          `chfl_topology_residues_count`.
/// @return The status code
CHFL_EXPORT CHFL_RESIDUE* chfl_residue_from_topology(
    const CHFL_TOPOLOGY* const topology, uint64_t i
);

/// @brief Get a the residue containing a given atom, or NULL if the atom is not
///        in a residue.
/// @param topology The topology
/// @param i The atom index
/// @return The status code
CHFL_EXPORT CHFL_RESIDUE* chfl_residue_for_atom(
    const CHFL_TOPOLOGY* const topology, uint64_t i
);

/// @brief Get the number of atoms in a residue
/// @param residue The residue
/// @param size The size of the residue
/// @return The status code
CHFL_EXPORT chfl_status chfl_residue_atoms_count(
    const CHFL_RESIDUE* const residue, uint64_t* size
);

/// @brief Get the identifier of a residue in the initial topology file
/// @param residue The residue
/// @param id The id of the residue
/// @return The status code
CHFL_EXPORT chfl_status chfl_residue_id(
    const CHFL_RESIDUE* const residue, uint64_t* id
);

/// @brief Get the name of a residue
/// @param residue The residue
/// @param name A string buffer to be filled with the name
/// @param buffsize The size of the string buffer
/// @return The status code
CHFL_EXPORT chfl_status chfl_residue_name(
    const CHFL_RESIDUE* const residue, char* name, uint64_t buffsize
);

/// @brief Add the atom at index `i` in the residue
/// @param residue The residue
/// @param i The atomic index
/// @return The status code
CHFL_EXPORT chfl_status chfl_residue_add_atom(
    CHFL_RESIDUE* const residue, uint64_t i
);

/// @brief Check if the atom at index `i` is in the residue
/// @param residue The residue
/// @param i The atomic index
/// @param result true if the atom is in the residue, false otherwise
/// @return The status code
CHFL_EXPORT chfl_status chfl_residue_contains(
    const CHFL_RESIDUE* const residue, uint64_t i, bool* result
);

/// @brief Destroy a residue, and free the associated memory
/// @param residue The residue to destroy
/// @return The status code
CHFL_EXPORT chfl_status chfl_residue_free(CHFL_RESIDUE* const residue);

#ifdef __cplusplus
}
#endif

#endif
