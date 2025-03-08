// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <typeinfo>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/optional.hpp"

namespace chemfiles {
    class Frame;
    class MemoryBuffer;
}

using namespace chemfiles;

#if defined(__GNUC__) && !defined(__clang__)
#define IGNORING_SUGGEST_ATTRIBUTE_NORETURN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#endif

void Format::read_at(size_t /*unused*/, Frame& /*unused*/) {
    throw format_error(
        "'read_at' is not implemented for this format ({})",
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

#if defined(IGNORING_SUGGEST_ATTRIBUTE_NORETURN)
#pragma GCC diagnostic pop
#endif

TextFormat::TextFormat(std::string path, File::Mode mode, File::Compression compression) :
    file_(std::move(path), mode, compression) {}

TextFormat::TextFormat(std::shared_ptr<MemoryBuffer> memory, File::Mode mode, File::Compression compression) :
    file_(std::move(memory), mode, compression) {}

void TextFormat::scan_all() {
    if (eof_found_) {
        return;
    }

    optional<TextFile> tmp_read_file = nullopt;
    if (file_.mode() == File::Mode::APPEND && file_.compression() == File::Compression::GZIP) {
        tmp_read_file = TextFile(file_.path(), File::Mode::READ, file_.compression());
        // `forward()` needs a readable file
        std::swap(*tmp_read_file, file_);
    }

    auto before = file_.tellpos();
    while (!file_.eof()) {
        auto position = forward();
        if (!position) {
            break;
        }
        frame_positions_.push_back(position.value());
    }

    eof_found_ = true;
    // reset failbit in the file
    file_.clear();

    if (tmp_read_file) {
        // use original file for all further write operations
        std::swap(file_, *tmp_read_file);
    }

    if (before == 0 && !frame_positions_.empty()) {
        file_.seekpos(frame_positions_[0]);
    } else {
        file_.seekpos(before);
    }
}

void TextFormat::read_at(size_t index, Frame& frame) {
    // Start by checking if we know this index, if not, look for all frames in
    // the file
    if (index >= frame_positions_.size()) {
        scan_all();
    }

    // If the step is still too big, this is an error
    if (index >= frame_positions_.size()) {
        if (frame_positions_.size() == 0) {
            throw file_error(
                "can not read file '{}' at index {}, it does not contain any frames",
                file_.path(), index
            );
        } else {
            throw file_error(
                "can not read file '{}' at index {}: maximal index is {}",
                file_.path(), index, frame_positions_.size() - 1
            );
        }
    }

    index_ = index;
    file_.seekpos(frame_positions_[index]);
    read_next(frame);
}

void TextFormat::read(Frame& frame) {
    file_.seekpos(frame_positions_[index_]);
    index_++;
    read_next(frame);
}

void TextFormat::write(const Frame& frame) {
    write_next(frame);
    frame_positions_.push_back(file_.tellpos());
    index_++;
}

size_t TextFormat::size() {
    scan_all();
    return frame_positions_.size();
}
