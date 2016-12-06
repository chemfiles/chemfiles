// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_FORMAT_XYZ_HPP
#define CHEMFILES_FORMAT_XYZ_HPP

#include <string>

#include "chemfiles/Format.hpp"
#include "chemfiles/FormatFactory.hpp"

namespace chemfiles {

/*!
 * @class XYZFormat formats/XYZ.hpp formats/XYZ.cpp
 * @brief XYZ file format reader and writer.
 *
 * The format is described at http://openbabel.org/wiki/XYZ
 */
class XYZFormat final: public Format {
public:
    XYZFormat(const std::string& path, File::Mode mode);

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    std::string description() const override;
    size_t nsteps() override;

    // Register the xyz format with the ".xyz" extension and the "XYZ" description.
    FORMAT_NAME(XYZ)
    FORMAT_EXTENSION(.xyz)
private:
    //! Quick forward the file for `nsteps`, returning `false` if the file does
    //! not seems to contain `nsteps` more steps.
    bool forward(size_t nsteps);
    //! Text file where we read from
    std::unique_ptr<TextFile> file_;
    //! Newt step we will read.
    size_t step_cursor_ = 0;
};

} // namespace chemfiles

#endif
