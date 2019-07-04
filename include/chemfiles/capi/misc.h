// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_LOGGING_H
#define CHEMFILES_CHFL_LOGGING_H

#include "chemfiles/capi/types.h"
#include "chemfiles/exports.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Get the last error message.
///
/// The last error message is a thread local variable, so you need to call this
/// function in the thread from where the error happened.
///
/// @example{capi/chfl_last_error.c}
/// @return A null-terminated string containing the last error message
CHFL_EXPORT const char* chfl_last_error(void);

/// Clear the thread local last error message.
///
/// The last error message is a thread local variable, so this function will
/// only clear it in the thread where it is called.
///
/// @example{capi/chfl_clear_errors.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_clear_errors(void);

/// Callback type that can be used to process warning events.
typedef void (*chfl_warning_callback)(const char* message);  // NOLINT: this is both a C and C++ file

/// Set the global warning `callback` to be used for each warning event.
///
/// @example{capi/chfl_set_warning_callback.c}
/// @return `CHFL_SUCCESS`
CHFL_EXPORT chfl_status chfl_set_warning_callback(chfl_warning_callback callback);

/// Read configuration data from the file at `path`.
///
/// By default, chemfiles reads configuration from any file named
/// `.chemfiles.toml` or `chemfiles.toml` in the current directory or any parent
/// directory. This function can be used to add data from another configuration
/// file.
///
/// This function will fail if there is no file at `path`, or if the file is
/// incorectly formatted. Data from the new configuration file will overwrite
/// any existing data.
///
/// @example{capi/chfl_add_configuration.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_add_configuration(const char* path);

#ifdef __cplusplus
}
#endif

#endif
