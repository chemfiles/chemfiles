// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <map>
#include <cassert>

#include "chemfiles/Error.hpp"
#include "chemfiles/capi/errors.h"
#include "chemfiles/capi.hpp"
using namespace chemfiles;

static_assert(sizeof(chfl_status) == sizeof(int), "Wrong size for chfl_status enum");

std::string chemfiles::CAPI_LAST_ERROR = "";

extern "C" const char* chfl_version(void) {
    return CHEMFILES_VERSION;
}

extern "C" const char* chfl_last_error(void) {
    return CAPI_LAST_ERROR.c_str();
}

extern "C" chfl_status chfl_clear_errors(void) {
    CHFL_ERROR_CATCH(
        CAPI_LAST_ERROR = "";
    )
}

extern "C" chfl_status chfl_set_warning_callback(chfl_warning_callback callback) {
    CHFL_ERROR_CATCH(
        set_warning_callback([callback](std::string message) {
            callback(message.c_str());
        });
    )
}
