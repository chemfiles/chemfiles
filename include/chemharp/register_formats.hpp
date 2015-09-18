/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_REGISTER_FORMAT_HPP
#define HARP_REGISTER_FORMAT_HPP

#include <unordered_map>
#include <memory>

#include "chemharp/Format.hpp"
#include "chemharp/TrajectoryFactory.hpp"

namespace harp {

//! Function to create a file
template <class file_t>
unique_ptr<File> new_file(const string& p, const string& m){
    return unique_ptr<File>(new file_t(p, m));
}

//! Function to create a format
template <class format_t>
unique_ptr<Format> new_format(File& f){
    return unique_ptr<Format>(new format_t(f));
}

//! Register a format by associating it to a format name, and no file type.
#define REGISTER(format_t, name)                          \
bool format_t::_registered_format_ =                      \
TrajectoryFactory::register_format(name, {                \
    new_format<format_t>,                                 \
    new_file<typename format_t::file_t>                   \
});

//! Register a format by associating it to an extention, and no file type.
//! The extension should starts with a "."
#define REGISTER_EXTENSION(format_t, extension)           \
bool format_t::_registered_extension_ =                   \
TrajectoryFactory::register_extension(extension, {        \
    new_format<format_t>,                                 \
    new_file<typename format_t::file_t>                   \
});

//! Add the static members in a class to register a format.
#define REGISTER_FORMAT                   \
    static bool _registered_extension_;   \
    static bool _registered_format_

#define FORMAT_NAME(x) static constexpr const char name[] = x;
#define FORMAT_EXTENSION(x) static constexpr const char extension[] = x;

} // namespace harp

#endif
