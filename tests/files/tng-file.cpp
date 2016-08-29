#include "catch.hpp"
#include "chemfiles.hpp"
#include "chemfiles/files/TNGFile.hpp"
using namespace chemfiles;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;

#define DATADIR SRCDIR "/data/"

TEST_CASE("Opening TNG files", "[Files]"){
    SECTION("Read") {
        // Just checking constructor and destructor
        TNGFile file(DATADIR "tng/example.tng", File::READ);
    }

    auto TESTFILE = "testing.tng";

    SECTION("Write") {
        // Just checking constructor and destructor
        TNGFile file(TESTFILE, File::WRITE);
    }

    SECTION("Check write") {
        tng_trajectory_t trajectory = nullptr;
        CHECK(tng_util_trajectory_open(TESTFILE, 'r', &trajectory) == TNG_SUCCESS);
        CHECK(tng_file_headers_read(trajectory, TNG_USE_HASH) == TNG_SUCCESS);

        char buffer[1024] = {0};
        CHECK(tng_first_program_name_get(trajectory, buffer, 1024) == TNG_SUCCESS);
        CHECK(std::string(buffer) == "chemfiles");

        memset(buffer, 0, 1024);
        CHECK(tng_last_program_name_get(trajectory, buffer, 1024) == TNG_SUCCESS);
        CHECK(std::string(buffer) == "chemfiles");

        CHECK(tng_util_trajectory_close(&trajectory) == TNG_SUCCESS);
        fs::remove(TESTFILE);
    }

    SECTION("Append") {
        fs::copy_file(DATADIR "tng/example.tng", TESTFILE);
        // Just checking constructor and destructor
        TNGFile file(TESTFILE, File::APPEND);
    }

    SECTION("Check append") {
        tng_trajectory_t trajectory = nullptr;
        CHECK(tng_util_trajectory_open(TESTFILE, 'r', &trajectory) == TNG_SUCCESS);
        CHECK(tng_file_headers_read(trajectory, TNG_USE_HASH) == TNG_SUCCESS);

        char buffer[1024] = {0};
        CHECK(tng_first_program_name_get(trajectory, buffer, 1024) == TNG_SUCCESS);
        CHECK(std::string(buffer) == "");

        memset(buffer, 0, 1024);
        CHECK(tng_last_program_name_get(trajectory, buffer, 1024) == TNG_SUCCESS);
        CHECK(std::string(buffer) == "chemfiles");

        CHECK(tng_util_trajectory_close(&trajectory) == TNG_SUCCESS);
        fs::remove(TESTFILE);
    }
}
