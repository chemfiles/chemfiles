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
typedef std::function<void(std::string message)> warning_callback;

/// Set the global callback for warning events. The default is to print them
/// on the standard error stream.
void CHFL_EXPORT set_warning_callback(warning_callback callback);

/// Read configuration from the file at `path`. If the same configuration data
/// is already present in a previouly read configuration file, the data is
/// replaced by the one in this file.
///
/// If the file at `path` can not be opened, a `ConfigurationError` is thrown.
/// This function is not protected against data-race: calling it while using
/// the configuration from other thread is undefined.
void CHFL_EXPORT add_configuration(const std::string& path);

} // namespace chemfiles

#endif
