/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp/TrajectoryFactory.hpp"


#include "chemharp/formats/XYZ.hpp"
#include "chemharp/formats/NCFormat.hpp"
#include "chemharp/formats/Molfile.hpp"

#include "chemharp/files/NCFile.hpp"
using namespace harp;

typedef FORMATS_LIST formats_list;

template <typename T>
inline void register_all_formats(FormatList<T>) {
    auto creator = trajectory_builder_t{new_format<T>, new_file<typename T::file_t>};
    if (T::extension() != std::string("")){
        TrajectoryFactory::register_extension(T::extension(), creator);
    }
    if (T::name() != std::string("")){
        TrajectoryFactory::register_format(T::name(), creator);
    }
}

template <typename T, typename S, typename ...Types>
inline void register_all_formats(FormatList<T, S, Types...>) {
    register_all_formats(FormatList<T>());
    register_all_formats(FormatList<S, Types...>());
}

// Initializing to false before main.
bool TrajectoryFactory::initialized = false;

trajectory_map_t& TrajectoryFactory::names(){
    static auto umap = trajectory_map_t();
    return umap;
}

trajectory_map_t& TrajectoryFactory::extensions(){
    static auto umap = trajectory_map_t();
    return umap;
}

trajectory_builder_t TrajectoryFactory::format(const string& name){
    if (!TrajectoryFactory::initialized) {
        register_all_formats(formats_list());
        TrajectoryFactory::initialized = true;
    }
    if (names().find(name) == names().end())
        throw FormatError("Can not find the format \"" + name + "\".");
    return names()[name];
}

trajectory_builder_t TrajectoryFactory::by_extension(const string& ext){
    if (!TrajectoryFactory::initialized) {
        register_all_formats(formats_list());
        TrajectoryFactory::initialized = true;
    }
    if (extensions().find(ext) == extensions().end())
        throw FormatError("Can not find a format associated with the \""
                           + ext + "\" extension.");
    return extensions()[ext];
}

void TrajectoryFactory::register_format(const string& name, trajectory_builder_t tb){
    if (names().find(name) != names().end())
        throw FormatError("The name \"" + name + "\" is already "
                              "associated with a format.");
    names().emplace(name, tb);
}

void TrajectoryFactory::register_extension(const string& ext, trajectory_builder_t tb){
    if (extensions().find(ext) != extensions().end())
        throw FormatError("The extension \"" + ext + "\" is already "
                          "associated with a format.");
    extensions().emplace(ext, tb);
}
