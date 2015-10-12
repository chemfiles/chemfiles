/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_BASIC_FILES_HPP
#define CHEMFILES_BASIC_FILES_HPP

#include <fstream>
#include "chemfiles/File.hpp"

namespace chemfiles {

/*!
 * @class BasicFile BasicFile.hpp BasicFile.cpp
 *
 * Basic text file, only a thin wrapper on top of standard C++ fstreams.
 */
class BasicFile : public TextFile {
public:
    /*!
     * Open a text file.
     *
     * @param filename The file path. In \c "w" or \c "a" modes, the file is
     *                 created if it does not exist yet. In "r" mode, and
     *                 exception is throwed is the file does not exist yet.
     * @param mode Opening mode for the file. Supported modes are "r" for read,
     *             "w" for write, and "a" for append. "w" mode discard any
     *             previously existing file.
     *
     */
    explicit BasicFile(const std::string& filename, const std::string& mode);

    virtual const std::string& getline() override;
    virtual BasicFile& operator>>(std::string& line) override;
    virtual const std::vector<std::string>& readlines(size_t n) override;

    virtual void rewind() override{
        stream.clear();
        stream.seekg(0, std::ios::beg);
    }
    virtual size_t nlines() override;

    virtual bool is_open() override {return stream.is_open();}
    virtual bool eof() override {return stream.eof();}

    virtual void sync() override {
        std::iostream::sync();
        std::iostream::flush();
    }

    virtual void writeline(const std::string&) override;
    virtual void writelines(const std::vector<std::string>&) override;
private:
    std::fstream stream;
    // Caching a vector of strings
    std::vector<std::string> lines;
};

} // namespace chemfiles

#endif
