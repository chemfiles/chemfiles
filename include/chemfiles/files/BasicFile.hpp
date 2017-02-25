// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

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
    ~BasicFile() noexcept {}

    const std::string& readline() override;
    const std::vector<std::string>& readlines(size_t n) override;

    void rewind() override;
    bool eof() override;
private:
    std::fstream stream_;
    // Caching a vector of strings
    std::vector<std::string> lines_;
};

} // namespace chemfiles

#endif
