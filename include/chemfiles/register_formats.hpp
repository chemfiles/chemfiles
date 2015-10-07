/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_REGISTER_FORMAT_HPP
#define CHEMFILES_REGISTER_FORMAT_HPP

#include <unordered_map>
#include <memory>

#include "chemfiles/Format.hpp"
#include "chemfiles/TrajectoryFactory.hpp"

namespace chemfiles {

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

#define FORMAT_EXTENSION(x) static const char* extension() {static constexpr char val[] = #x; return val;}
#define FORMAT_NAME(x) static const char* name() {static constexpr char val[] = #x; return val;}

// Create a vector of types at compile time
struct Void {};

template <typename ...> struct concat;

template <template <typename ...> class List, typename T>
struct concat<List<Void>, T>{
    typedef List<T> type;
};

template <template <typename ...> class List, typename ...Types, typename T>
struct concat<List<Types...>, T> {
    typedef List<Types..., T> type;
};

template <typename...> struct FormatList {};

template <> struct FormatList<Void> {};
typedef FormatList<Void> FormatListVoid;
#define FORMATS_LIST FormatListVoid

} // namespace chemfiles

#endif
