// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_MISC_HPP
#define CHEMFILES_MISC_HPP

#include <string>
#include <functional>
#include "chemfiles/exports.h"

// A few exported function that can not be in the same header as the main
// functionality; because the rest of the functionality is not exported.
// The implementation is to be found with the associated functionality.
namespace chemfiles {

/// Callback type used to process a warning event
typedef std::function<void(const std::string& message)> warning_callback_t; // NOLINT: doxygen fails to generate the right XLM from this

/// Set the global callback for warning events. The default is to print them
/// on the standard error stream.
///
/// @example{set_warning_callback.cpp}
///
/// @param callback callback function that will be called on each warning
void CHFL_EXPORT set_warning_callback(warning_callback_t callback);

/// Read configuration data from the file at `path`.
///
/// By default, chemfiles reads configuration from any file named
/// `.chemfiles.toml` or `chemfiles.toml` in the current directory or any parent
/// directory. This function can be used to add data from another configuration
/// file.
///
/// This function will throw a `ConfigurationError` if there is no file at
/// `path`, or if the file is incorectly formatted. Data from the new
/// configuration file will overwrite any existing data.
///
/// @example{add_configuration.cpp}
///
/// @param path path to the configuration file to add
///
/// @throws ConfigurationError if the file at `path` can not be read, or if it
///                            is invalid.
void CHFL_EXPORT add_configuration(const std::string& path);

} // namespace chemfiles

#endif
