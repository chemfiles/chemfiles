// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_GZ_FILES_HPP
#define CHEMFILES_GZ_FILES_HPP

#include <cstdint>
#include <string>

#include "chemfiles/File.hpp"

typedef struct gzFile_s *gzFile;

namespace chemfiles {

/// An implementation of TextFile for gzip files
class GzFile final: public TextFileImpl {
public:
    /// Open a text file with name `filename` and mode `mode`.
    GzFile(const std::string& path, File::Mode mode);
    ~GzFile() override;

    size_t read(char* data, size_t count) override;
    void write(const char* data, size_t count) override;

    void clear() noexcept override;
    void seek(uint64_t position) override;

private:
    /// Check if any error happened while reading/writing the file. Returns the
    /// corresponding error message or `nullptr` if no error occured.
    const char* check_error() const;

    gzFile file_ = nullptr;
};

}

#endif
