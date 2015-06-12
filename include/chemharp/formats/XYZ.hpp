/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_FORMAT_XYZ_HPP
#define HARP_FORMAT_XYZ_HPP

#include <string>

#include "chemharp/Format.hpp"
#include "chemharp/TrajectoryFactory.hpp"

namespace harp {

class TextFile;

/*!
 * @class XYZFormat formats/XYZ.hpp formats/XYZ.cpp
 * @brief XYZ file format reader.
 *
 * The format is described at http://openbabel.org/wiki/XYZ
 */
class XYZFormat : public Format {
public:
    XYZFormat() = default;
    ~XYZFormat() = default;

    virtual void read_step(File* file, const size_t step, Frame& frame) override;
    virtual void read(File* file, Frame& frame) override;
    virtual void write(File* file, const Frame& frame) override;
    virtual std::string description() const override;
    virtual size_t nsteps(File* file) const override;
private:
    REGISTER_FORMAT;
};

} // namespace harp

#endif
