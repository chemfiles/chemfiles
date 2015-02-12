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

#include <unordered_map>
#include <memory>

#include "Format.hpp"
#include "Error.hpp"

namespace harp {

using std::shared_ptr;
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
    //! Function type to create a format
    typedef unique_ptr<Format> (*format_creator_t) ();
    //! Files extensions to format reader associations
    typedef std::unordered_map<string, format_creator_t> format_map_t;

    //! Static reader_map instance
    static format_map_t& map(){
        static format_map_t umap = format_map_t();
        return umap;
    }
public:
    /*!
     * @brief Get a format reader from an \c extention.
     *
     * Throw an error if the format can not be found
     */
    static unique_ptr<Format> format(const string& ext){
        auto m = map();
        if (map().find(ext) == map().end())
            throw HarpFormatError("Can not find a format "
                "associated with the \"" + ext + "\" extension.");
        return (*map()[ext])();
    }
    //! Register a format in the internal list.
    static bool register_format(const string& ext, format_creator_t fc){
        if (map().find(ext) != map().end())
            throw HarpFormatError("The extension " + ext +
                " is already associated with a format.");
        map().emplace(ext, fc);
        return true;
    }
};


//! Register a Format by associating it to a specific extension.
#define REGISTER(type, extension)                              \
bool type::_registered_extension_ =                            \
FormatFactory::register_format(extension, [](){return unique_ptr<Format>(new type());});

//! Add a static member in the class to register a format.
#define REGISTER_FORMAT static bool _registered_extension_
} // namespace harp

#endif
