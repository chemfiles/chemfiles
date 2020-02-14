// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_INCHI_HPP
#define CHEMFILES_FORMAT_INCHI_HPP

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"

namespace chemfiles {

/// [INCHI] file format reader and writer.
///
/// [INCHI]: http://inchi-trust.org/
class InChIFormat final: public TextFormat {
public:
    InChIFormat(std::string path, File::Mode mode, File::Compression compression):
        TextFormat(std::move(path), mode, compression) {}
    InChIFormat(std::shared_ptr<MemoryBuffer> memory, File::Mode mode, File::Compression compression):
        TextFormat(std::move(memory), mode, compression) {}

    void read_next(Frame& frame) override;
    void write_next(const Frame& frame) override;
    optional<uint64_t> forward() override;
};

template<> FormatInfo format_information<InChIFormat>();

} // namespace chemfiles

#endif
