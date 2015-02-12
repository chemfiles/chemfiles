/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_FORMAT_XYZ_HPP
#define HARP_FORMAT_XYZ_HPP

#include <string>

#include "Format.hpp"
#include "FormatFactory.hpp"

namespace harp {

class TextFile;

/*!
 * @class XYZFormat formats/XYZ.hpp formats/XYZ.cpp
 * @brief XYZ file format reader.
 *
 * TODO: add a reference to the format
 */
class XYZFormat : public Format {
public:
    void read_at_step(shared_ptr<File> file, const size_t step, Frame& frame);
    void read_next_step(shared_ptr<File> file, Frame& frame);
    std::string description() const;
private:
    REGISTER_FORMAT;
};

} // namespace harp

#endif
