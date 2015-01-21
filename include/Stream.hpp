/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_IO_HPP
#define HARP_IO_HPP

#include "Frame.hpp"

namespace harp {

/*!
* @class Stream Stream.hpp Stream.cpp
* @brief Jonction of Format and File.
*
* The Stream class puts together a format and a file, and implement the main
* read/write operations
*/
class Stream {
public:
    virtual ~Stream() = 0;

    //! Read operator, stream form
    virtual Stream& operator>>(Frame& frame);
    //! Read operator, method form
    virtual Frame& read_next_step();
    //! Read operator, method form with specific step
    virtual Frame& read_at_step(int);

    //! Write operator, stream form
    virtual Stream& operator<<(const Frame& frame);
    //! Write operator, method form
    virtual void write_step(Frame& frame);
protected:
    bool read_mode;
    bool write_mode;
};

/*!
* @class Reader<File, Format> Stream.hpp Stream.cpp
* @brief The Reader class read a file using a specific Format
*/
template <class File, class Format>
class Reader : public Stream, private File, private Format {
public:
    explicit Reader(std::string filename);
    ~Reader();

    Reader& operator>>(Frame& frame);
    Frame& read_next_step();
    Frame& read_at_step(int);
private:
    //! The current step
    int current_step;
    //! The total number of steps in the file
    int n_steps;
    //! The last read frame
    Frame curr_frame;
private:
};

/*!
* @class Writer<Format> Stream.hpp Stream.cpp
* @brief The Writer class write frames to a file using a specific Format
*/
template <class File, class Format>
class Writer : public Stream, private File, private Format {
public:
    explicit Writer(std::string filename);
    ~Writer();

    //! Write operator, stream form
    Writer& operator<<(const Frame& frame);
    //! Write operator, method form
    void write_step(const Frame& frame);
private:
};

} // namespace harp

#endif
