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

#include "netcdfcpp.h"

namespace harp {

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
    explicit NCFile(const std::string& filename, const std::string& = "");
    ~NCFile(){}

    //! Get a global attribut from the file
    std::string global_attribute(const std::string& name) const;
    //! Get the value of a specific dimmension
    size_t dimmension(const std::string& name) const;
    //! Get a valid pointer to a NetCDF variable
    NcVar* variable(const std::string& name) const;
    //! Get a string attribute from a variable
    std::string s_attribute(const std::string& variable, const std::string& name) const;
    //! Get a float attribute from a file
    float f_attribute(const std::string& variable, const std::string& name) const;

    virtual bool is_open(void);
    virtual void close(void);
private:
    // Underlying NetCDF file
    NcFile file;
    // Behaviour on error
    NcError error_behaviour;
};

} // namespace harp

#endif

#endif // HAVE_NETCDF
