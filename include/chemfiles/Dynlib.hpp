/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/


#ifndef CHEMFILES_DYNLIB_HPP
#define CHEMFILES_DYNLIB_HPP

#include <string>
#include <sstream>
#include "chemfiles/Error.hpp"
#include "chemfiles/config.hpp"

#ifdef CHFL_WINDOWS
    #include <direct.h>
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <dlfcn.h>
#endif

namespace chemfiles {

/*!
 * @class Dynlib Dynlib.hpp
 *
 * Cross-platform dynamic library loading, and symbol resolving
 */
class Dynlib {
public:
    //! Load a library from it path
    explicit Dynlib(const std::string& path) : handle_(nullptr) {
        #ifdef CHFL_WINDOWS
            handle_ = LoadLibrary(TEXT(path.c_str()));
            if (!handle_)
                throw PluginError("Cannot load library: " + path);
        #else
            handle_ = dlopen(path.c_str(), RTLD_LAZY);
            if (!handle_)
                throw PluginError("Cannot load library: " + path + ". " + dlerror());
        #endif
    }
    //! A default constructor with no library associated
    Dynlib() : handle_(nullptr) {}

    Dynlib(Dynlib&& other) : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    Dynlib& operator=(Dynlib&& other) {
        handle_ = other.handle_;
        other.handle_ = nullptr;
        return *this;
    }

    Dynlib(const Dynlib& other) = delete;
    Dynlib& operator=(const Dynlib& other) = delete;

    ~Dynlib() {
        if (handle_) {
            #ifdef CHFL_WINDOWS
                FreeLibrary(handle_);
            #else
                dlclose(handle_);
            #endif
        }
    }

    //! Load a specific symbol from the library. The template parameter is the
    //! typedef of the function
    template<class function_t>
    function_t symbol(const std::string& name){
        if (handle_ == nullptr)
            throw PluginError("The dynamic library was not opened.");
            #ifdef CHFL_WINDOWS
                function_t sym = reinterpret_cast<function_t>(GetProcAddress(handle_, name.c_str()));
                if (!sym){
                    std::stringstream message;
                    message << "Cannot load symbol " << name << ": " << GetLastError();
                    throw PluginError(message.str());
                }
            #else
                dlerror(); // reset errors
                function_t sym = reinterpret_cast<function_t>(dlsym(handle_, name.c_str()));
                const char* dlsym_error = dlerror();
                if (dlsym_error)
                    throw PluginError("Cannot load symbol " + name + ": " + std::string(dlsym_error));
            #endif
            return sym;
    }
private:
    #ifdef CHFL_WINDOWS
        HINSTANCE handle_;
    #else
        void* handle_;
    #endif
};

} // namespace chemfiles

#endif
