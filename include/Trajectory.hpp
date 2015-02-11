/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_HARPFILE_HPP
#define HARP_HARPFILE_HPP

#include <string>
#include <memory>

namespace harp {

class Frame;
class HarpIO;

/*!
* @class Trajectory Trajectory.hpp Trajectory.cpp
* @brief The main entry point for harp library
*/
class Trajectory {
public:
    /*!
    * @brief Open a trajectory file
    * @param filename The file path
    * @param mode Opening mode for the file. Default mode is "r" for read.
    *
    * Open a file, automatically gessing the file format and type from the
    * extension.
    */
    //!
    Trajectory(const std::string& filename, const std::string& mode = "r");
    Trajectory(Trajectory&&);
    Trajectory& operator=(Trajectory&&);
    ~Trajectory();

    //! Read operator, stream form
    Trajectory& operator>>(Frame& frame);
    //! Read operator, method form
    Frame& read_next_step();
    //! Read operator, method form with specific step
    Frame& read_at_step(const size_t step);

    //! Write operator, stream form
    Trajectory& operator<<(const Frame& frame);
    //! Write operator, method form
    void write_step(Frame& frame);
private:
    // PIMPL pointer
    std::unique_ptr<HarpIO> file;
};

} // namespace harp
#endif
