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

#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/mpl/vector.hpp>


namespace harp {
namespace bte = boost::type_erasure;
namespace mpl = boost::mpl;
typedef bte::any<mpl::vector<bte::ostreamable<>,
                             bte::copy_constructible<>>> any;

/*!
* @class File File.hpp File.cpp
* @brief File base abstraction
*/
class File {
public:
    virtual ~File(){}

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
    const std::string& filename() const {return _filename;}
protected:
    explicit File(const std::string& path) : _filename(path) {}
private:
    const std::string _filename;
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
    virtual ~TextFile() {}

    //! Read a line from the file
    virtual const std::string& getline(void) = 0;
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
    virtual TextFile& operator<<(const any&) = 0;
    //! Write a string to the file
    virtual void writeline(const std::string&) = 0;
    //! Write a vector of lines to the file
    virtual void writelines(const std::vector<std::string>&) = 0;
protected:
    explicit TextFile(const std::string& path, const std::string& = "") : File(path) {}
};

/*!
* @class BinaryFile File.hpp
* @brief Binary file abstraction class.
*
* Because the binary formats can be everything, this class does not provides any
* of the usual methods for working with streams, and is not intended to be
* instanciated directly, but rather to serve as a base class for all the binary
* file operations.
*/
class BinaryFile : public File {
protected:
    explicit BinaryFile(const std::string& path, const std::string& = "") : File(path) {}
};

} // namespace harp

#endif
