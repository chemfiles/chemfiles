// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chemfiles/formats/GRO.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<GROFormat>() {
    return FormatInfo("GRO").with_extension(".gro").description(
        "GROMACS GRO text format"
    );
}

/// Check the number of digits before the decimal separator to be sure than
/// we can represen them. In case of error, use the given `context` in the error
/// message
static void check_values_size(const Vector3D& values, unsigned width, const std::string& context);
/// Fast-forward the file for one step, returning `false` if the file does
/// not contain one more step.
static bool forward(TextFile& file);

GROFormat::GROFormat(const std::string& path, File::Mode mode)
    : file_(TextFile::create(path, mode))
{
    while (!file_->eof()) {
        auto position = file_->tellg();
        if (!file_ || position == std::streampos(-1)) {
            throw format_error("IO error while reading '{}' as GRO", path);
        }
        if (forward(*file_)) {
            steps_positions_.push_back(position);
        }
    }
    file_->rewind();
}

size_t GROFormat::nsteps() {
    return steps_positions_.size();
}

void GROFormat::read_step(const size_t step, Frame& frame) {
    assert(step < steps_positions_.size());
    file_->seekg(steps_positions_[step]);
    read(frame);
}

void GROFormat::read(Frame& frame) {
    long count = 0;
    try {
        frame.set("name", (file_->readline())); // GRO comment line;
        count = std::stol(file_->readline());
    } catch (const std::exception& e) {
        throw format_error("can not read next step as GRO: {}", e.what());
    }
    if (count < 0) {
        throw format_error("the number of atoms can not be negative in GRO format");
    }
    size_t natoms = static_cast<size_t>(count);

    frame.reserve(natoms);
    frame.resize(0);

    for (const auto& line: file_->readlines(natoms)) {
        auto resid = std::stoul(line.substr(0,5));

        std::string resname = line.substr(5, 5);

        std::string atomname= line.substr(10,5);

        long atomid = std::stol(line.substr(15,5));

        // GRO files store atoms in NM, we need to convert to Angstroms
        auto x = std::stof(line.substr(20,8)) * 10;
        auto y = std::stof(line.substr(28,8)) * 10;
        auto z = std::stof(line.substr(36,8)) * 10;

        if (line.length() > 44) {
            auto vx = std::stof(line.substr(44,8)) * 10;
            auto vy = std::stof(line.substr(52,8)) * 10;
            auto vz = std::stof(line.substr(60,8)) * 10;

            frame.add_atom(Atom(trim(atomname)),
                Vector3D(x, y, z),
                Vector3D(vx, vy, vz)
            );
        } else {
            frame.add_atom(Atom(trim(atomname)),
                Vector3D(x, y, z)
            );
        }

        assert(frame.size() == static_cast<size_t>(atomid));

        auto cf_atomid = frame.size() - 1;
        try {
            if (residues_.find(resid) == residues_.end()) {
                Residue residue(trim(resname), resid);
                residue.add_atom(cf_atomid);

                residues_.insert({resid, residue});
            } else {
                // Just add this atom to the residue
                residues_.at(resid).add_atom(cf_atomid);
            }
        } catch (std::invalid_argument&) {
            // No residue information
        }

    }

    for (auto& residue: residues_) {
        frame.add_residue(residue.second);
    }
}

static std::string to_gro_index(uint64_t i) {
    auto id = i + 1;

    if (id >= 100000) {
        warning("Too many atoms for PDB format, removing atomic id");
        return "*****";
    } else {
        return std::to_string(i + 1);
    }
}

void GROFormat::write(const Frame& frame) {
    //fmt::print(*file_, "MODEL {:>4}\n", models_ + 1);
    fmt::print(*file_, "{: > 5}", frame.size());

    // Only use numbers bigger than the biggest residue id as "resSeq" for
    // atoms without associated residue.
    uint64_t max_resid = 0;
    for (const auto& residue: frame.topology().residues()) {
        auto resid = residue.id();
        if (resid && resid.value() > max_resid) {
            max_resid = resid.value();
        }
    }

    auto& positions = frame.positions();
    for (size_t i = 0; i < frame.size(); i++) {

        std::string resname;
        std::string resid;
        auto residue = frame.topology().residue_for_atom(i);
        if (residue) {
            resname = residue->name();
            if (resname.length() > 5) {
                warning(
                    "Residue '{}' has a name too long for GRO format, it will be truncated.",
                    resname
                );
                resname = resname.substr(0, 5);
            }

            if (residue->id()) {
                auto value = residue->id().value();
                if (value > 99999) {
                    warning("Too many residues for GRO format, removing residue id");
                    resid = "  -1";
                } else {
                    resid = std::to_string(residue->id().value());
                }
            } else {
                resid = "  -1";
            }
        } else {
            resname = "XXXXX";
            auto value = max_resid++;
            if (value < 99999) {
                resid = to_gro_index(value);
            } else {
                resid = "  -1";
            }
        }

        assert(resname.length() <= 5);
        auto pos = positions[i] / 10;
        check_values_size(pos, 8, "atomic position");

        if (frame.velocities()) {
            auto vel = (*frame.velocities())[i] / 10;
            fmt::print(
                *file_,
                "{: >5}{: <5}{: >5}{: >5}{:8.3f}{:8.3f}{:8.3f}{:8.4f}{:8.4f}{:8.4f}\n",
                resid, resname, frame[i].name(), to_gro_index(i), pos[0], pos[1], pos[2], vel[0], vel[1], vel[2]
            );
        } else {
            fmt::print(
                *file_,
                "{: >5}{: <5}{: >5}{: >5}{:8.3f}{:8.3f}{:8.3f}\n",
                resid, resname, frame[i].name(), to_gro_index(i), pos[0], pos[1], pos[2]
            );
        }
    }

    /*auto& cell = frame.cell();
    check_values_size(Vector3D(cell.a(), cell.b(), cell.c()), 9, "cell lengths");
    fmt::print(
        *file_,
        // Do not try to guess the space group and the z value, just use the
        // default one.
        "CRYST1{:9.3f}{:9.3f}{:9.3f}{:7.2f}{:7.2f}{:7.2f} P 1           1\n",
        cell.a(), cell.b(), cell.c(), cell.alpha(), cell.beta(), cell.gamma());*/

    steps_positions_.push_back(file_->tellg());
}

void check_values_size(const Vector3D& values, unsigned width, const std::string& context) {
    double max_pos = std::pow(10.0, width) - 1;
    double max_neg = -std::pow(10.0, width - 1) + 1;
    if (values[0] > max_pos || values[1] > max_pos || values[2] > max_pos ||
        values[0] < max_neg || values[1] < max_neg || values[2] < max_neg) {
        throw format_error(
            "value in {} is too big for representation in GRO format", context
        );
    }
}

bool forward(TextFile& file) {
    if (!file) {return false;}

    long long natoms = 0;
    try {
        auto line = file.readline();
        natoms = std::stoll(line);
    } catch (const FileError&) {
        // No more line left in the file
        return false;
    } catch (const std::invalid_argument&) {
        // We could not read an integer, so give up here
        return false;
    }

    if (natoms < 0) {
        throw format_error(
            "number of atoms can not be negative in '{}'", file.filename()
        );
    }

    try {
        file.readlines(static_cast<size_t>(natoms) + 1);
    } catch (const FileError&) {
        // We could not read the lines from the file
        throw format_error(
            "not enough lines in '{}' for GRO format", file.filename()
        );
    }
    return true;
}
