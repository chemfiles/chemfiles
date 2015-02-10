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

#include <memory>

#include "Frame.hpp"

namespace harp {

class File;
class Format;

/*!
* @class HarpIO HarpIO.hpp HarpIO.cpp
* @brief Jonction of Format and File.
*
* The HarpIO class puts together a format and a file, and implement the main
* read/write operations
*/
class HarpIO {
public:
    virtual ~HarpIO() = 0;

    //! Read operator, in *stream* version
    virtual HarpIO& operator>>(Frame& frame);
    //! Read operator, in *method* version
    virtual Frame& read_next_step();
    //! Read operator, in *method* version with specific step
    virtual Frame& read_at_step(size_t);

    //! Write operator, in *stream* version
    virtual HarpIO& operator<<(const Frame& frame);
    //! Write operator, in *method* version
    virtual void write_step(Frame& frame);
protected:
    bool read_mode;
    bool write_mode;

    Format* _format;
    std::unique_ptr<File> _file;
};

} // namespace harp

#endif
