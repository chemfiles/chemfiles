// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/generic.hpp"
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
