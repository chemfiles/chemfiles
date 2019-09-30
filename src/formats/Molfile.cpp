// From vmdconio.h
#define VMDCON_WARN      2
#define VMDCON_ERROR     3

extern "C" {
    #include <vmdplugin.h>
    #include <molfile_plugin.h>
}

#include <cassert>
#include <array>
#include <string>
#include <vector>
#include <unordered_map>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/Topology.hpp"

#include "chemfiles/types.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/span.hpp"
#include "chemfiles/external/optional.hpp"

#include "chemfiles/formats/Molfile.hpp"

using namespace chemfiles;

/******************************************************************************/
#define PLUGINS_DATA(FORMAT, PLUGIN, READER, VELOCITIES)                       \
    extern "C" int PLUGIN##_register(void*, vmdplugin_register_cb); /*NOLINT*/ \
    extern "C" int PLUGIN##_fini(void); /*NOLINT*/                             \
    extern "C" int PLUGIN##_init(void); /*NOLINT*/                             \
    template <> struct MolfilePluginData<FORMAT> {                             \
        int init() { return PLUGIN##_init(); }                                 \
        int registration(void* data, vmdplugin_register_cb callback) {         \
            return PLUGIN##_register(data, callback);                          \
        }                                                                      \
        int fini() { return PLUGIN##_fini(); }                                 \
        std::string format() const {return #FORMAT;}                           \
        std::string plugin_name() const {return #PLUGIN;}                      \
        std::string reader() const {return #READER;}                           \
        bool have_velocities() const {return VELOCITIES;}                     \
    }

namespace chemfiles {
    PLUGINS_DATA(DCD,               dcdplugin,          dcd,            false);
    PLUGINS_DATA(TRJ,               gromacsplugin,      trj,            false);
    PLUGINS_DATA(LAMMPS,            lammpsplugin,       lammpstrj,      true);
    PLUGINS_DATA(MOLDEN,            moldenplugin,       molden,         false);
}

#undef PLUGINS_FUNCTIONS
/******************************************************************************/

template <MolfileFormat F> static int register_plugin(void* user_data, vmdplugin_t* vmd_plugin) {
    auto user_plugin = static_cast<molfile_plugin_t**>(user_data);
    assert(std::string(MOLFILE_PLUGIN_TYPE) == std::string(vmd_plugin->type));

    auto plugin = reinterpret_cast<molfile_plugin_t*>(vmd_plugin);
    // When this callback is called multiple times with more than one plugin,
    // only register the one whe want
    if (MolfilePluginData<F>().reader() == plugin->name) {
        *user_plugin = plugin;
    }

    return VMDPLUGIN_SUCCESS;
}

static int molfiles_to_chemfiles_warning(int level, const char* message) {
    if (level == VMDCON_ERROR || level == VMDCON_WARN) {
        warning(message);
    }
    return 0;
}

/******************************************************************************/

template <MolfileFormat F>
Molfile<F>::Molfile(std::string path, File::Mode mode, File::Compression compression)
    : path_(std::move(path)), plugin_handle_(nullptr), data_(nullptr), natoms_(0) {
    if (mode != File::READ) {
        throw format_error(
            "molfiles based format {} is only available in read mode", plugin_data_.format()
        );
    }

    if (compression != File::DEFAULT) {
        throw format_error(
            "molfiles based format {} do not support compression", plugin_data_.format()
        );
    }

    if (plugin_data_.init()) {
        throw format_error(
            "could not initialize the {} plugin", plugin_data_.format()
        );
    }

    if (plugin_data_.registration(&plugin_handle_, register_plugin<F>)) {
        throw format_error(
            "could not register the {} plugin", plugin_data_.format()
        );
    }

    // Check the ABI version of the plugin
    assert(plugin_handle_->abiversion == vmdplugin_ABIVERSION);
    // Redirect console output to chemfiles warnings
    plugin_handle_->cons_fputs = molfiles_to_chemfiles_warning;

    // Check that needed functions are here
    if (!plugin_handle_->open_file_read ||
        (!plugin_handle_->read_next_timestep && !plugin_handle_->read_timestep) ||
        (!plugin_handle_->close_file_read)) {
        throw format_error(
            "the {} plugin does not have read capacities", plugin_data_.format()
        );
    }

    data_ = plugin_handle_->open_file_read(
        path_.c_str(), plugin_handle_->name, &natoms_
    );

    if (!data_) {
        throw format_error(
            "could not open the file at '{}' with {} plugin", path_, plugin_data_.format()
        );
    }

    read_topology();
}

template <MolfileFormat F> Molfile<F>::~Molfile() noexcept {
    if (data_) {
        plugin_handle_->close_file_read(data_);
    }
    plugin_data_.fini();
}

template <MolfileFormat F> int Molfile<F>::read_next_timestep(molfile_timestep_t* timestep) {
    if (plugin_handle_->read_next_timestep) {
        // This function is provided by classical molecular simulation format.
        return plugin_handle_->read_next_timestep(data_, natoms_, timestep);
    } else if (plugin_handle_->read_timestep) {
        // This function is provided by quantum molecular simulation format.
        return plugin_handle_->read_timestep(
            data_, natoms_, timestep, nullptr, nullptr
        );
    } else {
        throw format_error(
            "both read_next_timestep and read_timestep are missing in this "
            "plugin. This is a bug"
        );
    }
}

template <MolfileFormat F> void Molfile<F>::read(Frame& frame) {
    std::vector<float> coords(3 * static_cast<size_t>(natoms_));
    std::vector<float> velocities(0);

    molfile_timestep_t timestep{nullptr, nullptr, 0, 0, 0, 0, 0, 0, 0};
    timestep.coords = coords.data();
    if (plugin_data_.have_velocities()) {
        velocities.resize(3 * static_cast<size_t>(natoms_));
        timestep.velocities = velocities.data();
    }

    int status = read_next_timestep(&timestep);
    if (status != MOLFILE_SUCCESS) {
        throw format_error(
            "error while reading the file at '{}' with {} plugin",
            path_, plugin_data_.format()
        );
    }

    if (topology_) {
        frame.resize(topology_->size());
        frame.set_topology(*topology_);
    }
    molfile_to_frame(timestep, frame);

    frames_.emplace_back(std::move(frame.clone()));
}

template <MolfileFormat F> void Molfile<F>::read_step(size_t step, Frame& frame) {
    while (step >= frames_.size()) {
        Frame new_frame;
        this->read(new_frame);
    }
    frame = frames_.at(step).clone();
}

template <MolfileFormat F> size_t Molfile<F>::nsteps() {
    if (!plugin_handle_->read_next_timestep) {
        // FIXME: this is hacky, but the molden plugin does not respect a NULL
        // argument for molfile_timestep_t, so for now we are only able to read
        // a sinle step from all the QM format plugins.
        return 1;
    }
    size_t n = 0;
    int status = MOLFILE_SUCCESS;
    while (true) {
        status = read_next_timestep(nullptr);
        if (status == MOLFILE_SUCCESS) {
            n++;
        } else {
            break;
        }
    }
    // We need to close and re-open the file
    plugin_handle_->close_file_read(data_);
    int unused = 0;
    data_ = plugin_handle_->open_file_read(path_.c_str(), plugin_handle_->name, &unused);
    read_topology();

    return n;
}

template <MolfileFormat F>
void Molfile<F>::molfile_to_frame(const molfile_timestep_t& timestep,
                                  Frame& frame) {
    frame.set_cell({
        static_cast<double>(timestep.A), 
        static_cast<double>(timestep.B), 
        static_cast<double>(timestep.C), 
        static_cast<double>(timestep.alpha),
        static_cast<double>(timestep.beta),
        static_cast<double>(timestep.gamma)
    });

    frame.resize(static_cast<size_t>(natoms_));
    auto positions = frame.positions();
    for (size_t i = 0; i < static_cast<size_t>(natoms_); i++) {
        positions[i][0] = static_cast<double>(timestep.coords[3 * i + 0]);
        positions[i][1] = static_cast<double>(timestep.coords[3 * i + 1]);
        positions[i][2] = static_cast<double>(timestep.coords[3 * i + 2]);
    }

    if (plugin_data_.have_velocities()) {
        frame.add_velocities();
        auto velocities = frame.velocities();
        for (size_t i = 0; i < static_cast<size_t>(natoms_); i++) {
            (*velocities)[i][0] = static_cast<double>(timestep.velocities[3 * i + 0]);
            (*velocities)[i][1] = static_cast<double>(timestep.velocities[3 * i + 1]);
            (*velocities)[i][2] = static_cast<double>(timestep.velocities[3 * i + 2]);
        }
    }
}

template <MolfileFormat F> void Molfile<F>::read_topology() {
    if (plugin_handle_->read_structure == nullptr) {
        return;
    }

    std::vector<molfile_atom_t> atoms(static_cast<size_t>(natoms_));
    int optflags = 0;
    int status = plugin_handle_->read_structure(data_, &optflags, atoms.data());
    if (status != MOLFILE_SUCCESS) {
        throw format_error(
            "could not read the molecule structure with {} plugin",
            plugin_data_.format()
        );
    }

    topology_ = Topology();

    auto residues = std::unordered_map<size_t, Residue>();
    size_t atom_id = 0;
    for (auto& molfile_atom : atoms) {
        Atom atom(molfile_atom.name, molfile_atom.type);
        if (optflags & MOLFILE_MASS) {
            atom.set_mass(static_cast<double>(molfile_atom.mass));
        }
        if (optflags & MOLFILE_CHARGE) {
            atom.set_charge(static_cast<double>(molfile_atom.charge));
        }

        topology_->add_atom(std::move(atom));

        if (molfile_atom.resname != std::string("")) {
            auto resid = static_cast<size_t>(molfile_atom.resid);
            auto residue = Residue(molfile_atom.resname, resid);
            auto inserted = residues.insert({resid, std::move(residue)});
            inserted.first->second.add_atom(atom_id);
        }
        atom_id++;
    }

    if (plugin_handle_->read_bonds == nullptr) {
        return;
    }

    int nbonds = 0;
    int* from = nullptr;
    int* to = nullptr;

    int dummy = 0;
    float* _float = nullptr;
    int* _int = nullptr;
    char** _char = nullptr;

    status = plugin_handle_->read_bonds(
        data_, &nbonds, &from, &to, &_float, &_int, &dummy, &_char
    );
    if (status != MOLFILE_SUCCESS) {
        throw format_error(
            "could not read bonds with {} plugin", plugin_data_.format()
        );
    }

    for (size_t i = 0; i < static_cast<size_t>(nbonds); i++) {
        // Indexes are 1-based in Molfile
        topology_->add_bond(static_cast<size_t>(from[i] - 1),
                            static_cast<size_t>(to[i]) - 1);
    }
}

// Instanciate all the templates
template class chemfiles::Molfile<DCD>;
template class chemfiles::Molfile<TRJ>;
template class chemfiles::Molfile<LAMMPS>;
template class chemfiles::Molfile<MOLDEN>;

template<> FormatInfo chemfiles::format_information<Molfile<DCD>>() {
    return FormatInfo("DCD").with_extension(".dcd").description(
        "DCD binary format"
    );
}

template<> FormatInfo chemfiles::format_information<Molfile<TRJ>>() {
    return FormatInfo("TRJ").with_extension(".trj").description(
        "GROMACS .trj binary format"
    );
}

template<> FormatInfo chemfiles::format_information<Molfile<LAMMPS>>() {
    return FormatInfo("LAMMPS").with_extension(".lammpstrj").description(
        "LAMMPS text trajectory format"
    );
}

template<> FormatInfo chemfiles::format_information<Molfile<MOLDEN>>() {
    return FormatInfo("Molden").with_extension(".molden").description(
        "Molden text format"
    );
}
