/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_FRAME_HPP
#define HARP_FRAME_HPP

#include <array>
#include <vector>

namespace harp {

typedef std::array<float, 3> Vector3D;

/*!
 * @class Frame Frame.hpp Frame.cpp
 * @brief Frame i.e. data from one simulation step
 *
 *
 */
class Frame {
    public:
        Frame();
        ~Frame();

    private:
        int step;

        std::vector<Vector3D> positions;
        std::vector<Vector3D> velocities;
};

} // namespace harp

#endif
