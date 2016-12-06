// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "chemfiles/utils.hpp"
#include "chemfiles/config.hpp"

#ifdef CHEMFILES_WINDOWS
#include <windows.h>
#include <lmcons.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif



std::string user_name() {
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

std::string hostname() {
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
