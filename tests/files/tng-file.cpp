// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
#include "chemfiles/files/TNGFile.hpp"
using namespace chemfiles;

#define CHECK_SUCCESS(x) CHECK(x == TNG_SUCCESS)

TEST_CASE("TNG files") {
    SECTION("Read") {
        // Just checking constructor and destructor
        TNGFile file("data/tng/example.tng", File::READ);

        CHECK_THROWS_WITH(
            TNGFile("not-there.tng", File::READ),
            "could not open the file at 'not-there.tng'"
        );
    }

    SECTION("Write") {
        auto filename = NamedTempPath(".tng");
        {
            // Just checking constructor and destructor
            TNGFile file(filename, File::WRITE);
        }

        // open the file manually and check it
        tng_trajectory_t trajectory = nullptr;
        CHECK_SUCCESS(tng_util_trajectory_open(filename.path().c_str(), 'r', &trajectory));
        CHECK_SUCCESS(tng_file_headers_read(trajectory, TNG_USE_HASH));

        char buffer[1024] = {0};
        CHECK_SUCCESS(tng_first_program_name_get(trajectory, buffer, 1024));
        CHECK(std::string(buffer) == "chemfiles");

        memset(buffer, 0, 1024);
        CHECK_SUCCESS(tng_last_program_name_get(trajectory, buffer, 1024));
        CHECK(std::string(buffer) == "chemfiles");

        CHECK_SUCCESS(tng_util_trajectory_close(&trajectory));
    }

    SECTION("Append") {
        auto filename = NamedTempPath(".tng");
        copy_file("data/tng/example.tng", filename.path());
        {
            // Just checking constructor and destructor
            TNGFile file(filename, File::APPEND);
        }

        tng_trajectory_t trajectory = nullptr;
        CHECK_SUCCESS(tng_util_trajectory_open(filename.path().c_str(), 'r', &trajectory));
        CHECK_SUCCESS(tng_file_headers_read(trajectory, TNG_USE_HASH));

        char buffer[1024] = {0};
        CHECK_SUCCESS(tng_first_program_name_get(trajectory, buffer, 1024));
        CHECK(std::string(buffer) == "");

        memset(buffer, 0, 1024);
        CHECK_SUCCESS(tng_last_program_name_get(trajectory, buffer, 1024));
        CHECK(std::string(buffer) == "chemfiles");

        CHECK_SUCCESS(tng_util_trajectory_close(&trajectory));
    }
}
