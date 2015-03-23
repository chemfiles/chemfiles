/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_MAPPED_FILES_HPP
#define HARP_MAPPED_FILES_HPP

#include "files/File.hpp"

namespace harp {

/*!
 * @class MappedFile MappedFile.hpp MappedFile.cpp
 * @brief Memory-mapped file
 *
 */
class MappedFile : public TextFile {
public:
    explicit MappedFile(const std::string& filename, const std::string&);
    ~MappedFile();

    virtual const std::string& getline(void);
    virtual MappedFile& operator>>(std::string& line);
    virtual const std::vector<std::string>& readlines(size_t n);

    virtual void rewind();
    virtual size_t nlines();

    virtual bool is_open(void);
    virtual void close(void);

    virtual void writeline(const std::string& line);
    virtual MappedFile& operator<<(const any& line);
    virtual void writelines(const std::vector<std::string>& lines);
private:
};

} // namespace harp

#endif
