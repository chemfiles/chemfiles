/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_FILES_HPP
#define CHEMFILES_FILES_HPP

#include "chemfiles/Error.hpp"

#include <string>
#include <vector>
#include <ostream>
#include <fstream>

namespace chemfiles {

/*!
* @class File File.hpp File.cpp
* @brief Abstract base class for file representation.
*/
class File {
public:
    virtual ~File() = default;

    // Removing default copy constructors
    File(File const&) = delete;
    File& operator=(File const&) = delete;

    //! Is the file opended ?
    virtual bool is_open() = 0;
    //! Sync any content in the underlying buffer to the disk
    virtual void sync() = 0;
    //! File name, i.e. complete path to this file on disk.
    const std::string& filename() const {return _filename;}
    //! File opening mode.
    const std::string& mode() const {return _mode;}
protected:
    File(const std::string& path, const std::string& mode) : _filename(path), _mode(mode) {}
private:
    const std::string _filename;
    const std::string _mode;
};

/*!
 * @class TextFile File.hpp File.cpp
 *
 * Abstract base class representing a text file. This class is inteded to be inherited by
 * any form of text files: compressed files, memory-mapped files, and any other.
 */
class TextFile : public File, public std::ostream {
public:
    virtual ~TextFile() = default;

    //! Read a line from the file
    virtual const std::string& getline() = 0;
    //! Read a line from the file, stream version
    virtual TextFile& operator>>(std::string& line) = 0;
    //! Read \c n lines from the file
    virtual const std::vector<std::string>& readlines(size_t n) = 0;
    //! Reset the file cursor
    virtual void rewind() = 0;
    //! Number of lines in the file
    virtual size_t nlines() = 0;
    //! Are we at the end of the file ?
    virtual bool eof() = 0;

    //! Write any data to the file in stream version
    using std::ostream::operator<<;
    //! Write a string to the file
    virtual void writeline(const std::string&) = 0;
    //! Write a vector of lines to the file
    virtual void writelines(const std::vector<std::string>&) = 0;

    //! Set the underlying buffer. This is needed in order to make operator<<
    //! work.
    using std::ostream::rdbuf;

    //! Needed for resolving the overload ambiguity when using const char[] or
    //! const char* arguments.
    TextFile& operator<<(const char* val) {
        *this << std::string(val);
        return *this;
    }
protected:
    explicit TextFile(const std::string& path, const std::string& mode) : File(path, mode), std::ostream(nullptr) {}
};

/*!
* @class BinaryFile File.hpp
* @brief Abstract base class for binary files representation
*
* Because the binary formats can be everything, this class does not provides any
* of the usual methods for working with streams, and is not intended to be
* instanciated, but rather to serve as a base class for all the binary
* file classes.
*/
class BinaryFile : public File {
public:
    virtual ~BinaryFile() = default;
protected:
    explicit BinaryFile(const std::string& path, const std::string& mode) : File(path, mode) {}
};

} // namespace chemfiles

#endif
