// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <array>
#include <string>
#include <vector>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/files/NcFile.hpp"

#include "chemfiles/types.hpp"
#include "chemfiles/config.h"
#include "chemfiles/warnings.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/span.hpp"
#include "chemfiles/external/optional.hpp"

#include "chemfiles/formats/AmberNetCDF.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<AmberNetCDFFormat>() {
    return FormatInfo("Amber NetCDF").with_extension(".nc").description(
        "Amber convention for binary NetCDF molecular trajectories"
    );
}

//! Check the validity of a NetCDF file
static bool is_valid(const NcFile& file_, size_t natoms) {
    bool writing = (natoms != static_cast<size_t>(-1));

    if (file_.global_attribute("Conventions") != "AMBER") {
        if (!writing) {
            warning("we can only read AMBER convention NetCDF files");
        }
        return false;
    }

    if (file_.global_attribute("ConventionVersion") != "1.0") {
        if (!writing) {
            warning("we can only read version 1.0 of AMBER convention NetCDF files");
        }
        return false;
    }

    if (file_.dimension("spatial") != 3) {
        if (!writing) {
            warning("wrong size for spatial dimension: should be 3, is {}", file_.dimension("spatial"));
        }
        return false;
    }

    if (writing) {
        if (file_.dimension("atom") != natoms) {
            warning(
                "wrong size for atoms dimension: should be {}, is {}",
                natoms, file_.dimension("atom")
            );
            return false;
        }
    }
    return true;
}

AmberNetCDFFormat::AmberNetCDFFormat(std::string path, File::Mode mode, File::Compression compression)
    : file_(std::move(path), mode), step_(0), validated_(false) {
    if (file_.mode() == File::READ || file_.mode() == File::APPEND) {
        if (!is_valid(file_, static_cast<size_t>(-1))) {
            throw format_error("invalid AMBER NetCDF file at '{}'", file_.path());
        }
        validated_ = true;
    }
    if (compression != File::DEFAULT) {
        throw format_error("compression is not supported with NetCDF format");
    }
}

size_t AmberNetCDFFormat::nsteps() {
    return file_.dimension("frame");
}

void AmberNetCDFFormat::read_step(const size_t step, Frame& frame) {
    // Set the internal step_ before further reading
    step_ = step;
    frame.set_cell(read_cell());

    frame.resize(file_.dimension("atom"));
    read_array(frame.positions(), "coordinates");
    if (file_.variable_exists("velocities")) {
        frame.add_velocities();
        read_array(*frame.velocities(), "velocities");
    }
}

void AmberNetCDFFormat::read(Frame& frame) {
    read_step(step_, frame);
    step_++;
}

UnitCell AmberNetCDFFormat::read_cell() {
    if (!file_.variable_exists("cell_lengths") ||
        !file_.variable_exists("cell_angles")) {
        return {}; // No UnitCell information
    }

    if (file_.optional_dimension("cell_spatial", 0) != 3 ||
        file_.optional_dimension("cell_angular", 0) != 3) {
            return {}; // No UnitCell information
    }

    auto length_var = file_.variable<nc::NcFloat>("cell_lengths");
    auto angles_var = file_.variable<nc::NcFloat>("cell_angles");

    std::vector<size_t> start{step_, 0};
    std::vector<size_t> count{1, 3};

    auto length = length_var.get(start, count);
    auto angles = angles_var.get(start, count);

    assert(length.size() == 3);
    assert(angles.size() == 3);

    return {
        static_cast<double>(length[0]),
        static_cast<double>(length[1]),
        static_cast<double>(length[2]),
        static_cast<double>(angles[0]),
        static_cast<double>(angles[1]),
        static_cast<double>(angles[2])
    };
}

void AmberNetCDFFormat::read_array(span<Vector3D> array, const std::string& name) {
    auto array_var = file_.variable<nc::NcFloat>(name);
    auto natoms = file_.dimension("atom");
    assert(array.size() == natoms);

    std::vector<size_t> start{step_, 0, 0};
    std::vector<size_t> count{1, natoms, 3};
    auto data = array_var.get(start, count);

    for (size_t i = 0; i < natoms; i++) {
        array[i][0] = static_cast<double>(data[3 * i + 0]);
        array[i][1] = static_cast<double>(data[3 * i + 1]);
        array[i][2] = static_cast<double>(data[3 * i + 2]);
    }
}

// Initialize a file, assuming that it is empty
static void initialize(NcFile& file, size_t natoms, bool with_velocities) {
    file.set_nc_mode(NcFile::DEFINE);

    file.add_global_attribute("Conventions", "AMBER");
    file.add_global_attribute("ConventionVersion", "1.0");
    file.add_global_attribute("program", "Chemfiles");
    file.add_global_attribute("programVersion", CHEMFILES_VERSION);

    file.add_dimension("frame");
    file.add_dimension("spatial", 3);
    file.add_dimension("atom", natoms);
    file.add_dimension("cell_spatial", 3);
    file.add_dimension("cell_angular", 3);
    file.add_dimension("label", nc::STRING_MAXLEN);

    auto spatial = file.add_variable<nc::NcChar>("spatial", "spatial");
    auto cell_spatial = file.add_variable<nc::NcChar>("cell_spatial", "cell_spatial");
    auto cell_angular =
        file.add_variable<nc::NcChar>("cell_angular", "cell_angular", "label");

    auto coordinates =
        file.add_variable<nc::NcFloat>("coordinates", "frame", "atom", "spatial");
    coordinates.add_attribute("units", "angstrom");

    auto cell_lenght =
        file.add_variable<nc::NcFloat>("cell_lengths", "frame", "cell_spatial");
    cell_lenght.add_attribute("units", "angstrom");

    auto cell_angles =
        file.add_variable<nc::NcFloat>("cell_angles", "frame", "cell_angular");
    cell_angles.add_attribute("units", "degree");

    if (with_velocities) {
        auto velocities =
            file.add_variable<nc::NcFloat>("velocities", "frame", "atom", "spatial");
        velocities.add_attribute("units", "angstrom/picosecond");
    }
    file.set_nc_mode(NcFile::DATA);

    spatial.add("xyz");
    cell_spatial.add("abc");
    cell_angular.add({"alpha", "beta", "gamma"});
}

void AmberNetCDFFormat::write(const Frame& frame) {
    auto natoms = frame.size();
    // If we created the file, let's initialize it.
    if (!validated_) {
        initialize(file_, natoms, bool(frame.velocities()));
        assert(is_valid(file_, natoms));
        validated_ = true;
    }
    write_cell(frame.cell());
    write_array(frame.positions(), "coordinates");
    auto velocities = frame.velocities();
    if (velocities) {
        write_array(*velocities, "velocities");
    }

    step_++;
}

void AmberNetCDFFormat::write_array(const std::vector<Vector3D>& array, const std::string& name) {
    auto var = file_.variable<nc::NcFloat>(name);
    auto natoms = array.size();
    std::vector<size_t> start{step_, 0, 0};
    std::vector<size_t> count{1, natoms, 3};

    auto data = std::vector<float>(natoms * 3);
    for (size_t i = 0; i < natoms; i++) {
        data[3 * i + 0] = static_cast<float>(array[i][0]);
        data[3 * i + 1] = static_cast<float>(array[i][1]);
        data[3 * i + 2] = static_cast<float>(array[i][2]);
    }
    var.add(start, count, data);
}

void AmberNetCDFFormat::write_cell(const UnitCell& cell) {
    auto length = file_.variable<nc::NcFloat>("cell_lengths");
    auto angles = file_.variable<nc::NcFloat>("cell_angles");

    auto length_data = std::vector<float>{static_cast<float>(cell.a()),
                                          static_cast<float>(cell.b()),
                                          static_cast<float>(cell.c())};

    auto angles_data = std::vector<float>{static_cast<float>(cell.alpha()),
                                          static_cast<float>(cell.beta()),
                                          static_cast<float>(cell.gamma())};

    std::vector<size_t> start{step_, 0};
    std::vector<size_t> count{1, 3};
    length.add(start, count, length_data);
    angles.add(start, count, angles_data);
}
