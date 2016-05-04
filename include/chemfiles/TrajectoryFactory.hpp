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
#include <unordered_map>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"

namespace chemfiles {

//! Function type to create a format
typedef std::unique_ptr<Format> (*format_creator_t)(File& f);
//! Function type to create a file
typedef std::unique_ptr<File> (*file_creator_t)(const std::string& path, File::Mode mode);

//! Function to create a file
template <class file_t>
std::unique_ptr<File> new_file(const std::string& path, File::Mode mode) {
    return std::unique_ptr<File>(new file_t(path, mode));
}

//! Function to create a format
template <class format_t> std::unique_ptr<Format> new_format(File& file) {
    return std::unique_ptr<Format>(new format_t(file));
}

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

/*!
* @class trajectory_builder TrajectoryFactory.hpp
* @brief Structure associating format and file classes builder functions.
*/
struct trajectory_builder_t {
    format_creator_t format_creator;
    file_creator_t file_creator;
};

//! Files extensions to trajectory builder associations
using trajectory_map_t = std::unordered_map<std::string, trajectory_builder_t>;

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
class TrajectoryFactory {
private:
    TrajectoryFactory();

public:
    //! Get the instance of the TrajectoryFactory
    static TrajectoryFactory& get();

    /*!
     * @brief Get a trajectory_builder from a format type name.
     * @param name the format name
     * @return A trajectory_builder corresponding to the format, if the format
     *         name is found in the list of registered formats.
     *
     * Throws an error if the format can not be found
     */
    trajectory_builder_t format(const std::string& name);

    /*!
     * @brief Get a trajectory_builder from a format \c extention.
     * @param ext the format extention
     * @return A trajectory_builder corresponding to the format, if the format
     *         extension is found in the list of registered extensions.
     *
     * Throws an error if the format can not be found
     */
    trajectory_builder_t by_extension(const std::string& ext);

    //! Register a trajectory_builder in the internal format names list.
    void register_format(const std::string& name, trajectory_builder_t tb);
    //! Register an trajectory_builder in the internal extensions list.
    void register_extension(const std::string& ext, trajectory_builder_t tb);

private:
    //! Trajectory map associating format descriptions and readers
    trajectory_map_t formats_;
    //! Trajectory map associating format descriptions and readers
    trajectory_map_t extensions_;
};

} // namespace chemfiles

#endif
