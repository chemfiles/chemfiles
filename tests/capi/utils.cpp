// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <stdexcept>

#include "catch.hpp"
#include "helpers.hpp"

#include "chemfiles/capi/utils.hpp"
#include "chemfiles/Error.hpp"

#include "chemfiles.h"

using namespace chemfiles;

struct CustomError: Error {
    CustomError(std::string message): Error(message) {}
};

struct std_derived: public std::exception {
    std_derived(std::string): std::exception() {}
};

struct std_derived_2: public std::runtime_error {
    std_derived_2(std::string message): std::runtime_error(message) {}
};

#define generate_function_status(__error__)                                    \
static chfl_status throw_ ##__error__ () {                                     \
    CHFL_ERROR_CATCH(                                                          \
        throw __error__ (#__error__);                                          \
    )                                                                          \
    return CHFL_SUCCESS;                                                       \
}

generate_function_status(MemoryError)
generate_function_status(FileError)
generate_function_status(FormatError)
generate_function_status(SelectionError)
generate_function_status(ConfigurationError)
generate_function_status(OutOfBounds)
generate_function_status(PropertyError)
generate_function_status(Error)
generate_function_status(CustomError)

using std::runtime_error;
using std::string;
generate_function_status(runtime_error)
generate_function_status(std_derived)
generate_function_status(std_derived_2)
generate_function_status(string)


#define generate_function_goto(__error__)                                      \
static int goto_ ##__error__ () {                                              \
    CHFL_ERROR_GOTO(                                                           \
        throw __error__ (#__error__);                                          \
        return 0;                                                              \
    )                                                                          \
    error:                                                                     \
        return 1;                                                              \
}

generate_function_goto(MemoryError)
generate_function_goto(FileError)
generate_function_goto(FormatError)
generate_function_goto(SelectionError)
generate_function_goto(ConfigurationError)
generate_function_goto(OutOfBounds)
generate_function_goto(PropertyError)
generate_function_goto(Error)
generate_function_goto(CustomError)
generate_function_goto(runtime_error)
generate_function_goto(std_derived)
generate_function_goto(std_derived_2)
generate_function_goto(string)


TEST_CASE("Error handling") {
    SECTION("Status code") {
        CHECK(throw_Error() == CHFL_GENERIC_ERROR);
        CHECK(std::string(chfl_last_error()) == "Error");

        CHECK(throw_MemoryError() == CHFL_MEMORY_ERROR);
        CHECK(std::string(chfl_last_error()) == "MemoryError");

        CHECK(throw_FileError() == CHFL_FILE_ERROR);
        CHECK(std::string(chfl_last_error()) == "FileError");

        CHECK(throw_FormatError() == CHFL_FORMAT_ERROR);
        CHECK(std::string(chfl_last_error()) == "FormatError");

        CHECK(throw_SelectionError() == CHFL_SELECTION_ERROR);
        CHECK(std::string(chfl_last_error()) == "SelectionError");

        CHECK(throw_ConfigurationError() == CHFL_CONFIGURATION_ERROR);
        CHECK(std::string(chfl_last_error()) == "ConfigurationError");

        CHECK(throw_OutOfBounds() == CHFL_OUT_OF_BOUNDS);
        CHECK(std::string(chfl_last_error()) == "OutOfBounds");

        CHECK(throw_PropertyError() == CHFL_PROPERTY_ERROR);
        CHECK(std::string(chfl_last_error()) == "PropertyError");

        CHECK(throw_CustomError() == CHFL_GENERIC_ERROR);
        CHECK(std::string(chfl_last_error()) == "CustomError");

        CHECK(throw_runtime_error() == CHFL_CXX_ERROR);
        CHECK(std::string(chfl_last_error()) == "runtime_error");

        CHECK_STATUS(chfl_clear_errors());
        CHECK(std::string(chfl_last_error()) == "");

        CHECK(throw_std_derived() == CHFL_CXX_ERROR);
        CHECK(std::string(chfl_last_error()) != "");

        CHECK(throw_std_derived_2() == CHFL_CXX_ERROR);
        CHECK(std::string(chfl_last_error()) == "std_derived_2");

        CHECK(throw_string() == CHFL_CXX_ERROR);
        CHECK(std::string(chfl_last_error()) == "UNKNOWN ERROR");
    }

    SECTION("goto") {
        CHECK(goto_Error() == 1);
        CHECK(std::string(chfl_last_error()) == "Error");

        CHECK(goto_MemoryError() == 1);
        CHECK(std::string(chfl_last_error()) == "MemoryError");

        CHECK(goto_FileError() == 1);
        CHECK(std::string(chfl_last_error()) == "FileError");

        CHECK(goto_FormatError() == 1);
        CHECK(std::string(chfl_last_error()) == "FormatError");

        CHECK(goto_SelectionError() == 1);
        CHECK(std::string(chfl_last_error()) == "SelectionError");

        CHECK(goto_ConfigurationError() == 1);
        CHECK(std::string(chfl_last_error()) == "ConfigurationError");

        CHECK(goto_OutOfBounds() == 1);
        CHECK(std::string(chfl_last_error()) == "OutOfBounds");

        CHECK(goto_PropertyError() == 1);
        CHECK(std::string(chfl_last_error()) == "PropertyError");

        CHECK(goto_CustomError() == 1);
        CHECK(std::string(chfl_last_error()) == "CustomError");

        CHECK(goto_runtime_error() == 1);
        CHECK(std::string(chfl_last_error()) == "runtime_error");

        CHECK_STATUS(chfl_clear_errors());
        CHECK(std::string(chfl_last_error()) == "");

        CHECK(goto_std_derived() == 1);
        CHECK(std::string(chfl_last_error()) != "");

        CHECK(goto_std_derived_2() == 1);
        CHECK(std::string(chfl_last_error()) == "std_derived_2");

        CHECK(goto_string() == 1);
        CHECK(std::string(chfl_last_error()) == "UNKNOWN ERROR");
    }
}
