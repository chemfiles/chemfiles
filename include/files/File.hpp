/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
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
    explicit File(const std::string& path) : filename(path) {}
    virtual ~File(){};

    // Removing default copy constructors
    File(File const&) = delete;
    File& operator=(File const&) = delete;
    // Removing default comparison operator
    bool operator==(File const&) = delete;
    bool operator!=(File const&) = delete;

    //! Is the file opended ?
    virtual bool is_open(void) = 0;
    //! Close the file before the destructor call.
    virtual void close(void) = 0;
    //! File name
    const std::string& name() const {return filename;};
protected:
    const std::string filename;
};

/*!
 * @class TextFile File.hpp File.cpp
 * @brief Text file abstraction
 *
 * This class is inteded to be inherited by any form of text files: compressed
 * files, memory-mapped files, and any other.
 */
class TextFile : public File {
public:
    explicit TextFile(const std::string& path, const std::string& = "") : File(path) {}
    virtual ~TextFile() {};

    //! Read a line from the file
    virtual const std::string& getline(void) = 0;
    //! Read a line from the file, stream version
    virtual TextFile& operator>>(std::string& line) = 0;
    //! Read \c n lines from the file
    virtual const std::vector<std::string>& readlines(size_t n) = 0;
    //! Reset the file cursor
    virtual inline void rewind() = 0;
    //! Number of lines in the file
    virtual size_t nlines() = 0;

    //! Write a line to the file
    virtual void writeline(const std::string& line) = 0;
    //! Write a line to the file, stream version
    virtual TextFile& operator<<(const std::string& line) = 0;
    //! Read \c n lines to the file
    virtual void writelines(const std::vector<std::string>& lines) = 0;
};

/*!
 * @class BasicFile File.hpp File.cpp
 * @brief Basic text file
 *
 * This is only a thin wrapper on top of standard C++ streams. It can be updated
 * later to use directly
 */
class BasicFile : public TextFile {
public:
    explicit BasicFile(const std::string& filename, const std::string& mode = "r");
    ~BasicFile();

    const std::string& getline(void);
    BasicFile& operator>>(std::string& line);
    const std::vector<std::string>& readlines(size_t n);
    inline void rewind(){
        stream.seekg(0);
        stream.clear();
    }
    size_t nlines();

    inline bool is_open(void) {return stream.is_open();}
    inline void close(void) {return stream.close();}

    void writeline(const std::string& line);
    BasicFile& operator<<(const std::string& line);
    void writelines(const std::vector<std::string>& lines);
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
class BinaryFile : public File {
public:
    explicit BinaryFile(const std::string& path, const std::string& = "") : File(path) {}
};

} // namespace harp

#endif
