// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_NC_HPP
#define CHEMFILES_FORMAT_NC_HPP

#include <string>
#include <vector>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"

#include "chemfiles/files/NcFile.hpp"

namespace chemfiles {

class Frame;
class UnitCell;
class Vector3D;
class FormatMetadata;

template <class T> class span;

enum AmberFormat {
    AMBER_NC_RESTART,     ///< AMBER NetCDF restart format
    AMBER_NC_TRAJECTORY,  ///< AMBER NetCDF trajectory format
};

/// Amber NetCDF file format reader.
template <AmberFormat F>
class Amber final: public Format {
public:
    Amber(std::string path, File::Mode mode, File::Compression compression);

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;

    size_t nsteps() override;
private:
    /// Generate the range of indices [start, stop] for a 3D vector at the current step.
    std::array<std::vector<size_t>, 2> vec3d_range();
    /// Generate the range of indices [start, stop] for multiple 3D vectors at the current step.
    std::array<std::vector<size_t>, 2> vec3d_n_range(size_t n);
    /// Read the unit cell at the current internal step, the file is assumed to
    /// be valid.
    UnitCell read_cell();
    /// Generic function to read an std::vector<Vector3D> at the current
    /// internal step, the file is assumed to be valid.
    void read_array(span<Vector3D> array, const std::string& name);

    /// Write an std::vector<Vector3D> to the file, as a variable with the name
    /// `name`, at the current internal step.
    void write_array(const std::vector<Vector3D>& array, const std::string& name);
    /// Write an UnitCell to the file, at the current internal step
    void write_cell(const UnitCell& cell);

    /// Associated NetCDF file.
    NcFile file_;
    /// Last read step
    size_t step_;
    /// Was the associated file validated?
    bool validated_;
};

template<> const FormatMetadata& format_metadata<Amber<AMBER_NC_RESTART>>();
template<> const FormatMetadata& format_metadata<Amber<AMBER_NC_TRAJECTORY>>();

} // namespace chemfiles

#endif
