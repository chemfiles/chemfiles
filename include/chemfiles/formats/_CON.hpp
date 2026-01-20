// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_CON_HPP
#define CHEMFILES_FORMAT_CON_HPP

#include <memory>
#include <string>
#include <vector>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/external/optional.hpp"
#include "chemfiles/parse.hpp"

namespace chemfiles {
class Frame;
class MemoryBuffer;
class FormatMetadata;

/// Con file format reader and writer.
class ConFormat final : public TextFormat {
  public:
    ConFormat(std::string path, File::Mode mode, File::Compression compression)
        : TextFormat(std::move(path), mode, compression) {}

    ConFormat(std::shared_ptr<MemoryBuffer> memory, File::Mode mode,
              File::Compression compression)
        : TextFormat(std::move(memory), mode, compression) {}

    void read_next(Frame &frame) override;
    void write_next(const Frame &frame) override;
    optional<uint64_t> forward() override;

  private:
    // Helper to read vector data from the header (e.g. atom counts, masses)
    std::vector<double> read_header_vector(size_t count);
    std::vector<size_t> read_header_sizen(size_t count);
};

template <> const FormatMetadata &format_metadata<ConFormat>();

// Standard whitespace characters: space, tab, carriage return
static const char *WHITESPACE = " \t\r";

// Helper to split a line and parse N numbers using string_view.
// This is necessary because 'scan' requires a fixed number of arguments known
// at compile time, but lines 8 and 9 in CON files have a variable number of
// elements.
template <typename T>
std::vector<T> parse_vector(std::string_view line, size_t count) {
    std::vector<T> values;
    values.reserve(count);

    size_t start = line.find_first_not_of(WHITESPACE);
    while (start != std::string_view::npos) {
        size_t end = line.find_first_of(WHITESPACE, start);
        auto token = line.substr(start, end - start);
        values.push_back(parse<T>(token));

        if (end == std::string_view::npos) {
            break;
        }
        start = line.find_first_not_of(WHITESPACE, end);
    }

    if (values.size() != count) {
        throw format_error("con format: expected {} values, got {}", count,
                           values.size());
    }
    return values;
}

} // namespace chemfiles

#endif
