// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_PLAIN_FILES_HPP
#define CHEMFILES_PLAIN_FILES_HPP

#include <fstream>
#include "chemfiles/File.hpp"

namespace chemfiles {

/// Simple TextFile implementation, only a thin wrapper on top of standard C++
/// fstreams. Reads plain, uncompressed files
class PlainFile final: public TextFile {
public:
    /// Open a text file with name `filename` and mode `mode`.
    ///
    /// An `FileError` exception is thrown if the file does not exists in
    /// `'r'` or `'a'` mode.
    PlainFile(const std::string& filename, File::Mode mode);

private:
    std::filebuf buffer_;
};

} // namespace chemfiles

#endif
