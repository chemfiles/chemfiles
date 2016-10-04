/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include "chemfiles/formats/Molfile.hpp"

#include <cstdlib>
#include <map>

#include "chemfiles/Frame.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/Topology.hpp"
using namespace chemfiles;

/******************************************************************************/

struct plugin_data_t {
    std::string format;
    std::string plugin;
    std::string reader;
    std::string extension;
    bool have_velocities;
};

static std::map<MolfileFormat, plugin_data_t> molfile_plugins{
    {DCD, {"DCD", "dcdplugin", "dcd", ".dcd", false}},
    {GRO, {"GRO", "gromacsplugin", "gro", ".gro", false}},
    {TRR, {"TRR", "gromacsplugin", "trr", ".trr", true}},
    {XTC, {"XTC", "gromacsplugin", "xtc", ".xtc", false}},
    {TRJ, {"TRJ", "gromacsplugin", "trj", ".trj", true}},
    {LAMMPS, {"LAMMPS", "lammpsplugin", "lammpstrj", ".lammpstrj", false}},
};

struct plugin_reginfo_t {
    plugin_reginfo_t() : plugin(nullptr) {}
    molfile_plugin_t* plugin;
};

template <MolfileFormat F> static int register_plugin(void* v, vmdplugin_t* p) {
    plugin_reginfo_t* reginfo = static_cast<plugin_reginfo_t*>(v);
    assert(std::string(MOLFILE_PLUGIN_TYPE) == std::string(p->type));

    auto plugin = reinterpret_cast<molfile_plugin_t*>(p);
    if (molfile_plugins[F].reader == plugin->name) {
        // When this callback is called multiple times with more
        // than one plugin, only register the one whe want
        reginfo->plugin = plugin;
    }

    return VMDPLUGIN_SUCCESS;
}

/******************************************************************************/

template <MolfileFormat F>
Molfile<F>::Molfile(File& file)
    : Format(file), plugin_(nullptr), file_handler_(nullptr), natoms_(0) {

    if (functions_.init()) {
        throw FormatError("Could not initialize the " +
                          molfile_plugins[F].format + " plugin_.");
    }

    plugin_reginfo_t reginfo;
    // The first argument in 'register_fun' is passed as the first argument to
    // register_plugin ...
    if (functions_.registration(&reginfo, register_plugin<F>)) {
        throw FormatError("Could not register the " +
                          molfile_plugins[F].format + " plugin_.");
    }
    plugin_ = reginfo.plugin;

    // Check the ABI version of the loaded plugin_
    assert(plugin_->abiversion == vmdplugin_ABIVERSION);

    // Check that needed functions are here
    if ((plugin_->open_file_read == nullptr) ||
        (plugin_->read_next_timestep == nullptr) ||
        (plugin_->close_file_read == nullptr)) {
        throw FormatError("The " + molfile_plugins[F].format +
                          " plugin_ does not have read capacities");
    }

    file_handler_ = plugin_->open_file_read(file.filename().c_str(),
                                            plugin_->name, &natoms_);

    if (file_handler_ == nullptr) {
        throw FileError("Could not open the file: " + file.filename() +
                        " with VMD molfile");
    }

    read_topology();
}

template <MolfileFormat F> Molfile<F>::~Molfile() noexcept {
    if (file_handler_) {
        plugin_->close_file_read(file_handler_);
    }
    functions_.fini();
}

template <MolfileFormat F> std::string Molfile<F>::description() const {
    return "Molfile-based reader for the " + molfile_plugins[F].format +
           "format";
}

template <MolfileFormat F> void Molfile<F>::read(Frame& frame) {
    std::vector<float> coords(3 * static_cast<size_t>(natoms_));
    std::vector<float> velocities(0);

    molfile_timestep_t timestep{nullptr, nullptr, 0, 0, 0, 0, 0, 0, 0};
    timestep.coords = coords.data();
    if (molfile_plugins[F].have_velocities) {
        velocities.resize(3 * static_cast<size_t>(natoms_));
        timestep.velocities = velocities.data();
    }

    int status = plugin_->read_next_timestep(file_handler_, natoms_, &timestep);
    if (status != MOLFILE_SUCCESS) {
        throw FormatError("Error while reading the file " + file_.filename() +
                          " using Molfile format " + molfile_plugins[F].format);
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
        status = plugin_->read_next_timestep(file_handler_, natoms_, nullptr);
        if (status == MOLFILE_SUCCESS) {
            n++;
        } else {
            break;
        }
    }
    // We need to close and re-open the file
    plugin_->close_file_read(file_handler_);
    int tmp = 0;
    file_handler_ =
        plugin_->open_file_read(file_.filename().c_str(), plugin_->name, &tmp);
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

    if (molfile_plugins[F].have_velocities) {
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
    if (plugin_->read_structure == nullptr) {
        return;
    }

    std::vector<molfile_atom_t> atoms(static_cast<size_t>(natoms_));
    int optflags = 0;
    int status = plugin_->read_structure(file_handler_, &optflags, atoms.data());
    if (status != MOLFILE_SUCCESS) {
        throw FormatError("Error while reading atomic names.");
    }

    topology_ = Topology();

    auto residues = std::map<size_t, Residue>();
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

    if (plugin_->read_bonds == nullptr) {
        return;
    }

    int nbonds = 0, nbondtypes = 0;
    int* from = nullptr;
    int* to = nullptr;
    float* bondorder = nullptr;
    int* bondtype = nullptr;
    char** bondtypename = nullptr;

    status = plugin_->read_bonds(file_handler_, &nbonds, &from, &to, &bondorder,
                              &bondtype, &nbondtypes, &bondtypename);
    if (status != MOLFILE_SUCCESS) {
        throw FormatError("Error while reading bonds.");
    }

    for (size_t i = 0; i < static_cast<size_t>(nbonds); i++) {
        // Indexes are 1-based in Molfile
        topology_->add_bond(static_cast<size_t>(from[i] - 1),
                            static_cast<size_t>(to[i]) - 1);
    }
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
// Instanciate all the templates

#define PLUGINS_FUNCTIONS(PLUGIN, FORMAT)                                      \
    extern "C" int PLUGIN##_register(void*, vmdplugin_register_cb);            \
    extern "C" int PLUGIN##_fini(void);                                        \
    extern "C" int PLUGIN##_init(void);                                        \
    template <> struct VMDFunctions<FORMAT> {                                  \
        int init() { return PLUGIN##_init(); }                                 \
        int registration(void* data, vmdplugin_register_cb callback) {         \
            return PLUGIN##_register(data, callback);                          \
        }                                                                      \
        int fini() { return PLUGIN##_fini(); }                                 \
    }

namespace chemfiles {
PLUGINS_FUNCTIONS(dcdplugin, DCD);
PLUGINS_FUNCTIONS(gromacsplugin, GRO);
PLUGINS_FUNCTIONS(gromacsplugin, TRR);
PLUGINS_FUNCTIONS(gromacsplugin, XTC);
PLUGINS_FUNCTIONS(gromacsplugin, TRJ);
PLUGINS_FUNCTIONS(lammpsplugin, LAMMPS);
}

#undef PLUGINS_FUNCTIONS

template class chemfiles::Molfile<DCD>;
template class chemfiles::Molfile<GRO>;
template class chemfiles::Molfile<TRR>;
template class chemfiles::Molfile<XTC>;
template class chemfiles::Molfile<TRJ>;
template class chemfiles::Molfile<LAMMPS>;
