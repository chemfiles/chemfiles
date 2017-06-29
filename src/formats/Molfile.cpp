// From vmdconio.h
#define VMDCON_ALL       0
#define VMDCON_INFO      1
#define VMDCON_WARN      2
#define VMDCON_ERROR     3
#define VMDCON_ALWAYS    4
#define VMDCON_LOG       5

#include "chemfiles/formats/Molfile.hpp"

#include "chemfiles/Frame.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/Topology.hpp"
using namespace chemfiles;

/******************************************************************************/
#define PLUGINS_DATA(FORMAT, PLUGIN, READER, VELOCITIES)                       \
    extern "C" int PLUGIN##_register(void*, vmdplugin_register_cb);            \
    extern "C" int PLUGIN##_fini(void);                                        \
    extern "C" int PLUGIN##_init(void);                                        \
    template <> struct MolfilePluginData<FORMAT> {                             \
        int init() { return PLUGIN##_init(); }                                 \
        int registration(void* data, vmdplugin_register_cb callback) {         \
            return PLUGIN##_register(data, callback);                          \
        }                                                                      \
        int fini() { return PLUGIN##_fini(); }                                 \
        std::string format() const {return #FORMAT;}                           \
        std::string plugin_name() const {return #PLUGIN;}                      \
        std::string reader() const {return #READER;}                           \
        bool have_velocities() const {return false;}                           \
    }

namespace chemfiles {
    PLUGINS_DATA(DCD,    dcdplugin,     dcd,       false);
    PLUGINS_DATA(GRO,    gromacsplugin, gro,       false);
    PLUGINS_DATA(TRR,    gromacsplugin, trr,       true);
    PLUGINS_DATA(XTC,    gromacsplugin, xtc,       false);
    PLUGINS_DATA(TRJ,    gromacsplugin, trj,       true);
    PLUGINS_DATA(LAMMPS, lammpsplugin,  lammpstrj, false);
}

#undef PLUGINS_FUNCTIONS
/******************************************************************************/

struct plugin_reginfo_t {
    plugin_reginfo_t() : plugin(nullptr) {}
    molfile_plugin_t* plugin;
};

template <MolfileFormat F> static int register_plugin(void* v, vmdplugin_t* p) {
    plugin_reginfo_t* reginfo = static_cast<plugin_reginfo_t*>(v);
    assert(std::string(MOLFILE_PLUGIN_TYPE) == std::string(p->type));

    auto plugin = reinterpret_cast<molfile_plugin_t*>(p);
    if (MolfilePluginData<F>().reader() == plugin->name) {
        // When this callback is called multiple times with more
        // than one plugin, only register the one whe want
        reginfo->plugin = plugin;
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
Molfile<F>::Molfile(const std::string& path, File::Mode mode)
    : path_(path), plugin_handle_(nullptr), file_handle_(nullptr), natoms_(0) {
    if (mode != File::READ) {
        throw FormatError(
            "VMD Molfile plugin is only available in read mode."
        );
    }

    if (plugin_data_.init()) {
        throw FormatError(
            "Could not initialize the " + plugin_data_.format() + " plugin."
        );
    }

    plugin_reginfo_t reginfo;
    // The first argument in 'register_fun' is passed as the first argument to
    // register_plugin ...
    if (plugin_data_.registration(&reginfo, register_plugin<F>)) {
        throw FormatError(
            "Could not register the " + plugin_data_.format() + " plugin."
        );
    }
    plugin_handle_ = reginfo.plugin;

    // Check the ABI version of the plugin
    assert(plugin_handle_->abiversion == vmdplugin_ABIVERSION);
    // Redirect console output to chemfiles warnings
    plugin_handle_->cons_fputs = molfiles_to_chemfiles_warning;

    // Check that needed functions are here
    if ((plugin_handle_->open_file_read == nullptr) ||
        (plugin_handle_->read_next_timestep == nullptr) ||
        (plugin_handle_->close_file_read == nullptr)) {
        throw FormatError(
            "The " + plugin_data_.format() + " plugin does not have read capacities"
        );
    }

    file_handle_ = plugin_handle_->open_file_read(
        path.c_str(), plugin_handle_->name, &natoms_
    );

    if (file_handle_ == nullptr) {
        throw FileError(
            "Could not open the file: " + path + " with VMD molfile"
        );
    }

    read_topology();
}

template <MolfileFormat F> Molfile<F>::~Molfile() noexcept {
    if (file_handle_) {
        plugin_handle_->close_file_read(file_handle_);
    }
    plugin_data_.fini();
}

template <MolfileFormat F> std::string Molfile<F>::description() const {
    return "VMD molfile based reader for the " + plugin_data_.format() + "format";
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

    int status = plugin_handle_->read_next_timestep(file_handle_, natoms_, &timestep);
    if (status != MOLFILE_SUCCESS) {
        throw FormatError(
            "Error while reading the file " + path_ + " with VMD molfile " +
            plugin_data_.format() + " reader"
        );
    }

    if (topology_) {
        frame.resize(topology_->natoms());
        frame.set_topology(*topology_);
    }
    molfile_to_frame(timestep, frame);
}

template <MolfileFormat F> size_t Molfile<F>::nsteps() {
    size_t n = 0;
    int status = MOLFILE_SUCCESS;
    while (true) {
        status = plugin_handle_->read_next_timestep(file_handle_, natoms_, nullptr);
        if (status == MOLFILE_SUCCESS) {
            n++;
        } else {
            break;
        }
    }
    // We need to close and re-open the file
    plugin_handle_->close_file_read(file_handle_);
    int tmp = 0;
    file_handle_ =
        plugin_handle_->open_file_read(path_.c_str(), plugin_handle_->name, &tmp);
    read_topology();

    return n;
}

template <MolfileFormat F>
void Molfile<F>::molfile_to_frame(const molfile_timestep_t& timestep,
                                  Frame& frame) {
    auto cell = UnitCell(timestep.A, timestep.B, timestep.C, timestep.alpha,
                         timestep.beta, timestep.gamma);
    frame.set_cell(cell);

    frame.resize(static_cast<size_t>(natoms_));
    auto positions = frame.positions();
    for (size_t i = 0; i < static_cast<size_t>(natoms_); i++) {
        positions[i][0] = timestep.coords[3 * i];
        positions[i][1] = timestep.coords[3 * i + 1];
        positions[i][2] = timestep.coords[3 * i + 2];
    }

    if (plugin_data_.have_velocities()) {
        frame.add_velocities();
        auto velocities = frame.velocities();
        for (size_t i = 0; i < static_cast<size_t>(natoms_); i++) {
            (*velocities)[i][0] = timestep.velocities[3 * i];
            (*velocities)[i][1] = timestep.velocities[3 * i + 1];
            (*velocities)[i][2] = timestep.velocities[3 * i + 2];
        }
    }
}

template <MolfileFormat F> void Molfile<F>::read_topology() {
    if (plugin_handle_->read_structure == nullptr) {
        return;
    }

    std::vector<molfile_atom_t> atoms(static_cast<size_t>(natoms_));
    int optflags = 0;
    int status = plugin_handle_->read_structure(file_handle_, &optflags, atoms.data());
    if (status != MOLFILE_SUCCESS) {
        throw FormatError("Error while reading atomic names.");
    }

    topology_ = Topology();

    auto residues = std::unordered_map<size_t, Residue>();
    size_t atom_id = 0;
    for (auto& vmd_atom : atoms) {
        Atom atom(vmd_atom.type, vmd_atom.name);
        if (optflags & MOLFILE_MASS) {
            atom.set_mass(vmd_atom.mass);
        }

        topology_->append(atom);

        if (vmd_atom.resname != std::string("")) {
            auto resid = static_cast<size_t>(vmd_atom.resid);
            auto residue = Residue(vmd_atom.resname, resid);
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
        file_handle_, &nbonds, &from, &to, &_float, &_int, &dummy, &_char
    );
    if (status != MOLFILE_SUCCESS) {
        throw FormatError("Error while reading bonds.");
    }

    for (size_t i = 0; i < static_cast<size_t>(nbonds); i++) {
        // Indexes are 1-based in Molfile
        topology_->add_bond(static_cast<size_t>(from[i] - 1),
                            static_cast<size_t>(to[i]) - 1);
    }
}

// Instanciate all the templates
template class chemfiles::Molfile<DCD>;
template class chemfiles::Molfile<GRO>;
template class chemfiles::Molfile<TRR>;
template class chemfiles::Molfile<XTC>;
template class chemfiles::Molfile<TRJ>;
template class chemfiles::Molfile<LAMMPS>;
