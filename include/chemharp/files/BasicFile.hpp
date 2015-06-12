/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_BASIC_FILES_HPP
#define HARP_BASIC_FILES_HPP

#include "chemharp/File.hpp"

namespace harp {

/*!
 * @class BasicFile BasicFile.hpp BasicFile.cpp
 * @brief Basic text file
 *
 * This is only a thin wrapper on top of standard C++ streams. It can be updated
 * later to use directly the C functions.
 */
class BasicFile : public TextFile {
public:
    explicit BasicFile(const std::string& filename, const std::string& mode = "r");
    ~BasicFile();

    virtual const std::string& getline() override;
    virtual BasicFile& operator>>(std::string& line) override;
    virtual const std::vector<std::string>& readlines(size_t n) override;

    virtual void rewind() override{
        stream.clear();
        stream.seekg(0, std::ios::beg);
    }
    virtual size_t nlines() override;

    virtual bool is_open() override {return stream.is_open();}
    virtual void close() override {return stream.close();}
    virtual bool eof() override {return stream.eof();}

    virtual BasicFile& operator<<(const any&) override;
    virtual void writeline(const std::string&) override;
    virtual void writelines(const std::vector<std::string>&) override;
private:
    std::fstream stream;
    // Caching a vector of strings
    std::vector<std::string> lines;
};

} // namespace harp

#endif
