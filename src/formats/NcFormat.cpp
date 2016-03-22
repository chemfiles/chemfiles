/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include "chemfiles/config.hpp"
#if HAVE_NETCDF

#include "chemfiles/formats/NcFormat.hpp"

#include "chemfiles/Error.hpp"
#include "chemfiles/Logger.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/files/NcFile.hpp"
using namespace chemfiles;

std::string NCFormat::description() const {
    return "Amber NetCDF file format.";
}

//! Check the validity of a NetCDF file
static bool is_valid(const NcFile& ncfile_, size_t natoms){
    bool writing;
    if (natoms == static_cast<size_t>(-1)) {
        writing = false;
    } else {
        writing = true;
    }

    if (ncfile_.global_attribute("Conventions") != "AMBER"){
        if (!writing)
            Logger::log(LogLevel::ERROR, "We can only read AMBER convention NetCDF files.");
        return false;
    }

    if (ncfile_.global_attribute("ConventionVersion") != "1.0"){
        if (!writing)
            Logger::log(LogLevel::ERROR, "We can only read version 1.0 of AMBER convention NetCDF files.");
        return false;
    }

    if (ncfile_.dimension("spatial") != 3){
        if (!writing)
            Logger::log(LogLevel::ERROR,
                "Wrong size for spatial dimension. Should be 3, is " +
                std::to_string(ncfile_.dimension("spatial")) + ".");
        return false;
    }

    if (writing) {
        if (ncfile_.dimension("atom") != natoms){
            Logger::log(LogLevel::ERROR,
                "Wrong size for atoms dimension. Should be " + std::to_string(natoms) +
                ", is " + std::to_string(ncfile_.dimension("atom")) + ".");
        return false;
        }
    }
    return true;
}

NCFormat::NCFormat(File& file) : Format(file), ncfile_(static_cast<NcFile&>(file)), step_(0), validated_(false) {
    if (ncfile_.mode() == "r" || ncfile_.mode() == "a") {
        if (!is_valid(ncfile_, static_cast<size_t>(-1))) {
            throw FormatError("Invalid AMBER NetCDF file " + file.filename());
        }
        validated_ = true;
    }
}

size_t NCFormat::nsteps() const {
    return static_cast<size_t>(ncfile_.dimension("frame"));
}

void NCFormat::read_step(const size_t step, Frame& frame){
    // Set the internal step_ before further reading
    step_ = step;
    frame.set_cell(read_cell());

    auto& positions = frame.positions();
    read_array3D(positions, "coordinates");

    if (ncfile_.variable_exists("velocities")) {
        auto& velocities = frame.velocities();
        if (!velocities) {
            velocities = Array3D();
        }
        read_array3D(*velocities, "velocities");
    }
}

void NCFormat::read(Frame& frame) {
    read_step(step_, frame);
    step_++;
}

UnitCell NCFormat::read_cell() const {
    if (ncfile_.dimension("cell_spatial") != 3 or ncfile_.dimension("cell_angular") != 3)
        return UnitCell(); // No UnitCell information

    if (!ncfile_.variable_exists("cell_lengths") || !ncfile_.variable_exists("cell_angles")) {
        return UnitCell(); // No UnitCell information
    }

    auto length_var = ncfile_.variable<float>("cell_lengths");
    auto angles_var = ncfile_.variable<float>("cell_angles");

    std::vector<size_t> start{step_, 0};
    std::vector<size_t> count{1, 3};

    auto length = length_var.get(start, count);
    auto angles = angles_var.get(start, count);

    assert(length.size() == 3);
    assert(angles.size() == 3);

    return UnitCell(length[0], length[1], length[2], angles[0], angles[1], angles[2]);
}

void NCFormat::read_array3D(Array3D& array, const string& name) const{
    auto array_var = ncfile_.variable<float>(name);
    auto natoms = ncfile_.dimension("atom");

    std::vector<size_t> start{step_, 0, 0};
    std::vector<size_t> count{1, natoms, 3};
    auto data = array_var.get(start, count);

    array.resize(natoms);
    for (size_t i=0; i<natoms; i++) {
        array[i][0] = data[3*i + 0];
        array[i][1] = data[3*i + 1];
        array[i][2] = data[3*i + 2];
    }
}

// Initialize a file, assuming that it is empty
static void initialize(NcFile& file, size_t natoms, bool with_velocities){
    file.set_file_mode(NcFile::DEFINE);

    file.add_global_attribute("Conventions", "AMBER");
    file.add_global_attribute("ConventionVersion", "1.0");
    file.add_global_attribute("program", "Chemfiles");
    file.add_global_attribute("programVersion", CHEMFILES_VERSION);

    file.add_dimension("frame");
    file.add_dimension("spatial", 3);
    file.add_dimension("atom", natoms);
    file.add_dimension("cell_spatial", 3);
    file.add_dimension("cell_angular", 3);
    file.add_dimension("label", NC_STRING_MAXLEN);

    auto spatial = file.add_variable<char>("spatial", "spatial");
    auto cell_spatial = file.add_variable<char>("cell_spatial", "cell_spatial");
    auto cell_angular = file.add_variable<char>("cell_angular", "cell_angular", "label");

    auto coordinates = file.add_variable<float>("coordinates", "frame", "atom", "spatial");
    coordinates.add_attribute("units", "angstrom");

    auto cell_lenght = file.add_variable<float>("cell_lengths", "frame", "cell_spatial");
    cell_lenght.add_attribute("units", "angstrom");

    auto cell_angles = file.add_variable<float>("cell_angles", "frame", "cell_angular");
    cell_angles.add_attribute("units", "degree");

    if (with_velocities) {
        auto velocities = file.add_variable<float>("velocities", "frame", "atom", "spatial");
        velocities.add_attribute("units", "angstrom/picosecond");
    }
    file.set_file_mode(NcFile::DATA);

    spatial.add("xyz");
    cell_spatial.add("abc");
    cell_angular.add({"alpha", "beta", "gamma"});
}

void NCFormat::write(const Frame& frame) {
    auto natoms = frame.natoms();
    // If we created the file, let's initialize it.
    if (!validated_) {
        initialize(ncfile_, natoms, bool(frame.velocities()));
        assert(is_valid(ncfile_, natoms));
        validated_ = true;
    }
    write_cell(frame.cell());
    write_array3D(frame.positions(), "coordinates");
    auto& velocities = frame.velocities();
    if (velocities) {
        write_array3D(*velocities, "velocities");
    }

    step_++;
}

void NCFormat::write_array3D(const Array3D& arr, const string& name) const {
    auto var = ncfile_.variable<float>(name);
    auto natoms = arr.size();
    std::vector<size_t> start{step_, 0, 0};
    std::vector<size_t> count{1, natoms, 3};

    auto data = std::vector<float>(natoms * 3);
    for (size_t i=0; i<natoms; i++){
        data[3*i + 0] = arr[i][0];
        data[3*i + 1] = arr[i][1];
        data[3*i + 2] = arr[i][2];
    }
    var.add(start, count, data);
}

void NCFormat::write_cell(const UnitCell& cell) const {
    auto length = ncfile_.variable<float>("cell_lengths");
    auto angles = ncfile_.variable<float>("cell_angles");

    auto length_data = std::vector<float>{
        static_cast<float>(cell.a()),
        static_cast<float>(cell.b()),
        static_cast<float>(cell.c())
    };

    auto angles_data = std::vector<float>{
        static_cast<float>(cell.alpha()),
        static_cast<float>(cell.beta()),
        static_cast<float>(cell.gamma())
    };

    std::vector<size_t> start{step_, 0};
    std::vector<size_t> count{1, 3};
    length.add(start, count, length_data);
    angles.add(start, count, angles_data);
}

#endif // HAVE_NETCDF
