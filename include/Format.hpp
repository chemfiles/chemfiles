/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_FORMAT_HPP
#define HARP_FORMAT_HPP

namespace harp {

class Frame;
class File;

/*!
 * @class FormatReader Format.hpp Format.cpp
 * @brief Abstract format reader class
 *
 * Abstract base class for file formats reader
 */
class FormatReader {
public:
    /*!
    * @brief Read a specific step from a file.
    * @param file The file to read from
    * @param step The step to read
    * @return A reference to the read frame
    *
    * Can throw an exception in case of error.
    *
    */
    virtual Frame& read_at_step(const File& file, const int step) = 0;

    /*!
    * @brief Read a specific step from a file.
    * @param file The file to read from
    * @param step The step to read
    * @return A reference to the read frame
    *
    * Can throw an exception in case of error.
    *
    */
    virtual Frame& read_next_step(const File& file, const int step) = 0;

private:
};

/*!
* @class FormatWriter Format.hpp Format.cpp
* @brief Abstract format writer class
*
* Abstract base class for file formats writer.
*/
class FormatWriter {
public:
    /*!
    * @brief Write a step (frame) to a file.
    * @param file The file to write to
    * @param frame The frame to be writen
    * @return A reference to the read frame
    *
    * Can throw an exception in case of error.
    *
    */
    virtual void write_step(const File& file, const Frame& frame) = 0;

private:
};

} // namespace harp

#endif
