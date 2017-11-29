// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/generic.hpp"
#include "chemfiles/config.hpp"
#include "chemfiles/capi/misc.h"
#include "chemfiles/capi.hpp"
using namespace chemfiles;

static_assert(sizeof(chfl_status) == sizeof(int), "Wrong size for chfl_status enum");

static CHFL_THREAD_LOCAL std::string CAPI_LAST_ERROR;

void chemfiles::set_last_error(const std::string& message) {
    CAPI_LAST_ERROR = message;
}

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

extern "C" chfl_status chfl_add_configuration(const char* path) {
    CHFL_ERROR_CATCH(
        add_configuration(path);
    )
}
