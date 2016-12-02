#include <fstream>
#include <sstream>
#include <cstring>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

static std::string generate_chemfiles_error() {
    // Generate a log event
    CHECK(chfl_trajectory_open("noformat", 'r') == NULL);
    return "Can not find a format associated with the \"\" extension.";
}

TEST_CASE("Errors", "[CAPI]") {
    CHECK(chfl_strerror(CHFL_SUCCESS) == std::string("operation was sucessfull"));
    CHECK(chfl_last_error() == std::string(""));

    CHECK_STATUS(chfl_log_silent());
    generate_chemfiles_error();
    CHECK_STATUS(chfl_log_stderr());

    CHECK(chfl_last_error() != std::string(""));
    CHECK_STATUS(chfl_clear_errors());
    CHECK(chfl_last_error() == std::string(""));
}

// Global variables for access from callback and main
static char* buffer = NULL;
static chfl_log_level_t last_level;

static void callback(chfl_log_level_t level, const char* message) {
    size_t size = strlen(message) + 1;
    REQUIRE(buffer == NULL);
    buffer = static_cast<char*>(malloc(sizeof(char) * size));
    strcpy(buffer, message);
    last_level = level;
}

TEST_CASE("Logging", "[CAPI]") {
    // Just checking return values
    CHECK_STATUS(chfl_log_stdout());
    CHECK_STATUS(chfl_log_silent());
    CHECK_STATUS(chfl_log_stderr());

    SECTION("Log level") {
        chfl_log_level_t level;
        CHECK_STATUS(chfl_loglevel(&level));
        CHECK(level == CHFL_LOG_WARNING);

        CHECK_STATUS(chfl_set_loglevel(CHFL_LOG_DEBUG));
        CHECK_STATUS(chfl_loglevel(&level));
        CHECK(level == CHFL_LOG_DEBUG);

        CHECK_STATUS(chfl_set_loglevel(CHFL_LOG_WARNING));
    }

    SECTION("logfile") {
        const char* filename = "test.log";
        CHECK_STATUS(chfl_logfile(filename));

        std::string message = generate_chemfiles_error();

        std::ifstream file(filename);
        REQUIRE(file.is_open());
        std::stringstream content;
        content << file.rdbuf();
        file.close();

        CHECK(content.str() == "Chemfiles error: " + message + "\n");

        remove(filename);
        CHECK_STATUS(chfl_log_stderr());
    }

    SECTION("Callback") {
        CHECK_STATUS(chfl_log_callback(callback));

        std::string message = generate_chemfiles_error();

        CHECK(buffer == message);
        CHECK(last_level == CHFL_LOG_ERROR);
        free(buffer);

        CHECK_STATUS(chfl_log_stderr());
    }
}

TEST_CASE("Version", "[CAPI]") {
    std::ifstream file(SRCDIR "/VERSION");
    REQUIRE(file.is_open());
    std::stringstream content;
    content << file.rdbuf();
    file.close();

    // Remove trailling \n
    std::string version = content.str().substr(0, content.str().length() - 1);
    CHECK(chfl_version() == version);
}
