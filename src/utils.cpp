// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/utils.hpp"
#include "chemfiles/config.hpp"

#ifdef CHEMFILES_WINDOWS
#include <windows.h>
#include <lmcons.h>
#else
#include <unistd.h>
#include <pwd.h>
#include <cstdlib>
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
    auto passwd = getpwuid(getuid());
    return passwd ? passwd->pw_name : "";
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

std::string chemfiles::home_path() {
#ifdef CHEMFILES_WINDOWS
    if (const char* env_home = std::getenv("HOME")) {
        return std::string(env_home);
    } else if (const char* env_user = std::getenv("USERPROFILE")) {
        return std::string(env_user);
    } else {
        const char* env_drive = std::getenv("HOMEDRIVE");
        const char* env_path = std::getenv("HOMEPATH");
        return std::string(env_drive)+std::string(env_path);
    }
#else
    if (const char* env_home = std::getenv("HOME")) {
        return std::string(env_home);
    } else {
        auto passwd = getpwuid(getuid());
        return passwd->pw_dir;
    }
#endif
}

