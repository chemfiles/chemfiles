/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
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
 * @class HarpError Error.hpp
 * @brief Base exception for Harp library
 */
class HarpError : public std::runtime_error {
public:
    HarpError(const std::string &message) : std::runtime_error(message) {}
};

/*!
 * @class HarpFileError Error.hpp
 * @brief Exception for files related failures
 */
class HarpFileError : public HarpError {
public:
    HarpFileError(const std::string &message) : HarpError(message) {}
};

/*!
 * @class HarpMemoryError Error.hpp
 * @brief Exception for memory related failures
 */
class HarpMemoryError : public HarpError {
public:
    HarpMemoryError(const std::string &message) : HarpError(message) {}
};

/*!
 * @class HarpFileError Error.hpp
 * @brief Exception for formats related failures
 */
class HarpFormatError : public HarpError {
public:
    HarpFormatError(const std::string &message) : HarpError(message) {}
};


} // namespace harp

#endif
