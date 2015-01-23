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

/*!
 * @class XYZReader formats/XYZ.hpp formats/XYZ.cpp
 * @brief XYZ file format reader.
 *
 * TODO: add a reference to the format
 */
class XYZReader : public FormatReader {
public:
    XYZReader();
    ~XYZReader();

    Frame& read_at_step(const File& file, const int step);
    Frame& read_next_step(const File& file, const int step);
private:
    READER_REGISTER_MEMBER(XYZReader);
};

} // namespace harp

#endif
