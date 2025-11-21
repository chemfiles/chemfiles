// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_BCIF_HPP
#define CHEMFILES_FORMAT_BCIF_HPP

#include <cstddef>
#include <cstdint>

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"

namespace chemfiles {
class Frame;
class UnitCell;
class MemoryBuffer;
class FormatMetadata;

/// BinaryCIF (BCIF) file format reader.
///
/// BinaryCIF is a binary encoding of mmCIF files using MessagePack as the
/// container format with additional compression schemes.
class BCIFFormat final: public Format {
public:
    struct BCIFData ;
    struct BCIFDataDtor { void operator()(BCIFData*) noexcept; };

    BCIFFormat(std::string path, File::Mode mode, File::Compression compression);
    BCIFFormat(std::shared_ptr<MemoryBuffer> memory, File::Mode mode, File::Compression compression);


    void read_at(size_t index, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t size() override;

private:
    /// Perform the BCIF decoding and parsing
    void decode(const std::string& data);

    /// Perform the BCIF encoding for writing
    std::vector<char> encode(const Frame& frame);

    /// Storage for the parsed BCIF data
    std::unique_ptr<BCIFData, BCIFDataDtor>  data_ = nullptr;

    /// Current model being read. Ranges from [0, data_.num_models)
    size_t model_index_ = 0;

    /// Has the file been decoded yet?
    bool decoded_ = false;

    /// Location of BCIF file on disk
    std::string filename_;

    /// Memory buffer (for memory-based reading)
    std::shared_ptr<MemoryBuffer> memory_;

    /// File mode (READ or WRITE)
    File::Mode mode_ = File::READ;

    /// Compression for writing
    File::Compression compression_ = File::DEFAULT;

    /// Whether any frames have been written
    bool has_written_ = false;
};

template<> const FormatMetadata& format_metadata<BCIFFormat>();

} // namespace chemfiles

#endif
