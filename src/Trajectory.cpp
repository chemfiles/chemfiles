// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Trajectory.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/Configuration.hpp"
#include "chemfiles/FormatFactory.hpp"

using namespace chemfiles;

//! Get the extension part of a filename.
static std::string extension(const std::string& filename) {
    auto idx = filename.rfind('.');

    if (idx != std::string::npos) {
        return filename.substr(idx);
    } else {
        throw file_error(
            "file at '{}' does not have an extension, provide a format name to read it",
            filename
        );
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
        throw file_error("unknown file mode '{}'", mode);
    }
}

Trajectory::Trajectory(std::string path, char mode, const std::string& format)
    : path_(std::move(path)), mode_(mode), step_(0), nsteps_(0), format_(nullptr) {
    format_creator_t format_creator;
    if (format == "") {
        format_creator = FormatFactory::get().extension(extension(path_));
    } else {
        format_creator = FormatFactory::get().name(format);
    }

    auto filemode = char_to_file_mode(mode);
    format_ = format_creator(path_, filemode);

    if (mode == 'r' || mode == 'a') {
        nsteps_ = format_->nsteps();
    }
}

Trajectory::~Trajectory() = default;
Trajectory::Trajectory(Trajectory&&) = default;
Trajectory& Trajectory::operator=(Trajectory&&) = default;

void Trajectory::pre_read(size_t step) {
    if (step >= nsteps_) {
        throw file_error(
            "can not read file '{}' at step {}: maximal step is {}",
            path_, step, nsteps_
        );
    }
    if (!(mode_ == File::READ || mode_ == File::APPEND)) {
        throw file_error(
            "the file at '{}' was not openened in read or append mode", path_
        );
    }
}

void Trajectory::post_read(Frame& frame) {
    frame.set_step(step_);

    if (custom_topology_) {
        frame.set_topology(*custom_topology_);
    } else {
        for (auto& atom: frame) {
            atom.set_type(Configuration::rename(atom.type()));
        }
    }
    if (custom_cell_) {
        frame.set_cell(*custom_cell_);
    }
}

void Trajectory::check_opened() const {
    if (!format_) {
        throw file_error("can not use a closed trajectory");
    }
}

size_t Trajectory::nsteps() const  {
    check_opened();
    return nsteps_;
}

Frame Trajectory::read() {
    check_opened();
    pre_read(step_);

    Frame frame;
    format_->read(frame);
    post_read(frame);

    step_++;
    return frame;
}

Frame Trajectory::read_step(const size_t step) {
    check_opened();
    pre_read(step);

    Frame frame;
    step_ = step;
    format_->read_step(step_, frame);

    post_read(frame);
    return frame;
}

void Trajectory::write(const Frame& frame) {
    check_opened();
    if (!(mode_ == File::WRITE || mode_ == File::APPEND)) {
        throw file_error(
            "the file at '{}' was not openened in write or append mode", path_
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

void Trajectory::set_topology(const Topology& topology) {
    check_opened();
    custom_topology_ = topology;
}

void Trajectory::set_topology(const std::string& filename, const std::string& format) {
    check_opened();
    Trajectory topolgy_file(filename, 'r', format);
    assert(topolgy_file.nsteps() > 0);

    auto frame = topolgy_file.read_step(0);
    set_topology(frame.topology());
}

void Trajectory::set_cell(const UnitCell& cell) {
    check_opened();
    custom_cell_ = cell;
}

bool Trajectory::done() const {
    check_opened();
    return step_ >= nsteps_;
}

void Trajectory::close() {
    check_opened();
    // delete the format and set the pointer to nullptr
    format_.reset();
}
