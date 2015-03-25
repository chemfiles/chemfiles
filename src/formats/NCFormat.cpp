/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include "formats/NCFormat.hpp"

#include "Error.hpp"
#include "Logger.hpp"
#include "Frame.hpp"
#include "files/NCFile.hpp"
using namespace harp;

#include <algorithm>
#include <iostream>
#include <algorithm>
using std::endl;
using std::vector;

#if HAVE_NETCDF

std::string NCFormat::description() const {
    return "Amber NetCDF file format.";
}

// Register the Amber NetCDF format with the ".nc" extension and the
// "AmberNetCDF" description.
REGISTER_WITH_FILE(NCFormat, "AmberNetCDF", NCFile);
REGISTER_EXTENSION_AND_FILE(NCFormat, ".nc", NCFile);

NCFormat::NCFormat() : step(0), last_file_hash(0), last_file_was_valid(false) {}

static bool is_valid(NCFile* file){
    bool validity=true;
    if (file->global_attribute("Conventions") != "AMBER"){
        LOG(ERROR) << "We can only read AMBER convention NetCDF files." << endl;
        validity = false;
    }

    if (file->global_attribute("ConventionVersion") != "1.0"){
        LOG(ERROR) << "We can only read version 1.0 of AMBER convention NetCDF files." << endl;
        validity = false;
    }

    if (file->dimmension("spatial") != 3){
        LOG(ERROR) << "Wrong size for spatial dimmension. Should be 3, is "
                   << file->dimmension("spatial") << "." << endl;
        validity = false;
    }

    if (not validity){
        LOG(ERROR) << "Invalid Amber NetCDF file." << std::endl;
        try {
            auto program = file->global_attribute("program");
            auto version = file->global_attribute("programVersion");
            LOG(WARNING) << "File was writen by " << program << " version " << version << endl;
        } catch (...) {} // Do nothing, we are already trying to recover from an error
    }

    return validity;
}

void NCFormat::read_at_step(File* file, const size_t _step, Frame& frame){
    auto ncfile = dynamic_cast<NCFile*>(file);
    // Using the pointer adress before I come up with a better hash function
    auto file_hash = reinterpret_cast<size_t>(file);
    if (last_file_hash != file_hash){
        last_file_hash = file_hash;
        last_file_was_valid = is_valid(ncfile);
    }
    if (not last_file_was_valid)
        throw FormatError("Invalid AMBER NetCDF file " + file->name());

    auto nsteps = ncfile->dimmension("frame");
    if (step > nsteps)
        throw FormatError("Can not read step " + std::to_string(step) +
                          ". Maximum step is " + std::to_string(nsteps) );

    // Set the internal step before further reading
    step = _step;
    reserve(ncfile->dimmension("atom"));
    frame.cell(read_cell(ncfile));
    read_positions(ncfile, frame.positions());
    read_velocities(ncfile, frame.velocities());
}

void NCFormat::read_next_step(File* file, Frame& frame) {
    step++;
    read_at_step(file, step, frame);
}

void NCFormat::reserve(size_t natoms) const{
    cache = vector<float>(3*natoms);
    std::fill(begin(cache), end(cache), 0);
}

UnitCell NCFormat::read_cell(NCFile* file) const {
    if (file->dimmension("cell_spatial") != 3 or file->dimmension("cell_angular") != 3)
        return UnitCell(); // No UnitCell information

    NcVar* length_var;
    NcVar* angles_var;
    try {
        length_var = file->variable("cell_lengths");
        angles_var = file->variable("cell_angles");
    }
    catch (const FileError& e) {
        return UnitCell(); // No UnitCell information
    }

    float length[3];
    float angles[3];

    length_var->set_cur(static_cast<long>(step), 0);
    angles_var->set_cur(static_cast<long>(step), 0);

    length_var->get(length, 1, 3);
    angles_var->get(angles, 1, 3);

    return UnitCell(length[0], length[1], length[2], angles[0], angles[1], angles[2]);
}

void NCFormat::read_array3D(NCFile* file, Array3D& arr, const string& name) const{
    NcVar* array_var = nullptr;
    try {
        array_var = file->variable(name);
    }
    catch (const FileError& e) {
        return; // No information for this variable in the file
    }

    auto natoms = file->dimmension("atom");

    array_var->set_cur(static_cast<long>(step), 0, 0);
    array_var->get(cache.data(), 1, natoms, 3);

    arr = Array3D(natoms);
    std::fill(begin(arr), end(arr), Vector3D(0, 0, 0));

    for (size_t i=0; i<natoms; i++) {
        arr[i][0] = cache[3*i + 0];
        arr[i][1] = cache[3*i + 1];
        arr[i][2] = cache[3*i + 2];
    }
}

void NCFormat::read_positions(NCFile* file, Array3D& pos) const {
    read_array3D(file, pos, "coordinates");
}

void NCFormat::read_velocities(NCFile* file, Array3D& vel) const {
    read_array3D(file, vel, "velocities");
}

#endif // HAVE_NETCDF
