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

/// Read configuration data from the file at `path`.
///
/// By default, chemfiles reads configuration from any file named
/// `.chemfiles.toml` or `chemfiles.toml` in the current directory or any parent
/// directory. This function can be used to add data from another configuration
/// file.
///
/// This function will throw a `ConfigurationError` if there is no file at
/// `path`, or if the file is incorrectly formatted. Data from the new
/// configuration file will overwrite any existing data.
///
/// @example{add_configuration.cpp}
///
/// @param path path to the configuration file to add
///
/// @throws ConfigurationError if the file at `path` can not be read, or if it
///                            is invalid.
void CHFL_EXPORT add_configuration(const std::string& path);

/// Get the list of formats chemfiles knows about, and all associated metadata
///
/// @example{formats_list.cpp}
std::vector<std::reference_wrapper<const FormatMetadata>> CHFL_EXPORT formats_list();

/// Get the format that chemfiles would use to read a file at the given path.
///
/// Most of the time, the format is only guessed from the filename extension,
/// without reading the file to guess the format. When two or more format can
/// share the same extension (for example CIF and mmCIF), chemfiles tries to
/// read the file to distinguish between them. If reading fails, the default
/// format for this extension is returned.
///
/// Opening the file using the returned format string might still fail. For
/// example, it will fail if the file is not actually formatted according to the
/// guessed format; or the format/compression combination is not supported (e.g.
/// `XTC / GZ` will not work since the XTC reader does not support compressed
/// files).
///
/// The format is represented in a way compatible with the various `Trajectory`
/// constructors, i.e. `"<format name> [/ <compression>]"`, where compression is
/// optional.
///
/// @param path path of the file we are trying to read
/// @param mode the opening mode of the file, can be 'r', 'a' or 'w'
/// @return guessed format of the file
/// @throw FormatError if no format matching this filename is found.
///
/// @example{guess_format.cpp}
std::string CHFL_EXPORT guess_format(std::string path, char mode = 'r');

} // namespace chemfiles

#endif
