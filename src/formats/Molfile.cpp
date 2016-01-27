/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <map>
#include <cstdlib>

#include "chemfiles/formats/Molfile.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Topology.hpp"
using namespace chemfiles;

/******************************************************************************/

struct plugin_data_t {
    std::string format;
    std::string path;
    std::string plugin_name;
    std::string extension;
    bool have_velocities;
};

static std::map<MolfileFormat, plugin_data_t> molfile_plugins {
    {PDB, {"PDB", "pdbplugin.so", "pdb", ".pdb", false}},
    {DCD, {"DCD", "dcdplugin.so", "dcd", ".dcd", false}},
    {GRO, {"GRO", "gromacsplugin.so", "gro", ".gro", false}},
    {TRR, {"TRR", "gromacsplugin.so", "trr", ".trr", true}},
    {XTC, {"XTC", "gromacsplugin.so", "xtc", ".xtc", false}},
    {TRJ, {"TRJ", "gromacsplugin.so", "trj", ".trj", true}},
};

struct plugin_reginfo_t {
    plugin_reginfo_t() : plugin(nullptr) {}
    molfile_plugin_t* plugin;
};

template<MolfileFormat F>
static int register_plugin(void *v, vmdplugin_t *p) {
    plugin_reginfo_t *reginfo = static_cast<plugin_reginfo_t *>(v);
    if (std::string(MOLFILE_PLUGIN_TYPE) != std::string(p->type))
        throw PluginError("Wrong plugin type");

    auto plugin = reinterpret_cast<molfile_plugin_t *>(p);
    if (molfile_plugins[F].plugin_name == plugin->name){
        // When this callback is called multiple times with more
        // than one plugin, only register the one whe want
        reginfo->plugin = plugin;
    }

    return VMDPLUGIN_SUCCESS;
}

//! Check if a path exists on the filesystem
static inline bool exists(const std::string& path) {
    return std::ifstream(path).good();
}


//! Get the path to the molfile plugin `name`
static std::string plugin_path(const std::string& name){
    std::string path = "";
    // First look for the CHEMFILES_PLUGINS environement variable
    if(const char* plugin_dir = std::getenv("CHEMFILES_PLUGINS")) {
        path = plugin_dir + std::string("/") + name;
    } else { // Use the compile-time macro INSTALL_MOLFILE_DIR
        path = INSTALL_MOLFILE_DIR + std::string("/") + name;
    }

    if (exists(path)) {
        return path;
    } else {
        throw PluginError(
            "Could not find the '" + name + "' shared library. Try setting "\
            "the CHEMFILES_PLUGINS environement variable."
        );
    }
}

/******************************************************************************/

template <MolfileFormat F> Molfile<F>::Molfile(File& file) : Format(file),
plugin_(nullptr), fini_fun_(nullptr), file_handler_(nullptr),
natoms_(0), use_topology_(false) {
    // Open the _library
    lib_ = Dynlib(plugin_path(molfile_plugins[F].path));

    // Get the pointer to the initialization function
    auto init_fun = lib_.symbol<init_function_t>("vmdplugin_init");
    if (init_fun())
        throw PluginError("Could not initialize the " + molfile_plugins[F].format + " plugin_.");

    // Get the pointer to the registration function
    auto register_fun = lib_.symbol<register_function_t>("vmdplugin_register");

    // Get the pointer to the freeing function
    fini_fun_ = lib_.symbol<init_function_t>("vmdplugin_fini");

    plugin_reginfo_t reginfo;
    // The first argument in 'register_fun' is passed as the first argument to register_plugin ...
    if (register_fun(&reginfo, register_plugin<F>))
        throw PluginError("Could not register the " + molfile_plugins[F].format + " plugin_.");
    plugin_ = reginfo.plugin;

    // Check the ABI version of the loaded plugin_
    if (plugin_->abiversion != vmdplugin_ABIVERSION)
        throw PluginError("The ABI version does not match! Please recompile "
                          "chemfiles or provide another plugin_");
    // Check that needed functions are here
    if ((plugin_->open_file_read == NULL)      ||
        (plugin_->read_next_timestep  == NULL) ||
        (plugin_->close_file_read == NULL))
            throw PluginError("The " + molfile_plugins[F].format +
                              " plugin_ does not have read capacities");

    std::cout << file.filename() << std::endl;

    file_handler_ = plugin_->open_file_read(file.filename().c_str(), plugin_->name, &natoms_);

    if (!file_handler_) {
        throw FileError("Could not open the file: " + file.filename() + " with VMD molfile");
    }

    read_topology();
}

template <MolfileFormat F> Molfile<F>::~Molfile(){
    if (file_handler_) {
        plugin_->close_file_read(file_handler_);
    }
    fini_fun_();
}

template <MolfileFormat F>
std::string Molfile<F>::description() const {
    return "Molfile-based reader for the " + molfile_plugins[F].format + "format";
}

template <MolfileFormat F>
void Molfile<F>::read(Frame& frame){
    std::vector<float> coords(3*static_cast<size_t>(natoms_));
    std::vector<float> velocities(0);

    molfile_timestep_t timestep;
    timestep.coords = coords.data();
    if (molfile_plugins[F].have_velocities){
        velocities.resize(3*static_cast<size_t>(natoms_));
        timestep.velocities = velocities.data();
    }

    int result = plugin_->read_next_timestep(file_handler_, natoms_, &timestep);
    if (result != MOLFILE_SUCCESS){
        throw FormatError("Error while reading the file " + file_.filename() +
                          " using Molfile format " + molfile_plugins[F].format);
    }

    if (use_topology_){
        frame.topology(topology_);
    }
    molfile_to_frame(timestep, frame);
}

template <MolfileFormat F>
size_t Molfile<F>::nsteps() const {
    size_t n = 0;
    int result = MOLFILE_SUCCESS;
    while (true) {
        result = plugin_->read_next_timestep(file_handler_, natoms_, NULL);
        if (result == MOLFILE_SUCCESS)
            n++;
        else
            break;
    }
    // We need to close and re-open the file
    plugin_->close_file_read(file_handler_);
    int tmp = 0;
    file_handler_ = plugin_->open_file_read(file_.filename().c_str(), plugin_->name, &tmp);
    read_topology();

    return n;
}

template <MolfileFormat F>
void Molfile<F>::molfile_to_frame(const molfile_timestep_t& timestep, Frame& frame){
    auto cell = UnitCell(timestep.A, timestep.B, timestep.C,
                         timestep.alpha, timestep.beta, timestep.gamma);
    frame.cell(cell);

    auto& positions = frame.positions();
    positions.resize(static_cast<size_t>(natoms_));
    for (size_t i=0; i<static_cast<size_t>(natoms_); i++) {
        positions[i][0] = timestep.coords[3*i];
        positions[i][1] = timestep.coords[3*i + 1];
        positions[i][2] = timestep.coords[3*i + 2];
    }

    if (molfile_plugins[F].have_velocities){
        auto& velocities = frame.velocities();
        if (!velocities) {
            velocities = Array3D(static_cast<size_t>(natoms_));
        } else {
            velocities->resize(static_cast<size_t>(natoms_));
        }

        for (size_t i=0; i<static_cast<size_t>(natoms_); i++) {
            (*velocities)[i][0] = timestep.velocities[3*i];
            (*velocities)[i][1] = timestep.velocities[3*i + 1];
            (*velocities)[i][2] = timestep.velocities[3*i + 2];
        }
    }
}

template <MolfileFormat F>
void Molfile<F>::read_topology() const {
    if (plugin_->read_structure == NULL)
        return;
    else
        use_topology_ = true;

    std::vector<molfile_atom_t> atoms(static_cast<size_t>(natoms_));
    int optflags;
    int ret = plugin_->read_structure(file_handler_, &optflags, atoms.data());

    if (ret != MOLFILE_SUCCESS){
        throw PluginError("Error while reading atomic names.");
    }

    topology_ = Topology();

    for (auto& vmd_atom : atoms){
        Atom atom(vmd_atom.name);
        if (optflags & MOLFILE_MASS){
            atom.mass(vmd_atom.mass);
        }
        topology_.append(atom);
    }

    if (plugin_->read_bonds == NULL)
        return;

    int nbonds = 0, nbondtypes = 0;

    int* from = NULL;
    int* to = NULL;
    float* bondorder = NULL;
    int* bondtype = NULL;
    char** bondtypename = NULL ;

    ret = plugin_->read_bonds(file_handler_, &nbonds, &from, &to, &bondorder,
                              &bondtype, &nbondtypes, &bondtypename);

    if (ret != MOLFILE_SUCCESS){
        throw PluginError("Error while reading bonds.");
    }

    for (size_t i=0; i<static_cast<size_t>(nbonds); i++){
        // Indexes are 1-based in Molfile
        topology_.add_bond(static_cast<size_t>(from[i] - 1),
                           static_cast<size_t>(to[i]) - 1);
    }
    topology_.recalculate();
}

template <MolfileFormat F> const char* Molfile<F>::name() {
    static const char* val = molfile_plugins[F].format.c_str();
    return val;
}

template <MolfileFormat F> const char* Molfile<F>::extension() {
    static const char* val = molfile_plugins[F].extension.c_str();
    return val;
}

/******************************************************************************/

// Instanciate the templates
template class chemfiles::Molfile<PDB>;
template class chemfiles::Molfile<DCD>;
template class chemfiles::Molfile<GRO>;
template class chemfiles::Molfile<TRR>;
template class chemfiles::Molfile<XTC>;
template class chemfiles::Molfile<TRJ>;
