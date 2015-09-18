/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/


#ifndef HARP_FORMAT_MOLFILE_HPP
#define HARP_FORMAT_MOLFILE_HPP

#include "chemharp/Format.hpp"
#include "chemharp/Dynlib.hpp"
#include "chemharp/register_formats.hpp"
#include "chemharp/Topology.hpp"

#include <memory>

extern "C" {
#include "vmdplugin.h"
#include "molfile_plugin.h"
}

namespace harp {

class Topology;

/*!
 * List all the VMD molfile plugins enabled. For more documentation about VMD molfile
 * plugins, please see: http://www.ks.uiuc.edu/Research/vmd/plugins/molfile/
 */
enum MolfileFormat {
    PDB, ///< PDB file format
    DCD, ///< DCD binary file format
    GRO, ///< Gromacs .gro file format
    TRR, ///< Gromacs .trr file format
    XTC, ///< Gromacs .xtc file format
    TRJ, ///< Gromacs .trj file format
};

/*!
 * @class Molfile formats/Molfile.hpp formats/Molfile.cpp
 *
 * Use of VMD Molfile plugins as format reader/writer. This class is templated by a value
 * in the MolfileFormat enum.
 */
template <MolfileFormat F>
class Molfile : public Format {
public:
    Molfile(File& file);
    ~Molfile();

    virtual void read(Frame& frame) override;
    virtual std::string description() const override;
    virtual size_t nsteps() const override;

    static const char* name();
    static const char* extension();
private:
    /// Convert a molfile timestep to a chemharp frame
    void molfile_to_frame(const molfile_timestep_t& timestep, Frame& frame);
    /// Read topological information in the current file, if any.
    void read_topology() const;

    /// Dynamic library associated with the VMD plugin
    Dynlib _lib;
    /// VMD molfile plugin
    molfile_plugin_t* _plugin;

    typedef int (*init_function_t)(void);
    typedef int (*register_function_t)(void*, vmdplugin_register_cb);
    /// Function to call at in the destructor
    init_function_t _fini_fun;

    /// The file handler
    mutable void* _file_handler;
    /// The number of atoms in the last trajectory read
    int _natoms;

    /// Do we have topological information in this plugin ?
    mutable bool _use_topology;
    /// Store topological information
    mutable Topology _topology;
};

typedef typename concat<FORMATS_LIST, Molfile<PDB>>::type molfile_list_1;
typedef typename concat<molfile_list_1, Molfile<DCD>>::type molfile_list_2;
typedef typename concat<molfile_list_2, Molfile<GRO>>::type molfile_list_3;
typedef typename concat<molfile_list_3, Molfile<TRR>>::type molfile_list_4;
typedef typename concat<molfile_list_4, Molfile<XTC>>::type molfile_list_5;
typedef typename concat<molfile_list_5, Molfile<TRJ>>::type molfile_list_6;

#undef FORMATS_LIST
#define FORMATS_LIST molfile_list_6

}

#endif
