// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_WARNINGS_H
#define CHEMFILES_WARNINGS_H

#include <string>
#include <fmt/format.h>
#include "chemfiles/exports.hpp"

namespace chemfiles {

/// Send a warning with the given message
void warning(const std::string& message);

/// Create a message using the given `format` and `arguments`, and send a
/// warning with this message.
///
/// `format` and `arguments` will be used to construct a string using the [fmt]
/// library.
///
/// [fmt]: https://github.com/fmtlib/fmt
template<typename... Args>
void warning(const char* format, Args const&... arguments) {
    warning(fmt::format(format, arguments...));
}

} // namespace chemfiles

#endif
