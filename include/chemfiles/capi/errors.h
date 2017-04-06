// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_LOGGING_H
#define CHEMFILES_CHFL_LOGGING_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// Get the last error message.
///
/// @example{tests/capi/doc/chfl_last_error.c}
/// @return A null-terminated string containing the last error message
CHFL_EXPORT const char* chfl_last_error(void);

/// Clear the last error message.
///
/// @example{tests/capi/doc/chfl_clear_errors.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_clear_errors(void);

/// Callback type that can be used to process warning events.
typedef void (*chfl_warning_callback)(const char* message);

/// Set the global warning `callback` to be used for each warning event.
///
/// @example{tests/capi/doc/chfl_set_warning_callback.c}
/// @return `CHFL_SUCCESS`
CHFL_EXPORT chfl_status chfl_set_warning_callback(chfl_warning_callback callback);

#ifdef __cplusplus
}
#endif

#endif
