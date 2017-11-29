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

static unsigned edit_distance(const std::string& first, const std::string& second);

FormatFactory::FormatFactory() {
    this->add_format<XYZFormat>();
    this->add_format<PDBFormat>();
    this->add_format<TNGFormat>();
    this->add_format<AmberNetCDFFormat>();
    this->add_format<TinkerFormat>();
    this->add_format<LAMMPSDataFormat>();

    // VMD molfile plugins
    this->add_format<Molfile<DCD>>();
    this->add_format<Molfile<GRO>>();
    this->add_format<Molfile<TRR>>();
    this->add_format<Molfile<XTC>>();
    this->add_format<Molfile<TRJ>>();
    this->add_format<Molfile<MOL2>>();
    this->add_format<Molfile<LAMMPS>>();
    this->add_format<Molfile<MOLDEN>>();
}

FormatFactory& FormatFactory::get() {
    static FormatFactory instance_;
    return instance_;
}

FormatFactory::iterator FormatFactory::find_name(const formats_map_t& formats, const std::string& name) {
    for (auto it = formats.begin(); it != formats.end(); it++) {
        if (it->first.name() == name) {
            return it;
        }
    }
    return formats.end();
}

FormatFactory::iterator FormatFactory::find_extension(const formats_map_t& formats, const std::string& extension) {
    for (auto it = formats.begin(); it != formats.end(); it++) {
        if (it->first.extension() == extension) {
            return it;
        }
    }
    return formats.end();
}

void FormatFactory::register_format(FormatInfo info, format_creator_t creator) {
    auto formats = formats_.lock();
    if (info.name() == "" && info.extension() == "") {
        throw format_error(
            "can not register a format with no name and no extension"
        );
    } else if (info.name() != "" && find_name(*formats, info.name()) != formats->end()) {
        throw format_error(
            "the name '{}' is already associated with a format."
        );
    } else if (info.extension() != "" && find_extension(*formats, info.extension()) != formats->end()) {
        throw format_error(
            "the extension '{}' is already associated with a format."
        );
    } else {
        formats->push_back({info, creator});
    }
}

format_creator_t FormatFactory::name(const std::string& name) {
    auto formats = formats_.lock();
    auto it = find_name(*formats, name);
    if (it == formats->end()) {
        auto suggestions = std::vector<std::string>();
        for (auto& node: *formats) {
            if (edit_distance(name, node.first.name()) < 4) {
                suggestions.push_back(node.first.name());
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
    return it->second;
}

format_creator_t FormatFactory::extension(const std::string& extension) {
    auto formats = formats_.lock();
    auto it = find_extension(*formats, extension);
    if (it == formats->end()) {
        throw format_error(
            "can not find a format associated with the '{}' extension.", extension
        );
    }
    return it->second;
}

std::vector<FormatInfo> FormatFactory::formats() {
    auto formats = formats_.lock();
    auto metadata = std::vector<FormatInfo>();
    metadata.reserve(formats->size());
    for (auto& pair: *formats) {
        metadata.emplace_back(pair.first);
    }
    return metadata;
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
