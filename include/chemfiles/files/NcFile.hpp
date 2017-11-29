// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_NCFILE_HPP
#define CHEMFILES_NCFILE_HPP

#include <vector>
#include <string>

#include <netcdf.h>

#include "chemfiles/File.hpp"
#include "chemfiles/ErrorFmt.hpp"

namespace chemfiles {

class NcFile;

namespace nc {
    /// Check for netcdf return `status`. This will throw a `FileError` with the
    /// given `message` in case of error.
    template <typename... Args>
    inline void check(int status, const char *format, const Args & ... arguments) {
        if (status != NC_NOERR) {
            throw file_error("{}: {}", fmt::format(format, arguments...), nc_strerror(status));
        }
    }

    /// Maximum lenght for strings in variables
    const size_t STRING_MAXLEN = 10;

    /// NetCDF id type definition
    using netcdf_id_t = int;

    /// Count for variable stride and starting point
    using count_t = std::vector<size_t>;
    /// Get the number of elements in a NetCDF hyperslab with `count` elements.
    size_t hyperslab_size(const count_t& count);

    /// Wrapper around NetCDF variable
    class NcVariable {
    public:
        /// Create a new variable from `file` with id `var`.
        NcVariable(NcFile& file, netcdf_id_t var);
        virtual ~NcVariable() = default;
        NcVariable(const NcVariable&) = default;
        NcVariable& operator=(const NcVariable&) = default;
        NcVariable(NcVariable&&) = default;
        NcVariable& operator=(NcVariable&&) = default;

        /// Get the dimensions size for this variable
        std::vector<size_t> dimmensions() const;

        /// Get the attribute `name`.
        std::string attribute(const std::string& name) const;
        /// Add an attribute with the given `value` and `name`.
        void add_attribute(const std::string& name, const std::string& value);
    protected:
        netcdf_id_t file_id_;
        netcdf_id_t var_id_;
    };

    class NcFloat final: public NcVariable {
    public:
        NcFloat(NcFile& file, netcdf_id_t var) : NcVariable(file, var) {}
        /// Get `count` values starting at `start` from this variable
        std::vector<float> get(count_t start, count_t count) const;
        /// Add `cout` values from `data` starting at `start` in this variable
        void add(count_t start, count_t count, std::vector<float> data);
    };

    class NcChar final: public NcVariable {
    public:
        NcChar(NcFile& file, netcdf_id_t var) : NcVariable(file, var) {}
        /// Put a single string of data in this variable
        void add(const std::string& data);
        /// Put multiple strings of data in this variable
        void add(const std::vector<std::string>& data);
    };

    template<typename NcType> struct nc_type;
    template<> struct nc_type<NcFloat> {static constexpr auto value = NC_FLOAT;};
    template<> struct nc_type<NcChar> {static constexpr auto value = NC_CHAR;};
} // namespace nc

/// RAII wrapper around NetCDF 3 binary files
///
/// This interface only provide basic functionalities needed by the Amber NetCDF
/// format. All the operation are guaranteed to return a valid value or throw an
/// error.
///
/// The template functions are manually specialized for float and char data types.
class NcFile final: public File {
public:
    NcFile(const std::string& filename, File::Mode mode);
    ~NcFile() noexcept override;
    NcFile(NcFile&&) = default;
    NcFile& operator=(NcFile&&) = delete;
    NcFile(NcFile const&) = delete;
    NcFile& operator=(NcFile const&) = delete;

    /// Possible file mode. By default, files are in the DATA mode.
    enum NcMode {
        /// Files in DEFINE mode can have there attributes, dimmensions and variables
        /// modified, but no data can be read or written using NcVariable.
        DEFINE,
        /// Files in data mode acces read and write access to NcVariables.
        DATA,
    };
    /// Set the file mode for this file
    void set_nc_mode(NcMode mode);
    /// Get the file mode for this file
    NcMode nc_mode() const;

    /// Get the NetCDF id of this file.
    nc::netcdf_id_t netcdf_id() const {
        return file_id_;
    }

    /// Get a global string attribut from the file
    std::string global_attribute(const std::string& name) const;
    /// Create a global attribut in the file_
    void add_global_attribute(const std::string& name, const std::string& value);

    /// Get the value of a specific dimmension
    size_t dimension(const std::string& name) const;
    /// Get the value of an optional dimmension, or the default `value` if the
    /// dimmension is not in the file
    size_t optional_dimension(const std::string& name, size_t value) const;

    /// Create a dimmension with the specified value. If `value == NC_UNLIMITED`,
    /// then the dimension is infinite.
    void add_dimension(const std::string& name, size_t value =  NC_UNLIMITED);

    /// Check if a variable exists
    bool variable_exists(const std::string& name) const;
    /// Get a NetCDF variable
    template <typename NcType>
    NcType variable(const std::string& name) {
        auto var_id = nc::netcdf_id_t(-1);
        auto status = nc_inq_varid(file_id_, name.c_str(), &var_id);
        nc::check(status, "can not get variable id for '{}", name);
        return NcType(*this, var_id);
    }

    /// Create a new variable of type `NcType` with name `name` along the dimensions in
    /// `dims`. `dims` must be string or string-like values.
    template <typename NcType, typename ...Dims>
    NcType add_variable(const std::string& name, Dims... dims) {
        assert(nc_mode() == DEFINE && "File must be in define mode to add variable");

        auto dim_ids = get_dimmensions(dims...);
        auto var_id = nc::netcdf_id_t(-1);

        auto status = nc_def_var(
            // Variable file and name
            file_id_, name.c_str(),
            // Variable type
            nc::nc_type<NcType>::value,
            // Variable dimmensions
            sizeof...(dims), dim_ids.data(),
            &var_id
        );
        nc::check(status, "can not add variable '{}'", name);

        return NcType(*this, var_id);
    }

private:
    template <typename ...Dims>
    std::vector<nc::netcdf_id_t> get_dimmensions(Dims... dims) {
        std::vector<std::string> dimmensions = {dims...};
        std::vector<nc::netcdf_id_t> dim_ids;
        for (auto& name: dimmensions) {
            auto dim_id = nc::netcdf_id_t(-1);
            auto status = nc_inq_dimid(file_id_, name.c_str(), &dim_id);
            nc::check(status, "can not get dimmension id for '{}'", name);
            dim_ids.push_back(dim_id);
        }
        return dim_ids;
    }

    nc::netcdf_id_t file_id_ = -1;
    NcMode nc_mode_;
};

} // namespace chemfiles

#endif
