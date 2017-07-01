// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/FormatFactory.hpp"

#include "chemfiles/formats/Molfile.hpp"
#include "chemfiles/formats/AmberNetCDF.hpp"
#include "chemfiles/formats/LAMMPSData.hpp"
#include "chemfiles/formats/PDB.hpp"
#include "chemfiles/formats/XYZ.hpp"
#include "chemfiles/formats/TNG.hpp"

using namespace chemfiles;

namespace chemfiles {
    extern template class Molfile<DCD>;
    extern template class Molfile<GRO>;
    extern template class Molfile<TRR>;
    extern template class Molfile<XTC>;
    extern template class Molfile<TRJ>;
    extern template class Molfile<LAMMPS>;
}

FormatFactory::FormatFactory() : formats_(), extensions_() {
    this->register_name<XYZFormat>("XYZ");
    this->register_extension<XYZFormat>(".xyz");

    this->register_name<PDBFormat>("PDB");
    this->register_extension<PDBFormat>(".pdb");

    this->register_name<TNGFormat>("TNG");
    this->register_extension<TNGFormat>(".tng");

    this->register_name<AmberNetCDFFormat>("Amber NetCDF");
    this->register_extension<AmberNetCDFFormat>(".nc");

    this->register_name<LAMMPSDataFormat>("LAMMPS Data");

    // VMD molfile plugins
    this->register_name<Molfile<DCD>>("DCD");
    this->register_extension<Molfile<DCD>>(".dcd");

    this->register_name<Molfile<GRO>>("GRO");
    this->register_extension<Molfile<GRO>>(".gro");

    this->register_name<Molfile<TRR>>("TRR");
    this->register_extension<Molfile<TRR>>(".trr");

    this->register_name<Molfile<XTC>>("XTC");
    this->register_extension<Molfile<XTC>>(".xtc");

    this->register_name<Molfile<TRJ>>("TRJ");
    this->register_extension<Molfile<TRJ>>(".trj");

    this->register_name<Molfile<LAMMPS>>("LAMMPS");
    this->register_extension<Molfile<LAMMPS>>(".lammpstrj");
}

FormatFactory& FormatFactory::get() {
    static auto instance = FormatFactory();
    return instance;
}

format_creator_t FormatFactory::name(const std::string& name) {
    if (formats_.find(name) == formats_.end()) {
        throw FormatError("Can not find the " + name + " format.");
    }
    return formats_.at(name);
}

format_creator_t FormatFactory::extension(const std::string& extension) {
    if (extensions_.find(extension) == extensions_.end()) {
        throw FormatError(
            "Can not find a format associated with the '" + extension + "' extension."
        );
    }
    return extensions_.at(extension);
}
