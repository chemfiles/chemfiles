/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "Trajectory.hpp"
#include "HarpIO.hpp"
#include "FormatFactory.hpp"

using namespace harp;

Trajectory::Trajectory(std::string filename, std::string mode){
    // TODO
}

Trajectory::~Trajectory(){}

Trajectory& Trajectory::operator>>(Frame& frame){
    *file >> frame;
    return *this;
};

Frame& Trajectory::read_next_step(){
    return file->read_next_step();
};

Frame& Trajectory::read_at_step(int step){
    return file->read_at_step(step);
}

Trajectory& Trajectory::operator<<(const Frame& frame){
    *file << frame;
    return *this;
}

void Trajectory::write_step(Frame& frame){
    file->write_step(frame);
}
