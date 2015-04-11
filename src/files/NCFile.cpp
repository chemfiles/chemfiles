/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <cassert>
#include <vector>

#include "files/NCFile.hpp"
#include "Error.hpp"
#include "Logger.hpp"
using namespace harp;
using namespace netCDF;
using std::string;

#if HAVE_NETCDF

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

string NCFile::global_attribute(const string& name) const {
    string value;
    try {
        auto attr = file.getAtt(name);
        if(attr.isNull())
            FileError("Invalid attribute " + name + ".");
        attr.getValues(value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read attribute " + name + ".\n" + e.what());
    }
    return value;
}

size_t NCFile::dimmension(const string& name) const {
    size_t size;
    try {
        auto dim = file.getDim(name.c_str());
        if(dim.isNull())
           FileError("Invalid dimension " + name + ".");
        size = dim.getSize();
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read dimmension " + name + ".\n" + e.what());
    }
    return size;
}

NcVar NCFile::variable(const string& name) const {
    NcVar var;
    try {
        var = file.getVar(name);
        if(var.isNull())
            FileError("Invalid variable " + name + ".");
        var.getName(); // Force error when the var is built with a bad ncid
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read variable " + name + ".\n" + e.what());
    }
    return var;
}

string NCFile::s_attribute(const string& varname, const string& name) const {
    auto var = variable(varname);
    string value;
    try {
        auto attr = var.getAtt(name);
        if(attr.isNull())
            FileError("Invalid attribute " + name + ".");
        attr.getValues(value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read attribute " + name + ".\n     " + e.what());
    }
    return value;
}

float NCFile::f_attribute(const string& varname, const string& name) const {
    auto var = variable(varname.c_str());
    float value;
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

/******************************************************************************/

void NCFile::add_global_attribute(const string& _name, const string& value) {
    try {
        file.putAtt(_name, value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Could not add the \"" + _name + "\" global attribute " +
                        "with value \"" + value + "\".\n" + e.what());
    }
}

void NCFile::add_dimmension(const string& name, size_t value) {
    try {
        if (value == static_cast<size_t>(-1) )
            file.addDim(name);
        else
            file.addDim(name, value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not add dimension \"" + name + "\"" + e.what());
    }
}

void NCFile::add_f_variable(const string& name, const string& dim_i, const string& dim_j) {
    try {
        auto dims = std::vector<NcDim>();

        dims.push_back(file.getDim(dim_i));
        if (dims.back().isNull())
            throw FileError("Can not get dimensions \"" + dim_i + "\".");

        dims.push_back(file.getDim(dim_j));
        if (dims.back().isNull())
            throw FileError("Can not get dimensions \"" + dim_j + "\".");

        file.addVar(name, ncFloat, dims);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not add variable \"" + name + "\".\n" + e.what());
    }
}

void NCFile::add_s_attribute(const string& var, const string& name, const string& value) {
    auto ncvar = variable(var);
    try {
        ncvar.putAtt(name, value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not add atribute \"" + name + "\".\n" + e.what());
    }
}

void NCFile::add_f_attribute(const string& var, const string& name, float value) {
    auto ncvar = variable(var);
    try {
        ncvar.putAtt(name, ncFloat, value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not add atribute \"" + name + "\".\n" + e.what());
    }
}

bool NCFile::is_open(void) {
    return (not file.isNull());
}

void NCFile::close(void) {
    file.close();
}

#endif // HAVE_NETCDF
