// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/files/MemoryBuffer.hpp"

#include "chemfiles/files/GzFile.hpp"
#include "chemfiles/files/XzFile.hpp"
#include "chemfiles/files/Bz2File.hpp"

#include "chemfiles/unreachable.hpp"
#include "chemfiles/error_fmt.hpp"

using namespace chemfiles;

void MemoryBuffer::decompress(File::Compression compression) {
    switch(compression) {
    case File::GZIP:
        write_mem_ = gzinflate_in_place(this->data(), this->size());
        begin_ = write_mem_.data();
        size_ = write_mem_.size();
        break;
    case File::LZMA:
        write_mem_ = xzinflate_in_place(this->data(), this->size());
        begin_ = write_mem_.data();
        size_ = write_mem_.size();
        break;
    case File::BZIP2:
        write_mem_ = bz2inflate_in_place(this->data(), this->size());
        begin_ = write_mem_.data();
        size_ = write_mem_.size();
        break;
    case File::DEFAULT:
        // nothing to do
        break;
    default:
        unreachable();
    }
}
