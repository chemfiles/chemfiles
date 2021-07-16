// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <array>
#include <string>
#include <vector>

#include "chemfiles/types.hpp"
#include "chemfiles/config.h"
#include "chemfiles/warnings.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/span.hpp"
#include "chemfiles/external/optional.hpp"


#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/FormatMetadata.hpp"

#include "chemfiles/files/NcFile.hpp"
#include "chemfiles/formats/AmberNetCDF.hpp"

using namespace chemfiles;

template<AmberFormat F>
struct FormatSpec;

template<> struct FormatSpec<AMBER_NC_RESTART>  {
    typedef nc::NcDouble nc_type;
    typedef double real_type;
    static constexpr char const* convention = "AMBERRESTART";
};

template<> struct FormatSpec<AMBER_NC_TRAJECTORY>  {
    typedef nc::NcFloat nc_type;
    typedef float real_type;
    static constexpr char const* convention = "AMBER";
};

//! Check the validity of a NetCDF file
template <AmberFormat F>
static bool is_valid(const NcFile& file_, optional<size_t> natoms) {
    bool writing = static_cast<bool>(natoms);
    auto convention = FormatSpec<F>::convention;

    if (file_.global_attribute("Conventions") != convention) {
        if (!writing) {
            warning("Amber NetCDF reader", "we can only read {} convention", convention);
        }
        return false;
    }

    if (file_.global_attribute("ConventionVersion") != "1.0") {
        if (!writing) {
            warning("Amber NetCDF reader", "we can only read version 1.0 of {} convention",
                    convention);
        }
        return false;
    }

    if (file_.dimension("spatial") != 3) {
        if (!writing) {
            warning("Amber NetCDF reader",
                "wrong size for spatial dimension: should be 3, is {}",
                file_.dimension("spatial")
            );
        }
        return false;
    }

    if (writing) {
        if (file_.dimension("atom") != *natoms) {
            warning("Amber NetCDF writer",
                "wrong size for atoms dimension: should be {}, is {}",
                *natoms, file_.dimension("atom")
            );
            return false;
        }
    }
    return true;
}

template <AmberFormat F>
Amber<F>::Amber(std::string path, File::Mode mode, File::Compression compression)
    : file_(std::move(path), mode), step_(0), validated_(false) {
    if (file_.mode() == File::READ || file_.mode() == File::APPEND) {
        if (!is_valid<F>(file_, nullopt)) {
            throw format_error("invalid AMBER NetCDF file at '{}'", file_.path());
        }
        validated_ = true;
    }
    if (compression != File::DEFAULT) {
        throw format_error("compression is not supported with NetCDF format");
    }
}

template <AmberFormat F>
void Amber<F>::read_step(const size_t step, Frame& frame) {
    // Set the internal step_ before further reading
    step_ = step;

    if (F == AMBER_NC_RESTART && step_ != 0) {
        throw format_error("AMBER Restart format only supports reading one frame");
    }

    frame.set_cell(read_cell());

    if (file_.global_attribute_exists("title")) {
        frame.set("name", file_.global_attribute("title"));
    }

    frame.resize(file_.dimension("atom"));
    read_array(frame.positions(), "coordinates");
    if (file_.variable_exists("velocities")) {
        frame.add_velocities();
        read_array(*frame.velocities(), "velocities");
    }
}

template <AmberFormat F>
void Amber<F>::read(Frame& frame) {
    read_step(step_, frame);
    step_++;
}

namespace chemfiles {
    template <>
    std::array<std::vector<size_t>, 2> Amber<AMBER_NC_RESTART>::vec3d_range() {
        return {std::vector<size_t>({0}), std::vector<size_t>({3})};
    }

    template <>
    std::array<std::vector<size_t>, 2> Amber<AMBER_NC_TRAJECTORY>::vec3d_range() {
        return {std::vector<size_t>({step_, 0}), std::vector<size_t>({1, 3})};
    }

    template <>
    std::array<std::vector<size_t>, 2> Amber<AMBER_NC_RESTART>::vec3d_n_range(size_t n) {
        return {std::vector<size_t>({0, 0}), std::vector<size_t>({n, 3})};
    }

    template <>
    std::array<std::vector<size_t>, 2> Amber<AMBER_NC_TRAJECTORY>::vec3d_n_range(size_t n) {
        return {std::vector<size_t>({step_, 0, 0}), std::vector<size_t>({1, n, 3})};
    }

    template <>
    size_t Amber<AMBER_NC_RESTART>::nsteps() {
        return 1;
    }

    template <>
    size_t Amber<AMBER_NC_TRAJECTORY>::nsteps() {
        return file_.dimension("frame");
    }
}

template <AmberFormat F>
static typename FormatSpec<F>::nc_type get_variable(NcFile& file_, const std::string& name) {
    return file_.variable<typename FormatSpec<F>::nc_type>(name);
}

template <AmberFormat F>
UnitCell Amber<F>::read_cell() {
    if (!file_.variable_exists("cell_lengths") ||
        !file_.variable_exists("cell_angles")) {
        return {}; // No UnitCell information
    }

    if (file_.optional_dimension("cell_spatial", 0) != 3 ||
        file_.optional_dimension("cell_angular", 0) != 3) {
            return {}; // No UnitCell information
    }

    auto length_var = get_variable<F>(file_, "cell_lengths");
    auto angles_var = get_variable<F>(file_, "cell_angles");

    auto range = vec3d_range();
    auto lengths_f = length_var.get(range[0], range[1]);
    auto angles_f = angles_var.get(range[0], range[1]);

    auto lengths = Vector3D(
        static_cast<double>(lengths_f[0]),
        static_cast<double>(lengths_f[1]),
        static_cast<double>(lengths_f[2])
    );
    auto angles = Vector3D(
        static_cast<double>(angles_f[0]),
        static_cast<double>(angles_f[1]),
        static_cast<double>(angles_f[2])
    );

    if (length_var.attribute_exists("scale_factor")) {
        lengths *= static_cast<double>(length_var.float_attribute("scale_factor"));
    }

    if (angles_var.attribute_exists("scale_factor")) {
        angles *= static_cast<double>(angles_var.float_attribute("scale_factor"));
    }

    return UnitCell(lengths, angles);
}

template <AmberFormat F>
void Amber<F>::read_array(span<Vector3D> array, const std::string& name) {
    auto array_var = get_variable<F>(file_, name);
    auto natoms = file_.dimension("atom");
    assert(array.size() == natoms);


    auto range = vec3d_n_range(natoms);
    auto data = array_var.get(range[0], range[1]);

    if (array_var.attribute_exists("scale_factor")) {
        float scale_factor = array_var.float_attribute("scale_factor");
        for (auto& value : data) {
            value *= scale_factor;
        }
    }

    for (size_t i = 0; i < natoms; i++) {
        array[i][0] = static_cast<double>(data[3 * i + 0]);
        array[i][1] = static_cast<double>(data[3 * i + 1]);
        array[i][2] = static_cast<double>(data[3 * i + 2]);
    }
}

template <AmberFormat F>
static void init_frame(NcFile& file, bool with_velocities);

template<>
void init_frame<AMBER_NC_RESTART>(NcFile& file, bool with_velocities) {
    auto coordinates = file.add_variable<nc::NcDouble>("coordinates", "atom", "spatial");
    coordinates.add_string_attribute("units", "angstrom");

    auto cell_lenght = file.add_variable<nc::NcDouble>("cell_lengths", "cell_spatial");
    cell_lenght.add_string_attribute("units", "angstrom");

    auto cell_angles = file.add_variable<nc::NcDouble>("cell_angles", "cell_angular");
    cell_angles.add_string_attribute("units", "degree");

    if (with_velocities) {
        auto velocities = file.add_variable<nc::NcDouble>("velocities", "atom", "spatial");
        velocities.add_string_attribute("units", "angstrom/picosecond");
    }
}

template<>
void init_frame<AMBER_NC_TRAJECTORY>(NcFile& file, bool with_velocities) {
    file.add_dimension("frame");

    auto coordinates =
        file.add_variable<nc::NcFloat>("coordinates", "frame", "atom", "spatial");
    coordinates.add_string_attribute("units", "angstrom");

    auto cell_length =
        file.add_variable<nc::NcFloat>("cell_lengths", "frame", "cell_spatial");
    cell_length.add_string_attribute("units", "angstrom");

    auto cell_angles =
        file.add_variable<nc::NcFloat>("cell_angles", "frame", "cell_angular");
    cell_angles.add_string_attribute("units", "degree");

    if (with_velocities) {
        auto velocities =
            file.add_variable<nc::NcFloat>("velocities", "frame", "atom", "spatial");
        velocities.add_string_attribute("units", "angstrom/picosecond");
    }
}

// Initialize a file, assuming that it is empty
template<AmberFormat F>
void initialize(NcFile& file, size_t natoms, bool with_velocities, optional<const std::string&> name) {
    file.set_nc_mode(NcFile::DEFINE);

    file.add_global_attribute("Conventions", FormatSpec<F>::convention);
    file.add_global_attribute("ConventionVersion", "1.0");
    file.add_global_attribute("program", "Chemfiles");
    file.add_global_attribute("programVersion", CHEMFILES_VERSION);

    if (name) {
        file.add_global_attribute("title", name.value());
    }

    file.add_dimension("spatial", 3);
    file.add_dimension("atom", natoms);
    file.add_dimension("cell_spatial", 3);
    file.add_dimension("cell_angular", 3);
    file.add_dimension("label", nc::STRING_MAXLEN);
    auto spatial = file.add_variable<nc::NcChar>("spatial", "spatial");
    auto cell_spatial = file.add_variable<nc::NcChar>("cell_spatial", "cell_spatial");
    auto cell_angular =
        file.add_variable<nc::NcChar>("cell_angular", "cell_angular", "label");
    init_frame<F>(file, with_velocities);
    file.set_nc_mode(NcFile::DATA);

    spatial.add("xyz");
    cell_spatial.add("abc");
    cell_angular.add({"alpha", "beta", "gamma"});
}

template <AmberFormat F>
void Amber<F>::write(const Frame& frame) {
    if (F == AMBER_NC_RESTART && step_ != 0) {
        throw format_error("AMBER Restart format only supports writing one frame");
    }

    auto natoms = frame.size();
    // If we created the file, let's initialize it.
    if (!validated_) {
        auto name = frame.get<Property::STRING>("name");
        initialize<F>(file_, natoms, bool(frame.velocities()), name);
        assert(is_valid<F>(file_, natoms));
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

template <AmberFormat F>
void Amber<F>::write_array(const std::vector<Vector3D>& array, const std::string& name) {
    using real_t = typename FormatSpec<F>::real_type;

    auto var = get_variable<F>(file_, name);
    auto natoms = array.size();
    auto range = vec3d_n_range(natoms);

    auto data = std::vector<real_t>(natoms * 3);
    for (size_t i = 0; i < natoms; i++) {
        data[3 * i + 0] = static_cast<real_t>(array[i][0]);
        data[3 * i + 1] = static_cast<real_t>(array[i][1]);
        data[3 * i + 2] = static_cast<real_t>(array[i][2]);
    }
    var.add(range[0], range[1], data);
}

template <AmberFormat F> void Amber<F>::write_cell(const UnitCell& cell) {
    using real_t = typename FormatSpec<F>::real_type;

    auto length = get_variable<F>(file_, "cell_lengths");
    auto angles = get_variable<F>(file_, "cell_angles");

    auto cell_lengths = cell.lengths();
    auto cell_angles = cell.angles();

    auto length_data = std::vector<real_t>{
        static_cast<real_t>(cell_lengths[0]),
        static_cast<real_t>(cell_lengths[1]),
        static_cast<real_t>(cell_lengths[2])};

    auto angles_data = std::vector<real_t>{
        static_cast<real_t>(cell_angles[0]),
        static_cast<real_t>(cell_angles[1]),
        static_cast<real_t>(cell_angles[2])};

    auto range = vec3d_range();
    length.add(range[0], range[1], length_data);
    angles.add(range[0], range[1], angles_data);
}

// Instantiate all the templates
template class chemfiles::Amber<AMBER_NC_RESTART>;
template class chemfiles::Amber<AMBER_NC_TRAJECTORY>;

template<> const FormatMetadata& chemfiles::format_metadata<Amber<AMBER_NC_RESTART>>() {
    static FormatMetadata metadata;
    metadata.name = "Amber Restart";
    metadata.extension = ".ncrst";
    metadata.description = "Amber convention for binary NetCDF restart files";
    metadata.reference = "http://ambermd.org/netcdf/nctraj.xhtml";

    metadata.read = true;
    metadata.write = true;
    metadata.memory = false;

    metadata.positions = true;
    metadata.velocities = true;
    metadata.unit_cell = true;
    metadata.atoms = false;
    metadata.bonds = false;
    metadata.residues = false;
    return metadata;
}

template<> const FormatMetadata& chemfiles::format_metadata<Amber<AMBER_NC_TRAJECTORY>>() {
    static FormatMetadata metadata;
    metadata.name = "Amber NetCDF";
    metadata.extension = ".nc";
    metadata.description = "Amber convention for binary NetCDF molecular trajectories";
    metadata.reference = "http://ambermd.org/netcdf/nctraj.xhtml";

    metadata.read = true;
    metadata.write = true;
    metadata.memory = false;

    metadata.positions = true;
    metadata.velocities = true;
    metadata.unit_cell = true;
    metadata.atoms = false;
    metadata.bonds = false;
    metadata.residues = false;
    return metadata;
}
