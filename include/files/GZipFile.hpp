/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
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
    explicit GzipFile(const std::string& filename, const std::string&);
    ~GzipFile();

    virtual const std::string& getline(void);
    virtual GzipFile& operator>>(std::string& line);
    virtual const std::vector<std::string>& readlines(size_t n);

    virtual void rewind();
    virtual size_t nlines();

    virtual bool is_open(void);
    virtual void close(void);

    virtual void writeline(const std::string& line);
    virtual GzipFile& operator<<(const any& line);
    virtual void writelines(const std::vector<std::string>& lines);
private:
};

} // namespace harp

#endif
