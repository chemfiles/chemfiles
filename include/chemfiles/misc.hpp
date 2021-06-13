// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_MISC_HPP
#define CHEMFILES_MISC_HPP

#include <string>
#include <vector>
#include <functional>

#include "chemfiles/exports.h"

// A few exported function that can not be in the same header as the main
// functionality; because the rest of the functionality is not exported.
// The implementation is to be found with the associated functionality.
namespace chemfiles {
class FormatMetadata;

/// Callback type used to process a warning event
typedef std::function<void(const std::string& message)> warning_callback_t; // NOLINT: doxygen fails to generate the right XLM from this

/// Set the global callback for warning events. The default is to print them
/// on the standard error stream.
///
/// @example{set_warning_callback.cpp}
///
/// @param callback callback function that will be called on each warning
void CHFL_EXPORT set_warning_callback(warning_callback_t callback);

/// Get the list of formats chemfiles knows about, and all associated metadata
///
/// @example{formats_list.cpp}
std::vector<std::reference_wrapper<const FormatMetadata>> CHFL_EXPORT formats_list();

} // namespace chemfiles

#endif
