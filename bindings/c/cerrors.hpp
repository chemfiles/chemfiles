/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

/*! @file cerrors.h
* Handling C++ errors from the C API
*/

#ifndef HARP_CAPI_ERRORS_H
#define HARP_CAPI_ERRORS_H

#include <string>

namespace harp {

//! @brief struct to associate the values of status code to messages.
struct CAPIStatus {
    enum {
        //! Everythig is OK
        SUCESS = 0,
        //! Error in the C++ standard library
        STD_ERROR,
        //! Catch all harp::Error errors
        GENERIC,
        //! Memory error: wrong pre-allocated arrays, ...
        MEMORY,
        //! File error: inexistent, can not open, ...
        FILE,
        //! Error in the C++ standard library
        FORMAT,
        //! Counter for the number of error codes.
        LAST,
    };

    CAPIStatus() {
        messages[SUCESS] = "Operation was sucessfull";
        messages[STD_ERROR] = "Error in C++ runtime. Use chrp_last_error for more informations.";
        messages[GENERIC] = "Error in Chemharp library. Use chrp_last_error for more informations.";
        messages[MEMORY] = "Memory error.";
        messages[FILE] = "Error while reading a file.";
        messages[FORMAT] = "Error while reading a format.";
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

#define CATCH_AND_RETURN_STATUS(error, status)        \
    catch(const harp::error& e) {                     \
        CAPIStatus.last_error = string(e.what());     \
        LOG(ERROR) << e.what() << endl;               \
        return CAPIStatus::status;                    \
    }

///! Wrap \c instructions in a try/catch bloc automatically
#define CHRP_ERROR_WRAP(instructions)                       \
    try {                                                   \
        instructions                                        \
    }                                                       \
    CATCH_AND_RETURN_STATUS(FileError, FILE)                \
    CATCH_AND_RETURN_STATUS(MemoryError, MEMORY)            \
    CATCH_AND_RETURN_STATUS(FormatError, FORMAT)            \
    CATCH_AND_RETURN_STATUS(Error, GENERIC)                 \
    catch(const std::exception& e) {                        \
        CAPIStatus.last_error = string(e.what());           \
        return CAPIStatus::STD_ERROR;                       \
    }                                                       \
    return CAPIStatus::SUCESS;

}

#endif
