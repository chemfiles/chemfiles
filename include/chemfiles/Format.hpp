// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_HPP
#define CHEMFILES_FORMAT_HPP

#include <memory>
#include <string>
#include <functional>

#include "chemfiles/exports.hpp"
#include "chemfiles/Error.hpp"

namespace chemfiles {
class Frame;

/// The `Format` class defines the interface to implement in order to add a new
/// format to chemfiles. It is possible to implement only one of `Format::read`;
/// `Format::read_step` or `Format::write`. In that case, only the corresponding
/// operations will be available from the corresponding `chemfiles::Trajectory`.
class CHFL_EXPORT Format {
public:
    Format() = default;
    virtual ~Format() noexcept = default;
    Format(const Format&) = delete;
    Format& operator=(const Format&) = delete;
    Format(Format&&) = default;
    Format& operator=(Format&&) = default;

    /// @brief Read a specific step from the trajectory file.
    ///
    /// @throw FormatError if the file does not follow the format
    /// @throw FileError if their is an OS error while reading the file
    ///
    /// @param step The step to read
    /// @param frame The frame to fill
    virtual void read_step(size_t step, Frame& frame);

    /// @brief Read a specific step from the trajectory file.
    ///
    /// @throw FormatError if the file does not follow the format
    /// @throw FileError if their is an OS error while reading the file
    ///
    /// @param frame The frame to fill
    virtual void read(Frame& frame);

    /// @brief Write a frame to the trajectory file.
    ///
    /// @throw FormatError if the file does not follow the format
    /// @throw FileError if their is an OS error while reading the file
    ///
    /// @param frame The frame to be writen
    virtual void write(const Frame& frame);

    /// @brief Get the number of frames in the associated file
    /// @return The number of frames
    virtual size_t nsteps() = 0;
};

/// Metadata associated with a format.
///
/// This class uses the builder patern, chaining functions calls to set the
/// class attributes:
///
/// ```cpp
/// auto meta = FormatInfo("MyFormat").with_extension(".mft").description(
///     "some description"
/// );
/// ```
class CHFL_EXPORT FormatInfo {
public:
    /// Create a `FormatInfo` with the given `name`.
    ///
    /// @throws Error if the format name is the empty string
    FormatInfo(std::string name): name_(std::move(name)) {
        if (name_ == "") {
            throw Error("a format name can not be an empty string");
        }
    }

    /// Get the format name
    const std::string& name() const {
        return name_;
    }

    /// Set the format extension, use this to allow users to use this format
    /// automatically with files having this extension. The extension should
    /// start by a dot (`".xxx"`).
    FormatInfo& with_extension(std::string extension) {
        if (extension.length() < 1 || extension[0] != '.') {
            throw Error("a format extension must start with a dot");
        }
        extension_ = std::move(extension);
        return *this;
    }

    /// Get the format extension.
    ///
    /// If the extension was not set, this returns an empty string
    const std::string& extension() const {
        return extension_;
    }

    /// Add a format description to this format
    FormatInfo& description(std::string description) {
        description_ = std::move(description);
        return *this;
    }

    /// Get the format description.
    const std::string& description() const {
        return description_;
    }

private:
    std::string name_;
    std::string extension_;
    std::string description_;
};

/// Get the metadata associated with `Format`.
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
///     template<> FormatInfo format_information<MyFormat>() {
///         return FormatInfo("MyFormat").with_extension(".mft");
///     }
/// }
/// ```
template<class Format>
FormatInfo format_information() {
    throw Error("format_informations is unimplemented for this format");
}

} // namespace chemfiles

#endif
