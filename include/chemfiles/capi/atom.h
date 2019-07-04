// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_ATOM_H
#define CHEMFILES_CHFL_ATOM_H

#include <stdint.h>

#include "chemfiles/capi/types.h"
#include "chemfiles/exports.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Create an atom with the given `name`, and set the atom type to `name`.
///
/// The caller of this function should free the associated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_atom/chfl_atom.c}
/// @return A pointer to the atom, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_ATOM* chfl_atom(const char* name);

/// Get a copy of an `atom`.
///
/// The caller of this function should free the associated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_atom/copy.c}
/// @return A pointer to the new atom, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_ATOM* chfl_atom_copy(const CHFL_ATOM* atom);

/// Get access to the atom at the given `index` from a `frame`.
///
/// Any modification to the atom will be reflected in the `frame`. The `frame`
/// will be kept alive, even if `chfl_free(frame)` is called, until `chfl_free`
/// is also called on the pointer returned by this function.
///
/// The pointer returned by this function points directly inside the frame, and
/// will be invalidated if any of the following function is called on the frame:
///
/// - `chfl_frame_resize`
/// - `chfl_frame_add_atom`
/// - `chfl_frame_remove`
/// - `chfl_frame_set_topology`
/// - `chfl_trajectory_read`
/// - `chfl_trajectory_read_step`
///
/// Calling any function on an invalidated pointer is undefined behavior. Even
/// if the pointer if invalidated, it stills needs to be released with
/// `chfl_free`.
///
/// @example{capi/chfl_atom/from_frame.c}
///
/// @return A pointer to the atom, or NULL in case of error or if `index` is out
/// of bounds. You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_ATOM* chfl_atom_from_frame(CHFL_FRAME* frame, uint64_t index);

/// Get access to the atom at the given `index` from a `topology`
///
/// Any modification to the atom will be reflected in the `topology`. The
/// `topology` will be kept alive, even if `chfl_free(topology)` is called,
/// until `chfl_free` is also called on the pointer returned by this function.
///
/// The pointer returned by this function points directly inside the topology,
/// and will be invalidated if any of the following function is called on the
/// topology:
///
/// - `chfl_topology_resize`
/// - `chfl_topology_add_atom`
/// - `chfl_topology_remove`
///
/// Calling any function on an invalidated pointer is undefined behavior. Even
/// if the pointer if invalidated, it stills needs to be released with
/// `chfl_free`.
///
/// @example{capi/chfl_atom/from_topology.c} @return A pointer to the
/// atom, or NULL in case of error or if `index` is out of bounds. You can use
/// `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_ATOM* chfl_atom_from_topology(
    CHFL_TOPOLOGY* topology, uint64_t index
);

/// Get the mass of an `atom`, in the double pointed to by `mass`.
///
/// The mass is given in atomic mass units.
///
/// @example{capi/chfl_atom/mass.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_mass(const CHFL_ATOM* atom, double* mass);

/// Set the mass of an `atom` to `mass`.
///
/// The mass must be in atomic mass units.
///
/// @example{capi/chfl_atom/set_mass.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_set_mass(CHFL_ATOM* atom, double mass);

/// Get the charge of an `atom`, in the double pointed to by `charge`.
///
/// The charge is in number of the electron charge *e*.
///
/// @example{capi/chfl_atom/charge.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_charge(const CHFL_ATOM* atom, double* charge);

/// Set the charge of an `atom` to `charge`.
///
/// The charge must be in number of the electron charge *e*.
///
/// @example{capi/chfl_atom/set_charge.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_set_charge(CHFL_ATOM* atom, double charge);

/// Get the type of an `atom` in the string buffer `type`.
///
/// The buffer size must be passed in `buffsize`. This function will truncate
/// the atomic type to fit in the buffer.
///
/// @example{capi/chfl_atom/type.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_type(
    const CHFL_ATOM* atom, char* type, uint64_t buffsize
);

/// Set the type of an `atom` to `type`.
///
/// `type` must be a null terminated string.
///
/// @example{capi/chfl_atom/set_type.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_set_type(CHFL_ATOM* atom, const char* type);

/// Get the name of an `atom` in the string buffer `name`.
///
/// The buffer size must be passed in `buffsize`. This function will truncate
/// the name to fit in the buffer.
///
/// @example{capi/chfl_atom/name.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_name(
    const CHFL_ATOM* atom, char* name, uint64_t buffsize
);

/// Set the name of an `atom` to `name`.
///
/// `name` must be a null terminated string.
///
/// @example{capi/chfl_atom/set_name.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_set_name(CHFL_ATOM* atom, const char* name);

/// Get the full name of an `atom` from its type in the string buffer `name`
///
/// The buffer size must be passed in `buffsize`. This function will truncate
/// the name to fit in the buffer.
///
/// @example{capi/chfl_atom/full_name.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_full_name(
    const CHFL_ATOM* atom, char* name, uint64_t buffsize
);

/// Get the Van der Waals radius of an `atom` from the atom type, in the double
/// pointed to by `radius`.
///
/// If the radius in unknown, this function set `radius` to 0.
///
/// @example{capi/chfl_atom/vdw_radius.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_vdw_radius(const CHFL_ATOM* atom, double* radius);

/// Get the covalent radius of an `atom` from the atom type, in the double
/// pointed to by `radius`.
///
/// If the radius in unknown, this function set `radius` to 0.
///
/// @example{capi/chfl_atom/covalent_radius.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_covalent_radius(const CHFL_ATOM* atom, double* radius);

/// Get the atomic number of an `atom` from the atom type, in the integer
/// pointed to by `number`.
///
/// If the atomic number in unknown, this function set `number` to 0.
///
/// @example{capi/chfl_atom/atomic_number.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_atomic_number(const CHFL_ATOM* atom, uint64_t* number);

/// Get the number of properties associated with this `atom` in `count`.
///
/// @example{capi/chfl_atom/properties_count.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_properties_count(
    const CHFL_ATOM* atom, uint64_t* count
);

/// Get the names of all properties of this `atom` in the pre-allocated array
/// `names` of size `count`.
///
/// `names` size must be passed in the `count` parameter, and be equal to the
/// result of `chfl_atom_properties_count`.
///
/// The pointers in `names` are only valid until a new property is added to the
/// atom with `chfl_atom_set_property`.
///
/// @example{capi/chfl_atom/list_properties.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_list_properties(
    const CHFL_ATOM* atom, const char* names[], uint64_t count
);

/// Add a new `property` with the given `name` to this `atom`.
///
/// If a property with the same name already exists, this function override the
/// existing property with the new one.
///
/// @example{capi/chfl_atom/property.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_set_property(
    CHFL_ATOM* atom, const char* name, const CHFL_PROPERTY* property
);

/// Get a property with the given `name` in this `atom`.
///
/// This function returns `NULL` if no property exists with the given name.
///
/// The user of this function is responsible to deallocate memory using the
/// `chfl_free` function.
///
/// @example{capi/chfl_atom/property.c}
/// @return A pointer to the property, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_PROPERTY* chfl_atom_get_property(
    const CHFL_ATOM* atom, const char* name
);

#ifdef __cplusplus
}
#endif

#endif // CHEMFILES_CAPI_ATOM_H
