// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_XTC_FORMAT_HPP
#define CHEMFILES_XTC_FORMAT_HPP

#include <cstddef>
#include <cstdint>

#include <string>
#include <vector>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"

#include "chemfiles/files/XDRFile.hpp"

namespace chemfiles {
class Frame;
class FormatMetadata;

/// GROMACS XTC file format reader.
class XTCFormat final : public Format {
  public:
    XTCFormat(std::string path, File::Mode mode, File::Compression compression);

    void read_at(size_t index, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t size() override;

  private:
    struct FrameHeader {
        int32_t magic; // Magic number indicating the file format
        size_t natoms; // The total number of atoms
        size_t step;   // Current step number
        float time;    // Current time
        bool is_long_format() const;
    };

    /// Read header of the Frame at the current position
    FrameHeader read_frame_header();
    /// Write header of a Frame
    void write_frame_header(const FrameHeader& header);
    /// Determine the number of frames
    /// and the corresponding offset within the file
    void determine_frame_offsets();
    /// Get the total number of bytes of the next frame
    /// The returned value is aligned to 4 bytes
    uint64_t read_framebytes(bool is_long_format);

    /// Associated XDR file
    XDRFile file_;
    /// Offsets within file for fast indexing
    std::vector<uint64_t> frame_positions_;
    /// index of the next step to read
    size_t index_ = 0;
    /// The number of atoms in the trajectory
    size_t natoms_ = 0;
};

template <> const FormatMetadata& format_metadata<XTCFormat>();

} // namespace chemfiles

#endif
