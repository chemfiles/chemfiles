/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
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

    //! Static reader_map instance associating format descriptions and readers
    static format_map_t& formats(){
        static format_map_t umap = format_map_t();
        return umap;
    }
    //! Static reader_map instance associating format descriptions and readers
    static format_map_t& extensions(){
        static format_map_t umap = format_map_t();
        return umap;
    }
public:
    /*!
     * @brief Get a format reader from its type.
     * @param name the format name
     * @return An unique_ptr<Format> to the format, if it is found in the list
     *         of registered formats
     *
     * Throws an error if the format can not be found
     */
    static unique_ptr<Format> format(const string& name){
        if (formats().find(name) == formats().end())
            throw HarpFormatError("Can not find the format \"" + name + "\".");
        return (*formats()[name])();
    }

    /*!
     * @brief Get a format reader from an \c extention.
     * @param ext the format extention
     * @return An unique_ptr<Format> to the format, if it is found in the list
     *         of registered formats
     *
     * Throws an error if the format can not be found
     */
    static unique_ptr<Format> by_extension(const string& ext){
        if (extensions().find(ext) == extensions().end())
            throw HarpFormatError("Can not find a format associated with the \""
                                   + ext + "\" extension.");
        return (*extensions()[ext])();
    }


    //! Register a format in the internal list.
    static bool register_format(const string& name, format_creator_t fc){
        if (formats().find(name) != formats().end())
            throw HarpFormatError("The name \"" + name + "\" is already "
                                  "associated with a format.");
        formats().emplace(name, fc);
        return true;
    }
    //! Register an extension in the internal list.
    static bool register_extension(const string& ext, format_creator_t fc){
        if (extensions().find(ext) != extensions().end())
            throw HarpFormatError("The extension \"" + ext + "\" is already "
                                  "associated with a format.");
        extensions().emplace(ext, fc);
        return true;
    }
};


//! Register a Format by associating it to a description
#define REGISTER(type, name)                              \
bool type::_registered_format_ =                          \
FormatFactory::register_format(name, [](){return unique_ptr<Format>(new type());});

//! Register a Format by associating it to an extention. The extension should
//! starts with a "."
#define REGISTER_EXTENSION(type, extension)                    \
bool type::_registered_extension_ =                            \
FormatFactory::register_extension(extension, [](){return unique_ptr<Format>(new type());});

//! Add a static member in the class to register a format.
#define REGISTER_FORMAT \
    static bool _registered_extension_; \
    static bool _registered_format_;
} // namespace harp

#endif
