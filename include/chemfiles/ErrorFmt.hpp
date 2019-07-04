// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_ERROR_FMT_HPP
#define CHEMFILES_ERROR_FMT_HPP

#include <fmt/format.h>
#include "chemfiles/Error.hpp"  // IWYU pragma: export

/// This file contains [fmt] based error constructors. These constructors are
/// not defined in the `Error.hpp` file because this would expose fmt headers
/// to all chemfiles users.
///
/// [fmt]: https://github.com/fmtlib/fmt

namespace chemfiles {

/// Create an `Error` using the given `format` and `arguments`.
template <typename... Args>
inline Error error(const char *format, const Args & ... arguments) {
    return Error(fmt::format(format, arguments...));
}

/// Create a `FileError` using the given `format` and `arguments`.
template <typename... Args>
inline FileError file_error(const char *format, const Args & ... arguments) {
    return FileError(fmt::format(format, arguments...));
}

/// Create a `MemoryError` using the given `format` and `arguments`.
template <typename... Args>
inline MemoryError memory_error(const char *format, const Args & ... arguments) {
    return MemoryError(fmt::format(format, arguments...));
}

/// Create a `FormatError` using the given `format` and `arguments`.
template <typename... Args>
inline FormatError format_error(const char *format, const Args & ... arguments) {
    return FormatError(fmt::format(format, arguments...));
}

/// Create a `SelectionError` using the given `format` and `arguments`.
template <typename... Args>
inline SelectionError selection_error(const char *format, const Args & ... arguments) {
    return SelectionError(fmt::format(format, arguments...));
}

/// Create a `ConfigurationError` using the given `format` and `arguments`.
template <typename... Args>
inline ConfigurationError configuration_error(const char *format, const Args & ... arguments) {
    return ConfigurationError(fmt::format(format, arguments...));
}

/// Create an `OutOfBounds` error using the given `format` and `arguments`.
template <typename... Args>
inline OutOfBounds out_of_bounds(const char *format, const Args & ... arguments) {
    return OutOfBounds(fmt::format(format, arguments...));
}

/// Create a `PropertyError` using the given `format` and `arguments`.
template <typename... Args>
inline PropertyError property_error(const char *format, const Args & ... arguments) {
    return PropertyError(fmt::format(format, arguments...));
}

} // namespace chemfiles

#endif
