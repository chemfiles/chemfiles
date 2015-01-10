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

#include "File.hpp"
#include "Frame.hpp"

namespace harp {

/*!
* @class HarpIO HarpIO.hpp HarpIO.cpp
* @brief Abstract base class for file IO
*
*
*/
template <class FormatClass>
class HarpIO {
public:

protected:
    //! The file to read
    File file;
    //! The file format
    FormatClass format;
    //! The current step
    int current_step;
private:
};

//! Open a file, automatically gessing the file format and type from the extension.
HarpIO harp_open(std::string filename, std::string mode = "r");

/*!
* @class Reader<Format> HarpIO.hpp HarpIO.cpp
* @brief The Reader class read a file using a specific Format
*/
template <class FormatReaderClass>
class Reader : public HarpIO<FormatReaderClass> {
public:
    //! Read a specific step
    void read_at_step(int);
    //! Read the next step
    void read_next_step();
    //! Read the next step
    inline Frame& frame() const {
        return frame;
    };
private:
    //! The total number of steps in the file
    int n_steps;
    //! The last read frame
    Frame curr_frame;
};

/*!
* @class Writer<Format> HarpIO.hpp HarpIO.cpp
* @brief The Writer class write frames to a file using a specific Format
*/
template <class FormatWriterClass>
class Writer : public HarpIO<FormatWriterClass> {
public:
    //! Write a frame
    void write_step(Frame& frame);
private:
};

} // namespace harp

#endif
