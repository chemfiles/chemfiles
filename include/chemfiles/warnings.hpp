/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_WARNINGS_H
#define CHEMFILES_WARNINGS_H

#include <string>
#include "fmt/format.h"
#include "chemfiles/exports.hpp"

namespace chemfiles {

/// Send a warning with the given message
void CHFL_EXPORT warning(std::string message);

/// Create a message using the given `format` and `arguments`, and send a
/// warning with this message.
///
/// `format` and `arguments` will be used to construct a string using [fmtlib].
///
/// [fmtlib]: https://github.com/fmtlib/fmt
template<typename... Args>
void CHFL_EXPORT warning(const char* format, Args const&... arguments) {
    warning(fmt::format(format, arguments...));
}

} // namespace chemfiles

#endif
