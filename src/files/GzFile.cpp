// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/files/GzFile.hpp"
#include "chemfiles/ErrorFmt.hpp"
using namespace chemfiles;


gzstreambuf::gzstreambuf(size_t buffer_size): buffer_(buffer_size) {
    auto end = &buffer_.back() + 1;
    setg(end, end, end);

    auto begin = &buffer_.front();
    setp(begin, end - 1);
}

gzstreambuf::~gzstreambuf() {
    sync();
    gzclose(file_);
    file_ = nullptr;
}

void gzstreambuf::open(const std::string& path, const std::string& mode) {
    if (is_open()) {
        throw file_error("can not open a gz file twice with the same gzstreambuf");
    }
    file_ = gzopen(path.c_str(), mode.c_str());
}

gzstreambuf::int_type gzstreambuf::underflow() {
    if (gptr() && (gptr() < egptr())) {
        return traits_type::to_int_type(*gptr());
    }

    if (!is_open()) {
        return traits_type::eof();
    }

    auto bytes = gzread(file_, &buffer_[0], static_cast<unsigned>(buffer_.size()));
    if (bytes <= 0) {
        setg(&buffer_[0], &buffer_[0], &buffer_[0]);
        return traits_type::eof();
    }

    setg(&buffer_[0], &buffer_[0], &buffer_[0] + bytes);

    return traits_type::to_int_type(*gptr());
}

// This function does not seems to be needed by the current implmentation of
// GzFile. I'll keep it around in case the code requires it at some point. It
// should work, but I could not test it.
//
// int gzstreambuf::overflow(int ch) {
//     if (ch != traits_type::eof()) {
//         *pptr() = traits_type::to_char_type(ch);
//         pbump(1);
//         if (sync() == 0) {
//             return ch;
//         }
//     }
//
//     return traits_type::eof();
// }

int gzstreambuf::sync() {
    auto bytes = static_cast<int>(pptr() - pbase());
    if (bytes > 0) {
        if (gzwrite(file_, pbase(), static_cast<unsigned>(bytes)) != bytes) {
            return EOF;
        }
        pbump(-bytes);
    }

    return 0;
}

bool gzstreambuf::is_open() const {
    return file_ != nullptr;
}

std::streampos gzstreambuf::seekoff(std::streamoff offset, std::ios_base::seekdir way, std::ios_base::openmode mode) {
    if (sync() == EOF) {
        return {EOF};
    }

    // fast return path for tellg
    if (offset == 0 && way == std::ios_base::cur) {
        return gztell(file_) - (egptr() - gptr());
    }

    int dir = -1;
    if (way == std::ios_base::cur) {
        dir = SEEK_CUR;
    } else if (way == std::ios_base::beg) {
        dir = SEEK_SET;
    } else if (way == std::ios_base::end) {
        throw file_error("zlib doesn't support SEEK_END in gzseek().");
    }

    auto position = gzseek(file_, static_cast<z_off_t>(offset), dir);
    if (position == -1) {
        throw file_error("internal error in gzseek");
    };

    if (mode & std::ios_base::in) {
        setg(&buffer_.back() + 1, &buffer_.back() + 1, &buffer_.back() + 1);
    } else if (mode & std::ios_base::out) {
        setp(&buffer_.front(), &buffer_.back());
    }

    return position;
}


GzFile::GzFile(std::string path, File::Mode mode)
    : TextFile(std::move(path), mode, File::GZIP, &buffer_), buffer_() {

    std::string openmode;
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

    buffer_.open(this->path(), openmode);
    if (!buffer_.is_open()) {
        throw file_error("could not open the file at '{}'", this->path());
    }
}
