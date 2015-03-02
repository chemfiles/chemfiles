/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <cassert>

#include "files/NCFile.hpp"
#include "Error.hpp"
using namespace harp;
using std::string;

#if HAVE_NETCDF

NCFile::NCFile(const std::string& _filename, const string& mode) : BinaryFile(_filename),
file(NcFile(_filename.c_str())), error_behaviour(NcError::silent_nonfatal) {
    if (not file.is_valid()) {
        throw FileError("Could not open the file " + filename);
    }
}

string NCFile::global_attribute(const string& name) const {
    auto attr = file.get_att(name.c_str());
    if (attr == NULL || not attr->is_valid()){
        throw FileError("Invalid attribute " + name + " in file " + filename + ".");
    }
    string value = attr->as_string(0);
    return value;
}

size_t NCFile::dimmension(const string& name) const {
    auto dim = file.get_dim(name.c_str());
    if (dim == NULL || not dim->is_valid()){
        throw FileError("Invalid dimmension " + name + " in file " + filename + ".");
    }
    return static_cast<size_t>(dim->size());
}

NcVar* NCFile::variable(const string& name) const {
    NcVar* var = file.get_var(name.c_str());
    if (var == NULL || not var->is_valid()){
        throw FileError("Invalid variable " + name + " in file " + filename + ".");
    }
    return var;
}

string NCFile::s_attribute(const string& varname, const string& name) const {
    auto var = variable(varname.c_str());
    auto attr = var->get_att(name.c_str());
    if (attr == NULL || not attr->is_valid()){
        throw FileError("Invalid attribute " + name + " in file " + filename + ".");
    }
    string value = attr->as_string(0);
    return value;
}

float NCFile::f_attribute(const string& varname, const string& name) const {
    auto var = variable(varname.c_str());
    auto attr = var->get_att(name.c_str());
    if (attr == NULL || not attr->is_valid()){
        throw FileError("Invalid attribute " + name + " in file " + filename + ".");
    }
    float value = attr->as_float(0);
    return value;
}

bool NCFile::is_open(void) {
    return file.is_valid();
}


void NCFile::close(void) {
    file.close();
}

#endif // HAVE_NETCDF
