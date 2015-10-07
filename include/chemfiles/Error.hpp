/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_ERROR_HPP
#define CHEMFILES_ERROR_HPP
#include <stdexcept>
#include <string>

#include "chemfiles/exports.hpp"

namespace chemfiles {

/*!
 * @class Error Error.hpp
 * @brief Base exception for chemfiles library
 */
struct CHFL_EXPORT Error : public std::runtime_error {
    using runtime_error::runtime_error;
};

/*!
 * @class FileError Error.hpp
 * @brief Exception for files related failures
 */
struct CHFL_EXPORT FileError : public Error {
    using Error::Error;
};

/*!
 * @class MemoryError Error.hpp
 * @brief Exception for memory related failures
 */
struct CHFL_EXPORT MemoryError : public Error {
    using Error::Error;
};

/*!
 * @class FormatError Error.hpp
 * @brief Exception for formats related failures
 */
struct CHFL_EXPORT FormatError : public Error {
    using Error::Error;
};

/*!
 * @class PluginError Error.hpp
 * @brief Exception for dynamic library loading errors
 */
struct CHFL_EXPORT PluginError : public Error {
    using Error::Error;
};


} // namespace chemfiles

#endif
