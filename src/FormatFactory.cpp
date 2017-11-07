// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <sstream>
#include <cctype>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chemfiles/FormatFactory.hpp"
#include "chemfiles/ErrorFmt.hpp"

#include "chemfiles/formats/Molfile.hpp"
#include "chemfiles/formats/AmberNetCDF.hpp"
#include "chemfiles/formats/LAMMPSData.hpp"
#include "chemfiles/formats/Tinker.hpp"
#include "chemfiles/formats/PDB.hpp"
#include "chemfiles/formats/XYZ.hpp"
#include "chemfiles/formats/TNG.hpp"

using namespace chemfiles;

static unsigned edit_distance(const std::string& first, const std::string& second);

namespace chemfiles {
    extern template class Molfile<DCD>;
    extern template class Molfile<GRO>;
    extern template class Molfile<TRR>;
    extern template class Molfile<XTC>;
    extern template class Molfile<TRJ>;
    extern template class Molfile<LAMMPS>;
    extern template class Molfile<MOL2>;
    extern template class Molfile<MOLDEN>;
}

FormatFactory::FormatFactory() {
    this->register_name<XYZFormat>("XYZ");
    this->register_extension<XYZFormat>(".xyz");

    this->register_name<PDBFormat>("PDB");
    this->register_extension<PDBFormat>(".pdb");

    this->register_name<TNGFormat>("TNG");
    this->register_extension<TNGFormat>(".tng");

    this->register_name<AmberNetCDFFormat>("Amber NetCDF");
    this->register_extension<AmberNetCDFFormat>(".nc");

    this->register_name<TinkerFormat>("Tinker");
    this->register_extension<TinkerFormat>(".arc");

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

    this->register_name<Molfile<MOL2>>("MOL2");
    this->register_extension<Molfile<MOL2>>(".mol2");

    this->register_name<Molfile<MOLDEN>>("Molden");
    this->register_extension<Molfile<MOLDEN>>(".molden");
}

FormatFactory& FormatFactory::get() {
    static FormatFactory instance_;
    return instance_;
}

format_creator_t FormatFactory::name(const std::string& name) {
    auto formats = formats_.lock();
    if (formats->find(name) == formats->end()) {
        auto suggestions = std::vector<std::string>();
        for (auto& node: *formats) {
            if (edit_distance(name, node.first) < 4) {
                suggestions.push_back(node.first);
            }
        }

        std::stringstream message;
        fmt::print(message, "can not find a format named '{}'.", name);

        if (!suggestions.empty()) {
            fmt::print(message, " Did you mean");
            bool first = true;
            for (auto& suggestion: suggestions) {
                if (!first) {
                    fmt::print(message, " or");
                }
                fmt::print(message, " '{}'", suggestion);
                first = false;
            }
            fmt::print(message, "?");
        }

        throw FormatError(message.str());
    }
    return formats->at(name);
}

format_creator_t FormatFactory::extension(const std::string& extension) {
    auto extensions = extensions_.lock();
    if (extensions->find(extension) == extensions->end()) {
        throw format_error(
            "can not find a format associated with the '{}' extension.", extension
        );
    }
    return extensions->at(extension);
}


// Compute the edit distance between two strings using Wagner–Fischer algorithm
unsigned edit_distance(const std::string& first, const std::string& second) {
    auto m = first.length() + 1;
    auto n = second.length() + 1;

   auto distances = std::vector<std::vector<unsigned>>(m, std::vector<unsigned>(n, 0));

   for (unsigned i=0; i<m; i++) {
       distances[i][0] = i;
   }

   for (unsigned j=0; j<n; j++) {
       distances[0][j] = j;
   }

   for (unsigned j=1; j<n; j++) {
        for (unsigned i=1; i<m; i++) {
            if (std::tolower(first[i - 1]) == std::tolower(second[j - 1])) {
                distances[i][j] = distances[i - 1][j - 1];
            } else {
                distances[i][j] = std::min(std::min(
                    distances[i - 1][j] + 1,
                    distances[i][j - 1] + 1),
                    distances[i - 1][j - 1] + 1
                );
            }
        }
   }

   return distances[m - 1][n - 1];
}
