// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CAPI_ERRORS_H
#define CHEMFILES_CAPI_ERRORS_H

#include "chemfiles/capi/types.h"
#include "chemfiles/Error.hpp"
#include "chemfiles/warnings.hpp"

namespace chemfiles {
    extern std::string CAPI_LAST_ERROR;

inline size_t checked_cast(uint64_t value) {
    if (static_cast<uint64_t>(value) > static_cast<uint64_t>(SIZE_MAX)) {
        throw chemfiles::Error("Got a value too big to be represented by a size_t on this system");
    }
    return static_cast<size_t>(value);
}

#define CATCH_AND_RETURN(exception, retval)                                    \
    catch (const chemfiles::exception& e) {                                    \
        CAPI_LAST_ERROR = std::string(e.what());                               \
        chemfiles::warning(e.what());                                          \
        return retval;                                                         \
    }

#define CATCH_AND_GOTO_ERROR(exception)                                        \
    catch (const chemfiles::exception& e) {                                    \
        CAPI_LAST_ERROR = std::string(e.what());                               \
        chemfiles::warning(e.what());                                          \
        goto error;                                                            \
    }

#define CHECK_POINTER(ptr)                                                     \
    do {                                                                       \
        if (ptr == nullptr) {                                                  \
            std::string message = "Parameter " + std::string(#ptr) + " cannot be NULL"; \
            CAPI_LAST_ERROR = message;                                         \
            chemfiles::warning(message);                                       \
            return CHFL_MEMORY_ERROR;                                          \
        }                                                                      \
    } while (false)

#define CHECK_POINTER_GOTO(ptr)                                                \
    do {                                                                       \
        if (ptr == nullptr) {                                                  \
            std::string message = "Parameter " + std::string(#ptr) + " cannot be NULL"; \
            CAPI_LAST_ERROR = message;                                         \
            chemfiles::warning(message);                                       \
            goto error;                                                        \
        }                                                                      \
    } while (false)


/// Wrap `instructions` in a try/catch bloc automatically, and return a status
/// code
#define CHFL_ERROR_CATCH(instructions)                                         \
    try {                                                                      \
        instructions                                                           \
    }                                                                          \
    CATCH_AND_RETURN(FileError, CHFL_FILE_ERROR)                               \
    CATCH_AND_RETURN(MemoryError, CHFL_MEMORY_ERROR)                           \
    CATCH_AND_RETURN(FormatError, CHFL_FORMAT_ERROR)                           \
    CATCH_AND_RETURN(SelectionError, CHFL_SELECTION_ERROR)                     \
    CATCH_AND_RETURN(ConfigurationError, CHFL_CONFIGURATION_ERROR)             \
    CATCH_AND_RETURN(OutOfBounds, CHFL_OUT_OF_BOUNDS)                          \
    CATCH_AND_RETURN(PropertyError, CHFL_PROPERTY_ERROR)                       \
    CATCH_AND_RETURN(Error, CHFL_GENERIC_ERROR)                                \
    catch (const std::exception& e) {                                          \
        CAPI_LAST_ERROR = std::string(e.what());                               \
        return CHFL_CXX_ERROR;                                                 \
    }                                                                          \
    return CHFL_SUCCESS;

/// Wrap `instructions` in a try/catch bloc automatically, and goto the
/// `error` label in case of error.
#define CHFL_ERROR_GOTO(instructions)                                          \
    try {                                                                      \
        instructions                                                           \
    }                                                                          \
    CATCH_AND_GOTO_ERROR(FileError)                                            \
    CATCH_AND_GOTO_ERROR(MemoryError)                                          \
    CATCH_AND_GOTO_ERROR(FormatError)                                          \
    CATCH_AND_GOTO_ERROR(SelectionError)                                       \
    CATCH_AND_GOTO_ERROR(ConfigurationError)                                   \
    CATCH_AND_GOTO_ERROR(OutOfBounds)                                          \
    CATCH_AND_GOTO_ERROR(PropertyError)                                        \
    CATCH_AND_GOTO_ERROR(Error)                                                \
    catch (const std::exception& e) {                                          \
        CAPI_LAST_ERROR = std::string(e.what());                               \
        goto error;                                                            \
    }

}

#endif
