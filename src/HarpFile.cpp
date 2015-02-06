/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "HarpFile.hpp"
#include "HarpIO.hpp"
#include "FormatFactory.hpp"

using namespace harp;

HarpFile::HarpFile(std::string filename, std::string mode){
    // TODO
}

HarpFile& HarpFile::operator>>(Frame& frame){
    *file >> frame;
    return *this;
};

Frame& HarpFile::read_next_step(){
    return file->read_next_step();
};

Frame& HarpFile::read_at_step(int step){
    return file->read_at_step(step);
}

HarpFile& HarpFile::operator<<(const Frame& frame){
    *file << frame;
    return *this;
}

void HarpFile::write_step(Frame& frame){
    file->write_step(frame);
}
