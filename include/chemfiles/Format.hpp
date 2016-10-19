/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_FORMAT_HPP
#define CHEMFILES_FORMAT_HPP

#include <memory>
#include <string>

#include "chemfiles/files/BasicFile.hpp"

namespace chemfiles {
class Frame;

/*!
 * @class Format Format.hpp Format.cpp
 *
 * Abstract base class for formats reader and writer
 */
class Format {
public:
    Format() = default;
    virtual ~Format() noexcept = default;
    Format& operator=(const Format&) = delete;
    Format(const Format&) = delete;
    /*!
    * @brief Read a specific step from the associated file.
    * @param step The step to read
    * @param frame The frame to fill
    *
    * This function can throw an exception in case of error.
    */
    virtual void read_step(size_t step, Frame& frame);

    /*!
    * @brief Read a specific step from the associated file.
    * @param frame The frame to fill
    *
    * This function can throw an exception in case of error.
    */
    virtual void read(Frame& frame);

    /*!
    * @brief Write a step (frame) to the associated file.
    * @param frame The frame to be writen
    *
    * This function can throw an exception in case of error.
    */
    virtual void write(const Frame& frame);

    /*!
    * @brief Get the number of frames in the associated file
    * @return The number of frames
    */
    virtual size_t nsteps() = 0;

    //! A short string describing the format.
    virtual std::string description() const = 0;
};

} // namespace chemfiles

#endif
