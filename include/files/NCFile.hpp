/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "config.hpp"
#if HAVE_NETCDF

#ifndef HARP_NCFILE_HPP
#define HARP_NCFILE_HPP

#include <string>

#include "files/File.hpp"

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
    size_t dimmension(const string& name) const;
    //! Get a valid pointer to a NetCDF variable
    netCDF::NcVar variable(const string& name) const;
    //! Get a string attribute from a variable
    string s_attribute(const string& var, const string& name) const;
    //! Get a float attribute from a file
    float f_attribute(const string& var, const string& name) const;

    //! Create a global attribut in the file
    void add_global_attribute(const string& name, const string& value);
    //! Create a dimmension with the specified value. If \c value == -1, then
    //! the dimension is infinite
    void add_dimmension(const string& name, size_t value = static_cast<size_t>(-1));
    //! Create a new float variable, with dimension \c dim_i x \c dim_j
    void add_f_variable(const string& name, const string& dim_i, const string& dim_j);
    //! Add a string attribute to a variable
    void add_s_attribute(const string& var, const string& name, const string& value);
    //! Add a float attribute to a file
    void add_f_attribute(const string& var, const string& name, float value);

    virtual bool is_open(void);
    virtual void close(void);
private:
    // Underlying NetCDF file
    netCDF::NcFile file;
};

} // namespace harp

#endif

#endif // HAVE_NETCDF
