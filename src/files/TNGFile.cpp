// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/files/TNGFile.hpp"

#include "chemfiles/Error.hpp"
#include "chemfiles/utils.hpp"
using namespace chemfiles;

#define STRING_0(x) #x
#define STRING(x) STRING_0(x)
#define CHECK(x) check_tng_error((x), (STRING(x)))

TNGFile::TNGFile(std::string filename, File::Mode mode): File(std::move(filename), mode), handle_(nullptr) {
    CHECK(tng_util_trajectory_open(this->filename().c_str(), mode, &handle_));

    if (mode == File::READ) {
        CHECK(tng_file_headers_read(handle_, TNG_USE_HASH));
    }

    if (mode == File::WRITE || mode == File::APPEND) {
        CHECK(tng_last_program_name_set(handle_, "chemfiles"));
        auto user = user_name();
        CHECK(tng_last_user_name_set(handle_, user.c_str()));
        auto host = hostname();
        CHECK(tng_last_computer_name_set(handle_, host.c_str()));
        // TODO: last pgp signature?

        if (mode == File::WRITE) {
            CHECK(tng_first_program_name_set(handle_, "chemfiles"));
            CHECK(tng_first_user_name_set(handle_, user.c_str()));
            CHECK(tng_first_computer_name_set(handle_, host.c_str()));
            // TODO: first pgp signature?
        }

        CHECK(tng_file_headers_write(handle_, TNG_USE_HASH));
    }
}

TNGFile::~TNGFile() noexcept {
    tng_util_trajectory_close(&handle_);
}

void chemfiles::check_tng_error(tng_function_status status, const std::string& function) {
    switch (status) {
    case TNG_FAILURE:
        throw chemfiles::FileError(
            "Error while calling " + function + " in the TNG library"
        );
    case TNG_CRITICAL:
        throw chemfiles::FileError(
            "Critical error while calling " + function + " in the TNG library"
        );
    case TNG_SUCCESS:
        // Do nothing, this is good
        break;
    default:
        throw chemfiles::FileError(
            "Unknown status code from TNG library: "
            + std::to_string(static_cast<unsigned>(status))
        );
        break;
    }
}
