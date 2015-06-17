/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include <map>
#include <cstdlib>

#include "chemharp/formats/Molfile.hpp"
#include "chemharp/Frame.hpp"
#include "chemharp/Topology.hpp"
using namespace harp;

/******************************************************************************/

struct plugin_data_t {
    std::string format;
    std::string path;
    std::string plugin_name;
    bool have_velocities;
};

static std::map<MolfileFormat, plugin_data_t> molfile_plugins {
    {PDB, {"PDB", "pdbplugin.so", "pdb", false}},
    {DCD, {"DCD", "dcdplugin.so", "dcd", false}},
    {GRO, {"GRO", "gromacsplugin.so", "gro", false}},
    {TRR, {"TRR", "gromacsplugin.so", "trr", true}},
    {XTC, {"XTC", "gromacsplugin.so", "xtc", false}},
    {TRJ, {"Gromacs trj", "gromacsplugin.so", "trj", true}},
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

static std::string libpath(const std::string& lib_name){
    // First look for the CHRP_MOLFILES environement variable
    if(const char* MOLFILE_DIR = std::getenv("CHRP_MOLFILES")) {
        return MOLFILE_DIR + lib_name;
    } else { // Use the compile-time macro CHRP_MOLFILES_DIR
        return CHRP_MOLFILES_DIR + lib_name;
    }
}

/******************************************************************************/

template <MolfileFormat F> Molfile<F>::Molfile()
: _plugin(nullptr), _fini_fun(nullptr), _last_file_name(""), _file_handler(nullptr),
_natoms(0), _use_topology(false) {
    // Open the _library
    _lib = Dynlib(libpath(molfile_plugins[F].path));

    // Get the pointer to the initialization function
    auto init_fun = _lib.symbol<init_function_t>("vmdplugin_init");
    if (init_fun())
        throw PluginError("Could not initialize the " + molfile_plugins[F].format + " _plugin.");

    // Get the pointer to the registration function
    auto register_fun = _lib.symbol<register_function_t>("vmdplugin_register");

    // Get the pointer to the freeing function
    _fini_fun = _lib.symbol<init_function_t>("vmdplugin_fini");

    plugin_reginfo_t reginfo;
    // The first argument in 'register_fun' is passed as the first argument to register_plugin ...
    if (register_fun(&reginfo, register_plugin<F>))
        throw PluginError("Could not register the " + molfile_plugins[F].format + " _plugin.");
    _plugin = reginfo.plugin;

    // Check the ABI version of the loaded _plugin
    if (_plugin->abiversion != vmdplugin_ABIVERSION)
        throw PluginError("The ABI version does not match! Please recompile "
                          "chemharp or provide another _plugin");
    // Check that needed functions are here
    if ((_plugin->open_file_read == NULL)      ||
        (_plugin->read_next_timestep  == NULL) ||
        (_plugin->close_file_read == NULL))
            throw PluginError("The " + molfile_plugins[F].format +
                              " _plugin does not have read capacities");
}

template <MolfileFormat F> Molfile<F>::~Molfile(){
    if (_file_handler) {
        _plugin->close_file_read(_file_handler);
    }
    _fini_fun();
}

template <MolfileFormat F>
std::string Molfile<F>::description() const {
    return "Molfile-based reader for the " + molfile_plugins[F].format + "format";
}

template <MolfileFormat F>
void Molfile<F>::open_file_if_needed(const std::string& path) const {
    if (path != _last_file_name){
        _last_file_name = path;

        // Cleanup
        if (_file_handler) {
            _plugin->close_file_read(_file_handler);
            _file_handler = nullptr;
        }

        _file_handler = _plugin->open_file_read(_last_file_name.c_str(), _plugin->name, &_natoms);
    }

    if (!_file_handler) {
        throw FileError("Could not open the file: " + path + " with VMD molfile");
    }

    read_topology();
}

template <MolfileFormat F>
void Molfile<F>::read(File* file, Frame& frame){
    open_file_if_needed(file->filename());

    std::vector<float> coords(3*static_cast<size_t>(_natoms));
    std::vector<float> velocities(0);

    molfile_timestep_t timestep;
    timestep.coords = coords.data();
    if (molfile_plugins[F].have_velocities){
        velocities.resize(3*static_cast<size_t>(_natoms));
        timestep.velocities = coords.data();
    }

    int result = _plugin->read_next_timestep(_file_handler, _natoms, &timestep);

    if (result != MOLFILE_SUCCESS){
        throw FormatError("Error while reading the file " + _last_file_name +
                          " using Molfile format " + molfile_plugins[F].format);
    }

    if (_use_topology){
        frame.topology(_topology);
    }
    molfile_to_frame(timestep, frame);
}

template <MolfileFormat F>
size_t Molfile<F>::nsteps(File* file) const {
    open_file_if_needed(file->filename());

    size_t n = 0;
    int result = MOLFILE_SUCCESS;
    while (true) {
        result = _plugin->read_next_timestep(_file_handler, _natoms, NULL);
        if (result == MOLFILE_SUCCESS)
            n++;
        else
            break;
    }
    // We need to close and re-open the file
    _plugin->close_file_read(_file_handler);
    _file_handler = _plugin->open_file_read(_last_file_name.c_str(), _plugin->name, &_natoms);

    return n;
}

template <MolfileFormat F>
void Molfile<F>::molfile_to_frame(const molfile_timestep_t& timestep, Frame& frame){
    auto cell = UnitCell(timestep.A, timestep.B, timestep.C,
                         timestep.alpha, timestep.beta, timestep.gamma);
    frame.cell(cell);

    auto& positions = frame.positions();
    positions.resize(static_cast<size_t>(_natoms));
    for (size_t i=0; i<static_cast<size_t>(_natoms); i++) {
        positions[i][0] = timestep.coords[3*i];
        positions[i][1] = timestep.coords[3*i + 1];
        positions[i][2] = timestep.coords[3*i + 2];
    }

    if (molfile_plugins[F].have_velocities){
        auto& velocities = frame.velocities();
        velocities.resize(static_cast<size_t>(_natoms));
        for (size_t i=0; i<static_cast<size_t>(_natoms); i++) {
            velocities[i][0] = timestep.velocities[3*i];
            velocities[i][1] = timestep.velocities[3*i + 1];
            velocities[i][2] = timestep.velocities[3*i + 2];
        }
    }
}

template <MolfileFormat F>
void Molfile<F>::read_topology() const {
    if (_plugin->read_structure == NULL)
        return;
    else
        _use_topology = true;

    std::vector<molfile_atom_t> atoms(static_cast<size_t>(_natoms));
    int optflags;
    int ret = _plugin->read_structure(_file_handler, &optflags, atoms.data());

    if (ret != MOLFILE_SUCCESS){
        throw PluginError("Error while reading atomic names.");
    }

    _topology = Topology();

    for (auto& vmd_atom : atoms){
        Atom atom(vmd_atom.name);
        if (optflags & MOLFILE_MASS){
            atom.mass(vmd_atom.mass);
        }
        _topology.append(atom);
    }

    if (_plugin->read_bonds == NULL)
        return;

    int nbonds = 0, nbondtypes = 0;

    int* from = NULL;
    int* to = NULL;
    float* bondorder = NULL;
    int* bondtype = NULL;
    char** bondtypename = NULL ;

    ret = _plugin->read_bonds(_file_handler, &nbonds, &from, &to, &bondorder,
                              &bondtype, &nbondtypes, &bondtypename);

    if (ret != MOLFILE_SUCCESS){
        throw PluginError("Error while reading bonds.");
    }

    for (size_t i=0; i<static_cast<size_t>(nbonds); i++){
        // Indexes are 1-based in Molfile
        _topology.add_bond(static_cast<size_t>(from[i] - 1),
                           static_cast<size_t>(to[i]) - 1);
    }
    _topology.recalculate();
}

/******************************************************************************/

// Instanciate the templates
template class harp::Molfile<PDB>;
template class harp::Molfile<DCD>;
template class harp::Molfile<TRR>;
template class harp::Molfile<XTC>;
template class harp::Molfile<TRJ>;

// Redefine the registering macros
#undef REGISTER
#undef REGISTER_EXTENSION

#define REGISTER(format_t, name)                          \
template<> bool format_t::_registered_format_ =           \
TrajectoryFactory::register_format(name, {                \
    new_format<format_t>,                                 \
    nullptr                                               \
});

#define REGISTER_EXTENSION(format_t, extension)           \
template<> bool format_t::_registered_extension_ =        \
TrajectoryFactory::register_extension(extension, {        \
    new_format<format_t>,                                 \
    nullptr                                               \
});

/******************************************************************************/

REGISTER(Molfile<PDB>, "PDB");
REGISTER_EXTENSION(Molfile<PDB>, ".pdb");

REGISTER(Molfile<DCD>, "DCD");
REGISTER_EXTENSION(Molfile<DCD>, ".dcd");

REGISTER(Molfile<GRO>, "GRO");
REGISTER_EXTENSION(Molfile<GRO>, ".gro");

REGISTER(Molfile<TRR>, "TRR");
REGISTER_EXTENSION(Molfile<TRR>, ".trr");

REGISTER(Molfile<XTC>, "XTC");
REGISTER_EXTENSION(Molfile<XTC>, ".xtc");

REGISTER(Molfile<TRJ>, "Gromacs trj");
REGISTER_EXTENSION(Molfile<TRJ>, ".trj");
