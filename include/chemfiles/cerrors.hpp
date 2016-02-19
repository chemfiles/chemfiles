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
        //! Error in selection parsing
        SELECTION = CHFL_SELECTION_ERROR,
        //! Catch all chemfiles::Error errors
        CHEMFILES = CHFL_GENERIC_ERROR,
        //! Error in the C++ standard library
        CXX_ERROR = CHFL_CXX_ERROR,
        //! Counter for the number of error codes.
        LAST,
    };

    CAPIStatus() {
        messages_[SUCCESS] = "Operation was sucessfull";
        messages_[MEMORY] = "Memory error. Use chfl_last_error for more informations.";
        messages_[FILE] = "Error while reading a file. Use chfl_last_error for more informations.";
        messages_[FORMAT] = "Error while reading a format. Use chfl_last_error for more informations.";
        messages_[SELECTION] = "Error in selection string parsing. Use chfl_last_error for more informations.";
        messages_[CHEMFILES] = "Error in chemfiles library. Use chfl_last_error for more informations.";
        messages_[CXX_ERROR] = "Error in C++ runtime. Use chfl_last_error for more informations.";
    }

    /// Retrive the message corresponding to an error code.
    const char* message(int i) const {
        if (i >= 0 && i < LAST)
            return messages_[i];
        else
            return "";
    }
    std::string last_error;
private:
    const char* messages_[LAST];
};

#define CATCH_AND_RETURN(exception, retval)           \
    catch(const chemfiles::exception& e) {            \
        status.last_error = std::string(e.what());    \
        Logger::log(LogLevel::ERROR, e.what());       \
        return retval;                                \
    }

#define CATCH_AND_GOTO(exception)                     \
    catch(const chemfiles::exception& e) {            \
        status.last_error = std::string(e.what());    \
        Logger::log(LogLevel::ERROR, e.what());       \
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
    CATCH_AND_RETURN(LexerError, CAPIStatus::SELECTION)                        \
    CATCH_AND_RETURN(ParserError, CAPIStatus::SELECTION)                       \
    CATCH_AND_RETURN(Error, CAPIStatus::CHEMFILES)                             \
    catch(const std::exception& e) {                                           \
        status.last_error = std::string(e.what());                             \
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
    CATCH_AND_GOTO(LexerError)                                                 \
    CATCH_AND_GOTO(ParserError)                                                \
    CATCH_AND_GOTO(Error)                                                      \
    catch(const std::exception& e) {                                           \
        status.last_error = std::string(e.what());                             \
        goto error;                                                            \
    }

} // namespace chemfiles

#endif
