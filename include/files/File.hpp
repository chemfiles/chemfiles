/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_FILES_HPP
#define HARP_FILES_HPP

#include <string>
#include <vector>

namespace harp {

/*!
* @class File File.hpp File.cpp
* @brief File base abstraction
*/
class File {
public:
    virtual ~File();

    // Removing default copy constructors
    File(File const&) = delete;
    File& operator=(File const&) = delete;
    // Removing default comparison operator
    bool operator==(File const&) = delete;
    bool operator!=(File const&) = delete;
};

/*!
 * @class TextFile File.hpp File.cpp
 * @brief Text file abstraction
 */
class TextFile : public File{
public:
    ~TextFile();

    //! Read a line from the file
    inline std::string& readline();
    //! Read \c n lines from the file
    inline std::vector<std::string>& readlines(int n);
    //! Reset the file cursor
    inline void rewind();
    //! Number of lines in the file
    inline int nlines();
private:
};

/*!
* @class TextFile File.hpp File.cpp
* @brief C text file abstraction class
*/
class BinaryFile : public File{
public:
    virtual ~BinaryFile();
private:
};

// using the Boost.Iostreams lib
// TODO: class GzipedFile : public TextFile
// TODO: class MappedFile : public TextFile

// TODO: class NetDCFFile : public BinaryFile

} // namespace harp

#endif
