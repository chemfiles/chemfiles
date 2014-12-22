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

#include "Frame.hpp"

namespace harp {

/*!
 * @class Format Format.hpp Format.cpp
 * @brief Abstract format class
 *
 *
 * Abstract base class for file formats
 *
 */
class Format {
    public:

    private:
        Frame frame;
};

} // namespace harp

#endif
