/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifdef WIN32
#include <windows.h>
#include <lmcons.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif

#include "chemfiles/utils.hpp"

std::string user_name() {
#ifdef WIN32
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
#ifdef WIN32
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
