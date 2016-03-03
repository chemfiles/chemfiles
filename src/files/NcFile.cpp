/* Chemfiles, an efficient IO library for chemistry file_ formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file_, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include "chemfiles/config.hpp"
#if HAVE_NETCDF

#include "chemfiles/files/NcFile.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/Logger.hpp"
using namespace chemfiles;

void chemfiles::check_nc_error(const std::string& message, int status) {
    if (status != NC_NOERR) {
        throw FileError(message + "\n    (NetCDF error is '" + nc_strerror(status) + "')");
    }
}

size_t chemfiles::hyperslab_size(const count_t& count) {
    size_t counted = 1;
    for (auto value: count) {
        counted *= value;
    }
    return counted;
}

NcFile::NcFile(const std::string& filename, const string& mode): BinaryFile(filename, mode), file_id_(-1), file_mode_(DATA) {
    int status = NC_NOERR;

    if (mode == "r"){
        status = nc_open(filename.c_str(), NC_NOWRITE, &file_id_);
    } else if (mode == "a"){
        status = nc_open(filename.c_str(), NC_WRITE, &file_id_);
    } else if (mode == "w"){
        status = nc_create(filename.c_str(), NC_64BIT_OFFSET | NC_CLASSIC_MODEL, &file_id_);
        // Put the file in DATA mode
        assert(nc_enddef(file_id_) == NC_NOERR);
    } else {
        throw FileError("Unknown mode for file opening: " + mode);
    }

    check_nc_error("Could not open the file '" + filename + "'", status);
}

NcFile::~NcFile() {
    int status = nc_close(file_id_);
    assert(status == NC_NOERR);
}

void NcFile::set_file_mode(FileMode mode) {
    if (mode == file_mode_) {
        return;
    }

    if (mode == DATA) {
        int status = nc_enddef(file_id_);
        check_nc_error("Could not change to data mode", status);
        file_mode_ = DATA;
    } else if (mode == DEFINE) {
        int status = nc_redef(file_id_);
        check_nc_error("Could not change to define mode", status);
        file_mode_ = DEFINE;
    }
}

NcFile::FileMode NcFile::file_mode() const {
    return file_mode_;
}

string NcFile::global_attribute(const string& name) const {
    size_t size = 0;
    int status = nc_inq_attlen(file_id_, NC_GLOBAL, name.c_str(), &size);
    check_nc_error("Can not read attribute '" + name + "'", status);

    string value(size, ' ');
    // &value[0] get a pointer to the first char in the string. In C++11, the string
    // storage must be contiguous, so we can use it here. value.c_str() returns a const
    // char *, and thus can not be used by nc_get_att_text.
    status = nc_get_att_text(file_id_, NC_GLOBAL, name.c_str(), &value[0]);
    check_nc_error("Can not read attribute '" + name + "'", status);

    return value;
}

void NcFile::add_global_attribute(const string& name, const string& value) {
    assert(file_mode() == DEFINE && "File must be in define mode to add attribute");
    int status = nc_put_att_text(file_id_, NC_GLOBAL, name.c_str(), value.size(), value.c_str());

    check_nc_error(
        "Could not add the \"" + name + "\" global attribute with value \"" + value,
        status
    );
}

size_t NcFile::dimension(const string& name) const {
    // Get the dimmension id
    netcdf_id_t dim_id = -1;
    int status = nc_inq_dimid(file_id_, name.c_str(), &dim_id);
    check_nc_error("Can not read dimmension '" + name + "'", status);

    // Get dimmension size
    size_t size = 0;
    status = nc_inq_dimlen(file_id_, dim_id, &size);
    check_nc_error("Can not read dimmension '" + name + "'", status);

    return size;
}

void NcFile::add_dimension(const string& name, size_t value) {
    assert(file_mode() == DEFINE && "File must be in define mode to add dimmension");
    netcdf_id_t dim_id = -1;
    int status = nc_def_dim(file_id_, name.c_str(), value, &dim_id);
    check_nc_error("Can not add dimension \"" + name + "\"", status);
}

void NcFile::sync() {
    assert(file_mode() == DATA && "File must be in data mode to sync");
    int status = nc_sync(file_id_);
    assert(status == NC_NOERR);
}

bool NcFile::variable_exists(const string& name) const {
    netcdf_id_t id;
    int status = nc_inq_varid(file_id_, name.c_str(), &id);
    return status == NC_NOERR;
}

#endif // HAVE_NETCDF
