/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <algorithm>
#include <iostream>
#include <vector>
using std::endl;
using std::vector;

#include "chemfiles/config.hpp"
#if HAVE_NETCDF

#include "chemfiles/formats/NCFormat.hpp"

#include "chemfiles/Error.hpp"
#include "chemfiles/Logger.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/files/NCFile.hpp"
using namespace chemfiles;
using namespace netCDF;

std::string NCFormat::description() const {
    return "Amber NetCDF file format.";
}

//! Check the validity of a NetCDF file
static bool is_valid(const NCFile& ncfile, size_t natoms){
    bool writing;
    if (natoms == static_cast<size_t>(-1)) {
        writing = false;
    } else {
        writing = true;
    }

    if (ncfile.global_attribute("Conventions") != "AMBER"){
        if (!writing)
            LOG(ERROR) << "We can only read AMBER convention NetCDF files." << endl;
        return false;
    }

    if (ncfile.global_attribute("ConventionVersion") != "1.0"){
        if (!writing)
            LOG(ERROR) << "We can only read version 1.0 of AMBER convention NetCDF files." << endl;
        return false;
    }

    if (ncfile.dimension("spatial") != 3){
        if (!writing)
            LOG(ERROR) << "Wrong size for spatial dimension. Should be 3, is "
                       << ncfile.dimension("spatial") << "." << endl;
        return false;
    }

    if (writing) {
        if (ncfile.dimension("atom") != natoms){
            LOG(ERROR) << "Wrong size for atoms dimension. Should be " << natoms
                       << ", is " << ncfile.dimension("atom") << "." << endl;
        return false;
        }
    }
    return true;
}

NCFormat::NCFormat(File& file) : Format(file), ncfile(static_cast<NCFile&>(file)), step(0), validated(false) {
    if (ncfile.mode() == "r" || ncfile.mode() == "a") {
        if (!is_valid(ncfile, static_cast<size_t>(-1))) {
            throw FormatError("Invalid AMBER NetCDF file " + file.filename());
        }
        validated = true;
    }
}

size_t NCFormat::nsteps() const {
    return static_cast<size_t>(ncfile.dimension("frame"));
}


void NCFormat::read_step(const size_t _step, Frame& frame){
    // Set the internal step before further reading
    step = _step;
    reserve(ncfile.dimension("atom"));
    frame.cell(read_cell());

    auto& pos = frame.positions();
    read_array3D(pos, "coordinates");
    auto& vel = frame.velocities();
    read_array3D(vel, "velocities");
}

void NCFormat::read(Frame& frame) {
    read_step(step, frame);
    step++;
}

void NCFormat::reserve(size_t natoms) const{
    cache = vector<float>(3*natoms);
    std::fill(begin(cache), end(cache), 0);
}

UnitCell NCFormat::read_cell() const {
    if (ncfile.dimension("cell_spatial") != 3 or ncfile.dimension("cell_angular") != 3)
        return UnitCell(); // No UnitCell information

    NcVar length_var;
    NcVar angles_var;
    try {
        length_var = ncfile.variable("cell_lengths");
        angles_var = ncfile.variable("cell_angles");
    }
    catch (const FileError&) {
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

void NCFormat::read_array3D(Array3D& arr, const string& name) const{
    NcVar array_var;
    try {
        array_var = ncfile.variable(name);
    }
    catch (const FileError&) {
        return; // No information for this variable in the file
    }

    auto natoms = ncfile.dimension("atom");

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
static void initialize(NCFile& ncfile, size_t natoms, bool velocities){
    ncfile.add_global_attribute("Conventions", "AMBER");
    ncfile.add_global_attribute("ConventionVersion", "1.0");
    ncfile.add_global_attribute("program", "Chemfiles");
    ncfile.add_global_attribute("programVersion", CHEMFILES_VERSION);

    ncfile.add_dimension("frame");
    ncfile.add_dimension("spatial", 3);
    ncfile.add_dimension("atom", natoms);
    ncfile.add_dimension("cell_spatial", 3);
    ncfile.add_dimension("cell_angular", 3);
    ncfile.add_dimension("label", 10);

    ncfile.add_variable<char>("spatial", "spatial");
    auto spatial = ncfile.variable("spatial");
    spatial.putVar("xyz");

    ncfile.add_variable<char>("cell_spatial", "cell_spatial");
    auto cell_spatial = ncfile.variable("cell_spatial");
    cell_spatial.putVar("abc");

    ncfile.add_variable<char>("cell_angular", "cell_angular", "label");
    auto cell_angular = ncfile.variable("cell_angular");
    const char angles[3][10]{"alpha", "beta", "gamma"};
    cell_angular.putVar(angles);

    ncfile.add_variable<float>("coordinates", "frame", "atom", "spatial");
    ncfile.add_attribute("coordinates", "units", "angstrom");

    ncfile.add_variable<float>("cell_lengths", "frame", "cell_spatial");
    ncfile.add_attribute("cell_lengths", "units", "angstrom");

    ncfile.add_variable<float>("cell_angles", "frame", "cell_angular");
    ncfile.add_attribute("cell_angles", "units", "degree");

    if (velocities) {
        ncfile.add_variable<float>("velocities", "frame", "atom", "spatial");
        ncfile.add_attribute("velocities", "units", "angstrom/picosecond");
    }
}

void NCFormat::write(const Frame& frame) {
    auto natoms = frame.natoms();
    // If we created the file, let's initialize it.
    if (!validated) {
        initialize(ncfile, natoms, frame.has_velocities());
        assert(is_valid(ncfile, natoms));
        validated = true;
    }
    write_cell(frame.cell());
    write_array3D(frame.positions(), "coordinates");
    if (frame.has_velocities())
        write_array3D(frame.velocities(), "velocities");

    step++;
}

void NCFormat::write_array3D(const Array3D& arr, const string& name) const {
    auto var = ncfile.variable(name);
    auto natoms = arr.size();
    vector<size_t> start{step, 0, 0};
    vector<size_t> count{1, natoms, 3};

    auto data = vector<float>(natoms * 3);
    for (size_t i=0; i<natoms; i++){
        data[3*i + 0] = arr[i][0];
        data[3*i + 1] = arr[i][1];
        data[3*i + 2] = arr[i][2];
    }
    var.putVar(start, count, data.data());
}

void NCFormat::write_cell(const UnitCell& cell) const {
    auto length = ncfile.variable("cell_lengths");
    auto angles = ncfile.variable("cell_angles");

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
