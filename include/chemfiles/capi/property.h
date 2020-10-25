// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_PROPERTY_H
#define CHEMFILES_CHFL_PROPERTY_H

#include <stdint.h>
#include <stdbool.h>  // IWYU pragma: keep

#include "chemfiles/capi/types.h"
#include "chemfiles/exports.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Possible values held by a CHFL_PROPERTY
typedef enum {  // NOLINT: this is both a C and C++ file
    /// Bool value
    CHFL_PROPERTY_BOOL = 0,
    /// Double value
    CHFL_PROPERTY_DOUBLE = 1,
    /// String value
    CHFL_PROPERTY_STRING = 2,
    /// chfl_vector3d value
    CHFL_PROPERTY_VECTOR3D = 3,
} chfl_property_kind;

/// Create a new property holding a boolean `value`.
///
/// The caller of this function should free the allocated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_property/bool.c}
/// @return A pointer to the property, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_PROPERTY* chfl_property_bool(bool value);

/// Create a new property holding a double `value`.
///
/// The caller of this function should free the allocated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_property/double.c}
/// @return A pointer to the property, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_PROPERTY* chfl_property_double(double value);

/// Create a new property holding a string `value`.
///
/// The caller of this function should free the allocated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_property/string.c}
/// @return A pointer to the property, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_PROPERTY* chfl_property_string(const char* value);

/// Create a new property holding a 3D vector `value`.
///
/// The caller of this function should free the allocated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_property/vector3d.c}
/// @return A pointer to the property, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_PROPERTY* chfl_property_vector3d(const chfl_vector3d value);

/// Get the type of value held by this `property` in `kind`.
///
/// @example{capi/chfl_property/kind.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_property_get_kind(
    const CHFL_PROPERTY* property, chfl_property_kind* kind
);

/// Get the boolean value held by this `property` in the location pointed to
/// by `value`.
///
/// This function returns CHFL_PROPERTY_ERROR if the property is not a boolean
/// property.
///
/// @example{capi/chfl_property/bool.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_property_get_bool(
    const CHFL_PROPERTY* property, bool* value
);

/// Get the double value holded by this `property` in the location pointed to
/// by `value`.
///
/// This function returns CHFL_PROPERTY_ERROR if the property is not a double
/// property.
///
/// @example{capi/chfl_property/double.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_property_get_double(
    const CHFL_PROPERTY* property, double* value
);

/// Get the string value held by this `property` in the given `buffer`.
///
/// This function returns CHFL_PROPERTY_ERROR if the property is not a string
/// property.
///
/// The buffer size must be passed in `buffsize`. This function will truncate
/// the property to fit in the buffer.
///
/// @example{capi/chfl_property/string.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_property_get_string(
    const CHFL_PROPERTY* property, char* buffer, uint64_t buffsize
);

/// Get the 3D vector value held by this `property` in the location pointed to
/// by `value`.
///
/// This function returns CHFL_PROPERTY_ERROR if the property is not a 3D vector
/// property.
///
/// @example{capi/chfl_property/vector3d.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_property_get_vector3d(
    const CHFL_PROPERTY* property, chfl_vector3d value
);

#ifdef __cplusplus
}
#endif

#endif
