/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "Trajectory.hpp"
#include "TrajectoryFactory.hpp"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

using namespace harp;
using std::string;

Trajectory::Trajectory(const string& filename, const string& mode, const string& format)
: _step(0), _nsteps(0)
{
    trajectory_builder_t builder;
    if (format == ""){
        // try to guess the format by extension
        auto ext = extension(filename);
        builder = TrajectoryFactory::by_extension(ext);
    }
    else {
        builder = TrajectoryFactory::format(format);
    }

    _format = builder.format_creator();
    if (builder.file_creator) {
        _file = builder.file_creator(filename, mode);
    }
    else {
        // Defaulting to a BasicFile
        _file = std::unique_ptr<File>(new BasicFile(filename, mode));
    }
    if (mode == "r")
        _nsteps = _format->nsteps(_file.get());
}

Trajectory::~Trajectory(){}

Trajectory& Trajectory::operator>>(Frame& frame){
    frame = read_next_step();
    return *this;
}

const Frame& Trajectory::read_next_step(){
    if (_step >= _nsteps)
        throw FileError("Can not read file \"" + _file->name() + "\" past end.");
    _format->read_next_step(_file.get(), _frame);
    _step++;
    return _frame;
}

const Frame& Trajectory::read_at_step(const size_t step){
    if (_step >= _nsteps)
        throw FileError(
            "Can not read file \"" + _file->name() + "\" at step " +
            std::to_string(step) + ". Max step is " + std::to_string(_nsteps) + "."
        );
    _step = step;
    _format->read_at_step(_file.get(), step, _frame);
    return _frame;
}

Trajectory& Trajectory::operator<<(const Frame& frame){
    write_step(frame);
    return *this;
}

void Trajectory::write_step(const Frame& frame){
    _format->write_step(_file.get(), frame);
    _step++;
    _nsteps++;
}

void Trajectory::close(){
    _file->close();
}

bool Trajectory::done() {
    return _step >= _nsteps;
}
