/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_NCFILE_HPP
#define HARP_NCFILE_HPP

#include <vector>
#include <string>

#include "File.hpp"

#include <netcdf>

namespace harp {
    using std::string;

/*!
 * @class NCFile NCFile.hpp NCFile.cpp
 * @brief Wrapper around NetCDF 3 binary files
 *
 * This interface only provide basic functionalities needed by the Amber NetCDF
 * format. This interface is read-only for now. All the operation are guaranteed
 * to return a valid value or throw an error.
 */
class NCFile : public BinaryFile {
public:
    explicit NCFile(const string& filename, const string& mode = "r");
    ~NCFile(){}

    //! Get a global attribut from the file
    string global_attribute(const string& name) const;
    //! Get the value of a specific dimmension
    size_t dimension(const string& name) const;
    //! Get a valid pointer to a NetCDF variable
    netCDF::NcVar variable(const string& name) const;
    //! Get an attribute of type \c T and name \c name from the variable of name \c var
    template <typename T>
    T attribute(const string& var, const string& name) const;

    //! Create a global attribut in the file
    void add_global_attribute(const string& name, const string& value);
    //! Create a dimmension with the specified value. If \c value == -1, then
    //! the dimension is infinite
    void add_dimension(const string& name, size_t value = static_cast<size_t>(-1));
    //! Create a new variable of type \c T with name \c name along the dimensions in \c dims
    template <typename T, class ...Dims>
    void add_variable(const string& name, Dims... dims);
    //! Add an attribute of type \c T and name \c name to the variable with name \c var
    template <typename T>
    void add_attribute(const string& var, const string& name, T value);

    virtual bool is_open(void) override;
    virtual void close(void) override;
private:
    // Underlying NetCDF file
    netCDF::NcFile file;
};

//! Get the NetCDF type associated to a c++ type
template <typename T> inline netCDF::NcType::ncType get_nctype() {
    throw FileError("Can not convert this type to NetCDF type.");
}

//! Get the NetCDF type associated to a c++ type: float -> nc_FLOAT
template <> inline netCDF::NcType::ncType get_nctype<float>() {
    return netCDF::NcType::nc_FLOAT;
}

//! Get the NetCDF type associated to a c++ type: char -> nc_CHAR
template <> inline netCDF::NcType::ncType get_nctype<char>() {
    return netCDF::NcType::nc_CHAR;
}

template <typename T>
inline T NCFile::attribute(const string& varname, const string& name) const {
    auto var = variable(varname);
    T value;
    try {
        auto attr = var.getAtt(name);
        if(attr.isNull())
            FileError("Invalid attribute " + name + ".");
        attr.getValues(&value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read attribute " + name + ".\n     " + e.what());
    }
    return value;
}

template <>
inline std::string NCFile::attribute(const string& varname, const string& attname) const {
    auto var = variable(varname);
    std::string value;
    try {
        auto attr = var.getAtt(attname);
        if(attr.isNull())
            FileError("Invalid attribute " + attname + ".");
        attr.getValues(value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read attribute " + attname + ".\n     " + e.what());
    }
    return value;
}

template <typename T, class ...Dims>
void NCFile::add_variable(const string& varname, Dims... dims) {
    try {
        auto dimensions = std::vector<netCDF::NcDim>();
        auto dim_names = std::vector<std::string>{dims...};

        for (auto name : dim_names){
            dimensions.push_back(file.getDim(name));
            if (dimensions.back().isNull())
                throw FileError("Can not get dimensions \"" + name + "\".");
        }

        file.addVar(varname, get_nctype<T>(), dimensions);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not add variable \"" + varname + "\".\n" + e.what());
    }
}

template <typename T>
inline void NCFile::add_attribute(const string& varname, const string& attname, T value) {
    auto ncvar = variable(varname);
    try {
        ncvar.putAtt(attname, get_nctype<T>(), value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not add atribute \"" + attname + "\".\n" + e.what());
    }
}

template <>
inline void NCFile::add_attribute(const string& varname, const string& attname, const char* value) {
    auto ncvar = variable(varname);
    try {
        ncvar.putAtt(attname, std::string(value));
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not add atribute \"" + attname + "\".\n" + e.what());
    }
}

} // namespace harp

#endif
