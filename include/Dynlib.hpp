/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/


#ifndef HARP_DYNLIB_HPP
#define HARP_DYNLIB_HPP

#include <string>
#include "Error.hpp"

#ifdef WIN32
    #include <direct.h>
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <dlfcn.h>
#endif

namespace harp {

/*!
 * @class DynLib DynLib.hpp DynLib.cpp
 *
 * Cross-platform dynamic library loading, and symbol resolving
 */
class Dynlib {
public:
    //! Load a library from it path
    explicit Dynlib(const std::string& path){
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
    //! A default constructor with no library associated
    Dynlib() : handle(nullptr) {}

    Dynlib(Dynlib&& other) : handle(other.handle) {
        other.handle = nullptr;
    }
    Dynlib& operator=(Dynlib&& other) {
        std::swap(handle, other.handle);
        return *this;
    }

    Dynlib(const Dynlib& other) = delete;
    Dynlib& operator=(const Dynlib& other) = delete;

    ~Dynlib() {
        if (handle) {
    #ifdef WIN32
        FreeLibrary(handle);
    #else
        dlclose(handle);
    #endif
        }
    }

    //! Load a specific symbol from the library. The template parameter is the
    //! typedef of the function
    template<class function_t>
    function_t symbol(const std::string& name){
        if (handle == nullptr)
            throw PluginError("The dynamic library was not opened.");
        #ifdef WIN32
            function_t sym = reinterpret_cast<function_t>(GetProcAddress(handle, name));
            if (!sym)
                throw PluginError("Cannot load symbol " + name + ": " + GetLastError());
        #else
            dlerror(); // reset errors
            function_t sym = reinterpret_cast<function_t>(dlsym(handle, name.c_str()));
            const char* dlsym_error = dlerror();
            if (dlsym_error)
                throw PluginError("Cannot load symbol " + name + ": " + std::string(dlsym_error));
        #endif
            return sym;
    }
private:
    #ifdef WIN32
        HINSTANCE handle;
    #else
        void* handle;
    #endif
};

}

#endif
