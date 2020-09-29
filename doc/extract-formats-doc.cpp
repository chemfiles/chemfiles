#include <fstream>
#include <vector>

#include "chemfiles.hpp"

static const char* yes_or_no(bool value) {
    return value ? "|yes|" : "|no|";
}

using metadata_list = std::vector<std::reference_wrapper<const chemfiles::FormatMetadata>>;
static void generate_overview(const metadata_list& list) {
    std::ofstream output("formats-overview.csv");
    output << "Format, Extension, Read, Write, Memory, Positions, Velocities, UnitCell, Atoms, Bonds, Residues" << std::endl;

    for (auto format: list) {
        auto& metadata = format.get();

        if (std::string(metadata.reference) != "") {
            output << "\"`" << metadata.name << " <" << metadata.reference << ">`_\", ";
        } else {
            output << "\"" << metadata.name << "\", ";
        }

        output << '"' << metadata.extension.value_or("|no|") << "\", ";
        output << '"' << yes_or_no(metadata.read) << "\", ";
        output << '"' << yes_or_no(metadata.write) << "\", ";
        output << '"' << yes_or_no(metadata.memory) << "\", ";
        output << '"' << yes_or_no(metadata.positions) << "\", ";
        output << '"' << yes_or_no(metadata.velocities) << "\", ";
        output << '"' << yes_or_no(metadata.unit_cell) << "\", ";
        output << '"' << yes_or_no(metadata.atoms) << "\", ";
        output << '"' << yes_or_no(metadata.bonds) << "\", ";
        output << '"' << yes_or_no(metadata.residues) << "\"";
        output << std::endl;
    }
}

int main() {
    auto list = chemfiles::formats_list();
    generate_overview(list);
}
