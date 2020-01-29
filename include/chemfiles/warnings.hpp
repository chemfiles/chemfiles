// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_WARNINGS_H
#define CHEMFILES_WARNINGS_H

#include <string>
#include <iterator>
#include <fmt/format.h>

namespace chemfiles {

/// Send a warning with the given message
void send_warning(const std::string& message) noexcept;

/// Create a message for the given `context` formatting the `message` with the
/// `arguments`, and send a warning with this message.
///
/// `message` and `arguments` will be used to construct a string using the [fmt]
/// library.
///
/// [fmt]: https://github.com/fmtlib/fmt
template<typename... Args>
void warning(std::string context, const char* message, Args &&... arguments) {
    if (context.empty()) {
        send_warning(fmt::format(message, std::forward<Args>(arguments)...));
    } else {
        context += ": ";
        fmt::format_to(std::back_inserter(context), message, std::forward<Args>(arguments)...);
        send_warning(context);
    }
}

} // namespace chemfiles

#endif
