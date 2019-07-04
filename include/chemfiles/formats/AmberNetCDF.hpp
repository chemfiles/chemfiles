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
template <class T> class span;

/// [Amber NetCDF][NetCDF] file format reader.
///
/// [NetCDF]: http://ambermd.org/netcdf/nctraj.xhtml
class AmberNetCDFFormat final: public Format {
public:
    AmberNetCDFFormat(std::string path, File::Mode mode, File::Compression compression);

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;

    size_t nsteps() override;
private:
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

template<> FormatInfo format_information<AmberNetCDFFormat>();

} // namespace chemfiles

#endif
