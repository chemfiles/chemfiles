// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CHFL_FRAME_H
#define CHEMFILES_CHFL_FRAME_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// @brief Create an empty frame with initial capacity of `natoms`. It will be
///        resized by the library as needed.
/// @param natoms the size of the wanted frame
/// @return A pointer to the frame, or NULL in case of error
CHFL_EXPORT CHFL_FRAME* chfl_frame(uint64_t natoms);

/// @brief Get the current number of atoms in the frame.
/// @param frame The frame to analyse
/// @param natoms the number of atoms in the frame
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_atoms_count(
    const CHFL_FRAME* const frame, uint64_t* natoms
);

/// @brief Get a pointer to the positions array from a frame.
///
/// The positions are stored as a N x 3 array, this function set a pointer to
/// point to the first element of this array, and give the value of N. If the
/// frame is resized (by writing to it, or calling `chfl_frame_resize`), the
/// pointer is invalidated.
///
/// @param frame The frame
/// @param data A pointer to a `chfl_vector_t` array, which will be set to point
///             to the positions data.
/// @param size A pointer to the an integer to be filled with the array size
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_positions(
    CHFL_FRAME* const frame, chfl_vector_t** data, uint64_t* size
);

/// @brief Get a pointer to the velocities array from a frame.
///
/// The velocities are stored as a `N x 3` array, this function set a pointer
/// to point to the first element of this array, and give the value of N. If the
/// frame is resized (by writing to it, or calling `chfl_frame_resize`), the
/// pointer is invalidated.
///
/// If the frame do not have velocity, this will return an error. Use
/// `chfl_frame_add_velocities` to add velocities to a frame before calling
/// this function.
///
/// @param frame The frame
/// @param data A pointer to a `chfl_vector_t` array, which will be set to point
///             to the velocity data.
/// @param size A pointer to the an integer to be filled with the array size
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_velocities(
    CHFL_FRAME* const frame, chfl_vector_t** data, uint64_t* size
);

/// @brief Add an atom and the corresponding position (and velocity) data to a
///        frame.
///
/// @param frame The frame
/// @param atom The atom to add
/// @param position The position of the atom
/// @param velocity The velocity of the atom. This parameter can be `NULL` if no
///                 velocity is associated with the atom.
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_add_atom(
    CHFL_FRAME* const frame, const CHFL_ATOM* const atom,
    chfl_vector_t position, chfl_vector_t velocity
);

/// @brief Resize the positions and the velocities in frame, to make space for
///        `natoms` atoms.
///
/// This function may invalidate any pointer to the positions or the
/// velocities if the new size is bigger than the old one. In all the cases,
/// previous data is conserved. This function conserve the presence of absence of
/// velocities.
///
/// @param frame The frame
/// @param natoms The new number of atoms.
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_resize(
    CHFL_FRAME* const frame, uint64_t natoms
);

/// @brief Add velocity storage to this frame.
///
/// The storage is initialized with the result of `chfl_frame_atoms_count` as
/// number of atoms. If the frame already have velocities, this does nothing.
///
/// @param frame The frame
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_add_velocities(CHFL_FRAME* const frame);

/// @brief Ask wether this frame contains velocity data or not.
/// @param frame The frame
/// @param has_velocities A boolean, will be true if the frame have velocities,
///                       false otherwise.
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_has_velocities(
    const CHFL_FRAME* const frame, bool* has_velocities
);

/// @brief Set the unit cell of a Frame.
/// @param frame The frame
/// @param cell The new cell
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_set_cell(
    CHFL_FRAME* const frame, const CHFL_CELL* const cell
);

/// @brief Set the Topology of a Frame.
/// @param frame The frame
/// @param topology The new topology
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_set_topology(
    CHFL_FRAME* const frame, const CHFL_TOPOLOGY* const topology
);

/// @brief Get the Frame step, i.e. the frame number in the trajectory
/// @param frame The frame
/// @param step This will contains the step number
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_step(
    const CHFL_FRAME* const frame, uint64_t* step
);

/// @brief Set the Frame step.
/// @param frame The frame
/// @param step The new frame step
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_set_step(
    CHFL_FRAME* const frame, uint64_t step
);

/// @brief  Guess the bonds, angles and dihedrals in the system.
///
/// The bonds are guessed using a distance-based algorithm, and then angles and
/// dihedrals are guessed from the bonds.
///
/// @param frame The Frame to analyse
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_guess_topology(CHFL_FRAME* const frame);

/// @brief Destroy a frame, and free the associated memory
/// @param frame The frame to destroy
/// @return The status code
CHFL_EXPORT chfl_status chfl_frame_free(CHFL_FRAME* const frame);

#ifdef __cplusplus
}
#endif

#endif
