// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cctype>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <fmt/ostream.h>

#include "chemfiles/mutex.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/FormatFactory.hpp"
#include "chemfiles/error_fmt.hpp"

#include "chemfiles/formats/Molfile.hpp"
#include "chemfiles/formats/AmberNetCDF.hpp"
#include "chemfiles/formats/LAMMPSData.hpp"
#include "chemfiles/formats/Tinker.hpp"
#include "chemfiles/formats/PDB.hpp"
#include "chemfiles/formats/XYZ.hpp"
#include "chemfiles/formats/SDF.hpp"
#include "chemfiles/formats/TNG.hpp"
#include "chemfiles/formats/MMTF.hpp"
#include "chemfiles/formats/CSSR.hpp"
#include "chemfiles/formats/GRO.hpp"
#include "chemfiles/formats/MOL2.hpp"
#include "chemfiles/formats/mmCIF.hpp"
#include "chemfiles/formats/CML.hpp"
#include "chemfiles/formats/SMI.hpp"
#include "chemfiles/formats/TRR.hpp"
#include "chemfiles/formats/XTC.hpp"

#define SENTINEL_INDEX (static_cast<size_t>(-1))

using namespace chemfiles;

namespace chemfiles {
    extern template class Molfile<DCD>;
    extern template class Molfile<TRJ>;
    extern template class Molfile<LAMMPS>;
    extern template class Molfile<MOLDEN>;
}

static unsigned edit_distance(const std::string& first, const std::string& second);
static size_t find_by_name(const std::vector<RegisteredFormat>& formats, const std::string& name);
static size_t find_by_extension(const std::vector<RegisteredFormat>& formats, const std::string& extension);

FormatFactory::FormatFactory() {
    this->add_format<XYZFormat>();
    this->add_format<PDBFormat>();
    this->add_format<TNGFormat>();
    this->add_format<AmberNetCDFFormat>();
    this->add_format<TinkerFormat>();
    this->add_format<LAMMPSDataFormat>();
    this->add_format<SDFFormat>();
    this->add_format<CSSRFormat>();
    this->add_format<GROFormat>();
    this->add_format<MOL2Format>();
    this->add_format<mmCIFFormat>();
    this->add_format<MMTFFormat>();
    this->add_format<CMLFormat>();
    this->add_format<SMIFormat>();
    this->add_format<TRRFormat>();
    this->add_format<XTCFormat>();

    // VMD molfile plugins
    this->add_format<Molfile<DCD>>();
    this->add_format<Molfile<TRJ>>();
    this->add_format<Molfile<LAMMPS>>();
    this->add_format<Molfile<MOLDEN>>();
}

FormatFactory& FormatFactory::get() {
    static FormatFactory instance_;
    return instance_;
}

void FormatFactory::register_format(FormatInfo info, format_creator_t creator) {
    auto guard = formats_.lock();
    auto& formats = *guard;

    if (info.name().empty()) {
        throw format_error(
            "can not register a format with no name"
        );
    }

    auto idx = find_by_name(formats, info.name());
    if (idx != SENTINEL_INDEX) {
        throw format_error(
            "there is already a format associated with the name '{}'", info.name()
        );
    }

    if (!info.extension().empty()) {
        idx = find_by_extension(formats, info.extension());
        if (idx != SENTINEL_INDEX) {
            throw format_error(
                "the extension '{}' is already associated with format '{}'",
                info.extension(), formats[idx].info.name()
            );
        }
    }

    // actually register the format
    formats.push_back({info, creator});
}

format_creator_t FormatFactory::name(const std::string& name) {
    auto guard = formats_.lock();
    auto& formats = *guard;

    auto idx = find_by_name(formats, name);
    if (idx == SENTINEL_INDEX) {
        auto suggestions = std::vector<std::string>();
        for (auto& other: formats) {
            if (edit_distance(name, other.info.name()) < 4) {
                suggestions.push_back(other.info.name());
            }
        }

        std::stringstream message;
        fmt::print(message, "can not find a format named '{}'", name);

        if (!suggestions.empty()) {
            fmt::print(message, ", did you mean");
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
    return formats.at(idx).creator;
}

format_creator_t FormatFactory::extension(const std::string& extension) {
    auto guard = formats_.lock();
    auto& formats = *guard;

    auto idx = find_by_extension(formats, extension);
    if (idx == SENTINEL_INDEX) {
        throw format_error(
            "can not find a format associated with the '{}' extension", extension
        );
    }
    return formats.at(idx).creator;
}

std::vector<FormatInfo> FormatFactory::formats() {
    auto formats = formats_.lock();
    auto metadata = std::vector<FormatInfo>();
    metadata.reserve(formats->size());
    for (auto& format: *formats) {
        metadata.emplace_back(format.info);
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

size_t find_by_name(const std::vector<RegisteredFormat>& formats, const std::string& name) {
    for (size_t i=0; i<formats.size(); i++) {
        if (formats[i].info.name() == name) {
            return i;
        }
    }
    return SENTINEL_INDEX;
}

size_t find_by_extension(const std::vector<RegisteredFormat>& formats, const std::string& extension) {
    for (size_t i=0; i<formats.size(); i++) {
        if (formats[i].info.extension() == extension) {
            return i;
        }
    }
    return SENTINEL_INDEX;
}
