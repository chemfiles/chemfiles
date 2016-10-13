/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
// clang-format off

#include <cassert>

#include "chemfiles.h"
#include "chemfiles/Logger.hpp"
#include "chemfiles/capi.hpp"
using namespace chemfiles;

static_assert(sizeof(chfl_status) == sizeof(int), "Wrong size for chfl_status enum");
static_assert(sizeof(chfl_log_level_t) == sizeof(int), "Wrong size for chfl_log_level_t enum");

const std::map<chfl_status, std::string> ERROR_MESSAGES = {
    {CHFL_SUCCESS, "operation was sucessfull"},
    {CHFL_MEMORY_ERROR, "memory allocation error."},
    {CHFL_FILE_ERROR, "system error while reading a file"},
    {CHFL_FORMAT_ERROR, "error while parsing a file"},
    {CHFL_SELECTION_ERROR, "error in selection parsing or evaluation"},
    {CHFL_GENERIC_ERROR, "unknown error from chemfiles library"},
    {CHFL_CXX_ERROR, "error from the C++ standard library"},
};

std::string chemfiles::CAPI_LAST_ERROR = "";

const char* chfl_version(void) {
    return CHEMFILES_VERSION;
}

const char* chfl_strerror(chfl_status code) {
    auto message = ERROR_MESSAGES.find(code);
    if (message != ERROR_MESSAGES.end()) {
        return message->second.c_str();
    } else {
        return "";
    }
}

const char* chfl_last_error(void) {
    return CAPI_LAST_ERROR.c_str();
}

chfl_status chfl_clear_errors(void) {
    CHFL_ERROR_CATCH(
        CAPI_LAST_ERROR = "";
    )
}

chfl_status chfl_loglevel(chfl_log_level_t* const level) {
    assert(level != nullptr);
    CHFL_ERROR_CATCH(
        *level = static_cast<chfl_log_level_t>(Logger::level());
    )
}

chfl_status chfl_set_loglevel(chfl_log_level_t level) {
    CHFL_ERROR_CATCH(
        Logger::set_level(static_cast<LogLevel>(level));
    )
}

chfl_status chfl_logfile(const char* file) {
    assert(file != nullptr);
    CHFL_ERROR_CATCH(
        Logger::to_file(std::string(file));
    )
}

chfl_status chfl_log_stdout(void) {
    CHFL_ERROR_CATCH(
        Logger::to_stdout();
    )
}

chfl_status chfl_log_stderr(void) {
    CHFL_ERROR_CATCH(
        Logger::to_stderr();
    )
}

chfl_status chfl_log_silent(void) {
    CHFL_ERROR_CATCH(
        Logger::silent();
    )
}

chfl_status chfl_log_callback(chfl_logging_cb callback) {
    CHFL_ERROR_CATCH(
        Logger::callback([callback](LogLevel level, const std::string& message) {
            callback(static_cast<chfl_log_level_t>(level), message.c_str());
        });
    )
}
