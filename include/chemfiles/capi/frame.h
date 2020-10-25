// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_FRAME_H
#define CHEMFILES_CHFL_FRAME_H

#include <stdint.h>
#include <stdbool.h>  // IWYU pragma: keep

#include "chemfiles/capi/types.h"
#include "chemfiles/exports.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Create a new empty frame.
///
/// The caller of this function should free the allocated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_frame/chfl_frame.c}
/// @return A pointer to the frame, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_FRAME* chfl_frame(void);

/// Get a copy of a `frame`.
///
/// The caller of this function should free the associated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_frame/copy.c}
/// @return A pointer to the new frame, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_FRAME* chfl_frame_copy(const CHFL_FRAME* frame);

/// Get the current number of atoms in a `frame` in the integer pointed to by
/// `count`
///
/// @example{capi/chfl_frame/atoms_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_atoms_count(
    const CHFL_FRAME* frame, uint64_t* count
);

/// Get a pointer to the positions array from a `frame`.
///
/// Positions are stored as a `size x 3` array, this function set the pointer
/// pointed to by `positions` to point to the first element of this array, and
/// give the number of atoms in the integer pointed to by `size`.
///
/// If the frame is resized (by writing to it, or calling `chfl_frame_resize`,
/// `chfl_frame_remove` or `chfl_frame_add_atom`), the pointer is invalidated.
///
/// If the frame memory is released using `chfl_free`, the memory behind the
/// `*positions` pointer is released too.
///
/// @example{capi/chfl_frame/positions.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_positions(
    CHFL_FRAME* frame, chfl_vector3d** positions, uint64_t* size
);

/// Get a pointer to the velocities array from a `frame`.
///
/// Velocities are stored as a `size x 3` array, this function set the pointer
/// pointed to by `velocities` to point to the first element of this array, and
/// give the number of atoms in the integer pointed to by `size`.
///
/// If the frame is resized (by writing to it, or calling `chfl_frame_resize`,
/// `chfl_frame_remove` or `chfl_frame_add_atom`), the pointer is invalidated.
///
/// If the frame memory is released using `chfl_free`, the memory behind the
/// `*velocities` pointer is released too.
///
/// If the frame does not have velocity, this will return an error. You can use
/// `chfl_frame_add_velocities` to ensure that a frame contains velocity data
/// before calling this function.
///
/// @example{capi/chfl_frame/velocities.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_velocities(
    CHFL_FRAME* frame, chfl_vector3d** velocities, uint64_t* size
);

/// Add an `atom` and the corresponding `position` and `velocity` data to a
/// `frame`.
///
/// `velocity` can be `NULL` if no velocity is associated with the atom.
///
/// @example{capi/chfl_frame/add_atom.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_add_atom(
    CHFL_FRAME* frame, const CHFL_ATOM* atom,
    const chfl_vector3d position, const chfl_vector3d velocity
);

/// Remove the atom at index `i` in the `frame`.
///
/// This modify all the atoms indexes after `i`, and invalidate any pointer
/// obtained using `chfl_frame_positions` or `chfl_frame_velocities`.
///
/// @example{capi/chfl_frame/remove.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_remove(CHFL_FRAME* frame, uint64_t i);

/// Resize the positions, velocities  and topology in the `frame`, to have space
/// for `size` atoms.
///
/// This function may invalidate any pointer to the positions or the velocities
/// if the new size is bigger than the old one. In all the cases, previous data
/// is conserved. This function conserve the presence or absence of velocities.
///
/// @example{capi/chfl_frame/resize.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_resize(
    CHFL_FRAME* frame, uint64_t size
);

/// Add velocity data to this `frame`.
///
/// The velocities are initialized to `(chfl_vector3d){0, 0, 0}`. If the frame
/// already has velocities, this does nothing.
///
/// @example{capi/chfl_frame/add_velocities.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_add_velocities(CHFL_FRAME* frame);

/// Check if this `frame` contains velocity data, and store the result in
/// `has_velocities`
///
/// @example{capi/chfl_frame/has_velocities.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_has_velocities(
    const CHFL_FRAME* frame, bool* has_velocities
);

/// Set the unit cell of a `frame` to `cell`.
///
/// @example{capi/chfl_frame/set_cell.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_set_cell(
    CHFL_FRAME* frame, const CHFL_CELL* cell
);

/// Set the topology of a `frame` to `topology`.
///
/// Calling this function with a topology that does not contain the right
/// number of atom will return an error.
///
/// @example{capi/chfl_frame/set_topology.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_set_topology(
    CHFL_FRAME* frame, const CHFL_TOPOLOGY* topology
);

/// Get a `frame` step, *i.e.* the frame number in the trajectory in the integer
/// pointed to by `step`.
///
/// @example{capi/chfl_frame/step.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_step(
    const CHFL_FRAME* frame, uint64_t* step
);

/// Set a `frame` step, *i.e.* the frame number in the trajectory to `step`.
///
/// @example{capi/chfl_frame/set_step.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_set_step(
    CHFL_FRAME* frame, uint64_t step
);

/// Guess the bonds, angles and dihedrals in a `frame`.
///
/// The bonds are guessed using a distance-based algorithm, and then angles and
/// dihedrals are guessed from the bonds.
///
/// @example{capi/chfl_frame/guess_bonds.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_guess_bonds(CHFL_FRAME* frame);

/// Get the distance between the atoms at indexes `i` and `j` in the `frame`,
/// accounting for periodic boundary conditions. The result is placed in
/// `distance`, and expressed in angstroms.
///
/// @example{capi/chfl_frame/distance.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_distance(
    const CHFL_FRAME* frame, uint64_t i, uint64_t j, double* distance
);

/// Get the angle formed by the atoms at indexes `i`, `j` and `k` in the
/// `frame`, accounting for periodic boundary conditions. The result is placed
/// in `angle`, and expressed in radians.
///
/// @example{capi/chfl_frame/angle.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_angle(
    const CHFL_FRAME* frame, uint64_t i, uint64_t j, uint64_t k, double* angle
);

/// Get the angle formed by the atoms at indexes `i`, `j`, `k` and `m` in the
/// `frame`, accounting for periodic boundary conditions. The result is placed
/// in `dihedral`, and expressed in radians.
///
/// @example{capi/chfl_frame/dihedral.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_dihedral(
    const CHFL_FRAME* frame, uint64_t i, uint64_t j, uint64_t k, uint64_t m, double* dihedral
);

/// Get the out of plane distance formed by the atoms at indexes `i`, `j`, `k`
/// and `m` in the `frame`, accounting for periodic boundary conditions. The
/// result is placed in `distance` and expressed in angstroms.
///
/// This is the distance between the atom j and the ikm plane. The j atom
/// is the center of the improper dihedral angle formed by i, j, k and m.
///
/// @example{capi/chfl_frame/out_of_plane.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_out_of_plane(
    const CHFL_FRAME* frame, uint64_t i, uint64_t j, uint64_t k, uint64_t m, double* distance
);

/// Get the number of properties associated with this `frame` in `count`.
///
/// @example{capi/chfl_frame/properties_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_properties_count(
    const CHFL_FRAME* frame, uint64_t* count
);

/// Get the names of all properties of this `frame` in the pre-allocated array
/// `names` of size `count`.
///
/// `names` size must be passed in the `count` parameter, and be equal to the
/// result of `chfl_frame_properties_count`.
///
/// The pointers in `names` are only valid until a new property is added to the
/// frame with `chfl_frame_set_property`.
///
/// @example{capi/chfl_frame/list_properties.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_list_properties(
    const CHFL_FRAME* frame, const char* names[], uint64_t count
);

/// Add a new `property` with the given `name` to this `frame`.
///
/// If a property with the same name already exists, this function override the
/// existing property with the new one.
///
/// @example{capi/chfl_frame/property.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_set_property(
    CHFL_FRAME* frame, const char* name, const CHFL_PROPERTY* property
);

/// Get a property with the given `name` in this `frame`.
///
/// This function returns `NULL` if no property exists with the given name.
///
/// The user of this function is responsible to deallocate memory using the
/// `chfl_free` function.
///
/// @example{capi/chfl_frame/property.c}
/// @return A pointer to the property, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_PROPERTY* chfl_frame_get_property(
    const CHFL_FRAME* frame, const char* name
);

/// Add a bond between the atoms at indexes `i` and `j` in the `frame`.
///
/// @example{capi/chfl_frame/add_bond.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_add_bond(
    CHFL_FRAME* frame, uint64_t i, uint64_t j
);

/// Add a bond between the atoms at indexes `i` and `j`
/// with bond order `bond_order` in the `frame`.
///
/// @example{capi/chfl_frame/add_bond.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_bond_with_order(
    CHFL_FRAME* frame, uint64_t i, uint64_t j, chfl_bond_order bond_order
);

/// Remove any existing bond between the atoms at indexes `i` and `j` in the
/// `frame`.
///
/// This function does nothing if there is no bond between `i` and `j`.
///
/// @example{capi/chfl_frame/remove_bond.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_remove_bond(
    CHFL_FRAME* frame, uint64_t i, uint64_t j
);

/// Remove all existing bonds, angles, dihedral angles and improper dihedral
/// angles in the `frame`.
///
/// @example{capi/chfl_frame/clear_bonds.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_clear_bonds(CHFL_FRAME* frame);

/// Add a copy of `residue` to this `frame`.
///
/// The residue id must not already be in this frame's topology, and the residue
/// must contain only atoms that are not already in another residue.
///
/// @example{capi/chfl_frame/add_residue.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_frame_add_residue(
    CHFL_FRAME* frame, const CHFL_RESIDUE* residue
);

#ifdef __cplusplus
}
#endif

#endif
