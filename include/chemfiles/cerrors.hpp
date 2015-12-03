/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_CAPI_ERRORS_H
#define CHEMFILES_CAPI_ERRORS_H

#include <string>
#include "chemfiles/Logger.hpp"
#include "chemfiles.h"

namespace chemfiles {

//! @brief struct to associate the values of status code to messages.
struct CAPIStatus {
    enum {
        //! Everythig is OK
        SUCCESS = CHFL_SUCCESS,
        //! Memory error: wrong pre-allocated arrays, ...
        MEMORY = CHFL_MEMORY_ERROR,
        //! File error: inexistent, can not open, ...
        FILE = CHFL_FILE_ERROR,
        //! Error in file formating
        FORMAT = CHFL_FORMAT_ERROR,
        //! Catch all chemfiles::Error errors
        CHEMFILES = CHFL_GENERIC_ERROR,
        //! Error in the C++ standard library
        CXX_ERROR = CHFL_CXX_ERROR,
        //! Counter for the number of error codes.
        LAST,
    };

    CAPIStatus() {
        messages[SUCCESS] = "Operation was sucessfull";
        messages[MEMORY] = "Memory error. Use chfl_last_error for more informations.";
        messages[FILE] = "Error while reading a file. Use chfl_last_error for more informations.";
        messages[FORMAT] = "Error while reading a format. Use chfl_last_error for more informations.";
        messages[CHEMFILES] = "Error in chemfiles library. Use chfl_last_error for more informations.";
        messages[CXX_ERROR] = "Error in C++ runtime. Use chfl_last_error for more informations.";
    }

    /// Retrive the message corresponding to an error code.
    const char* message(int i) const {
        if (i >= 0 && i < LAST)
            return messages[i];
        else
            return "";
    }
    std::string last_error;
private:
    const char* messages[LAST];
};

static CAPIStatus status = CAPIStatus();

#define CATCH_AND_RETURN(exception, retval)           \
    catch(const chemfiles::exception& e) {            \
        status.last_error = string(e.what());         \
        LOG(ERROR) << e.what() << std::endl;          \
        return retval;                                \
    }

#define CATCH_AND_GOTO(exception)                     \
    catch(const chemfiles::exception& e) {            \
        status.last_error = string(e.what());         \
        LOG(ERROR) << e.what() << std::endl;          \
        goto error;                                   \
    }

//! Wrap \c instructions in a try/catch bloc automatically, and return a status code
#define CHFL_ERROR_WRAP_RETCODE(instructions)                                  \
    try {                                                                      \
        instructions                                                           \
    }                                                                          \
    CATCH_AND_RETURN(FileError, CAPIStatus::FILE)                              \
    CATCH_AND_RETURN(MemoryError, CAPIStatus::MEMORY)                          \
    CATCH_AND_RETURN(FormatError, CAPIStatus::FORMAT)                          \
    CATCH_AND_RETURN(Error, CAPIStatus::CHEMFILES)                             \
    catch(const std::exception& e) {                                           \
        status.last_error = string(e.what());                                  \
        return CAPIStatus::CXX_ERROR;                                          \
    }                                                                          \
    return CAPIStatus::SUCCESS;

//! Wrap \c instructions in a try/catch bloc automatically, and goto the \c error
//! label in case of error.
#define CHFL_ERROR_WRAP(instructions)                                          \
    try {                                                                      \
        instructions                                                           \
    }                                                                          \
    CATCH_AND_GOTO(FileError)                                                  \
    CATCH_AND_GOTO(MemoryError)                                                \
    CATCH_AND_GOTO(FormatError)                                                \
    CATCH_AND_GOTO(Error)                                                      \
    catch(const std::exception& e) {                                           \
        status.last_error = string(e.what());                                  \
        goto error;                                                            \
    }

} // namespace chemfiles

#endif
