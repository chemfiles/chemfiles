// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <functional>
#include <memory>
#include <string>

#include "chemfiles/Trajectory.hpp"

#include "chemfiles/Atom.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/FormatFactory.hpp"
#include "chemfiles/Configuration.hpp"

#include "chemfiles/utils.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/string_view.hpp"
#include "chemfiles/external/optional.hpp"

using namespace chemfiles;

#define SENTINEL_VALUE (static_cast<size_t>(-1))

struct file_open_info {
    static file_open_info parse(const std::string& path, const std::string& format);
    std::string format = "";
    std::string extension = "";
    File::Compression compression = File::DEFAULT;
};

file_open_info file_open_info::parse(const std::string& path, const std::string& format) {
    file_open_info info;

    auto slash = format.find('/');
    if (slash != std::string::npos) {
        auto tmp = format.substr(slash + 1);
        auto compression = trim(tmp);
        if (compression == "GZ") {
            info.compression = File::GZIP;
        } else if (compression == "BZ2") {
            info.compression = File::BZIP2;
        } else if (compression == "XZ") {
            info.compression = File::LZMA;
        } else {
            throw file_error("unknown compression method '{}'", compression);
        }
    }

    auto tmp = format.substr(0, slash);
    info.format = trim(tmp).to_string();

    auto dot1 = path.rfind('.');
    if (dot1 != std::string::npos) {
        info.extension = path.substr(dot1);
        if (info.compression == File::DEFAULT) {
            bool new_extension = false;
            // check file extension for compressed file extension
            if (info.extension == ".gz") {
                new_extension = true;
                info.compression = File::GZIP;
            } else if (info.extension == ".bz2") {
                new_extension = true;
                info.compression = File::BZIP2;
            } else if (info.extension == ".xz") {
                new_extension = true;
                info.compression = File::LZMA;
            }

            if (new_extension) {
                auto dot2 = path.substr(0, dot1).rfind('.');
                if (dot2 != std::string::npos) {
                    info.extension = path.substr(0, dot1).substr(dot2);
                }
            }
        }
    }

    return info;
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

    auto info = file_open_info::parse(path_, format);
    format_creator_t format_creator;
    if (info.format != "") {
        format_creator = FormatFactory::get().name(info.format);
    } else if (info.extension != "") {
        format_creator = FormatFactory::get().extension(info.extension);
    } else {
        throw file_error(
            "file at '{}' does not have an extension, provide a format name to read it",
            path_
        );
    }

    format_ = format_creator(path_, char_to_file_mode(mode), info.compression);

    if (mode == 'r' || mode == 'a') {
        nsteps_ = format_->nsteps();
    }
}

Trajectory::~Trajectory() = default;
Trajectory::Trajectory(Trajectory&&) = default;
Trajectory& Trajectory::operator=(Trajectory&&) = default;

void Trajectory::pre_read(size_t step) {
    if (step >= nsteps_) {
        if (nsteps_ == 0) {
            throw file_error(
                "can not read file '{}' at step {}, it does not contain any step",
                path_, step
            );
        } else {
            throw file_error(
                "can not read file '{}' at step {}: maximal step is {}",
                path_, step, nsteps_ - 1
            );
        }
    }
    if (mode_ != File::READ) {
        throw file_error(
            "the file at '{}' was not openened in read mode", path_
        );
    }
}

void Trajectory::post_read(Frame& frame) {
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
    frame.set_step(SENTINEL_VALUE);
    format_->read(frame);
    post_read(frame);

    // Don't override the step set by a format
    if (frame.step() == SENTINEL_VALUE) {
        frame.set_step(step_);
    }

    step_++;
    return frame;
}

Frame Trajectory::read_step(const size_t step) {
    check_opened();
    pre_read(step);

    Frame frame;
    frame.set_step(SENTINEL_VALUE);
    step_ = step;
    format_->read_step(step_, frame);

    // Don't override the step set by a format
    if (frame.step() == SENTINEL_VALUE) {
        frame.set_step(step_);
    }

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
