// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_GENERIC_H
#define CHEMFILES_GENERIC_H

#include <string>
#include <functional>
#include "chemfiles/exports.hpp"

// A few exported function that can not be in the same header as the main
// functionality; because the rest of the functionality is not exported.
// The implementation is to be found with the associated functionality.
namespace chemfiles {

/// Callback type used to process a warning event
using warning_callback = std::function<void(const std::string& message)>;

/// Set the global callback for warning events. The default is to print them
/// on the standard error stream.
///
/// @example{tests/doc/set_warning_callback.cpp}
///
/// @param callback callback function that will be called on each warning
void CHFL_EXPORT set_warning_callback(warning_callback callback);

/// Read configuration from the file at `path`. If the same configuration data
/// is already present in a previouly read configuration file, the data is
/// replaced by the one in this file.
///
/// If the file at `path` can not be opened, or if the configuration file is
/// invalid, a `ConfigurationError` is thrown.
///
/// @example{tests/doc/add_configuration.cpp}
///
/// @param path path to the configuration file to add
///
/// @throws ConfigurationError if the file at `path` can not be read, or if it
///                            is invalid.
void CHFL_EXPORT add_configuration(const std::string& path);

} // namespace chemfiles

#endif
