/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include "chemfiles/TrajectoryFactory.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/formats/Molfile.hpp"
#include "chemfiles/formats/NcFormat.hpp"
#include "chemfiles/formats/PDB.hpp"
#include "chemfiles/formats/XYZ.hpp"

#include "chemfiles/files/NcFile.hpp"
using namespace chemfiles;

template <typename T>
void registration(trajectory_map_t& formats, trajectory_map_t& extensions) {
    auto creator =
        trajectory_builder_t{new_format<T>, new_file<typename T::file_t>};

    auto ext = std::string(T::extension());
    if (ext != "") {
        if (extensions.find(ext) != extensions.end()) {
            throw FormatError("The extension \"" + ext +
                              "\" is already associated with a format.");
        }
        extensions.emplace(ext, creator);
    }

    auto name = std::string(T::name());
    if (name != "") {
        if (formats.find(name) != formats.end()) {
            throw FormatError("The name \"" + name +
                              "\" is already associated with a format.");
        }
        formats.emplace(name, creator);
    }
}

TrajectoryFactory::TrajectoryFactory() : formats_(), extensions_() {
    registration<XYZFormat>(formats_, extensions_);
    registration<PDBFormat>(formats_, extensions_);

#if HAVE_NETCDF
    registration<NCFormat>(formats_, extensions_);
#endif

    // Molfile-based formats
    registration<Molfile<DCD>>(formats_, extensions_);
    registration<Molfile<GRO>>(formats_, extensions_);
    registration<Molfile<TRR>>(formats_, extensions_);
    registration<Molfile<XTC>>(formats_, extensions_);
    registration<Molfile<TRJ>>(formats_, extensions_);
    registration<Molfile<LAMMPS>>(formats_, extensions_);
}

TrajectoryFactory& TrajectoryFactory::get() {
    static auto instance = TrajectoryFactory();
    return instance;
}

trajectory_builder_t TrajectoryFactory::format(const string& name) {
    if (formats_.find(name) == formats_.end()) {
        throw FormatError("Can not find the format \"" + name + "\".");
    }
    return formats_[name];
}

trajectory_builder_t TrajectoryFactory::by_extension(const string& ext) {
    if (extensions_.find(ext) == extensions_.end()) {
        throw FormatError("Can not find a format associated with the \"" + ext +
                          "\" extension.");
    }
    return extensions_[ext];
}

void TrajectoryFactory::register_format(const string& name,
                                        trajectory_builder_t tb) {
    if (formats_.find(name) != formats_.end()) {
        throw FormatError("The name \"" + name +
                          "\" is already associated with a format.");
    }
    formats_.emplace(name, tb);
}

void TrajectoryFactory::register_extension(const string& ext,
                                           trajectory_builder_t tb) {
    if (extensions_.find(ext) != extensions_.end()) {
        throw FormatError("The extension \"" + ext +
                          "\" is already associated with a format.");
    }
    extensions_.emplace(ext, tb);
}
