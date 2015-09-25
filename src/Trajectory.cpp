/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp/Trajectory.hpp"
#include "chemharp/TrajectoryFactory.hpp"
#include "chemharp/Logger.hpp"
#include "chemharp/files/BasicFile.hpp"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

using namespace harp;
using std::string;

Trajectory::Trajectory(const string& filename, const string& mode, const string& format)
: _step(0), _nsteps(0), _topology(), _use_custom_topology(false), _cell(), _use_custom_cell(false)
{
    trajectory_builder_t builder;
    if (format == ""){
        // try to guess the format by extension
        auto ext = extension(filename);
        builder = TrajectoryFactory::get().by_extension(ext);
    }
    else {
        builder = TrajectoryFactory::get().format(format);
    }

    _file = builder.file_creator(filename, mode);
    _format = builder.format_creator(*_file);

    if (mode == "r" || mode == "a")
        _nsteps = _format->nsteps();
}

Trajectory::~Trajectory(){}

Trajectory& Trajectory::operator>>(Frame& frame){
    frame = read();
    return *this;
}

Frame Trajectory::read(){
    if (_step >= _nsteps) {
        throw FileError("Can not read file \"" + _file->filename() + "\" past end.");
    }
    if (!(_file->mode() == "r" || _file->mode() == "a")) {
        throw FileError("File \"" + _file->filename() + "\" was not openened in read or append mode.");
    }

    Frame frame;
    _format->read(frame);
    _step++;

    // Set the frame topology if needed
    if (_use_custom_topology)
        frame.topology(_topology);

    // Set the frame unit cell if needed
    if (_use_custom_cell)
        frame.cell(_cell);

    return frame;
}

Frame Trajectory::read_step(const size_t step){
    if (step >= _nsteps) {
        throw FileError(
            "Can not read file \"" + _file->filename() + "\" at step " +
            std::to_string(step) + ". Max step is " + std::to_string(_nsteps) + "."
        );
    }
    if (!(_file->mode() == "r" || _file->mode() == "a")) {
        throw FileError("File \"" + _file->filename() + "\" was not openened in read or append mode.");
    }

    Frame frame;
    _step = step;
    _format->read_step(_step, frame);

    // Set the frame topology if needed
    if (_use_custom_topology)
        frame.topology(_topology);

    // Set the frame unit cell if needed
    if (_use_custom_cell)
        frame.cell(_cell);

    return frame;
}

Trajectory& Trajectory::operator<<(const Frame& frame){
    write(frame);
    return *this;
}

void Trajectory::write(const Frame& input_frame){
    if (!(_file->mode() == "w" || _file->mode() == "a")) {
        throw FileError("File \"" + _file->filename() + "\" was not openened in write or append mode.");
    }

    // Maybe that is not the better way to do this, performance-wise. I'll have
    // to benchmark this part.
    Frame frame = input_frame;
    if (_use_custom_topology)
        frame.topology(_topology);
    if (_use_custom_cell)
        frame.cell(_cell);

    _format->write(frame);
    _step++;
    _nsteps++;
}

void Trajectory::topology(const Topology& top){
    _use_custom_topology = true;
    _topology = top;
}

void Trajectory::topology(const std::string& filename) {
    Trajectory topolgy_file(filename, "r");
    assert(topolgy_file.nsteps() > 0);

    auto frame = topolgy_file.read_step(0);
    Trajectory::topology(frame.topology());
}

void Trajectory::cell(const UnitCell& new_cell){
    _use_custom_cell = true;
    _cell = new_cell;
}

void Trajectory::close(){
    _file->close();
}

bool Trajectory::done() const {
    return _step >= _nsteps;
}
