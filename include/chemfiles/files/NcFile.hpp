/* Chemfiles, an efficient IO library for chemistry file_ formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file_, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemfiles/config.hpp"
#if HAVE_NETCDF

#ifndef CHEMFILES_NCFILE_HPP
#define CHEMFILES_NCFILE_HPP

#include <vector>
#include <string>
#include <cassert>

#include <netcdf.h>
#include "chemfiles/File.hpp"

namespace chemfiles {
using std::string;

//! Maximum lenght for strings in variables
const size_t NC_STRING_MAXLEN = 10;

//! NetCDF id type definition
using netcdf_id_t = int;

//! Count for variable stride and starting point
using count_t = std::vector<size_t>;
//! Get the number of elements in an hyperslab with `count` elements.
size_t hyperslab_size(const count_t& count);

//! Check for netcdf return `status`. This will throw a `FileError` with the
//! given `message` in case of error.
void check_nc_error(const std::string& message, int status);

//! Enable a member function at compile-time if T and U are the same types
template<class T, class U>
using enable_if_same = typename std::enable_if<std::is_same<T, U>::value>::type;

class NcFile;

/*!
 * @class NcVariable NcFile.hpp NcFile.cpp
 * @brief RAII wrapper around NetCDF variable
 *
 * This class is manually instanciated for float and char types, which are the
 * only ones needed by chemfiles.
 */
template <typename NcType>
class NcVariable {
public:
    //! Create a new variable from `file` with id `var`.
    NcVariable(NcFile& file, netcdf_id_t var);

    //! Get the attribute `name`.
    std::string attribute(const string& name);

    //! Add an attribute with the given `value` and `name`.
    void add_attribute(const string& name, const string& value);

    //! Get `count` values starting at `start` from this variable
    template<typename T = NcType, typename unused = enable_if_same<T, float>>
    std::vector<float> get(count_t start, count_t count) const;

    //! Add `cout` values from `data` starting at `start` in this variable
    template<typename T = NcType, typename unused = enable_if_same<T, float>>
    void add(count_t start, count_t count, std::vector<float> data);

    //! Put a single string of data in this variable
    template<typename T = NcType, typename unused = enable_if_same<T, char>>
    void add(std::string data);

    //! Put multiple strings of data in this variable
    template<typename T = NcType, typename unused = enable_if_same<T, char>>
    void add(std::vector<const char*> data);

    //! Get the dimensions size for this variable
    std::vector<size_t> dimmensions() const;
private:
    NcFile& file_;
    netcdf_id_t var_id_;
};

//! Mapping between C++ and NetCDF data types
template<typename NcType> struct nc_type {};

template<> struct nc_type<float> {static constexpr auto value = NC_FLOAT;};
template<> struct nc_type<char> {static constexpr auto value = NC_CHAR;};

/*!
 * @class NcFile NcFile.hpp NcFile.cpp
 * @brief RAII wrapper around NetCDF 3 binary files
 *
 * This interface only provide basic functionalities needed by the Amber NetCDF
 * format. All the operation are guaranteed to return a valid value or throw an
 * error.
 *
 * The template functions are manually specialized for float and char data types.
 */
class NcFile final: public BinaryFile {
public:
    NcFile(const string& filename, const string& mode);
    ~NcFile();

    //! Possible file mode. By default, files are in the DATA mode.
    enum FileMode {
        //! Files in DEFINE mode can have there attributes, dimmensions and variables
        //! modified, but no data can be read or written using NcVariable.
        DEFINE,
        //! Files in data mode acces read and write access to NcVariables.
        DATA,
    };
    //! Set the file mode for this file
    void set_file_mode(FileMode mode);
    //! Get the file mode for this file
    FileMode file_mode() const;

    //! Get the NetCDF id of this file.
    netcdf_id_t netcdf_id() const {
        return file_id_;
    }

    //! Get a global string attribut from the file
    string global_attribute(const string& name) const;
    //! Create a global attribut in the file_
    void add_global_attribute(const string& name, const string& value);

    //! Get the value of a specific dimmension
    size_t dimension(const string& name) const;
    //! Create a dimmension with the specified value. If `value == NC_UNLIMITED`,
    //! then the dimension is infinite.
    void add_dimension(const string& name, size_t value =  NC_UNLIMITED);

    //! Check if a variable exists
    bool variable_exists(const string& name) const;
    //! Get a NetCDF variable
    template <typename NcType>
    NcVariable<NcType> variable(const string& name) {
        netcdf_id_t var_id = -1;
        int status = nc_inq_varid(file_id_, name.c_str(), &var_id);
        check_nc_error("Can not read variable '" + name + "'", status);
        return NcVariable<NcType>(*this, var_id);
    }

    //! Create a new variable of type `NcType` with name `name` along the dimensions in
    //! `dims`. `dims` must be string or string-like values.
    template <typename NcType, typename ...Dims>
    NcVariable<NcType> add_variable(const string& name, Dims... dims) {
        assert(file_mode() == DEFINE && "File must be in define mode to add variable");

        auto dim_ids = get_dimmensions(dims...);
        netcdf_id_t var_id = -1;

        auto status = nc_def_var(
            // Variable file and name
            file_id_, name.c_str(),
            // Variable type
            nc_type<NcType>::value,
            // Variable dimmensions
            sizeof...(dims), dim_ids.data(),
            &var_id
        );
        check_nc_error("Can not add variable '" + name + "'", status);

        return NcVariable<NcType>(*this, var_id);
    }

    // A NcFile will always be in a consistent mode
    virtual bool is_open() override {return true;}
    virtual void sync() override;
private:
    template <typename ...Dims>
    std::vector<netcdf_id_t> get_dimmensions(Dims... dims) {
        std::vector<std::string> dimmensions = {dims...};
        std::vector<netcdf_id_t> dim_ids;
        for (auto& name: dimmensions) {
            netcdf_id_t dim_id = -1;
            auto status = nc_inq_dimid(file_id_, name.c_str(), &dim_id);
            check_nc_error("Can not get dimmension '" + name + "'", status);
            dim_ids.push_back(dim_id);
        }
        return dim_ids;
    }

    netcdf_id_t file_id_;
    FileMode file_mode_;
};

/******************************************************************************/

template <typename NcType>
NcVariable<NcType>::NcVariable(NcFile& file, netcdf_id_t var): file_(file), var_id_(var) {}

template <typename NcType>
string NcVariable<NcType>::attribute(const string& name) {
    size_t size = 0;
    int status = nc_inq_attlen(file_.netcdf_id(), var_id_, name.c_str(), &size);
    check_nc_error("Can not read attribute id '" + name + "'", status);

    string value(size, ' ');
    status = nc_get_att_text(file_.netcdf_id(), var_id_, name.c_str(), &value[0]);
    check_nc_error("Can not read attribute text '" + name + "'", status);
    return value;
}

template <typename NcType>
void NcVariable<NcType>::add_attribute(const string& name, const string& value) {
    assert(file_.file_mode() == NcFile::DEFINE && "File must be in define mode to add attribute");
    int status = nc_put_att_text(file_.netcdf_id(), var_id_, name.c_str(), value.size(), value.c_str());
    check_nc_error("Can not set attribute'" + name + "'", status);
}

template<> template<typename T, typename U>
std::vector<float> NcVariable<float>::get(count_t start, count_t count) const {
    auto size = hyperslab_size(count);
    auto result = std::vector<float>(size, 0.0);
    int status = nc_get_vara_float(
        file_.netcdf_id(), var_id_,
        start.data(), count.data(),
        result.data()
    );
    check_nc_error("Could not read variable", status);
    return result;
}

template<> template<typename T, typename U>
void NcVariable<float>::add(count_t start, count_t count, std::vector<float> data) {
    assert(data.size() == hyperslab_size(count));
    int status = nc_put_vara_float(
        file_.netcdf_id(), var_id_,
        start.data(), count.data(),
        data.data()
    );
    check_nc_error("Could not put data in variable", status);
}

template<> template<typename T, typename U>
void NcVariable<char>::add(std::string data) {
    int status = nc_put_var_text(file_.netcdf_id(), var_id_, data.c_str());
    check_nc_error("Could not put text data in variable", status);
}

template<> template<typename T, typename U>
void NcVariable<char>::add(std::vector<const char*> data) {
    size_t i = 0;
    for (auto string: data) {
        size_t start[] = {i, 0};
        size_t count[] = {1, NC_STRING_MAXLEN};
        int status = nc_put_vara_text(
            file_.netcdf_id(), var_id_,
            start, count,
            string
        );
        check_nc_error("Could not put vector text data in variable", status);
        i++;
    }

}

template<typename NcType>
std::vector<size_t> NcVariable<NcType>::dimmensions() const {
    int size = 0;
    int status = nc_inq_varndims(file_.netcdf_id(), var_id_, &size);
    check_nc_error("Could not get the number of dimmensions", status);

    auto dim_ids = std::vector<netcdf_id_t>(static_cast<size_t>(size), 0);
    status = nc_inq_vardimid(file_.netcdf_id(), var_id_, dim_ids.data());
    check_nc_error("Could not get the dimmensions id", status);

    std::vector<size_t> result;
    for (auto dim_id: dim_ids) {
        size_t length = 0;
        status = nc_inq_dimlen(file_.netcdf_id(), dim_id, &length);
        check_nc_error("Could not get the dimmensions size", status);
        result.push_back(length);
    }
    return result;
}

} // namespace chemfiles

#endif
#endif // HAVE_NETCDF
