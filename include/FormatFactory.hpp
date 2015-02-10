/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_FORMAT_FACTORY_HPP
#define HARP_FORMAT_FACTORY_HPP

#include <map>
#include <memory>

#include "Format.hpp"

namespace harp {

using std::unique_ptr;
using std::string;

/*!
* @class FormatFactory FormatFactory.hpp
* @brief Factory for FormatReader and FormatWriter classes
*
* This class allow to register at compile time various FormatReader and FormatWriter
* classes, giving at runtime the good format (by the associated extension) when
* asked politely.
*/
class FormatFactory{
private:
    //! Files extensions to format reader associations
    typedef std::map<string, Format*> format_map_t;

    //! Static reader_map instance
    static format_map_t& format_map(){
        static format_map_t map = format_map_t();
        return map;
    }
public:
    /*!
     * @brief Get a format reader from an \c extention.
     *
     * Throw an error if the format can not be found
     */
    static Format* format(const string& ext){
        // TODO check if the key is present and throw error
        return format_map()[ext];
    }
    //! Register a format in the internal list.
    static bool register_format(const string& ext, Format* fc){
        // TODO check if the key is already present and throw error
        format_map().insert(std::make_pair(ext, fc));
        return true;
    }
};

//! Register a FormatReader by associating it to a specific extension.
#define REGISTER_FORMAT(type, extension)              \
    /* Instanciate the template */                          \
    static type type ## instance = type();     \
    /* register the format */                               \
    static bool type ## _registered = \
                FormatFactory::register_format(extension, &type ## instance);

} // namespace harp

#endif
