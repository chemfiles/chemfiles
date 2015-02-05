/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_GZIP_FILES_HPP
#define HARP_GZIP_FILES_HPP

#include "files/File.hpp"

namespace harp {

/*!
 * @class GzipFile GzipFile.hpp GzipFile.cpp
 * @brief GZip compressed file
 *
 */
class GzipFile : public TextFile {
public:
    explicit GzipFile(std::string filename);
    ~GzipFile();

    //! Read a line from the file
    const std::string& getline(void);
    //! Read a line from the file, stream version
    GzipFile& operator>>(std::string& line);
    //! Read \c n lines from the file
    const std::vector<std::string>& readlines(size_t n);
    //! Reset the file cursor
    void rewind();
    //! Number of lines in the file
    size_t nlines();

    bool is_open(void);
    void close(void);

    //! Write a line to the file
    void writeline(const std::string& line);
    //! Write a line to the file, stream version
    GzipFile& operator<<(const std::string& line);
    //! Read \c n lines from the file
    void writelines(const std::vector<std::string>& lines);
private:
};

} // namespace harp

#endif
