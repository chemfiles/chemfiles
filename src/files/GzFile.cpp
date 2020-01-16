// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <zconf.h>
#include <zlib.h>

#include <cstdio>
#include <cstdint>
#include <limits>
#include <string>

#include "chemfiles/File.hpp"
#include "chemfiles/files/GzFile.hpp"

#include "chemfiles/error_fmt.hpp"
#include "chemfiles/string_view.hpp"

using namespace chemfiles;

static unsigned checked_cast(size_t value) {
    if (value < std::numeric_limits<unsigned>::max()) {
        return static_cast<unsigned>(value);
    } else {
        throw file_error("{} is too big for unsigned in call to zlib function", value);
    }
}

GzFile::GzFile(const std::string& path, File::Mode mode): TextFileImpl(path) {
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

void GzFile::write(const char* data, size_t count) {
    auto actual = gzwrite(file_, data, checked_cast(count));
    auto error = check_error();
    if (actual == 0 || error != nullptr) {
        throw file_error("error while writting to gziped file: {}", error);
    }
    if (static_cast<size_t>(actual) != count) {
        throw file_error("could not write data to the file at '{}'", this->path());
    }
}

const char* GzFile::check_error() const {
    int status = Z_OK;
    auto message = gzerror(file_, &status);
    return status == Z_OK ? nullptr : message;
}

void GzFile::clear() noexcept {
    gzclearerr(file_);
}

void GzFile::seek(uint64_t position) {
    static_assert(
        sizeof(uint64_t) == sizeof(z_off64_t),
        "uint64_t and z_off64_t do not have the same size"
    );
    auto status = gzseek64(file_, static_cast<z_off64_t>(position), SEEK_SET);
    if (status == -1) {
        auto message = check_error();
        throw file_error("error while seeking gziped file: {}", message);
    }
}


// Taken from https://www.cocoanetics.com/2012/02/decompressing-files-into-memory/
std::vector<char> chemfiles::gzinflate_in_place(char* src, size_t size) {
    size_t dataLength = size;
    size_t halfLength = dataLength / 2;

    bool done = false;
    int status;

    z_stream strm;
    strm.next_in = reinterpret_cast<unsigned char*>(src);
    strm.avail_in = dataLength;
    strm.total_out = 0;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;

    // inflateInit2 knows how to deal with gzip format
    status = inflateInit2(&strm, (15+32));
    if (status != Z_OK) {
	    throw file_error("error inflating GZ file: '{}'", strm.msg);
    }

    std::vector<char> dst(20480);

    do {
	    // extend decompressed if too short
	    if (strm.total_out >= dst.size()) {
		    dst.resize(dst.size() + halfLength);
	    }

	    strm.next_out = reinterpret_cast<unsigned char*>(dst.data() + strm.total_out);
    	strm.avail_out = dst.size() - strm.total_out;

    	// Inflate another chunk.
    	status = inflate (&strm, Z_SYNC_FLUSH);

        if (status == Z_STREAM_END) {
		    done = true;
        } else if (status != Z_OK) {
		    inflateEnd(&strm);
            throw file_error("error inflating GZ file: '{}'", strm.msg);
	    }
    } while(!done);

    status = inflateEnd (&strm);
    if (status != Z_OK || !done) {
	    throw file_error("error inflating GZ file: '{}'", strm.msg);
    }

    // set actual length, shrinking the vector is
    // very efficient in C++11!
    dst.resize(strm.total_out);
    return dst;
}
