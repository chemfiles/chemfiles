// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_XDR_FILE_HPP
#define CHEMFILES_XDR_FILE_HPP

#include <string>
#include <vector>

#include "chemfiles/files/BinaryFile.hpp"

namespace chemfiles {

/// Partial implementation of XDR according to RFC 4506
/// (see: https://datatracker.ietf.org/doc/html/rfc4506)
/// Including additional helper routines for GROMACS
class XDRFile final : public BigEndianFile {
  public:
    XDRFile(std::string path, File::Mode mode);
    ~XDRFile() = default;

    /// Read an XDR signed integer
    int32_t read_int() { return read_single_i32(); }
    /// Write an XDR signed integer
    void write_int(int32_t value) { write_single_i32(value); }

    /// Read an XDR unsigned integer
    uint32_t read_uint() { return read_single_u32(); }
    /// Write an XDR unsigned integer
    void write_uint(uint32_t value) { write_single_u32(value); }

    /// Read an XDR double
    double read_double() { return read_single_f64(); }

    /// Read an XDR float
    float read_float() { return read_single_f32(); }
    /// Write an XDR float
    void write_float(float value) { write_single_f32(value); }

    /// Read XDR doubles into an allocated vector
    void read_double_array(std::vector<double>& data) { read_f64(data.data(), data.size()); }

    /// Read XDR floats into an allocated vector
    void read_float_array(std::vector<float>& data) { read_f32(data.data(), data.size()); }
    /// Write XDR floats
    void write_float_array(const std::vector<float> data) { write_f32(data.data(), data.size()); }

    /// Read a non-compliant GROMACS string
    /// A GROMACS string stores the length of the string including the NULL
    /// terminator as int32 before the XDR compliant string data without the
    /// terminator. An XDR string has the same representation as opaque data.
    std::string read_gmx_string();
    /// Write a non-compliant GROMACS string
    void write_gmx_string(const std::string& value);

    /// Read compressed GROMACS floats and returns the precision
    float read_gmx_compressed_floats(std::vector<float>& data);
    /// Write compressed GROMACS floats with a given precision
    void write_gmx_compressed_floats(const std::vector<float>& data, float precision);

  private:
    /// Read XDR variable-length opaque data
    void read_opaque(std::vector<char>& data);
    /// Write XDR variable-length opaque data
    void write_opaque(const char* data, uint32_t count);
    /// Cache allocation for compressed data (XTC)
    std::vector<char> compressed_data_;
    /// Cache allocation for intermediate buffer (XTC)
    std::vector<int> intbuf_;
};

} // namespace chemfiles

#endif
