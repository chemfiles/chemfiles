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

using namespace harp;

Trajectory::Trajectory(const std::string& filename, const std::string& mode, const std::string& format){
    file = std::unique_ptr<HarpIO>(new HarpIO(filename, mode, format));
}

Trajectory::Trajectory(Trajectory&& other){
    file = std::move(other.file);
}

Trajectory& Trajectory::operator=(Trajectory&& other){
    file = std::move(other.file);
    return *this;
}

Trajectory::~Trajectory(){}

Trajectory& Trajectory::operator>>(Frame& frame){
    *file >> frame;
    return *this;
};

Frame& Trajectory::read_next_step(){
    return file->read_next_step();
};

Frame& Trajectory::read_at_step(const size_t step){
    return file->read_at_step(step);
}

Trajectory& Trajectory::operator<<(const Frame& frame){
    *file << frame;
    return *this;
}

void Trajectory::write_step(Frame& frame){
    file->write_step(frame);
}
