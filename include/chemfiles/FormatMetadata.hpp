// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_METADATA_HPP
#define CHEMFILES_FORMAT_METADATA_HPP

#include <string>

#include "chemfiles/exports.h"
#include "chemfiles/external/optional.hpp"
#include "chemfiles/Error.hpp"

namespace chemfiles {

/// Metadata associated with a format.
class CHFL_EXPORT FormatMetadata {
public:
    FormatMetadata() = default;
    ~FormatMetadata() = default;

    // Disable all copy & assignment operator to ensure no copy can be made.
    //
    // In turn, this allow to require only one instance of FormatMetadata to
    // exist for each format, and pass it around as a reference. The end goal
    // being to ensure these instances are alive when passing them through the C
    // API, so that we don't require a separate allocation for each field (name,
    // extension, etc.)
    //
    // This is also enabled by forcing format_metadata to return a const
    // reference, meaning it can only return `static` data, that will be alive
    // forever.
    FormatMetadata(const FormatMetadata&) = delete;
    FormatMetadata(FormatMetadata&&) = delete;
    FormatMetadata& operator=(const FormatMetadata&) = delete;
    FormatMetadata& operator=(FormatMetadata&&) = delete;

    /// Name of the format
    const char* name = "";
    /// Potential extension associated with the format
    optional<const char*> extension = nullopt;
    /// User facing format description
    const char* description = "";
    /// URL pointing to the format definition/reference
    const char* reference = "";

    /// Is reading files in this format implemented?
    bool read = false;
    /// Is writing files in this format implemented?
    bool write = false;
    /// Does this format support in-memory IO?
    bool memory = false;

    /// Does this format support storing atomic positions?
    bool positions = false;
    /// Does this format support storing atomic velocities?
    bool velocities = false;
    /// Does this format support storing unit cell information?
    bool unit_cell = false;
    /// Does this format support storing atom names or types?
    bool atoms = false;
    /// Does this format support storing bonds between atoms?
    bool bonds = false;
    /// Does this format support storing residues?
    bool residues = false;

    // Validate the format metadata
    void validate() const;
};

/// Get the metadata associated with `Format`.
///
/// The metadata should be a reference to static memory.
///
/// In order to implement a new format, one should specialise this function
/// with the corresponding format:
///
/// ```cpp
/// class MyFormat: public Format {
///     // ...
/// };
///
/// namespace chemfiles {
///     template<> const FormatMetadata& format_metadata<MyFormat>() {
///         static FormatMetadata metadata;
///         metadata.name = "MyFormat";
///         metadata.extension = ".mtf";
///         return metadata;
///     }
/// }
/// ```
template<class Format>
const FormatMetadata& format_metadata() {
    throw FormatError("format_metadata is not implemented for this format");
}

} // namespace chemfiles

#endif
