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

#include "formats/Molfile.hpp"
#include "Frame.hpp"
#include "Topology.hpp"
using namespace harp;

/******************************************************************************/

struct plugin_data_t {
    std::string format;
    std::string lib_name;
    bool have_velocities;
};

static std::map<MolfileFormat, plugin_data_t> molfile_plugins {
    {PDB, {"PDB", "pdbplugin.so", false}},
};


struct plugin_reginfo_t {
    void* plugin;
};

static int register_plugin(void *v, vmdplugin_t *p) {
    plugin_reginfo_t *r = static_cast<plugin_reginfo_t *>(v);
    if (std::string(MOLFILE_PLUGIN_TYPE) != std::string(p->type))
        throw PluginError("Wrong plugin type");
    r->plugin = static_cast<void *>(p);
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
: plugin(nullptr), fini_fun(nullptr), last_file_name(""), file_handler(nullptr), natoms(0){
    // Open the library
    lib = Dynlib(libpath(molfile_plugins[F].lib_name));

    // Get the pointer to the initialization function
    auto init_fun = lib.symbol<init_function_t>("vmdplugin_init");
    if (init_fun())
        throw PluginError("Could not initialize the " + molfile_plugins[F].format + " plugin.");

    // Get the pointer to the registration function
    auto register_fun = lib.symbol<register_function_t>("vmdplugin_register");

    // Get the pointer to the freeing function
    fini_fun = lib.symbol<init_function_t>("vmdplugin_fini");


    plugin_reginfo_t reginfo;
    reginfo.plugin = nullptr;

    // The first argument here is passed as the first argument to register_plugin ...
    if (register_fun(&reginfo, register_plugin))
        throw PluginError("Could not register the " + molfile_plugins[F].format + " plugin.");

    plugin = static_cast<molfile_plugin_t*>(reginfo.plugin);

    // Check the ABI version of the loaded plugin
    if (plugin->abiversion != vmdplugin_ABIVERSION)
        throw PluginError("The ABI version does not match! Please recompile "
                          "chemharp or provide another plugin");
    // Check that needed functions are here
    if ((plugin->open_file_read == NULL)      ||
        (plugin->read_next_timestep  == NULL) ||
        (plugin->close_file_read == NULL))
            throw PluginError("The " + molfile_plugins[F].format +
                              " plugin does not have read capacities");
}

template <MolfileFormat F> Molfile<F>::~Molfile(){
    if (file_handler) {
        plugin->close_file_read(file_handler);
    }
    fini_fun();
}

template <MolfileFormat F>
std::string Molfile<F>::description() const {
    return "Molfile-based reader for the " + molfile_plugins[F].format + "format";
}

template <MolfileFormat F>
void Molfile<F>::open_file_if_needed(const std::string& path) const {
    if (path != last_file_name){
        last_file_name = path;

        // Cleanup
        if (file_handler) {
            plugin->close_file_read(file_handler);
            file_handler = nullptr;
        }

        file_handler = plugin->open_file_read(last_file_name.c_str(), plugin->name, &natoms);
    }

    if (!file_handler) {
        throw FileError("Could not open the file: " + path + " with VMD molfile");
    }
}

template <MolfileFormat F>
void Molfile<F>::read(File* file, Frame& frame){
    open_file_if_needed(file->filename());

    molfile_timestep_t timestep;
    timestep.coords = new float[3*natoms];
    if (molfile_plugins[F].have_velocities)
        timestep.velocities = new float[3*natoms];

    if (timestep.coords == NULL || (molfile_plugins[F].have_velocities && timestep.velocities == NULL)) {
        delete[] timestep.coords;
        delete[] timestep.velocities;
        throw FormatError("Error allocating memory. Sorry ...");
    }

    int result = plugin->read_next_timestep(file_handler, natoms, &timestep);

    if (result != MOLFILE_SUCCESS){
        delete[] timestep.coords;
        delete[] timestep.velocities;
        throw FormatError("Error while reading the file " + last_file_name +
                          " using Molfile format " + molfile_plugins[F].format);
    }

    molfile_to_frame(timestep, frame);
    auto& topology = frame.topology();
    read_topology(topology);

    delete[] timestep.coords;
    delete[] timestep.velocities;
}

template <MolfileFormat F>
size_t Molfile<F>::nsteps(File* file) const {
    open_file_if_needed(file->filename());

    size_t n = 0;
    int result = MOLFILE_SUCCESS;
    while (true) {
        result = plugin->read_next_timestep(file_handler, natoms, NULL);
        if (result == MOLFILE_SUCCESS)
            n++;
        else
            break;
    }
    // We need to close and re-open the file
    plugin->close_file_read(file_handler);
    file_handler = plugin->open_file_read(last_file_name.c_str(), plugin->name, &natoms);

    return n;
}

template <MolfileFormat F>
void Molfile<F>::molfile_to_frame(const molfile_timestep_t& timestep, Frame& frame){
    auto cell = UnitCell(timestep.A, timestep.B, timestep.C,
                         timestep.alpha, timestep.beta, timestep.gamma);
    frame.cell(cell);

    auto& positions = frame.positions();
    positions.resize(static_cast<size_t>(natoms));
    for (size_t i=0; i<static_cast<size_t>(natoms); i++) {
        positions[i][0] = timestep.coords[3*i];
        positions[i][1] = timestep.coords[3*i + 1];
        positions[i][2] = timestep.coords[3*i + 2];
    }

    if (molfile_plugins[F].have_velocities){
        auto& velocities = frame.velocities();
        velocities.resize(static_cast<size_t>(natoms));
        for (size_t i=0; i<static_cast<size_t>(natoms); i++) {
            velocities[i][0] = timestep.velocities[3*i];
            velocities[i][1] = timestep.velocities[3*i + 1];
            velocities[i][2] = timestep.velocities[3*i + 2];
        }
    }
}

template <MolfileFormat F>
void Molfile<F>::read_topology(Topology&) {
    if (plugin->read_structure == NULL)
        return;

    // TODO: read atoms names

    if (plugin->read_bonds == NULL)
        return;

    // TODO: read bonds
}

/******************************************************************************/

// Instanciate the templates
template class harp::Molfile<PDB>;

// Redefine the registering macros
#undef REGISTER
#undef REGISTER_EXTENSION

#define REGISTER(format_t, name)                          \
template<> bool format_t::_registered_format_ =           \
TrajectoryFactory::register_format(name, {                \
    FORMAT_CREATOR(format_t),                             \
    nullptr                                               \
});

#define REGISTER_EXTENSION(format_t, extension)           \
template<> bool format_t::_registered_extension_ =        \
TrajectoryFactory::register_extension(extension, {        \
    FORMAT_CREATOR(format_t),                             \
    nullptr                                               \
});

/******************************************************************************/

REGISTER(Molfile<PDB>, "PDB");
REGISTER_EXTENSION(Molfile<PDB>, ".pdb");
