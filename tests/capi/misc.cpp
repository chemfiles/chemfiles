// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include <sstream>
#include <cstring>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

static std::string generate_chemfiles_error() {
    // Generate a log event
    CHECK_FALSE(chfl_trajectory_open("noformat", 'r'));
    return "file at 'noformat' does not have an extension, provide a format name to read it";
}

TEST_CASE("Errors") {
    CHECK(chfl_last_error() == std::string(""));

    generate_chemfiles_error();

    CHECK(chfl_last_error() != std::string(""));
    CHECK_STATUS(chfl_clear_errors());
    CHECK(chfl_last_error() == std::string(""));
}

TEST_CASE("Version") {
    std::ifstream file(VERSION_FILE_PATH);
    REQUIRE(file.is_open());
    std::stringstream content;
    content << file.rdbuf();
    file.close();

    // Remove trailling \n
    std::string version = content.str().substr(0, content.str().length() - 1);
    CHECK(chfl_version() == version);
}

TEST_CASE("Guess format") {
    char format[256] = {0};
    CHECK_STATUS(chfl_guess_format("filename.nc", format, sizeof(format)));
    CHECK(format == std::string("Amber NetCDF"));

    CHECK_STATUS(chfl_guess_format("filename.xyz.gz", format, sizeof(format)));
    CHECK(format == std::string("XYZ / GZ"));

    // buffer too small for the format
    chfl_status status = chfl_guess_format("filename.nc", format, 8);
    CHECK(status == CHFL_MEMORY_ERROR);

    // no associated format
    status = chfl_guess_format("filename.not-there", format, 8);
    CHECK(status == CHFL_FORMAT_ERROR);
}

// Global variables for access from callback and main
static char* buffer = nullptr;

static void callback(const char* message) {
    size_t size = strlen(message) + 1;
    CHECK_FALSE(buffer);
    buffer = static_cast<char*>(malloc(sizeof(char) * size));
    strcpy(buffer, message);
}

TEST_CASE("Warnings") {
    CHECK_STATUS(chfl_set_warning_callback(callback));

    std::string message = generate_chemfiles_error();

    CHECK(buffer == message);
    free(buffer);
}
