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

#include "chemharp/Format.hpp"
#include "chemharp/Error.hpp"

namespace harp {

using std::shared_ptr;
using std::unique_ptr;
using std::string;

//! Function type to create a format
typedef unique_ptr<Format> (*format_creator_t) (File& f);
//! Function type to create a file
typedef unique_ptr<File> (*file_creator_t) (const string& path, const string& mode);

/*!
* @class trajectory_builder TrajectoryFactory.hpp
* @brief Structure associating format and file classes builder functions.
*/
struct trajectory_builder_t {
    format_creator_t format_creator;
    file_creator_t file_creator;
};

//! Files extensions to trajectory builder associations
using trajectory_map_t = std::unordered_map<string, trajectory_builder_t>;

/*!
* @class TrajectoryFactory TrajectoryFactory.hpp
*
* This class allow to register at compile time various Format and the associated
* File class, giving at runtime the good format when asked politely.
*
* Each couple (File, Format) is represented by a trajectory_builder_t instance, and can
* be registered by an extension, or by a Format name.
*/
class TrajectoryFactory {
private:
    //! Get a static reader_map instance associating format descriptions and readers
    static trajectory_map_t& names();
    //! Static reader_map instance associating format descriptions and readers
    static trajectory_map_t& extensions();
public:
    /*!
     * @brief Get a trajectory_builder from a format type name.
     * @param name the format name
     * @return A trajectory_builder corresponding to the format, if the format
     *         name is found in the list of registered formats.
     *
     * Throws an error if the format can not be found
     */
    static trajectory_builder_t format(const string& name);

    /*!
     * @brief Get a trajectory_builder from a format \c extention.
     * @param ext the format extention
     * @return A trajectory_builder corresponding to the format, if the format
     *         extension is found in the list of registered extensions.
     *
     * Throws an error if the format can not be found
     */
    static trajectory_builder_t by_extension(const string& ext);

    //! Register a trajectory_builder in the internal format names list.
    static bool register_format(const string& name, trajectory_builder_t tb);
    //! Register an trajectory_builder in the internal extensions list.
    static bool register_extension(const string& ext, trajectory_builder_t tb);
};

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
