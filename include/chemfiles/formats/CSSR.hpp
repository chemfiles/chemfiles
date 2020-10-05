// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_CSSR_HPP
#define CHEMFILES_FORMAT_CSSR_HPP

#include <cstdint>
#include <string>
#include <memory>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/external/optional.hpp"

namespace chemfiles {
class Frame;
class MemoryBuffer;
class FormatMetadata;

/// CSSR (Cambridge Structure Search and Retrieval) file format reader
/// and writer. Only one frame can be read or written to this format.
class CSSRFormat final: public TextFormat {
public:
    CSSRFormat(std::string path, File::Mode mode, File::Compression compression);
    CSSRFormat(std::shared_ptr<MemoryBuffer> memory, File::Mode mode, File::Compression compression);

    void read_next(Frame& frame) override;
    void write_next(const Frame& frame) override;
    optional<uint64_t> forward() override;
};

template<> const FormatMetadata& format_metadata<CSSRFormat>();

} // namespace chemfiles

#endif
