// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/utils.hpp"
#include "chemfiles/config.hpp"
#include "chemfiles/warnings.hpp"

#include <algorithm>

#ifdef CHEMFILES_WINDOWS
#include <windows.h>  // GetUserName & GetComputerNameEx
#include <direct.h>  // _getcwd
#define getcwd _getcwd
#else
#include <unistd.h>
#include <pwd.h>
#endif

std::string chemfiles::user_name() {
#ifdef CHEMFILES_WINDOWS
    char name[1024] = {0};
    DWORD size = 1024;
    if (!GetUserName(name, &size)) {
        return "";
    }
    return name;
#else
    auto user = getpwuid(getuid());
    return user ? user->pw_name : "";
#endif
}

std::string chemfiles::hostname() {
#ifdef CHEMFILES_WINDOWS
    char name[1024] = {0};
    DWORD size = 1024;
    if (!GetComputerNameEx(ComputerNameDnsHostname, name, &size)) {
        return "";
    }
    return name;
#else
    #if defined(_POSIX_HOST_NAME_MAX)
    #define BUFFSIZE _POSIX_HOST_NAME_MAX
    #elif defined(MAXHOSTNAMELEN)
    #define BUFFSIZE MAXHOSTNAMELEN
    #else
    #define BUFFSIZE 1024
    #endif

    char name[BUFFSIZE];
    if (gethostname(name, sizeof(name))) {
        return "";
    }
    return name;
#endif
}

std::string chemfiles::current_directory() {
    // loop util buffer large enough
    for (size_t size = 128;; size *=2) {
        std::vector<char> buffer(size, '\0');
#ifdef CHEMFILES_WINDOWS
        auto result = getcwd(buffer.data(), static_cast<int>(size));
#else
        auto result = getcwd(buffer.data(), size);
#endif
        if (result == nullptr) {
            if (errno == ERANGE) {
                continue;
            } else {
                return "";
            }
        }
        // Remove additional '\0' from the string
        return std::string(buffer.data());
    }
}
