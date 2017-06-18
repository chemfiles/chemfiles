// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_TRAJECTORY_HPP
#define CHEMFILES_TRAJECTORY_HPP

#include <memory>
#include <string>

#include "chemfiles/Frame.hpp"
#include "chemfiles/exports.hpp"
#include "chemfiles/optional.hpp"

namespace chemfiles {

class Format;

/// A `Trajectory` is a chemistry file on the hard drive. It is the entry point
/// of the chemfiles library.
///
/// A `Trajectory` is made with a `File` and a `Format`. The `File` implements
/// all the physical operations, while the `Format` provides a way to interpret
/// the file.
class CHFL_EXPORT Trajectory {
public:

    /// Open a file, automatically gessing the file format and type from the
    /// extension.
    ///
    /// @param filename The file path. In `w` or `a` modes, the file is
    ///                 created if it does not exist yet. In `r` mode, an
    ///                 exception is thrown is the file does not exist yet.
    /// @param mode Opening mode for the file. Default mode is `r` for read.
    ///             Other supported modes depends on the underlying format and
    ///             are `w` for write, and `a` for append. `w` mode discard any
    ///             previously existing file.
    /// @param format Specific format to use. Needed when there is no way to
    ///               guess the format from the extension of the file, or when
    ///               this guess would be wrong.
    Trajectory(const std::string& filename, char mode = 'r', const std::string& format = "");
    Trajectory(Trajectory&&);
    Trajectory& operator=(Trajectory&&);
    ~Trajectory();

    Trajectory(const Trajectory&) = delete;
    Trajectory& operator=(const Trajectory&) = delete;

    /// Read the next frame in the trajectory
    Frame read();
    /// Read a single frame at specific step from the trajectory
    Frame read_step(size_t step);

    /// Write a single frame to the trajectory
    void write(const Frame& frame);

    /// Set the Topology of all the Frame read or written to `topology`. This
    /// replace any topology in the file being read, or in the Frame being
    /// written.
    void set_topology(const Topology&);
    /// Use the Topology of the first Frame of the following file as Topology
    /// for all the Frame read or written. This replace any topology in the
    /// file being read, or in the Frame being written. The optional parameter
    /// `format` can be used to specify the topology file format.
    void set_topology(const std::string& filename, const std::string& format = "");

    /// Set the unit cell of all the Frame read or written to `cell`. This
    /// replace any cell in the file being read, or in the Frame being
    /// written.
    void set_cell(const UnitCell&);

    /// Get the number of steps (the number of Frames) in this trajectory
    size_t nsteps() const { return nsteps_; }
    /// Have we read all the Frames in this file ?
    bool done() const;

private:
    /// Perform a few checks before reading a frame
    void pre_read(size_t step);
    /// Set the frame topology and/or cell after reading it
    void post_read(Frame& frame);

    /// Path of the associated file
    std::string path_;
    /// Opening mode of the associated file
    char mode_;
    /// Current step
    size_t step_;
    /// Number of steps in the file, if available
    size_t nsteps_;
    /// Format used to read the associated file
    std::unique_ptr<Format> format_;
    /// Topology to use for reading/writing files when no topological data is
    /// present
    optional<Topology> custom_topology_;
    /// UnitCell to use for reading/writing files when no unit cell information
    /// is present
    optional<UnitCell> custom_cell_;
};

} // namespace chemfiles

#endif
