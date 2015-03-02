/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "Trajectory.hpp"
#include "FormatFactory.hpp"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

using namespace harp;
using std::string;

Trajectory::Trajectory(const string& filename, const string& mode, const string& format) {
    if (format == ""){
        // try to guess the format by extension
        auto ext = extension(filename);
        _format = FormatFactory::by_extension(ext);
    }
    else {
        _format = FormatFactory::format(format);
    }

    // TODO: use mode to set the mode in file
    _file = std::unique_ptr<File>(new BasicFile(filename));
}

Trajectory::~Trajectory(){}

Trajectory& Trajectory::operator>>(Frame& frame){
    _format->read_next_step(_file.get(), frame);
    return *this;
}

Frame& Trajectory::read_next_step(){
    _format->read_next_step(_file.get(), _frame);
    return _frame;
}

Frame& Trajectory::read_at_step(const size_t step){
    _format->read_at_step(_file.get(), step, _frame);
    return _frame;
}

Trajectory& Trajectory::operator<<(const Frame& frame){
    _format->write_step(_file.get(), frame);
    return *this;
}

void Trajectory::write_step(Frame& frame){
    _format->write_step(_file.get(), frame);
}
