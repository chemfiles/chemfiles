// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "chemfiles/FormatFactory.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/formats/Molfile.hpp"
#include "chemfiles/formats/NcFormat.hpp"
#include "chemfiles/formats/PDB.hpp"
#include "chemfiles/formats/XYZ.hpp"
#include "chemfiles/formats/TNG.hpp"

#include "chemfiles/files/NcFile.hpp"
#include "chemfiles/files/TNGFile.hpp"
using namespace chemfiles;

namespace chemfiles {
    extern template class Molfile<DCD>;
    extern template class Molfile<GRO>;
    extern template class Molfile<TRR>;
    extern template class Molfile<XTC>;
    extern template class Molfile<TRJ>;
    extern template class Molfile<LAMMPS>;
}

template <typename T>
void registration(FormatFactory& factory) {
    auto creator = new_format<T>;

    auto extension = std::string(T::extension());
    if (extension != "") {
        factory.register_extension(extension, creator);
    }

    auto name = std::string(T::name());
    if (name != "") {
        factory.register_format(name, creator);
    }
}

FormatFactory::FormatFactory() : formats_(), extensions_() {
    registration<XYZFormat>(*this);
    registration<PDBFormat>(*this);
    registration<TNGFormat>(*this);
    registration<NCFormat>(*this);

    // Molfile-based formats
    registration<Molfile<DCD>>(*this);
    registration<Molfile<GRO>>(*this);
    registration<Molfile<TRR>>(*this);
    registration<Molfile<XTC>>(*this);
    registration<Molfile<TRJ>>(*this);
    registration<Molfile<LAMMPS>>(*this);
}

FormatFactory& FormatFactory::get() {
    static auto instance = FormatFactory();
    return instance;
}

format_creator_t FormatFactory::format(const std::string& name) {
    if (formats_.find(name) == formats_.end()) {
        throw FormatError("Can not find the format \"" + name + "\".");
    }
    return formats_[name];
}

format_creator_t FormatFactory::by_extension(const std::string& ext) {
    if (extensions_.find(ext) == extensions_.end()) {
        throw FormatError("Can not find a format associated with the \"" + ext +
                          "\" extension.");
    }
    return extensions_[ext];
}

void FormatFactory::register_format(const std::string& name,
                                        format_creator_t creator) {
    if (formats_.find(name) != formats_.end()) {
        throw FormatError("The name \"" + name +
                          "\" is already associated with a format.");
    }
    formats_.emplace(name, creator);
}

void FormatFactory::register_extension(const std::string& ext,
                                           format_creator_t creator) {
    if (extensions_.find(ext) != extensions_.end()) {
        throw FormatError("The extension \"" + ext +
                          "\" is already associated with a format.");
    }
    extensions_.emplace(ext, creator);
}
