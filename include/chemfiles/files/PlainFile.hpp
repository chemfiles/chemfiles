// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_PLAIN_FILES_HPP
#define CHEMFILES_PLAIN_FILES_HPP

#include <cstdint>
#include <cstdio>
#include <string>

#include "chemfiles/File.hpp"

namespace chemfiles {

/// Simple TextFileImpl reading plain, uncompressed files using `FILE*`.
class PlainFile final: public TextFileImpl {
public:
    /// Open a text file with name `filename` and mode `mode`.
    PlainFile(const std::string& path, File::Mode mode);
    ~PlainFile() override;

    size_t read(char* data, size_t count) override;
    void write(const char* data, size_t count) override;

    void clear() noexcept override;
    void seek(uint64_t position) override;

private:
    std::FILE* file_;
};

} // namespace chemfiles

#endif
