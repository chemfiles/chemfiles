/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include "chemfiles/TrajectoryFactory.hpp"

#include "chemfiles/formats/XYZ.hpp"
#include "chemfiles/formats/NCFormat.hpp"
#include "chemfiles/formats/Molfile.hpp"

#include "chemfiles/files/NCFile.hpp"
using namespace chemfiles;

typedef FORMATS_LIST formats_list;

template <typename T>
inline void register_all_formats(trajectory_map_t& formats, trajectory_map_t& extensions, FormatList<T>) {
    auto creator = trajectory_builder_t{new_format<T>, new_file<typename T::file_t>};

    auto ext = std::string(T::extension());
    if (ext != ""){
        if (extensions.find(ext) != extensions.end()) {
            throw FormatError("The extension \"" + ext + "\" is already associated with a format.");
        }
        extensions.emplace(ext, creator);
    }

    auto name = std::string(T::name());
    if (name != ""){
        if (formats.find(name) != formats.end()) {
            throw FormatError("The name \"" + name + "\" is already associated with a format.");
        }
        formats.emplace(name, creator);
    }
}

template <typename T, typename S, typename ...Types>
inline void register_all_formats(trajectory_map_t& formats, trajectory_map_t& extensions, FormatList<T, S, Types...>) {
    register_all_formats(formats, extensions, FormatList<T>());
    register_all_formats(formats, extensions, FormatList<S, Types...>());
}

TrajectoryFactory::TrajectoryFactory() : formats_(), extensions_() {
    register_all_formats(formats_, extensions_, formats_list());
}

TrajectoryFactory& TrajectoryFactory::get() {
    static auto instance = TrajectoryFactory();
    return instance;
}

trajectory_builder_t TrajectoryFactory::format(const string& name){
    if (formats_.find(name) == formats_.end()) {
        throw FormatError("Can not find the format \"" + name + "\".");
    }
    return formats_[name];
}

trajectory_builder_t TrajectoryFactory::by_extension(const string& ext){
    if (extensions_.find(ext) == extensions_.end()) {
        throw FormatError("Can not find a format associated with the \"" + ext + "\" extension.");
    }
    return extensions_[ext];
}

void TrajectoryFactory::register_format(const string& name, trajectory_builder_t tb){
    if (formats_.find(name) != formats_.end()) {
        throw FormatError("The name \"" + name + "\" is already associated with a format.");
    }
    formats_.emplace(name, tb);
}

void TrajectoryFactory::register_extension(const string& ext, trajectory_builder_t tb){
    if (extensions_.find(ext) != extensions_.end()) {
        throw FormatError("The extension \"" + ext + "\" is already associated with a format.");
    }
    extensions_.emplace(ext, tb);
}
