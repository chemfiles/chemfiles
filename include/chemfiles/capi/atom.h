// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CHFL_ATOM_H
#define CHEMFILES_CHFL_ATOM_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// @brief Create an atom from a name
/// @param name The new atom name
/// @return A pointer to the new atom, or NULL in case of error
CHFL_EXPORT CHFL_ATOM* chfl_atom(const char* name);

/// @brief Get a specific atom from a frame
/// @param frame The frame
/// @param idx The atom index in the frame
/// @return A pointer to the new atom, or NULL in case of error or if `idx`
///         is out of bounds
CHFL_EXPORT CHFL_ATOM* chfl_atom_from_frame(
    const CHFL_FRAME* const frame, uint64_t idx
);

/// @brief Get a specific atom from a topology
/// @param topology The topology
/// @param idx The atom index in the topology
/// @return A pointer to the new atom, or NULL in case of error or if `idx`
///         is out of bounds
CHFL_EXPORT CHFL_ATOM* chfl_atom_from_topology(
    const CHFL_TOPOLOGY* const topology, uint64_t idx
);

/// @brief Get the mass of an atom, in atomic mass units
/// @param atom The atom
/// @param mass The atom mass
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_mass(
    const CHFL_ATOM* const atom, double* mass
);

/// @brief Set the mass of an atom, in atomic mass units
/// @param atom The atom
/// @param mass The new atom mass
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_set_mass(
    CHFL_ATOM* const atom, double mass
);

/// @brief Get the charge of an atom, in number of the electron charge e
/// @param atom The atom
/// @param charge The atom charge
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_charge(
    const CHFL_ATOM* const atom, double* charge
);

/// @brief Set the charge of an atom, in number of the electron charge e
/// @param atom The atom
/// @param charge The new atom charge
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_set_charge(CHFL_ATOM* const atom, double charge);

/// @brief Get the type of an atom
/// @param atom The atom
/// @param type A string buffer to be filled with the atom type
/// @param buffsize The size of the string buffer
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_type(
    const CHFL_ATOM* const atom, char* const type, uint64_t buffsize
);

/// @brief Set the type of an atom
/// @param atom The atom
/// @param type A null terminated string containing the new atom type
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_set_type(
    CHFL_ATOM* const atom, const char* type
);

/// @brief Get the name of an atom
/// @param atom The atom
/// @param name A string buffer to be filled with the atom name
/// @param buffsize The size of the string buffer
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_name(
    const CHFL_ATOM* const atom, char* const name, uint64_t buffsize
);

/// @brief Set the name of an atom
/// @param atom The atom
/// @param name A null terminated string containing the new atom name
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_set_name(
    CHFL_ATOM* const atom, const char* name
);

/// @brief Try to get the full name of an atom from the atom type
/// @param atom The atom
/// @param name A string buffer to be filled with the full name
/// @param buffsize The size of the string buffer
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_full_name(
    const CHFL_ATOM* const atom, char* const name, uint64_t buffsize
);

/// @brief Try to get the Van der Waals radius of an atom from the atom type
/// @param atom The atom
/// @param radius The Van der Waals radius of the atom or -1 if no value could be
/// found.
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_vdw_radius(
    const CHFL_ATOM* const atom, double* radius
);

/// @brief Try to get the covalent radius of an atom from the atom type
/// @param atom The atom
/// @param radius The covalent radius of the atom or -1 if no value could be
/// found.
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_covalent_radius(
    const CHFL_ATOM* const atom, double* radius
);

/// @brief Try to get the atomic number of an atom from the atom type
/// @param atom The atom
/// @param number The atomic number, or -1 if no value could be found.
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_atomic_number(
    const CHFL_ATOM* const atom, int64_t* number
);

/// @brief Destroy an atom, and free the associated memory
/// @param atom The atom to destroy
/// @return The status code
CHFL_EXPORT chfl_status chfl_atom_free(CHFL_ATOM* const atom);

#ifdef __cplusplus
}
#endif

#endif // CHEMFILES_CAPI_ATOM_H
