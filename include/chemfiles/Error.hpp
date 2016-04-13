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

//! @brief Base exception class for chemfiles library
struct CHFL_EXPORT Error : public std::runtime_error {
    Error(const std::string& err) : std::runtime_error(err) {}
};

//! @brief Exception for files related failures
struct CHFL_EXPORT FileError : public Error {
    FileError(const std::string& err) : Error(err) {}
};

//! @brief Exception for memory related failures
struct CHFL_EXPORT MemoryError : public Error {
    MemoryError(const std::string& err) : Error(err) {}
};

//! @brief Exception for formats related failures
struct CHFL_EXPORT FormatError : public Error {
    FormatError(const std::string& err) : Error(err) {}
};

//! @brief Exception for Molfile plugins errors
struct CHFL_EXPORT PluginError : public Error {
    PluginError(const std::string& err) : Error(err) {}
};

//! @brief Exception for bad API usage
struct CHFL_EXPORT APIError : public Error {
    APIError(const std::string& err) : Error(err) {}
};

//! @brief Exception for syntaxic errors in selections
struct CHFL_EXPORT LexerError: public Error {
    LexerError(const std::string& err): Error(err) {}
};

//! @brief Exception for semantic and parsing errors in selections
struct CHFL_EXPORT ParserError: public Error {
    ParserError(const std::string& err): Error(err) {}
};

} // namespace chemfiles

#endif
