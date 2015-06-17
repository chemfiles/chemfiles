/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp/TrajectoryFactory.hpp"
using namespace harp;

trajectory_map_t& TrajectoryFactory::names(){
    static auto umap = trajectory_map_t();
    return umap;
}

trajectory_map_t& TrajectoryFactory::extensions(){
    static auto umap = trajectory_map_t();
    return umap;
}

trajectory_builder_t TrajectoryFactory::format(const string& name){
    if (names().find(name) == names().end())
        throw FormatError("Can not find the format \"" + name + "\".");
    return names()[name];
}

trajectory_builder_t TrajectoryFactory::by_extension(const string& ext){
    if (extensions().find(ext) == extensions().end())
        throw FormatError("Can not find a format associated with the \""
                           + ext + "\" extension.");
    return extensions()[ext];
}

bool TrajectoryFactory::register_format(const string& name, trajectory_builder_t tb){
    if (names().find(name) != names().end())
        throw FormatError("The name \"" + name + "\" is already "
                              "associated with a format.");
    names().emplace(name, tb);
    return true;
}

bool TrajectoryFactory::register_extension(const string& ext, trajectory_builder_t tb){
    if (extensions().find(ext) != extensions().end())
        throw FormatError("The extension \"" + ext + "\" is already "
                          "associated with a format.");
    extensions().emplace(ext, tb);
    return true;
}
