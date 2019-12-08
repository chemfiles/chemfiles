// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cstdint>
#include <string>
#include <vector>
#include <typeinfo>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/string_view.hpp"
#include "chemfiles/error_fmt.hpp"

namespace chemfiles {
    class Frame;
}

using namespace chemfiles;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#endif

void Format::read_step(size_t /*unused*/, Frame& /*unused*/) {
    throw format_error(
        "'read_step' is not implemented for this format ({})",
        typeid(*this).name()
    );
}

void Format::read(Frame& /*unused*/) {
    throw format_error(
        "'read' is not implemented for this format ({})",
        typeid(*this).name()
    );
}

void Format::write(const Frame& /*unused*/) {
    throw format_error(
        "'write' is not implemented for this format ({})",
        typeid(*this).name()
    );
}

void TextFormat::read_next(Frame& /*unused*/) {
    throw format_error(
        "'read' is not implemented for this format ({})",
        typeid(*this).name()
    );
}

void TextFormat::write_next(const Frame& /*unused*/) {
    throw format_error(
        "'write' is not implemented for this format ({})",
        typeid(*this).name()
    );
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

TextFormat::TextFormat(std::string path, File::Mode mode, File::Compression compression):
    file_(std::move(path), mode, compression) {}

void TextFormat::scan_all() {
    if (eof_found_) {
        return;
    }

    auto before = file_.tellpos();
    while (!file_.eof()) {
        auto position = forward();
        if (!position) {
            break;
        }
        steps_positions_.push_back(position.value());
    }

    eof_found_ = true;
    // reset failbit in the file
    file_.clear();

    if (before == 0 && !steps_positions_.empty()) {
        file_.seekpos(steps_positions_[0]);
    } else {
        file_.seekpos(before);
    }
}

void TextFormat::read_step(size_t step, Frame& frame) {
    // Start by checking if we know this step, if not, look for all steps in
    // the file
    if (step >= steps_positions_.size()) {
        scan_all();
    }

    // If the step is still too big, this is an error
    if (step >= steps_positions_.size()) {
        if (steps_positions_.size() == 0) {
            throw file_error(
                "can not read file '{}' at step {}, it does not contain any step",
                file_.path(), step
            );
        } else {
            throw file_error(
                "can not read file '{}' at step {}: maximal step is {}",
                file_.path(), step, steps_positions_.size() - 1
            );
        }
    }

    file_.seekpos(steps_positions_[step]);
    read_next(frame);
}

void TextFormat::read(Frame& frame) {
    read_next(frame);
}

void TextFormat::write(const Frame& frame) {
    write_next(frame);
    steps_positions_.push_back(file_.tellpos());
}

size_t TextFormat::nsteps() {
    scan_all();
    return steps_positions_.size();
}
