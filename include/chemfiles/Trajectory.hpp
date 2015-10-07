/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_TRAJECTORY_HPP
#define CHEMFILES_TRAJECTORY_HPP

#include <memory>
#include <string>

#include "chemfiles/Frame.hpp"
#include "chemfiles/exports.hpp"

namespace chemfiles {

class File;
class Format;

/*!
* @class Trajectory Trajectory.hpp Trajectory.cpp
* @brief A Trajectory is a chemistry file on the hard drive. It is the main entry point
*        of the chemfiles library.
*
* A Trajectory is made with a File and a Format. The File implements all the physical
* operations, while the Format provides a way to interpret the file.
*/
class CHFL_EXPORT Trajectory {
public:
    /*!
     * Open a file, automatically gessing the file format and type from the
     * extension.
     *
     * @param filename The file path. In \c "w" or \c "a" modes, the file is
     *                 created if it does not exist yet. In "r" mode, and
     *                 exception is throwed is the file does not exist yet.
     * @param format Specific format to use. Needed when there is no way to guess
     *               the format from the extension of the file, or when this guess
     *               would be wrong.
     * @param mode Opening mode for the file. Default mode is "r" for read. Other
     *             supported modes depends on the underlying format and are "w"
     *             for write, and "a" for append. "w" mode discard any
     *             previously existing file.
     */
    //!
    Trajectory(const std::string& filename, const std::string& mode = "r", const std::string& format = "");
    Trajectory(Trajectory&&);
    Trajectory& operator=(Trajectory&&);
    ~Trajectory();

    //! Read operator, in *stream* version
    Trajectory& operator>>(Frame& frame);
    //! Read operator, in *method* version
    Frame read();
    //! Read operator, in *method* version with specific step
    Frame read_step(const size_t);
    //! Close a trajectory
    void close();

    //! Write operator, in *stream* version
    Trajectory& operator<<(const Frame& frame);
    //! Write operator, in *method* version
    void write(const Frame& frame);

    //! Set a topology to use while writing or reading format where no
    //! topological information is present.
    void topology(const Topology&);
    //! Use the topology of the first frame of the following file to read or
    //! write all the following files.
    void topology(const std::string& filename);

    //! Set an unit cell to use while writing or reading format when no
    //! information about unit cell is present.
    void cell(const UnitCell&);

    //! Get the number of steps (the number of Frames) in this trajectory
    size_t nsteps() const {return _nsteps;}
    //! Have we read all the Frames in this file ?
    bool done() const;
private:
    //! Current step
    size_t _step;
    //! Number of steps in the file, if available
    size_t _nsteps;
    //! Format used to read the file
    std::unique_ptr<Format> _format;
    //! The file we are reading from
    std::unique_ptr<File> _file;
    //! Topology to use for reading/writing files when no topological data is present
    Topology _topology;
    //! Do we have to use a specific topology ?
    bool _use_custom_topology;
    //! UnitCell to use for reading/writing files when no unit cell information is present
    UnitCell _cell;
    //! Do we have to use a specific unit cell ?
    bool _use_custom_cell;
};

} // namespace chemfiles

#endif
