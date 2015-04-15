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

//! Function type to create a format
typedef unique_ptr<Format> (*format_creator_t) ();
//! Function type to create a file
typedef unique_ptr<File> (*file_creator_t) (const string& path, const string& mode);

/*!
* @class trajectory_builder TrajectoryFactory.hpp
* @brief Structure associating format and file classes
*/
struct trajectory_builder_t {
    format_creator_t format_creator;
    file_creator_t file_creator;
};

/*!
* @class TrajectoryFactory TrajectoryFactory.hpp
* @brief Factory for FormatReader and FormatWriter classes
*
* This class allow to register at compile time various FormatReader and FormatWriter
* classes, giving at runtime the good format (by the associated extension) when
* asked politely.
*/
class TrajectoryFactory{
private:
    //! Files extensions to format reader associations
    typedef std::unordered_map<string, trajectory_builder_t> trajectory_map_t;

    //! Static reader_map instance associating format descriptions and readers
    static trajectory_map_t& names(){
        static auto umap = trajectory_map_t();
        return umap;
    }
    //! Static reader_map instance associating format descriptions and readers
    static trajectory_map_t& extensions(){
        static auto umap = trajectory_map_t();
        return umap;
    }
public:
    /*!
     * @brief Get a trajectory_builder from a format type name.
     * @param name the format name
     * @return A trajectory_builder corresponding to the format, if the format
     *         name is found in the list of registered formats.
     *
     * Throws an error if the format can not be found
     */
     static trajectory_builder_t format(const string& name){
        if (names().find(name) == names().end())
            throw FormatError("Can not find the format \"" + name + "\".");
        return names()[name];
    }

    /*!
     * @brief Get a trajectory_builder from a format \c extention.
     * @param ext the format extention
     * @return A trajectory_builder corresponding to the format, if the format
     *         extension is found in the list of registered extensions.
     *
     * Throws an error if the format can not be found
     */
     static trajectory_builder_t by_extension(const string& ext){
        if (extensions().find(ext) == extensions().end())
            throw FormatError("Can not find a format associated with the \""
                                   + ext + "\" extension.");
        return extensions()[ext];
    }


    //! Register a trajectory_builder in the internal format names list.
    static bool register_format(const string& name, trajectory_builder_t tb){
        if (names().find(name) != names().end())
            throw FormatError("The name \"" + name + "\" is already "
                                  "associated with a format.");
        names().emplace(name, tb);
        return true;
    }
    //! Register an trajectory_builder in the internal extensions list.
    static bool register_extension(const string& ext, trajectory_builder_t tb){
        if (extensions().find(ext) != extensions().end())
            throw FormatError("The extension \"" + ext + "\" is already "
                                  "associated with a format.");
        extensions().emplace(ext, tb);
        return true;
    }
};

//! Lambda function to create a file
#define FILE_CREATOR(file_t)                              \
    [](const string& p, const string& m){                 \
        return unique_ptr<File>(new file_t(p, m));        \
    }                                                     \

//! Lambda function to create a format
#define FORMAT_CREATOR(format_t)                         \
    [](){return unique_ptr<Format>(new format_t());}     \

//! Register a format by associating it to a format name, and no file type.
#define REGISTER(format_t, name)                          \
bool format_t::_registered_format_ =                      \
TrajectoryFactory::register_format(name, {                \
    FORMAT_CREATOR(format_t),                             \
    nullptr                                               \
});

//! Register a format by associating it to a format name and a file type.
#define REGISTER_WITH_FILE(format_t, name, file_t)        \
bool format_t::_registered_format_ =                      \
TrajectoryFactory::register_format(name, {                \
    FORMAT_CREATOR(format_t),                             \
    FILE_CREATOR(file_t)                                  \
});

//! Register a format by associating it to an extention, and no file type.
//! The extension should starts with a "."
#define REGISTER_EXTENSION(format_t, extension)           \
bool format_t::_registered_extension_ =                   \
TrajectoryFactory::register_extension(extension, {        \
    FORMAT_CREATOR(format_t),                             \
    nullptr                                               \
});

//! Register a format by associating it to an extention and a file type.
//! The extension should starts with a "."
#define REGISTER_EXTENSION_AND_FILE(format_t, extension, file_t)  \
bool format_t::_registered_extension_ =                           \
TrajectoryFactory::register_extension(extension, {                \
    FORMAT_CREATOR(format_t),                                     \
    FILE_CREATOR(file_t)                                          \
});

//! Add the static members in a class to register a format.
#define REGISTER_FORMAT                   \
    static bool _registered_extension_;   \
    static bool _registered_format_

} // namespace harp

#endif
