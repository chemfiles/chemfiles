// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_BASIC_FILES_HPP
#define CHEMFILES_BASIC_FILES_HPP

#include <fstream>
#include "chemfiles/File.hpp"

namespace chemfiles {

/// Basic text file, only a thin wrapper on top of standard C++ fstreams.
class BasicFile final: public TextFile {
public:
    /// Open a text file with name `filename` and mode `mode`.
    ///
    /// An `FileError` exception is thrown if the file does not exists in
    /// `'r'` or `'a'` mode.
    BasicFile(const std::string& filename, File::Mode mode);

    std::string readline() override;
    std::vector<std::string> readlines(size_t n) override;
    void rewind() override;
    bool eof() override;
private:
    std::fstream stream_;
};

} // namespace chemfiles

#endif
