// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_ATOM_H
#define CHEMFILES_CHFL_ATOM_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// Create an atom with the given `name`, and set the atom type to `name`.
///
/// The caller of this function should free the associated memory using
/// `chfl_atom_free`.
///
/// @example{tests/capi/doc/chfl_atom/chfl_atom.c}
/// @return A pointer to the atom, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_ATOM* chfl_atom(const char* name);

/// Get a copy of an `atom`.
///
/// The caller of this function should free the associated memory using
/// `chfl_atom_free`.
///
/// @example{tests/capi/doc/chfl_atom/copy.c}
/// @return A pointer to the new atom, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_ATOM* chfl_atom_copy(const CHFL_ATOM* atom);

/// Get a copy of the atom at the given `index` from a `frame`
///
/// The caller of this function should free the associated memory using
/// `chfl_atom_free`.
///
/// @example{tests/capi/doc/chfl_atom/from_frame.c}
///
/// @return A pointer to the atom, or NULL in case of error or if `index` is
///         out of bounds. You can use `chfl_last_error` to learn about the
///         error.
CHFL_EXPORT CHFL_ATOM* chfl_atom_from_frame(const CHFL_FRAME* frame, uint64_t index);

/// Get a copy of the atom at the given `index` from a `topology`
///
/// The caller of this function should free the associated memory using
/// `chfl_atom_free`.
///
/// @example{tests/capi/doc/chfl_atom/from_topology.c}
/// @return A pointer to the atom, or NULL in case of error or if `index` is
///         out of bounds. You can use `chfl_last_error` to learn about the
///         error.
CHFL_EXPORT CHFL_ATOM* chfl_atom_from_topology(
    const CHFL_TOPOLOGY* topology, uint64_t index
);

/// Get the mass of an `atom`, in the double pointed to by `mass`.
///
/// The mass is given in atomic mass units.
///
/// @example{tests/capi/doc/chfl_atom/mass.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_mass(const CHFL_ATOM* atom, double* mass);

/// Set the mass of an `atom` to `mass`.
///
/// The mass must be in atomic mass units.
///
/// @example{tests/capi/doc/chfl_atom/set_mass.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_set_mass(CHFL_ATOM* atom, double mass);

/// Get the charge of an `atom`, in the double pointed to by `charge`.
///
/// The charge is in number of the electron charge *e*.
///
/// @example{tests/capi/doc/chfl_atom/charge.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_charge(const CHFL_ATOM* atom, double* charge);

/// Set the charge of an `atom` to `charge`.
///
/// The charge must be in number of the electron charge *e*.
///
/// @example{tests/capi/doc/chfl_atom/set_charge.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_set_charge(CHFL_ATOM* atom, double charge);

/// Get the type of an `atom` in the string buffer `type`.
///
/// The buffer size must be passed in `buffsize`. This function will truncate
/// the atomic type to fit in the buffer.
///
/// @example{tests/capi/doc/chfl_atom/type.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_type(
    const CHFL_ATOM* atom, char* type, uint64_t buffsize
);

/// Set the type of an `atom` to `type`.
///
/// `type` must be a null terminated string.
///
/// @example{tests/capi/doc/chfl_atom/set_type.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_set_type(CHFL_ATOM* atom, const char* type);

/// Get the name of an `atom` in the string buffer `name`.
///
/// The buffer size must be passed in `buffsize`. This function will truncate
/// the name to fit in the buffer.
///
/// @example{tests/capi/doc/chfl_atom/name.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_name(
    const CHFL_ATOM* atom, char* name, uint64_t buffsize
);

/// Set the name of an `atom` to `name`.
///
/// `name` must be a null terminated string.
///
/// @example{tests/capi/doc/chfl_atom/set_name.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_set_name(CHFL_ATOM* atom, const char* name);

/// Get the full name of an `atom` from its type in the string buffer `name`
///
/// The buffer size must be passed in `buffsize`. This function will truncate
/// the name to fit in the buffer.
///
/// @example{tests/capi/doc/chfl_atom/full_name.c}
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
/// @example{tests/capi/doc/chfl_atom/vdw_radius.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_vdw_radius(const CHFL_ATOM* atom, double* radius);

/// Get the covalent radius of an `atom` from the atom type, in the double
/// pointed to by `radius`.
///
/// If the radius in unknown, this function set `radius` to 0.
///
/// @example{tests/capi/doc/chfl_atom/covalent_radius.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_covalent_radius(const CHFL_ATOM* atom, double* radius);

/// Get the atomic number of an `atom` from the atom type, in the integer
/// pointed to by `number`.
///
/// If the atomic number in unknown, this function set `number` to 0.
///
/// @example{tests/capi/doc/chfl_atom/atomic_number.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_atomic_number(const CHFL_ATOM* atom, uint64_t* number);

/// Add a new `property` with the given `name` to this `atom`.
///
/// If a property with the same name already exists, this function override the
/// existing property with the new one.
///
/// @example{tests/capi/doc/chfl_atom/property.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_atom_set_property(
    CHFL_ATOM* atom, const char* name, const CHFL_PROPERTY* property
);

/// Get a property with the given `name` in this `atom`.
///
/// This function returns `NULL` is no property exist with the given name.
///
/// The user of this function is responsible to deallocate memory using the
/// `chfl_property_free` function.
///
/// @example{tests/capi/doc/chfl_atom/property.c}
/// @return A pointer to the property, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_PROPERTY* chfl_atom_get_property(
    const CHFL_ATOM* atom, const char* name
);

/// Free the memory associated with an `atom`.
///
/// @example{tests/capi/doc/chfl_atom/chfl_atom.c}
/// @return `CHFL_SUCCESS`
CHFL_EXPORT chfl_status chfl_atom_free(CHFL_ATOM* atom);

#ifdef __cplusplus
}
#endif

#endif // CHEMFILES_CAPI_ATOM_H
