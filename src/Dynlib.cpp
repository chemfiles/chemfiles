/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "Dynlib.hpp"

using namespace harp;

Dynlib::Dynlib() : handle(nullptr) {}

Dynlib::Dynlib(const std::string& path){
#ifdef WIN32
    // use SetDllDirectory to set the search path
    handle = LoadLibrary(TEXT(path.c_str()));
    if (!handle)
        throw PluginError("Cannot load library: " + path);
#else
    handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle)
        throw PluginError("Cannot load library: " + path + ". " + dlerror());
#endif
}

Dynlib::~Dynlib(){
#ifdef WIN32
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif
}
