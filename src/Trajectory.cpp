// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Trajectory.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/FormatFactory.hpp"
#include "chemfiles/Configuration.hpp"

using namespace chemfiles;

//! Get the extension part of a filename. Filename is assumed to be valid here.
static std::string extension(const std::string& filename) {
    auto idx = filename.rfind('.');

    if (idx != std::string::npos) {
        return filename.substr(idx);
    } else {
        return "";
    }
}

static File::Mode char_to_file_mode(char mode) {
    switch (mode) {
    case 'r':
    case 'R':
        return File::READ;
    case 'a':
    case 'A':
        return File::APPEND;
    case 'w':
    case 'W':
        return File::WRITE;
    default:
        throw FileError(
            "Got an unknown file mode '" + std::to_string(mode) + "'"
        );
    }
}

Trajectory::Trajectory(const std::string& path, char mode, const std::string& format)
    : path_(path), mode_(mode), step_(0), nsteps_(0), format_(nullptr), custom_topology_(), custom_cell_() {
    format_creator_t format_creator;
    if (format == "") {
        // try to guess the format by extension
        auto ext = extension(path);
        format_creator = FormatFactory::get().by_extension(ext);
    } else {
        format_creator = FormatFactory::get().format(format);
    }

    auto filemode = char_to_file_mode(mode);
    format_ = format_creator(path, filemode);

    if (mode == 'r' || mode == 'a') {
        nsteps_ = format_->nsteps();
    }
}

Trajectory::~Trajectory() = default;
Trajectory::Trajectory(Trajectory&&) = default;
Trajectory& Trajectory::operator=(Trajectory&&) = default;

Frame Trajectory::read() {
    if (step_ >= nsteps_) {
        throw FileError(
            "Can not read file '" + path_ + "' past end."
        );
    }
    if (!(mode_ == File::READ || mode_ == File::APPEND)) {
        throw FileError(
            "File '" + path_ + "' was not openened in read or append mode."
        );
    }

    Frame frame;
    format_->read(frame);
    step_++;

    // Set the frame topology and/or cell if needed
    if (custom_topology_) {
        frame.set_topology(*custom_topology_);
    }
    if (custom_cell_) {
        frame.set_cell(*custom_cell_);
    }

    auto configuration = Configuration::get();
    for (auto& atom: frame.topology()) {
        if (configuration.find(atom.name())) {
            atom.set_type(configuration.type(atom.name()));
        }
    }

    return frame;
}

Frame Trajectory::read_step(const size_t step) {
    if (step >= nsteps_) {
        throw FileError(
            "Can not read file '" + path_ + "' at step " +
            std::to_string(step) + ". Max step is " + std::to_string(nsteps_) + "."
        );
    }

    if (!(mode_ == File::READ || mode_ == File::APPEND)) {
        throw FileError(
            "File '" + path_ + "' was not openened in read or append mode."
        );
    }

    Frame frame;
    step_ = step;
    format_->read_step(step_, frame);

    // Set the frame topology and/or cell if needed
    if (custom_topology_) {
        frame.set_topology(*custom_topology_);
    }
    if (custom_cell_) {
        frame.set_cell(*custom_cell_);
    }

    auto configuration = Configuration::get();
    for (auto& atom: frame.topology()) {
        if (configuration.find(atom.name())) {
            atom.set_type(configuration.type(atom.name()));
        }
    }

    return frame;
}

void Trajectory::write(const Frame& frame) {
    if (!(mode_ == File::WRITE || mode_ == File::APPEND)) {
        throw FileError(
            "File '" + path_ + "' was not openened in write or append mode."
        );
    }

    if (custom_topology_ || custom_cell_) {
        Frame copy = frame.clone();
        if (custom_topology_) {
            copy.set_topology(*custom_topology_);
        }
        if (custom_cell_) {
            copy.set_cell(*custom_cell_);
        }
        format_->write(copy);
    } else {
        format_->write(frame);
    }

    step_++;
    nsteps_++;
}

void Trajectory::set_topology(const Topology& top) {
    custom_topology_ = top;
}

void Trajectory::set_topology(const std::string& filename, const std::string& format) {
    Trajectory topolgy_file(filename, 'r', format);
    assert(topolgy_file.nsteps() > 0);

    auto frame = topolgy_file.read_step(0);
    set_topology(frame.topology());
}

void Trajectory::set_cell(const UnitCell& new_cell) {
    custom_cell_ = new_cell;
}

bool Trajectory::done() const {
    return step_ >= nsteps_;
}
