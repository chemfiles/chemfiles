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
    : mode_(mode), file_(TextFile::open(std::move(path), mode, compression))
{

    if (mode_ == File::WRITE) {
        root_ = document_.append_child("cml");
        root_.append_attribute("xmlns") = "http://www.xml-cml.org/schema";
        root_.append_attribute("xmlns:cml") = "http://www.xml-cml.org/dict/cml";
        root_.append_attribute("xmlns:units") = "http://www.xml-cml.org/units/units";
        root_.append_attribute("xmlns:convention") = "http://www.xml-cml.org/convention";
        root_.append_attribute("convention") = "convention:molecular";
        root_.append_attribute("xmlns:iupac") = "http://www.iupac.org";
        return;
    }

    if (mode_ == File::APPEND) { // may need to check the file a CML node in the future, now just append
        root_ = document_;
        return;
    }

    auto result = document_.load(*file_);
    if (!result) {
        throw format_error("[CML] Parsing error: '{}'", result.description());
    }

    root_ = document_.child("cml");
    if (root_) {
        auto molecules = root_.children("molecule");
        current_ = molecules.begin();
        if (current_ == molecules.end()) {
            throw format_error("[CML] cml node has no valid children");
        }
        return;
    }

    // Not technically standard to have multiple molecule nodes as root nodes,
    // but some tools do this and we should support it 
    if (document_.child("molecule")) {
        auto molecules = document_.children("molecule");
        current_ = molecules.begin();
        root_ = document_;
        return;
    }

    throw format_error("[CML] no supported starting node found");
}

CMLFormat::~CMLFormat() {
    if (num_added_ && mode_ == File::WRITE) {
        document_.save(*file_, "  ");
        return;
    }

    // Don't bother to check if anything is added, the document will be blank
    // regardless
    if (mode_ == File::APPEND) {
        document_.save(*file_, "  ", pugi::format_no_declaration | pugi::format_default);
    }
}

size_t CMLFormat::nsteps() {
    auto children = root_.children("molecule");
    return static_cast<size_t>(std::distance(children.begin(), children.end()));
}

template<typename T>
static void read_property_(T& container, pugi::xml_node& node, const std::string& title) {

    std::string data_type_str = "xsd:string";
    auto data_type = node.attribute("dataType");
    if (!data_type) {
        warning("[CML] {} has no data type, assuming string.", title);
    } else {
        data_type_str = data_type.as_string(); 
    }

    auto value = node.text();
    if (data_type_str == "xsd:string") {
        container.set(title, value.as_string());
    } else if (data_type_str == "xsd:boolean") {
        container.set(title, value.as_bool());
    } else if (data_type_str == "xsd:double" || data_type_str == "xsd:integer") {
        container.set(title, value.as_double());
    } else {
        warning("[CML] Unknown data type: {}, assuming string", data_type_str);
        container.set(title, value.as_string());
    }
}

template<typename T>
static void read_property_(T& container, pugi::xml_node& node) {
    auto title = node.attribute("title");
    if (!title) {
        warning("[CML] Skipping untitled property");
        return;
    }

    read_property_(container, node, title.as_string());
}

void CMLFormat::read(Frame& frame) {
    const auto& current = *current_;
    for (auto attribute : current.attributes()) {
        auto name = std::string(attribute.name());
        if (name == "id") { // do nothing
        } else if(name == "title") {
            frame.set("name", attribute.as_string());
        } else {
            warning("[CML] Unknown molecule attribute: " + name);
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
                    warning("[CML] Unknown crytal scalar: " + name);
                }
            }
        }
        frame.set_cell(UnitCell(a, b, c, alpha, beta, gamma));
    }

    // Frame level properties
    auto properties_node = current.child("propertyList");
    if (properties_node) {
        for (auto property : properties_node.children("property")) {
            auto title = property.attribute("title");
            if (!title) {
                warning("[CML] Skipping untitled property");
                continue;
            }

            auto title_str = title.as_string();
            auto scalar_node = property.child("scalar");
            if (!scalar_node) {
                warning("[CML] {} has no scalar associated with it!", title_str);
                continue;
            }

            read_property_(frame, scalar_node, title_str);
        }
    }

    // Read the atoms 
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
                warning("[CML] Unknown atom attribute: " + name);
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

        for (auto scalar : atom.children("scalar")) {
            read_property_(frame[frame.size() - 1], scalar);
        }
    }

    for (auto bond : current.child("bondArray").children("bond")) {
        auto atomref = bond.attribute("atomRefs2");
        auto order = bond.attribute("order");

        if (!atomref) {
            warning("[CML] Bad bond");
            continue;
        }

        auto ids = split(atomref.as_string(), ' ');
        if (ids.size() != 2) {
            warning("[CML] Bad bond size");
            continue;
        }

        auto id1 = ref_to_id.find(ids[0]);
        auto id2 = ref_to_id.find(ids[1]);
        if (id1 == ref_to_id.end() || id2 == ref_to_id.end()) {
            warning("[CML] Bad bond ref");
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
                    warning("[CML] Unknown bond order: '{}'", order_str[0]);
                    break;
                }
            }
        }

        frame.add_bond((*id1).second, (*id2).second, bo);
    }

    ++current_;
}

void CMLFormat::read_step(size_t step, Frame& frame) {
    current_ = root_.children("molecule").begin();
    std::advance(current_, step);
    read(frame);
}

static void write_property_(const Property& p, pugi::xml_node& node) {
    Vector3D v;
    switch (p.kind()) {
    case Property::DOUBLE:
        node.append_attribute("dataType") = "xsd:double";
        node.text() = p.as_double();
        break;
    case Property::BOOL:
        node.append_attribute("dataType") = "xsd:boolean";
        node.text() = p.as_bool();
        break;
    case Property::STRING:
        node.append_attribute("dataType") = "xsd:string";
        node.text() = p.as_string().c_str();
        break;
    case Property::VECTOR3D: // A bit of a hack...
        node.append_attribute("dataType") = "xsd:string";
        v = p.as_vector3d();
        node.text() = (std::to_string(v[0]) + " " +
                       std::to_string(v[1]) + " " +
                       std::to_string(v[2])).c_str();
        break;
    }
}

void CMLFormat::write(const Frame& frame) {
    auto mol = root_.append_child("molecule");

    if (mode_ == File::WRITE) {
        mol.append_attribute("id") = ("m" + std::to_string(++num_added_)).c_str();
    }

    auto& unit_cell = frame.cell();
    if (unit_cell.shape() != UnitCell::INFINITE) {
        auto crystal_node = mol.append_child("crystal");
        auto scalar = crystal_node.append_child("scalar");
        scalar.append_attribute("units") = "units:angstrom";
        scalar.append_attribute("title") = "a";
        scalar.text() = unit_cell.a();

        scalar = crystal_node.append_child("scalar");
        scalar.append_attribute("units") = "units:angstrom";
        scalar.append_attribute("title") = "b";
        scalar.text() = unit_cell.b();

        scalar = crystal_node.append_child("scalar");
        scalar.append_attribute("units") = "units:angstrom";
        scalar.append_attribute("title") = "c";
        scalar.text() = unit_cell.c();

        scalar = crystal_node.append_child("scalar");
        scalar.append_attribute("units") = "units:degree";
        scalar.append_attribute("title") = "alpha";
        scalar.text() = unit_cell.alpha();

        scalar = crystal_node.append_child("scalar");
        scalar.append_attribute("units") = "units:degree";
        scalar.append_attribute("title") = "beta";
        scalar.text() = unit_cell.beta();

        scalar = crystal_node.append_child("scalar");
        scalar.append_attribute("units") = "units:degree";
        scalar.append_attribute("title") = "gamma";
        scalar.text() = unit_cell.gamma();
    }

    auto& properties = frame.properties();
    if (properties.size()) {
        auto prop_list = mol.append_child("propertyList");
        for (auto& prop : properties) {
            auto prop_node = prop_list.append_child("property");
            prop_node.append_attribute("title") = prop.first.c_str();

            auto scalar_node = prop_node.append_child("scalar");
            write_property_(prop.second, scalar_node);
        }
    }

    auto atom_array = mol.append_child("atomArray");
    for (size_t i = 0; i < frame.size(); ++i) {
        auto& atom = frame[i];
        auto atom_node = atom_array.append_child("atom");
        atom_node.append_attribute("id") = ("a" + std::to_string(i + 1)).c_str();
        atom_node.append_attribute("elementType") = atom.type().c_str();

        auto& position = frame.positions()[i];
        atom_node.append_attribute("x3") = position[0];
        atom_node.append_attribute("y3") = position[1];
        atom_node.append_attribute("z3") = position[2];

        auto& atom_properties = atom.properties();
        if (!atom_properties.size()) {
            continue;
        }
        for (auto& prop : atom_properties) {
            auto scalar_node = atom_node.append_child("scalar");
            scalar_node.append_attribute("title") = prop.first.c_str();
            write_property_(prop.second, scalar_node);
        }
    }

    auto& bonds = frame.topology().bonds();
    if (bonds.size() == 0) {
        return;
    }

    auto bond_array = mol.append_child("bondArray");
    auto& bond_orders = frame.topology().bond_orders();

    for (size_t i = 0; i < bonds.size(); ++i) {
        std::string refs2 = "a" + std::to_string(bonds[i][0] + 1) +
                           " a" + std::to_string(bonds[i][1] + 1);
        auto bond_node = bond_array.append_child("bond");
        bond_node.append_attribute("atomRefs2") = refs2.c_str();

        switch (bond_orders[i]) {
            case Bond::SINGLE:
                bond_node.append_attribute("order") = "1";
                break;
            case Bond::DOUBLE:
                bond_node.append_attribute("order") = "2";
                break;
            case Bond::TRIPLE:
                bond_node.append_attribute("order") = "3";
                break;
            case Bond::AROMATIC:
                bond_node.append_attribute("order") = "a";
                break;
            default:
                break;
        }
    }
}
