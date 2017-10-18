// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/utils.hpp"
#include "chemfiles/config.hpp"
#include "chemfiles/warnings.hpp"

#include <algorithm>

#ifdef CHEMFILES_WINDOWS
#include <windows.h>
#include <lmcons.h>
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#include <pwd.h>
#endif

std::string chemfiles::user_name() {
#ifdef CHEMFILES_WINDOWS
    char name[UNLEN + 1];
    DWORD size = sizeof(name);
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
    char name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(name);
    if (!GetComputerName(name, &size)) {
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
        auto result = getcwd(buffer.data(), size);
        if (result == nullptr) {
            if (errno == ERANGE) {
                continue;
            } else {
                return std::string("");
            }
        }
        // Get the first null character, and stop the copy there
        auto end = std::find(buffer.begin(), buffer.end(), '\0');
        return std::string(buffer.begin(), end);
    }
}
