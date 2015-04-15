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
     * Open a file, automatically gessing the file format and type from the
     * extension.
     *
     * @param filename The file path. In \c "w" or \c "wa" modes, the file is
     *                 created if it does not exist yet. In "r" mode, and
     *                 exception is throwed is the file does not exist yet.
     * @param format Specific format to use. Needed when there is no way to guess
     *               the format from the extension of the file, or when this guess
     *               would be wrong.
     * @param mode Opening mode for the file. Default mode is "r" for read. Other
     *             supported modes depends on the underlying format and are "w"
     *             for write, and "a" for append.
     */
    //!
    Trajectory(const std::string& filename, const std::string& mode = "r", const std::string& format = "");
    Trajectory(Trajectory&&);
    Trajectory& operator=(Trajectory&&);
    ~Trajectory();

    //! Read operator, in *stream* version
    Trajectory& operator>>(Frame& frame);
    //! Read operator, in *method* version
    Frame read_next_step();
    //! Read operator, in *method* version with specific step
    Frame read_at_step(const size_t);
    //! Close a trajectory
    void close();

    //! Write operator, in *stream* version
    Trajectory& operator<<(const Frame& frame);
    //! Write operator, in *method* version
    void write_step(const Frame& frame);

    //! Get the number of steps (the number of Frames) in this trajectory
    size_t nsteps() {return _nsteps;}
    //! Have we read all the Frames in this file ?
    bool done();
private:
    //! Current step
    size_t _step;
    //! Number of steps in the file, if available
    size_t _nsteps;
    //! TODO
    std::unique_ptr<Topology> _topology;
    //! Format used to read the file
    std::unique_ptr<Format> _format;
    //! The file we are reading from
    std::unique_ptr<File> _file;
};

} // namespace harp

#endif
