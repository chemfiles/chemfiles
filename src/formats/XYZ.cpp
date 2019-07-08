// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <exception>

#include <fmt/ostream.h>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Topology.hpp"

#include "chemfiles/types.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/ErrorFmt.hpp"

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
        natoms = parse<size_t>(file_->readline());
        file_->readline(); // XYZ comment line;
    } catch (const std::exception& e) {
        throw format_error("can not read next step as XYZ: {}", e.what());
    }

    frame.reserve(natoms);
    frame.resize(0);

    for (const auto& line: file_->readlines(natoms)) {
        double x = 0, y = 0, z = 0;
        char name[32] = {0};
        scan(line, "%31s %lf %lf %lf", &name[0], &x, &y, &z);
        frame.add_atom(Atom(name), Vector3D(x, y, z));
    }
}

void XYZFormat::write_next(const Frame& frame) {
    auto& topology = frame.topology();
    auto& positions = frame.positions();
    assert(frame.size() == topology.size());

    fmt::print(*file_, "{}\n", frame.size());
    fmt::print(*file_, "Written by the chemfiles library\n", frame.size());

    for (size_t i = 0; i < frame.size(); i++) {
        auto name = topology[i].name();
        if (name == "") {name = "X";}
        fmt::print(
            *file_, "{} {} {} {}\n",
            name, positions[i][0], positions[i][1], positions[i][2]
        );
    }
}

std::streampos XYZFormat::forward() {
    if (!*file_) {
        return std::streampos(-1);
    }

    auto position = file_->tellg();
    size_t natoms = 0;
    try {
        natoms = parse<size_t>(file_->readline());
    } catch (const FileError&) {
        // No more line left in the file
        return std::streampos(-1);
    } catch (const Error&) {
        // We could not read an integer, so give up here
        return std::streampos(-1);
    }

    try {
        file_->readlines(natoms + 1);
    } catch (const FileError& e) {
        // We could not read the lines from the file
        throw format_error(
            "not enough lines for XYZ format: {}", file_->path(), e.what()
        );
    }
    return position;
}
