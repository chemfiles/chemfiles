// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_NETCDF3_FILE_HPP
#define CHEMFILES_NETCDF3_FILE_HPP

// This file contains an implementation of netcdf3 format without using the
// unidata netcdf-c library. This reduces the amount of code included, lowering
// chemfiles overall size (this is mostly important in a WASM context); and is
// significantly faster as well. Only the subset of NetCDF 3 format required for
// Amber convention is supported. This implementation is inspired by
// scipy.io.netcdf_file.

#include <cstdio>
#include <cstdint>

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "chemfiles/external/optional.hpp"
#include "chemfiles/files/BinaryFile.hpp"

namespace chemfiles {
namespace netcdf3 {

namespace constants {
    // these values are taken from the main netcdf-c header, and match the
    // specification at
    // https://cdn.earthdata.nasa.gov/conduit/upload/496/ESDS-RFC-011v2.00.pdf.
    //
    // This first set of value represent the different data types the can be
    // stored in a netcdf3 file
    constexpr int32_t NC_BYTE = 1;
    constexpr int32_t NC_CHAR = 2;
    constexpr int32_t NC_SHORT = 3;
    constexpr int32_t NC_INT = 4;
    constexpr int32_t NC_FLOAT = 5;
    constexpr int32_t NC_DOUBLE = 6;

    // These values are used as marker to indicate the kind of data in the
    // following block in the file
    constexpr int32_t NC_DIMENSION = 10;
    constexpr int32_t NC_VARIABLE = 11;
    constexpr int32_t NC_ATTRIBUTE = 12;

    // these values come from the spec, and are used to fill data which was not
    // given by the user
    constexpr int8_t  NC_FILL_BYTE   = -127;
    constexpr char    NC_FILL_CHAR   = 0;
    constexpr int16_t NC_FILL_SHORT  = -32767;
    constexpr int32_t NC_FILL_INT    = -2147483647;
    constexpr float   NC_FILL_FLOAT  = 9.9692099683868690e+36f;
    constexpr double  NC_FILL_DOUBLE = 9.9692099683868690e+36;
}

class Netcdf3File;

/// Tagged enum representation for the variables or global attributes
class Value {
public:
    Value(char value): byte_(value), kind_(kind::BYTE) {}

    Value(int16_t value): short_(value), kind_(kind::SHORT) {}

    Value(int32_t value): int_(value), kind_(kind::INT) {}

    Value(float value): float_(value), kind_(kind::FLOAT) {}

    Value(double value): double_(value), kind_(kind::DOUBLE) {}

    Value(std::string value): string_(std::move(value)), kind_(kind::STRING) {}

    ~Value() {
        if (kind_ == kind::STRING) {
            using std::string;
            string_.~string();
        }
    }

    Value(Value&& other): Value(0) {
        *this = std::move(other);
    }

    Value& operator=(Value&& other);

    Value(const Value& other): Value(0) {
        *this = other;
    }

    Value& operator=(const Value& other);

    optional<const std::string&> as_string() const {
        if (kind_ == kind::STRING) {
            return string_;
        } else {
            return nullopt;
        }
    }

    optional<int8_t> as_i8() const {
        if (kind_ == kind::BYTE) {
            return byte_;
        } else {
            return nullopt;
        }
    }

    optional<int16_t> as_i16() const {
        if (kind_ == kind::SHORT) {
            return short_;
        } else {
            return nullopt;
        }
    }

    optional<int32_t> as_i32() const {
        if (kind_ == kind::INT) {
            return int_;
        } else {
            return nullopt;
        }
    }

    optional<float> as_f32() const {
        if (kind_ == kind::FLOAT) {
            return float_;
        } else {
            return nullopt;
        }
    }

    optional<double> as_f64() const {
        if (kind_ == kind::DOUBLE) {
            return double_;
        } else {
            return nullopt;
        }
    }

private:
    enum class kind {
        BYTE,
        SHORT,
        INT,
        FLOAT,
        DOUBLE,
        STRING,
    };

    union {
        /// char or byte
        int8_t byte_;
        /// short
        int16_t short_;
        /// int
        int32_t int_;
        /// float
        float float_;
        /// double
        double double_;
        /// string
        std::string string_;
    };
    kind kind_;
};


/// A single dimension for a variable.
struct Dimension {
    Dimension(std::string name_, int32_t size_, bool is_record_):
        name(std::move(name_)), size(size_), is_record(is_record_) {}

    /// Name of the dimension
    std::string name;
    /// size of the dimension
    int32_t size;
    /// Is this dimension the optional record (i.e. infinite) dimension?
    bool is_record;
};

struct VariableLayout {
    /// NetCDF type identifier
    int32_t type;
    /// non record variables: the size (in bytes) of the full array
    /// record variables: size in bytes of a single entry
    int32_t size;
    /// Offset in the file of the first byte in this variable
    int64_t begin;

    /// size of a single value of the current type
    size_t size_for_type() const;
    /// non record variables: number of values in the full array
    /// record variables: number of values in a single entry
    size_t count() const;
    /// User-facing name for the variable type
    std::string type_name() const;
};

/// Base class used to define both `Variable` and `RecordVariable`.
class BaseVariable {
public:
    /// Get all attributes for this variable
    const std::map<std::string, Value>& attributes() const {
        return attributes_;
    }

    /// Get the attribute with the given `name` for this variable, throwing
    /// an exception if it does not exist.
    const Value& attribute(const std::string& name) const {
        return attributes_.at(name);
    }

    /// Shape of this variable
    std::vector<size_t> shape() const;

    /// Get the type of this variable as one of the values in
    /// `chemfiles::netcdf3::constants`.
    int32_t type() const {
        return layout_.type;
    }

protected:
    friend class Netcdf3File;
    BaseVariable(
        Netcdf3File& file,
        std::vector<std::shared_ptr<Dimension>> dimensions,
        std::map<std::string, Value> attributes,
        VariableLayout layout
    );

    std::reference_wrapper<Netcdf3File> file_;
    std::vector<std::shared_ptr<Dimension>> dimensions_;
    std::map<std::string, Value> attributes_;

    VariableLayout layout_;
};

/// A single variable in a NetCDF3 file. None of the dimension for this variable
/// are a record (infinite) dimension.
class Variable: public BaseVariable {
public:
    /// read this variable as 16-bit integer, filling the given vector
    ///
    /// @throws if the vector is too small to fit all values for this variable
    void read_i16(std::vector<int16_t>& data);
    /// read this variable as 16-bit integer, writing data from `data` to `data
    /// + count`
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_i16(int16_t* data, size_t count);

    /// read this variable as 32-bit integer, filling the given vector
    ///
    /// @throws if the vector is too small to fit all values for this variable
    void read_i32(std::vector<int32_t>& data);
    /// read this variable as 32-bit integer, writing data from `data` to `data
    /// + count`
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_i32(int32_t* data, size_t count);

    /// read this variable as 32-bit floating point number, filling the given
    /// vector
    ///
    /// @throws if the vector is too small to fit all values for this variable
    void read_f32(std::vector<float>& data);
    /// read this variable as 32-bit floating point number, writing data from
    /// `data` to `data + count`
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_f32(float* data, size_t count);

    /// read this variable as 64-bit floating point number, filling the given
    /// vector
    ///
    /// @throws if the vector is too small to fit all values for this variable
    void read_f64(std::vector<double>& data);
    /// read this variable as 64-bit floating point number, writing data from
    /// `data` to `data + count`
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_f64(double* data, size_t count);

private:
    friend class Netcdf3File;
    Variable(BaseVariable base);
};

/// A single variable in a NetCDF3 file. One of the dimension for this variable
/// is a record (infinite) dimension.
class RecordVariable: public BaseVariable {
public:
    /// read this variable as 16-bit integer at the given `step`, filling the
    /// given vector
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_i16(size_t step, std::vector<int16_t>& data);
    /// read this variable as 32-bit integer at the given `step`, writing data
    /// from `data` to `data + count`
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_i16(size_t step, int16_t* data, size_t count);

    /// read this variable as 32-bit integer at the given `step`, filling the
    /// given vector
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_i32(size_t step, std::vector<int32_t>& data);
    /// read this variable as 32-bit integer at the given `step`, writing data
    /// from `data` to `data + count`
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_i32(size_t step, int32_t* data, size_t count);

    /// read this variable as 32-bit floating point number at the given `step`,
    /// filling the given vector
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_f32(size_t step, std::vector<float>& data);
    /// read this variable as 32-bit floating point number at the given `step`,
    /// writing data from `data` to `data + count`
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_f32(size_t step, float* data, size_t count);

    /// read this variable as 64-bit floating point number at the given `step`,
    /// filling the given vector
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_f64(size_t step, std::vector<double>& data);
    /// read this variable as 64-bit floating point number at the given `step`,
    /// writing data from `data` to `data + count`
    ///
    /// @throws if `count` is too small to fit all values for this variable
    void read_f64(size_t step, double* data, size_t count);

private:
    /// the size of a full record, containing a single entry for all record
    /// variable
    uint64_t record_size_;
    /// current number of records in the file
    size_t record_length_;

    friend class Netcdf3File;
    RecordVariable(BaseVariable base);
};


/// TODO: more docs
///
/// NetCDF 3 files are guaranteed to be stored as big endian, regardless of the
/// native system endianess.
class Netcdf3File: public BigEndianFile {
public:
    Netcdf3File(std::string filename, File::Mode mode);

    // disable moving/copying Netcdf3File since Variable instances take a
    // pointer to the file
    Netcdf3File(Netcdf3File&&) = delete;
    Netcdf3File(const Netcdf3File&) = delete;
    Netcdf3File& operator=(Netcdf3File&&) = delete;
    Netcdf3File& operator=(const Netcdf3File&) = delete;

    /// Get all global attribute for this file
    const std::map<std::string, Value>& attributes() const {
        return attributes_;
    }

    /// Get the global attribute with the given `name` for this file, throwing
    /// a `FileError` if it does not exist
    const Value& attribute(const std::string& name) const;

    /// Get all fixed size variables defined in this file
    const std::map<std::string, Variable>& variables() const {
        return variables_;
    }

    /// Get the fixed size variable with the given name in this file, throwing
    /// a `FileError` if it does not exist
    Variable& variable(const std::string& name);

    /// Get all record variables defined in this file
    const std::map<std::string, RecordVariable>& record_variables() const {
        return record_variables_;
    }

    /// Get the record variable with the given name in this file, throwing
    /// a `FileError` if it does not exist
    RecordVariable& record_variable(const std::string& name);

    /// Get all the dimensions defined in this file
    const std::vector<std::shared_ptr<Dimension>>& dimensions() const {
        return dimensions_;
    }

private:
    void skip_padding(size_t count);
    std::string read_pascal_string();

    std::map<std::string, Value> read_attributes();
    Value read_attribute_value();

    struct variable_list {
        /// record variables
        std::map<std::string, RecordVariable> records;
        /// non-record variables
        std::map<std::string, Variable> globals;
    };

    variable_list read_variable_list();

    /// current number of records in the file
    int32_t record_length_;
    std::vector<std::shared_ptr<Dimension>> dimensions_;
    std::map<std::string, Value> attributes_;
    std::map<std::string, Variable> variables_;
    std::map<std::string, RecordVariable> record_variables_;
};

} // namespace netcdf3
} // namespace chemfiles

#endif
