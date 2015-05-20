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

#include "formats/NCFormat.hpp"

#include "Error.hpp"
#include "Logger.hpp"
#include "Frame.hpp"
#include "files/NCFile.hpp"
using namespace harp;
using namespace netCDF;

#include <algorithm>
#include <iostream>
#include <algorithm>
using std::endl;
using std::vector;

std::string NCFormat::description() const {
    return "Amber NetCDF file format.";
}

size_t NCFormat::nsteps(File* file) const {
    auto ncfile = dynamic_cast<NCFile*>(file);
    validate(ncfile);
    auto nsteps = ncfile->dimension("frame");
    return static_cast<size_t>(nsteps);
}

// Register the Amber NetCDF format with the ".nc" extension and the
// "AmberNetCDF" description.
REGISTER_WITH_FILE(NCFormat, "AmberNetCDF", NCFile);
REGISTER_EXTENSION_AND_FILE(NCFormat, ".nc", NCFile);

NCFormat::NCFormat() : step(0), last_file_hash(0), last_file_was_valid(false) {}

static bool is_valid(NCFile* file, size_t natoms){
    bool writing;
    if (natoms == static_cast<size_t>(-1))
        writing = false;
    else
        writing = true;

    if (file->global_attribute("Conventions") != "AMBER"){
        if (writing)
            LOG(ERROR) << "We can only read AMBER convention NetCDF files." << endl;
        return false;
    }

    if (file->global_attribute("ConventionVersion") != "1.0"){
        if (writing)
            LOG(ERROR) << "We can only read version 1.0 of AMBER convention NetCDF files." << endl;
        return false;
    }

    if (file->dimension("spatial") != 3){
        if (writing)
            LOG(ERROR) << "Wrong size for spatial dimension. Should be 3, is "
                       << file->dimension("spatial") << "." << endl;
        return false;
    }

    if (writing) {
        if (file->dimension("atom") != natoms){
            LOG(ERROR) << "Wrong size for atoms dimension. Should be " << natoms
                       << ", is " << file->dimension("atom") << "." << endl;
        return false;
        }
    }
    return true;
}

void NCFormat::validate(NCFile* file, size_t natoms) const{
    // Using the pointer adress before I come up with a better hash function
    auto file_hash = reinterpret_cast<size_t>(file);
    if (last_file_hash != file_hash){
        last_file_hash = file_hash;
        last_file_was_valid = is_valid(file, natoms);
    }
    if (not last_file_was_valid)
        throw FormatError("Invalid AMBER NetCDF file " + file->filename());
}

void NCFormat::read_at(File* file, const size_t _step, Frame& frame){
    auto ncfile = dynamic_cast<NCFile*>(file);

    validate(ncfile);
    // Set the internal step before further reading
    step = _step;
    reserve(ncfile->dimension("atom"));
    frame.cell(read_cell(ncfile));

    auto& pos = frame.positions();
    read_array3D(ncfile, pos, "coordinates");
    auto& vel = frame.velocities();
    read_array3D(ncfile, vel, "velocities");
}

void NCFormat::read(File* file, Frame& frame) {
    read_at(file, step, frame);
    step++;
}

void NCFormat::reserve(size_t natoms) const{
    cache = vector<float>(3*natoms);
    std::fill(begin(cache), end(cache), 0);
}

UnitCell NCFormat::read_cell(NCFile* file) const {
    if (file->dimension("cell_spatial") != 3 or file->dimension("cell_angular") != 3)
        return UnitCell(); // No UnitCell information

    NcVar length_var;
    NcVar angles_var;
    try {
        length_var = file->variable("cell_lengths");
        angles_var = file->variable("cell_angles");
    }
    catch (const FileError& e) {
        return UnitCell(); // No UnitCell information
    }

    float length[3];
    float angles[3];

    vector<size_t> start{step, 0};
    vector<size_t> count{1, 3};

    length_var.getVar(start, count, length);
    angles_var.getVar(start, count, angles);

    return UnitCell(length[0], length[1], length[2], angles[0], angles[1], angles[2]);
}

void NCFormat::read_array3D(NCFile* file, Array3D& arr, const string& name) const{
    NcVar array_var;
    try {
        array_var = file->variable(name);
    }
    catch (const FileError& e) {
        return; // No information for this variable in the file
    }

    auto natoms = file->dimension("atom");

    vector<size_t> start{step, 0, 0};
    vector<size_t> count{1, natoms, 3};
    array_var.getVar(start, count, cache.data());

    arr = Array3D(natoms);

    for (size_t i=0; i<natoms; i++) {
        arr[i][0] = cache[3*i + 0];
        arr[i][1] = cache[3*i + 1];
        arr[i][2] = cache[3*i + 2];
    }
}

// Initialize a file, assuming that it is empty
static void initialize(NCFile* file, size_t natoms, bool velocities){
    file->add_global_attribute("Conventions", "AMBER");
    file->add_global_attribute("ConventionVersion", "1.0");
    file->add_global_attribute("program", "Chemharp");
    file->add_global_attribute("programVersion", CHRP_VERSION);

    file->add_dimension("frame");
    file->add_dimension("spatial", 3);
    file->add_dimension("atom", natoms);
    file->add_dimension("cell_spatial", 3);
    file->add_dimension("cell_angular", 3);
    file->add_dimension("label", 10);

    file->add_variable<char>("spatial", "spatial");
    auto spatial = file->variable("spatial");
    spatial.putVar("xyz");

    file->add_variable<char>("cell_spatial", "cell_spatial");
    auto cell_spatial = file->variable("cell_spatial");
    cell_spatial.putVar("abc");

    file->add_variable<char>("cell_angular", "cell_angular", "label");
    auto cell_angular = file->variable("cell_angular");
    const char angles[3][10]{"alpha", "beta", "gamma"};
    cell_angular.putVar(angles);

    file->add_variable<float>("coordinates", "frame", "atom", "spatial");
    file->add_attribute("coordinates", "units", "angstrom");

    file->add_variable<float>("cell_lengths", "frame", "cell_spatial");
    file->add_attribute("cell_lengths", "units", "angstrom");

    file->add_variable<float>("cell_angles", "frame", "cell_angular");
    file->add_attribute("cell_angles", "units", "degree");

    if (velocities) {
        file->add_variable<float>("velocities", "frame", "atom", "spatial");
        file->add_attribute("velocities", "units", "angstrom/picosecond");
    }
}

void NCFormat::write(File* file, const Frame& frame) {
    auto ncfile = dynamic_cast<NCFile*>(file);
    auto natoms = frame.natoms();
    try {
        validate(ncfile, natoms);
    } catch (const FileError&) {
        // If the file is invalid, try to initialize it.
        initialize(ncfile, natoms, frame.has_velocities());
    }
    write_cell(ncfile, frame.cell());
    write_array3D(ncfile, frame.positions(), "coordinates");
    if (frame.has_velocities())
        write_array3D(ncfile, frame.velocities(), "velocities");

    step++;
}

void NCFormat::write_array3D(NCFile* file, const Array3D& arr, const string& name) const {
    auto var = file->variable(name);
    auto natoms = arr.size();
    vector<size_t> start{step, 0, 0};
    vector<size_t> count{1, natoms, 3};

    auto data = new float[natoms * 3];
    for (size_t i=0; i<natoms; i++){
        data[3*i + 0] = arr[i][0];
        data[3*i + 1] = arr[i][1];
        data[3*i + 2] = arr[i][2];
    }
    var.putVar(start, count, data);
    delete[] data;
}

void NCFormat::write_cell(NCFile* file, const UnitCell& cell) const {
    auto length = file->variable("cell_lengths");
    auto angles = file->variable("cell_angles");

    float length_data[3], angles_data[3];
    length_data[0] = static_cast<float>(cell.a());
    length_data[1] = static_cast<float>(cell.b());
    length_data[2] = static_cast<float>(cell.c());

    angles_data[0] = static_cast<float>(cell.alpha());
    angles_data[1] = static_cast<float>(cell.beta());
    angles_data[2] = static_cast<float>(cell.gamma());

    vector<size_t> start{step, 0};
    vector<size_t> count{1, 3};
    length.putVar(start, count, length_data);
    angles.putVar(start, count, angles_data);
}

#endif // HAVE_NETCDF
