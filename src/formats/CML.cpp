// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <sstream>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "pugixml.cpp"

#include "chemfiles/formats/CML.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<CMLFormat>() {
    return FormatInfo("CML").with_extension(".cml").description(
        "Chemical Markup Language"
    );
}

CMLFormat::CMLFormat(std::string path, File::Mode mode, File::Compression compression)
    : file_(TextFile::open(std::move(path), mode, compression))
{
    auto result = document_.load(*file_);
    if (!result) {
        throw format_error("Error parsing CML file because '{}'", result.description());
    }

    root_ = document_.child("cml");
    if (root_) {
        current_ = root_.children("molecule").begin();
        is_multiple_frames_ = true;
        return;
    }

    root_ = document_.child("molecule");
    if (root_) {
        is_multiple_frames_ = false;
        return;
    }

    throw format_error("unsupported starting node");
}

size_t CMLFormat::nsteps() {
    if (!is_multiple_frames_) {
        return 1;
    }

    auto children = root_.children("molecule");
    return static_cast<size_t>(std::distance(children.begin(), children.end()));
}

void CMLFormat::read(Frame& frame) {
    const auto& current = is_multiple_frames_? *current_ : root_;
    for (auto attribute : current.attributes()) {
        auto name = std::string(attribute.name());
        if (name == "id") { // do nothing
        } else if(name == "title") {
            frame.set("name", attribute.as_string());
        } else {
            warning("Unknown molecule attribute: " + name);
        }
    }

    auto crystal_node = current.child("crystal");
    if (crystal_node) {
        double a, b, c, alpha, beta, gamma;
        a = b = c = alpha = beta = gamma = 0.0;
        for (auto cell_param : crystal_node.children("scalar")) {
            auto title = cell_param.attribute("title");
            if (title) {
                auto name = std::string(title.value());
                if (name == "a") {
                    a = cell_param.text().as_double();
                } else if (name == "b") {
                    b = cell_param.text().as_double();
                } else if (name == "c") {
                    c = cell_param.text().as_double();
                } else if (name == "alpha") {
                    alpha = cell_param.text().as_double();
                } else if (name == "beta") {
                    beta = cell_param.text().as_double();
                } else if (name == "gamma") {
                    gamma = cell_param.text().as_double();
                } else {
                    warning("Unknown crytal scalar: " + name);
                }
            }
        }
        frame.set_cell(UnitCell(a, b, c, alpha, beta, gamma));
    }

    std::unordered_map<std::string, size_t> ref_to_id;
    for (auto atom : current.child("atomArray").children("atom")) {
        double x2, y2, x3, y3, z3, xf, yf, zf;
        x2 = y2 = x3 = y3 = z3 = xf = yf = zf = 0.0;
        std::string id, element;
        for (auto attribute : atom.attributes()) {
            auto name = std::string(attribute.name());
            if (name == "id") {
                id = attribute.as_string();
            } else if (name == "elementType") {
                element = attribute.as_string();
            } else if (name == "x2") {
                x2 = attribute.as_double();
            } else if (name == "y2") {
                y2 = attribute.as_double();
            } else if (name == "x3") {
                x3 = attribute.as_double();
            } else if (name == "y3") {
                y3 = attribute.as_double();
            } else if (name == "z3") {
                z3 = attribute.as_double();
            } else if (name == "xFract") {
                xf = attribute.as_double();
            } else if (name == "yFract") {
                yf = attribute.as_double();
            } else if (name == "zFract") {
                zf = attribute.as_double();
            } else {
                warning("Unknown atom attribute: " + name);
            }
        }

        if (crystal_node && (xf != 0.0 || yf != 0.0 || zf != 0.0)) {
            auto cart = frame.cell().matrix() * Vector3D(xf, yf, zf);
            frame.add_atom(Atom(id, element), cart);
        } else if (x3 == 0.0 && y3 == 0.0 && z3 == 0.0) {
            frame.add_atom(Atom(id, element), Vector3D(x2, y2, 0));
        } else {
            frame.add_atom(Atom(id, element), Vector3D(x3, y3, z3));
        }

        ref_to_id[id] = frame.size() - 1;
    }

    for (auto bond : current.child("bondArray").children("bond")) {
        auto atomref = bond.attribute("atomRefs2");
        auto order = bond.attribute("order");

        if (!atomref) {
            warning("Bad bond");
            continue;
        }

        auto ids = split(atomref.as_string(), ' ');
        if (ids.size() != 2) {
            warning("Bad bond size");
            continue;
        }

        auto id1 = ref_to_id.find(ids[0]);
        auto id2 = ref_to_id.find(ids[1]);
        if (id1 == ref_to_id.end() || id2 == ref_to_id.end()) {
            warning("Bad bond ref");
            continue;
        }

        auto bo = Bond::UNKNOWN;
        if (order) {
            std::string order_str = order.as_string();
            if (!order_str.empty()) {
                switch (order_str[0]) {
                case '1':
                case 'S':
                case 's':
                    bo = Bond::SINGLE;
                    break;
                case '2':
                case 'D':
                case 'd':
                    bo = Bond::DOUBLE;
                    break;
                case '3':
                case 'T':
                case 't':
                    bo = Bond::TRIPLE;
                    break;
                case 'A':
                case 'a':
                    bo = Bond::AROMATIC;
                    break;
                default:
                    warning("Unknown bond order: '{}'", order_str[0]);
                    break;
                }
            }
        }

        frame.add_bond((*id1).second, (*id2).second, bo);
    }

    if (is_multiple_frames_) {
        ++current_;
    }
}

void CMLFormat::read_step(size_t step, Frame& frame) {
    if (!is_multiple_frames_) {
        read(frame);
        return;
    }

    current_ = root_.children("molecule").begin();
    std::advance(current_, step);
    read(frame);
}

void CMLFormat::write(const Frame& frame) {
    //TODO
}
