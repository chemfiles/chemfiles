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
class Stream;

/*!
* @class HarpFile HarpIO.hpp HarpIO.cpp
* @brief The main entry point for harp library
*/
class HarpFile {
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
    HarpFile(std::string filename, std::string mode = "r");

    //! Read operator, stream form
    inline HarpFile& operator>>(Frame& frame);
    //! Read operator, method form
    inline Frame& read_next_step();
    //! Read operator, method form with specific step
    inline Frame& read_at_step(int step);

    //! Write operator, stream form
    inline HarpFile& operator<<(const Frame& frame);
    //! Write operator, method form
    inline void write_step(Frame& frame);
private:
    // PIMPL pointer
    std::unique_ptr<Stream> file;
};

} // namespace harp
#endif
