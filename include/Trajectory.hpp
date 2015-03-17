/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_IO_HPP
#define HARP_IO_HPP

#include <memory>
#include <string>

#include "Frame.hpp"

namespace harp {

class File;
class Format;

/*!
* @class Trajectory Trajectory.hpp Trajectory.cpp
* @brief Jonction of a Format and a File.
*
* The Trajectory class puts together a format and a file, and implement the main
* read/write operations.
*
* The returned frame is cached, and a reference to it is used as return value.
* This means that any call to read_at_step or read_next_step will invalidate any
* previously used frame.
*/
class Trajectory {
public:
    /*!
    * @brief Open a trajectory file
    * @param filename The file path
    * @param format Specific format to use. Needed when there is no way to guess
    *               the format from the extension of the file.
    * @param mode Opening mode for the file. Default mode is "r" for read.
    *
    * Open a file, automatically gessing the file format and type from the
    * extension.
    */
    //!
    Trajectory(const std::string& filename, const std::string& mode = "r", const std::string& format = "");
    Trajectory(Trajectory&&);
    Trajectory& operator=(Trajectory&&);
    ~Trajectory();

    //! Read operator, in *stream* version
    Trajectory& operator>>(Frame& frame);
    //! Read operator, in *method* version
    Frame& read_next_step();
    //! Read operator, in *method* version with specific step
    Frame& read_at_step(const size_t);

    //! Write operator, in *stream* version
    Trajectory& operator<<(const Frame& frame);
    //! Write operator, in *method* version
    void write_step(Frame& frame);
private:
    //! Cache a frame, as it can get very heavy
    Frame _frame;
    //! Used format
    std::unique_ptr<Format> _format;
    //! File, to be shared with the format.
    std::unique_ptr<File> _file;
};

} // namespace harp

#endif
