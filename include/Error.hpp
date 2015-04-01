/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_ERROR_HPP
#define HARP_ERROR_HPP

#include <stdexcept>
#include <string>

namespace harp {

/*!
 * @class Error Error.hpp
 * @brief Base exception for Chemharp library
 */
class Error : public std::runtime_error {
public:
    Error(const std::string &message) : std::runtime_error(message) {}
};

/*!
 * @class FileError Error.hpp
 * @brief Exception for files related failures
 */
class FileError : public Error {
public:
    FileError(const std::string &message) : Error(message) {}
};

/*!
 * @class MemoryError Error.hpp
 * @brief Exception for memory related failures
 */
class MemoryError : public Error {
public:
    MemoryError(const std::string &message) : Error(message) {}
};

/*!
 * @class FormatError Error.hpp
 * @brief Exception for formats related failures
 */
class FormatError : public Error {
public:
    FormatError(const std::string &message) : Error(message) {}
};

/*!
 * @class PluginError Error.hpp
 * @brief Exception for dynamic library loading errors
 */
class PluginError : public Error {
public:
    PluginError(const std::string &message) : Error(message) {}
};


} // namespace harp

#endif
