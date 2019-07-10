// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <exception>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Topology.hpp"

#include "chemfiles/types.hpp"
#include "chemfiles/parse.hpp"
#include "chemfiles/string_view.hpp"
#include "chemfiles/error_fmt.hpp"

#include "chemfiles/formats/XYZ.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<XYZFormat>() {
    return FormatInfo("XYZ").with_extension(".xyz").description(
        "XYZ text format"
    );
}

void XYZFormat::read_next(Frame& frame) {
    size_t natoms = 0;
    try {
        natoms = parse<size_t>(file_.readline());
        file_.readline(); // XYZ comment line;
    } catch (const std::exception& e) {
        throw format_error("can not read next step as XYZ: {}", e.what());
    }

    frame.reserve(natoms);
    for (size_t i=0; i<natoms; i++) {
        auto line = file_.readline();
        double x = 0, y = 0, z = 0;
        std::string name;
        scan(line, name, x, y, z);
        frame.add_atom(Atom(std::move(name)), Vector3D(x, y, z));
    }
}

void XYZFormat::write_next(const Frame& frame) {
    auto& topology = frame.topology();
    auto& positions = frame.positions();
    assert(frame.size() == topology.size());

    file_.print("{}\n", frame.size());
    file_.print("Written by the chemfiles library\n", frame.size());

    for (size_t i = 0; i < frame.size(); i++) {
        auto name = topology[i].name();
        if (name.empty()) {
            name = "X";
        }
        file_.print("{} {} {} {}\n",
            name, positions[i][0], positions[i][1], positions[i][2]
        );
    }
}

optional<uint64_t> XYZFormat::forward() {
    auto position = file_.tellpos();
    size_t natoms = 0;
    try {
        natoms = parse<size_t>(file_.readline());
    } catch (const Error&) {
        // We could not read an integer, so give up here
        return nullopt;
    }
    static int step = 0;
    step++;

    for (size_t i=0; i<natoms + 1; i++) {
        if (file_.eof()) {
            throw format_error(
                "XYZ format: not enough lines at step {} (expected {}, got {})",
                step, natoms + 2, i + 1
            );
        }

        file_.readline();
    }

    return position;
}
