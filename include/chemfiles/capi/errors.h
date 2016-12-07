// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CHFL_LOGGING_H
#define CHEMFILES_CHFL_LOGGING_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// @brief Get the error message corresponding to an error code.
/// @param code The status code
/// @return A null-terminated string encoding the textual representation of the
///         status.
CHFL_EXPORT const char* chfl_strerror(chfl_status code);

/// @brief Get the last error message.
/// @return A null-terminated string encoding the textual representation of the
///         last error.
CHFL_EXPORT const char* chfl_last_error(void);

/// @brief Clear the last error message.
/// @return The status code
CHFL_EXPORT chfl_status chfl_clear_errors(void);

/// Callback type that can be used to process warning events.
typedef void (*chfl_warning_callback)(const char* message);


/// @brief Redirect all logging to user-provided logging. The `callback` function
/// will be called at each loggin operation with the level of the message, and
/// the message itself.
/// @return The status code
CHFL_EXPORT chfl_status chfl_set_warning_callback(chfl_warning_callback callback);

#ifdef __cplusplus
}
#endif

#endif
