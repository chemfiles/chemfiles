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

#include "files/NCFile.hpp"
#include "Error.hpp"
#include "Logger.hpp"
using namespace harp;
using namespace netCDF;
using std::string;

NCFile::NCFile(const std::string& _filename, const string& mode): BinaryFile(_filename){
    try {
        if (mode == "r"){
            file.open(_filename, NcFile::read);
        }
        else if (mode == "w"){
            file.open(_filename, NcFile::replace, NcFile::classic64);
        }
        else
            throw FileError("Unknown mode for file opening: " + mode);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Could not open the file " + filename + ".\n   " + e.what());
    }
}

string NCFile::global_attribute(const string& attname) const {
    string value;
    try {
        auto attr = file.getAtt(attname);
        if(attr.isNull())
            FileError("Invalid attribute " + attname + ".");
        attr.getValues(value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read attribute " + attname + ".\n" + e.what());
    }
    return value;
}

size_t NCFile::dimmension(const string& dimname) const {
    size_t size;
    try {
        auto dim = file.getDim(dimname.c_str());
        if(dim.isNull())
           FileError("Invalid dimension " + dimname + ".");
        size = dim.getSize();
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read dimmension " + dimname + ".\n" + e.what());
    }
    return size;
}

NcVar NCFile::variable(const string& varname) const {
    NcVar var;
    try {
        var = file.getVar(varname);
        if(var.isNull())
            FileError("Invalid variable " + varname + ".");
        var.getName(); // Force error when the var is built with a bad ncid
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read variable " + varname + ".\n" + e.what());
    }
    return var;
}

/******************************************************************************/

void NCFile::add_global_attribute(const string& attname, const string& value) {
    try {
        file.putAtt(attname, value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Could not add the \"" + attname + "\" global attribute " +
                        "with value \"" + value + "\".\n" + e.what());
    }
}

void NCFile::add_dimmension(const string& dimname, size_t value) {
    try {
        if (value == static_cast<size_t>(-1) )
            file.addDim(dimname);
        else
            file.addDim(dimname, value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not add dimension \"" + dimname + "\"" + e.what());
    }
}

bool NCFile::is_open(void) {
    return (not file.isNull());
}

void NCFile::close(void) {
    file.close();
}

#endif // HAVE_NETCDF
