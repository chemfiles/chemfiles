// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/files/MemoryFile.hpp"
#include "chemfiles/files/GzFile.hpp"
#include "chemfiles/files/XzFile.hpp"
#include "chemfiles/files/Bz2File.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/unreachable.hpp"

using namespace chemfiles;

std::streampos MemoryFileReader::vector_buffer_reader::seekpos(std::streampos sp, std::ios_base::openmode) {
    const auto pos = static_cast<off_type>(sp);
    auto ret = pos_type(off_type(-1));
    const auto* beg = this->eback();

    if (beg && pos >= 0 && pos <= this->egptr() - beg) {
        this->setg(this->eback(), this->eback() + pos, this->egptr());
        ret = sp;
    }

    return ret;
}

std::vector<char> MemoryFileReader::wrap(const char* src, size_t size, File::Compression compression) {
    switch(compression) {
    case File::GZIP:
        return gzinflate_in_place(src, size);
    case File::LZMA:
        return xzinflate_in_place(src, size);
    case File::BZIP2:
        return bz2inflate_in_place(src, size);
    case File::DEFAULT:
        return std::vector<char>(src, src + size);
    }
    unreachable();
}

void MemoryFileReader::clear() noexcept {
    data_.clear();
}

void MemoryFileReader::seek(uint64_t position) {

    data_.seekg(static_cast<std::streamoff>(position));

    if (data_.fail()) {
        throw file_error("error while seeking memory file");
    }
}

size_t MemoryFileReader::read(char* data, size_t count) {

    auto result = data_.readsome(data, static_cast<std::streamoff>(count));


    if (!data_) {
        throw file_error("IO error while reading the memory file");
    }

    return static_cast<size_t>(result);
}

void MemoryFileReader::write(const char*, size_t) {
    throw file_error("cannot write to a memory file for reading");
}

std::streampos MemoryFileWriter::vector_buffer_writer::seekpos(std::streampos sp, std::ios_base::openmode) {
    const auto pos = static_cast<off_type>(sp);
    auto ret = pos_type(off_type(-1));
    const auto* beg = this->pbase();

    if (beg && pos >= 0 && pos <= this->epptr() - beg) {
        this->setp(this->pbase() + pos, this->epptr());
        ret = sp;
    }

    return ret;
}

MemoryFileWriter::vector_buffer_writer::int_type MemoryFileWriter::vector_buffer_writer::overflow(int_type ch) {
    auto current_postion = this->epptr() - this->pbase();
    original_vector_.grow(); // TODO: Improve this.

    auto& memory = original_vector_.write_memory();
    setp(memory.data(), memory.data() + memory.size());
    this->pbump(static_cast<int>(current_postion));

    *this->pptr() = std::char_traits<char>::to_char_type(ch);
    this->pbump(1);

    return ch;
}

void MemoryFileWriter::clear() noexcept {
    data_.clear();
}

void MemoryFileWriter::seek(uint64_t position) {
    data_.seekp(static_cast<std::streamoff>(position));

    if (data_.fail()) {
        throw file_error("error while seeking memory file");
    }
}

size_t MemoryFileWriter::read(char*, size_t) {
    throw file_error("cannot read memory file for writing");
}

void MemoryFileWriter::write(const char* data, size_t count) {
    data_.write(data, static_cast<std::streamoff>(count));

    if (data_.fail()) {
        throw file_error("could not write data to the memory file");
    }
}
