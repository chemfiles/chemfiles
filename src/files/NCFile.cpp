/* Chemfiles, an efficient IO library for chemistry file_ formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file_, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include "chemfiles/config.hpp"
#if HAVE_NETCDF

#include <cassert>

#include "chemfiles/files/NCFile.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/Logger.hpp"
using namespace chemfiles;
using namespace netCDF;
using std::string;

NCFile::NCFile(const std::string& filename, const string& mode): BinaryFile(filename, mode){
    try {
        if (mode == "r"){
            file_.open(filename, NcFile::read);
        } else if (mode == "a"){
            file_.open(filename, NcFile::write, NcFile::classic64);
        } else if (mode == "w"){
            file_.open(filename, NcFile::replace, NcFile::classic64);
        } else {
            throw FileError("Unknown mode for file_ opening: " + mode);
        }
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Could not open the file_ " + filename + ".\n   " + e.what());
    }
}

string NCFile::global_attribute(const string& attname) const {
    string value;
    try {
        auto attr = file_.getAtt(attname);
        if(attr.isNull()) {
            FileError("Invalid attribute " + attname + ".");
        }
        attr.getValues(value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read attribute " + attname + ".\n" + e.what());
    }
    return value;
}

size_t NCFile::dimension(const string& dimname) const {
    size_t size;
    try {
        auto dim = file_.getDim(dimname.c_str());
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
        var = file_.getVar(varname);
        if(var.isNull()) {
            FileError("Invalid variable " + varname + ".");
        }
        var.getName(); // Force error when the var is built with a bad ncid
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not read variable " + varname + ".\n" + e.what());
    }
    return var;
}

/******************************************************************************/

void NCFile::add_global_attribute(const string& attname, const string& value) {
    try {
        file_.putAtt(attname, value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Could not add the \"" + attname + "\" global attribute " +
                        "with value \"" + value + "\".\n" + e.what());
    }
}

void NCFile::add_dimension(const string& dimname, size_t value) {
    try {
        if (value == static_cast<size_t>(-1) )
            file_.addDim(dimname);
        else
            file_.addDim(dimname, value);
    } catch (const netCDF::exceptions::NcException& e) {
        throw FileError("Can not add dimension \"" + dimname + "\"" + e.what());
    }
}

bool NCFile::is_open() {
    return (not file_.isNull());
}

void NCFile::sync() {
    int res = nc_enddef(file_.getId());
    assert(res == NC_NOERR && "Got a bad NetCDF id.");
    res = nc_sync(file_.getId());
    assert(res == NC_NOERR && "Got a bad NetCDF id.");
    if (mode() == "a" || mode() == "w") {
        res = nc_redef(file_.getId());
        assert(res == NC_NOERR && "Error in nc_redef!");
    }
}

#endif // HAVE_NETCDF
