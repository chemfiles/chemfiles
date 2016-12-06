/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_FORMAT_FACTORY_HPP
#define CHEMFILES_FORMAT_FACTORY_HPP

#include <memory>
#include <functional>
#include <unordered_map>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"

namespace chemfiles {

//! Function to create a format
template <class format_t> std::unique_ptr<Format> new_format(const std::string& path, File::Mode mode) {
    return std::unique_ptr<Format>(new format_t(path, mode));
}

typedef std::function<std::unique_ptr<Format>(const std::string& path, File::Mode mode)> format_creator_t;

#define FORMAT_EXTENSION(x)                                                    \
    static const char* extension() {                                           \
        static constexpr char val[] = #x;                                      \
        return val;                                                            \
    }
#define FORMAT_NAME(x)                                                         \
    static const char* name() {                                                \
        static constexpr char val[] = #x;                                      \
        return val;                                                            \
    }


//! Files extensions to trajectory builder associations
using trajectory_map_t = std::unordered_map<std::string, format_creator_t>;

/*!
* @class TrajectoryFactory TrajectoryFactory.hpp
*
* This class allow to register at compile time various Format and the associated
* File class, giving at runtime the good format when asked politely.
*
* Each couple (File, Format) is represented by a trajectory_builder_t instance,
* and can
* be registered by an extension, or by a Format name.
*/
class FormatFactory {
private:
    FormatFactory();

public:
    //! Get the instance of the TrajectoryFactory
    static FormatFactory& get();

    /*!
     * @brief Get a `format_creator_t` from a format type name.
     * @param name the format name
     * @return A `format_creator_t` corresponding to the format, if the format
     *         name is found in the list of registered formats.
     *
     * Throws an error if the format can not be found
     */
    format_creator_t format(const std::string& name);

    /*!
     * @brief Get a `format_creator_t` from a format extention.
     * @param ext the format extention
     * @return A `format_creator_t` corresponding to the format, if the format
     *         extension is found in the list of registered extensions.
     *
     * Throws an error if the format can not be found
     */
    format_creator_t by_extension(const std::string& ext);

    //! Register a `format_creator_t` in the internal format names list.
    void register_format(const std::string& name, format_creator_t creator);
    //! Register a `format_creator_t` in the internal extensions list.
    void register_extension(const std::string& ext, format_creator_t creator);

private:
    //! Trajectory map associating format descriptions and readers
    trajectory_map_t formats_;
    //! Trajectory map associating format descriptions and readers
    trajectory_map_t extensions_;
};

} // namespace chemfiles

#endif
