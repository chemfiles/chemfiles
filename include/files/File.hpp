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

#include "Error.hpp"

#include <string>
#include <vector>
#include <fstream>

namespace harp {

/*!
* @class File File.hpp File.cpp
* @brief File base abstraction
*/
class File {
public:
    // Providing a constructor.
    File(){}
    virtual ~File(){};

    // Removing default copy constructors
    File(File const&) = delete;
    File& operator=(File const&) = delete;
    // Removing default comparison operator
    bool operator==(File const&) = delete;
    bool operator!=(File const&) = delete;

    virtual bool is_open(void) = 0;
};

/*!
 * @class TextFile File.hpp File.cpp
 * @brief Text file abstraction
 *
 * This is only a thin wrapper around the C++ stream. This class is inteded to
 * be heritated by any form of text files: compressed files, memory-mapped files,
 * and any other.
 */
class TextFile : public File{
public:
    explicit TextFile(std::string filename);
    ~TextFile();

    //! Read a line from the file
    virtual const std::string& getline(void);
    //! Read a line from the file, stream version
    TextFile& operator>>(std::string& line);
    //! Read \c n lines from the file
    virtual const std::vector<std::string>& readlines(int n);
    //! Reset the file cursor
    virtual inline void rewind(){
        stream.seekg(0);
        stream.clear();
    }
    //! Number of lines in the file
    virtual int nlines();

    virtual inline bool is_open(void) {return stream.is_open();}
    virtual inline void close(void) {return stream.close();}

    //! Write a line to the file
    virtual void writeline(const std::string& line);
    //! Write a line to the file, stream version
    TextFile& operator<<(const std::string& line);
    //! Read \c n lines from the file
    virtual void writelines(const std::vector<std::string>& lines);
private:
    std::fstream stream;
    // Caching a vector of strings
    std::vector<std::string> lines;
};

/*!
* @class BinaryFile File.hpp File.cpp
* @brief Binary file abstraction class.
*
* Because the binary formats can be everything, this class does not provides any
* of the usual methods for working with streams, and is not intended to be
* instanciated directly, but rather to serve as a base class for all the binary
* file operations.
*/
class BinaryFile : public File{
public:
    virtual ~BinaryFile();
private:
};

// TODO: class NetDCFFile : public BinaryFile

} // namespace harp

#endif
