// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <zconf.h>
#include <zlib.h>

#include <limits>
#include <string>

#include "chemfiles/File.hpp"
#include "chemfiles/files/GzFile.hpp"
#include "chemfiles/ErrorFmt.hpp"

using namespace chemfiles;

static unsigned checked_cast(size_t value) {
    if (value < std::numeric_limits<unsigned>::max()) {
        return static_cast<unsigned>(value);
    } else {
        throw file_error("{} is too big for unsigned in call to zlib function", value);
    }
}

GzFile::GzFile(const std::string& path, File::Mode mode) {
    const char* openmode;
    switch (mode) {
    case File::READ:
        openmode = "rb";
        break;
    case File::WRITE:
        openmode = "wb7";
        break;
    case File::APPEND:
        throw file_error("appending (open mode 'a') is not supported with gziped files");
    }

    file_ = gzopen64(path.c_str(), openmode);
    if (file_ == nullptr) {
        throw file_error("could not open the file at '{}'", path);
    }
}

GzFile::~GzFile() {
    if (file_ != nullptr) {
        gzclose(file_);
    }
}

size_t GzFile::read(char* data, size_t count) {
    auto read = gzread(file_, data, checked_cast(count));
    auto error = check_error();
    if (read == -1 || error != nullptr) {
        throw file_error("error while reading gziped file: {}", error);
    }
    return static_cast<size_t>(read);
}

size_t GzFile::write(const char* data, size_t count) {
    auto written = gzwrite(file_, data, checked_cast(count));
    auto error = check_error();
    if (written == 0 || error != nullptr) {
        throw file_error("error while writting to gziped file: {}", error);
    }
    return static_cast<size_t>(written);
}

const char* GzFile::check_error() const {
    int status = Z_OK;
    auto message = gzerror(file_, &status);
    return status == Z_OK ? nullptr : message;
}

void GzFile::clear() {
    gzclearerr(file_);
}

void GzFile::seek(int64_t position) {
    static_assert(
        sizeof(int64_t) == sizeof(z_off64_t),
        "int64_t and z_off64_t do not have the same size"
    );
    auto status = gzseek64(file_, position, SEEK_SET);
    if (status == -1) {
        auto message = check_error();
        throw file_error("error while seeking gziped file: {}", message);
    }
}
