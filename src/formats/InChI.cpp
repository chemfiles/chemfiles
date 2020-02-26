// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <sstream>
#include <mutex>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <inchi_api.h>

#include "chemfiles/formats/InChI.hpp"

#include "chemfiles/error_fmt.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/periodic_table.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

template <>
FormatInfo chemfiles::format_information<InChIFormat>()
{
    return FormatInfo("InChI").with_extension(".inchi").description(
        "InChI format");
}

static bool double_to_int(double value, int& int_part) {
    double integer_section;
    auto remainder = std::modf(value, &integer_section);
    if (remainder == 0.0) {
        int_part = static_cast<int>(std::floor(integer_section));
        return true;
    }
    return false;
}

/// RAII wrapper around IXA_MOL_HANDLE
class IXAMolWrapper {
public:
    IXAMolWrapper() {
        status_ = IXA_STATUS_Create();
        handle_ = IXA_MOL_Create(status_);
        check_for_errors();
    }

    ~IXAMolWrapper() {
        IXA_MOL_Destroy(status_, handle_);
        IXA_STATUS_Destroy(status_);
    }

    void check_for_errors() {
        std::string errors;
        int count = IXA_STATUS_GetCount(status_);
        for (int index = 0; index < count; index++)
        {
            switch (IXA_STATUS_GetSeverity(status_, index))
            {
            case IXA_STATUS_ERROR:
                errors += IXA_STATUS_GetMessage(status_, index);
                errors += "\n";
                break;
            case IXA_STATUS_WARNING:
                warning("inchi IXA: ", "'{}'", IXA_STATUS_GetMessage(status_, index));
                break;
            }
        }

        if (!errors.empty()) {
            throw format_error("problem in inchi IXA: '{}'", errors);
        }

        IXA_STATUS_Clear(status_);
    }

    void read_string(const std::string& s) {
        IXA_MOL_ReadInChI(status_, handle_, s.c_str());
        check_for_errors();
    }

    std::pair<std::string, std::string> get_strings() {
        auto builder = IXA_INCHIBUILDER_Create(status_);
        IXA_INCHIBUILDER_SetMolecule(status_, builder, handle_);

        auto inchi_result = IXA_INCHIBUILDER_GetInChIEx(status_, builder);
        auto auxinfo_result = IXA_INCHIBUILDER_GetAuxInfo(status_, builder);
        check_for_errors();

        std::string inchi(inchi_result);
        std::string auxinfo(auxinfo_result);
        IXA_INCHIBUILDER_Destroy(status_, builder);
        return {inchi, auxinfo};
    }

    int atom_count() {
        auto num = IXA_MOL_GetNumAtoms(status_, handle_);
        check_for_errors();
        return num;
    }

    int bond_count() {
        auto num = IXA_MOL_GetNumBonds(status_, handle_);
        check_for_errors();
        return num;
    }

    int stereo_count() {
        auto num = IXA_MOL_GetNumStereos(status_, handle_);
        check_for_errors();
        return num;
    }

    IXA_ATOMID atom(int atom_index) {
        auto atom = IXA_MOL_GetAtomId(status_, handle_, atom_index);
        check_for_errors();
        return atom;
    }

    IXA_BONDID bond(int bond_index) {
        auto bond = IXA_MOL_GetBondId(status_, handle_, bond_index);
        check_for_errors();
        return bond;
    }

    IXA_BONDID bond(IXA_ATOMID& atom1, IXA_ATOMID& atom2) {
        auto bond = IXA_MOL_GetCommonBond(status_, handle_, atom1, atom2);
        check_for_errors();
        return bond;
    }

    IXA_STEREOID stereo(int stereo_index) {
        auto stereo = IXA_MOL_GetStereoId(status_, handle_, stereo_index);
        check_for_errors();
        return stereo;
    }

    size_t atom_index(IXA_ATOMID& atom) {
        auto index = IXA_MOL_GetAtomIndex(status_, handle_, atom);
        check_for_errors();
        return static_cast<size_t>(index);
    }

    Atom get_atom(IXA_ATOMID& atom_id) {
        auto atom = Atom(IXA_MOL_GetAtomElement(status_, handle_, atom_id));

        auto mass = IXA_MOL_GetAtomMass(status_, handle_, atom_id);
        if (mass != IXA_ATOM_NATURAL_MASS) {
            atom.set_mass(mass);
        }

        auto charge = IXA_MOL_GetAtomCharge(status_, handle_, atom_id);
        if (charge != 0) {
            atom.set_charge(charge);
        }

        auto radical = IXA_MOL_GetAtomRadical(status_, handle_, atom_id);
        if (radical != IXA_ATOM_RADICAL_NONE) {
            atom.set("radial", radical);
        }

        auto mass1 = IXA_MOL_GetAtomHydrogens(status_, handle_, atom_id, 1);
        auto mass2 = IXA_MOL_GetAtomHydrogens(status_, handle_, atom_id, 2);
        auto mass3 = IXA_MOL_GetAtomHydrogens(status_, handle_, atom_id, 3);

        if (mass1 != 0) {
            atom.set("hydrogen_count", mass1);
        }

        if (mass2 != 0) {
            atom.set("deuterium_count", mass2);
        }

        if (mass3 != 0) {
            atom.set("tritium_count", mass3);
        }

        return atom;
    }

    Vector3D atom_position(IXA_ATOMID& atom) {
        double x = IXA_MOL_GetAtomX(status_, handle_, atom);
        double y = IXA_MOL_GetAtomY(status_, handle_, atom);
        double z = IXA_MOL_GetAtomZ(status_, handle_, atom);
        check_for_errors();
        return {x, y, z};
    }

    // TODO support this for double bonds?
    IXA_BOND_WEDGE bond_wedge(IXA_ATOMID& central, IXA_ATOMID& other) {
        auto wedge = IXA_MOL_GetBondWedge(status_, handle_, bond(central, other), central);
        check_for_errors();
        return wedge;
    }

    IXA_BOND_TYPE bond_type(IXA_BONDID& bond) {
        auto bond_type = IXA_MOL_GetBondType(status_, handle_, bond);
        check_for_errors();
        return bond_type;
    }

    Bond bond(IXA_BONDID& bond) {
        auto atom1 = IXA_MOL_GetBondAtom1(status_, handle_, bond);
        auto atom2 = IXA_MOL_GetBondAtom2(status_, handle_, bond);

        auto index1 = IXA_MOL_GetAtomIndex(status_, handle_, atom1);
        auto index2 = IXA_MOL_GetAtomIndex(status_, handle_, atom2);
        check_for_errors();
        return {static_cast<size_t>(index1), static_cast<size_t>(index2)};
    }

    IXA_STEREO_TOPOLOGY stereo_topology(IXA_STEREOID& stereo) {
        auto topo = IXA_MOL_GetStereoTopology(status_, handle_, stereo);
        check_for_errors();
        return topo;
    }

    IXA_STEREO_PARITY stereo_parity(IXA_STEREOID& stereo) {
        auto parity = IXA_MOL_GetStereoParity(status_, handle_, stereo);
        check_for_errors();
        return parity;
    }

    IXA_ATOMID stereo_central_atom(IXA_STEREOID& stereo) {
        auto atom = IXA_MOL_GetStereoCentralAtom(status_, handle_, stereo);
        check_for_errors();
        return atom;
    }

    IXA_BONDID stereo_central_bond(IXA_STEREOID& stereo) {
        auto bond = IXA_MOL_GetStereoCentralBond(status_, handle_, stereo);
        check_for_errors();
        return bond;
    }

    int stereo_vertex_count(IXA_STEREOID& stereo) {
        auto num = IXA_MOL_GetStereoNumVertices(status_, handle_, stereo);
        check_for_errors();
        return num;
    }

    IXA_ATOMID stereo_vertex(IXA_STEREOID& stereo, int vert) {
        auto atom = IXA_MOL_GetStereoVertex(status_, handle_, stereo, vert);
        check_for_errors();
        return atom;
    }

    void create_atom(const Atom& atom, const Vector3D& pos) {
        auto new_atom = IXA_MOL_CreateAtom(status_, handle_);
        auto atomic_number = atom.atomic_number();
        if (atomic_number) {
            IXA_MOL_SetAtomAtomicNumber(status_, handle_, new_atom, static_cast<int>(*atomic_number));
        }

        IXA_MOL_SetAtomX(status_, handle_, new_atom, pos[0]);
        IXA_MOL_SetAtomY(status_, handle_, new_atom, pos[1]);
        IXA_MOL_SetAtomZ(status_, handle_, new_atom, pos[2]);

        int int_part;
        if (double_to_int(atom.mass(), int_part)) {
            IXA_MOL_SetAtomMass(status_, handle_, new_atom, int_part);
        }

        if (double_to_int(atom.charge(), int_part)) {
            IXA_MOL_SetAtomCharge(status_, handle_, new_atom, int_part);
        }

        // TODO: Add explicit hydrogen support
        IXA_MOL_SetAtomHydrogens(status_, handle_, new_atom, 0, -1);

        check_for_errors();
    }

    void create_bond(const Bond& bond, Bond::BondOrder order) {
        auto atom1 = atom(static_cast<int>(bond[0]));
        auto atom2 = atom(static_cast<int>(bond[1]));

        auto new_bond = IXA_MOL_CreateBond(status_, handle_, atom1, atom2);

        switch(order) {
        case Bond::SINGLE:
        case Bond::AMIDE:
        case Bond::DATIVE_L:
        case Bond::DATIVE_R:
        case Bond::UP:
        case Bond::DOWN:
            IXA_MOL_SetBondType(status_, handle_, new_bond, IXA_BOND_TYPE_SINGLE);
            break;
        case Bond::DOUBLE:
        case Bond::EVEN_RECTANGLE:
        case Bond::ODD_RECTANGLE:
            IXA_MOL_SetBondType(status_, handle_, new_bond, IXA_BOND_TYPE_DOUBLE);
            break;
        case Bond::TRIPLE:
            IXA_MOL_SetBondType(status_, handle_, new_bond, IXA_BOND_TYPE_TRIPLE);
            break;
        case Bond::AROMATIC:
            IXA_MOL_SetBondType(status_, handle_, new_bond, IXA_BOND_TYPE_AROMATIC);
            break;
        default:
            // Don't set
            break;
        }

        check_for_errors();
    }

    void set_stereo_parity(IXA_STEREOID stereo, string_view parity) {
        if (parity == "odd") {
            IXA_MOL_SetStereoParity(status_, handle_, stereo, IXA_STEREO_PARITY_ODD);
        } else if (parity == "even") {
            IXA_MOL_SetStereoParity(status_, handle_, stereo, IXA_STEREO_PARITY_EVEN);
        } else if (parity == "none") {
            IXA_MOL_SetStereoParity(status_, handle_, stereo, IXA_STEREO_PARITY_NONE);
        } else {
            IXA_MOL_SetStereoParity(status_, handle_, stereo, IXA_STEREO_PARITY_UNKNOWN);
        }
        check_for_errors();
    }

    IXA_STEREOID create_stereo_tetrahedron(size_t center, const std::vector<size_t>& a) {

        assert(a.size() >= 3);

        auto c0 = atom(static_cast<int>(center));
        auto v1 = atom(static_cast<int>(a[0]));
        auto v2 = atom(static_cast<int>(a[1]));
        auto v3 = atom(static_cast<int>(a[2]));
        auto v4 = a.size() >= 4 ? atom(static_cast<int>(a[3])) : IXA_ATOMID_IMPLICIT_H;

        // We have an implicit hydrogen or the fourth atom is a hydrogen,
        // so we need to swap it with the first vertex
        // I'm not sure why the IXA interface does not do this for us.
        // A similar solution can be found in RDkit 
        if (a.size() == 3 || IXA_MOL_GetAtomAtomicNumber(status_, handle_, v4) == 1) {
            std::swap(v1, v4);
        }

        auto stereo = IXA_MOL_CreateStereoTetrahedron(status_, handle_, c0, v1, v2, v3, v4);

        check_for_errors();

        return stereo;
    }

    void set_double_bond_configuration(size_t index) {
        auto b = bond(index);
        IXA_MOL_SetDblBondConfig(status_, handle_, b, IXA_DBLBOND_CONFIG_EITHER);
    }

    IXA_STEREOID create_stereo_rectangle(size_t center, size_t a, Bond cbond, size_t b) {

        auto c0 = bond(static_cast<int>(center));

        auto a1 = atom(static_cast<int>(a));
        auto a2 = atom(static_cast<int>(cbond[0]));

        auto b1 = atom(static_cast<int>(cbond[1]));
        auto b2 = atom(static_cast<int>(b));

        auto stereo = IXA_MOL_CreateStereoRectangle(status_, handle_, c0, a1, a2, b1, b2);

        check_for_errors();

        return stereo;
    }

private:
    IXA_STATUS_HANDLE status_;

    IXA_MOL_HANDLE handle_;
};

void InChIFormat::read_next(Frame &frame) {
    auto inchi = file_.readline();
    IXAMolWrapper mol;
    mol.read_string(inchi.to_string());

    for (int i = 0; i < mol.atom_count(); ++i) {
        auto atom_id = mol.atom(i);
        frame.add_atom(mol.get_atom(atom_id), mol.atom_position(atom_id));
    }

    for (int i = 0; i < mol.bond_count(); ++i) {
        auto bond_id = mol.bond(i);
        auto bond = mol.bond(bond_id);
        auto bond_type = mol.bond_type(bond_id);

        Bond::BondOrder bo = Bond::SINGLE;
        switch(bond_type) {
        case IXA_BOND_TYPE_SINGLE:
            bo = Bond::SINGLE;
            break;
        case IXA_BOND_TYPE_DOUBLE:
            bo = Bond::DOUBLE;
            break;
        case IXA_BOND_TYPE_TRIPLE:
            bo = Bond::TRIPLE;
            break;
        case IXA_BOND_TYPE_AROMATIC:
            // This shouldn't happen during a read as InChI kekulizes the molecule
            warning("InChI Reader", "unexpected aromatic bond, assuming single bond");
            break;
        }
        frame.add_bond(bond[0], bond[1], bo);
    }

    for (int i = 0; i < mol.stereo_count(); ++i) {
        auto stereo = mol.stereo(i);
        auto stereo_topology = mol.stereo_topology(stereo);
        auto stereo_parity = mol.stereo_parity(stereo);

        auto center_type = std::string("tetrahedron_");

        switch (stereo_topology) {
        case IXA_STEREO_TOPOLOGY_ANTIRECTANGLE:
            center_type = "antirectangle_";
        case IXA_STEREO_TOPOLOGY_TETRAHEDRON: {
            auto central_atom = mol.stereo_central_atom(stereo);
            auto central_idx = mol.atom_index(central_atom);

            if (stereo_parity == IXA_STEREO_PARITY_EVEN) {
                frame[central_idx].set("chirality", center_type + "even");
            } else if (stereo_parity == IXA_STEREO_PARITY_ODD) {
                frame[central_idx].set("chirality", center_type + "odd");
            } else if (stereo_parity == IXA_STEREO_PARITY_NONE) {
                frame[central_idx].set("chirality", center_type + "none");
            } else {
                frame[central_idx].set("chirality", center_type + "unknown");
            }
        }
        break;
        case IXA_STEREO_TOPOLOGY_RECTANGLE: {

            auto _bond = mol.stereo_central_bond(stereo);
            auto bond = mol.bond(_bond);

            //TODO Maybe covert to Up/Down
            if (stereo_parity == IXA_STEREO_PARITY_EVEN) {
                frame.remove_bond(bond[0], bond[1]); // Ugh
                frame.add_bond(bond[0], bond[1], Bond::EVEN_RECTANGLE);
            } else if (stereo_parity == IXA_STEREO_PARITY_ODD) {
                frame.remove_bond(bond[0], bond[1]);
                frame.add_bond(bond[0], bond[1], Bond::ODD_RECTANGLE);
            } else if (stereo_parity == IXA_STEREO_PARITY_UNKNOWN) {
                frame.remove_bond(bond[0], bond[1]);
                frame.add_bond(bond[0], bond[1], Bond::DOUBLE);
            }
        }
        break;
        default:
            break;
        }
    }
}

static bool is_zero_dimensions(const Frame &frame) {
    
    constexpr auto eps = 0.000001;

    auto size = frame.size();
    if (size == 0 || size == 1) {
        return true; // by definition!
    }

    auto& positions = frame.positions();
    for (auto i = 1UL; i < positions.size(); ++i) {
        if (frame.distance(0, i) > eps) {
            return false;
        }
    }

    return true;
}

static std::vector<size_t> stereo_verticies(const Frame& frame,
                                            const std::vector<std::vector<size_t>>& adj_list,
                                            size_t atom1, size_t atom2) {

    std::vector<size_t> result;
    size_t num_single_bonds = 0;
    std::copy_if(adj_list[atom1].begin(), adj_list[atom1].end(), std::back_inserter(result),
        [atom1, atom2, &frame, &num_single_bonds](size_t neighbor) {
            switch (frame.topology().bond_order(atom1, neighbor)) {
            case Bond::SINGLE:
            case Bond::UP:
            case Bond::DOWN:
                ++num_single_bonds;
                break;
            }
            return neighbor != atom2;
        }
    );

    return result;
}

void InChIFormat::write_next(const Frame &frame) {

    IXAMolWrapper writer;

    const auto& coords = frame.positions();

    for (size_t i = 0; i < frame.size(); i++) {
        writer.create_atom(frame[i], coords[i]);
    }

    auto bond_orders = frame.topology().bond_orders();
    auto bonds = frame.topology().bonds();
    for (size_t i = 0; i < bonds.size(); ++i) {
        writer.create_bond(bonds[i], bond_orders[i]);
    }

    if (is_zero_dimensions(frame)) {
        auto adj_list = std::vector<std::vector<size_t>>(frame.size());
        for (auto& bond : frame.topology().bonds()) {
            adj_list[bond[0]].push_back(bond[1]);
            adj_list[bond[1]].push_back(bond[0]);
        }

        for (size_t i = 0; i < frame.size(); ++i) {
            auto& atom = frame[i];

            auto chirality = atom.get<Property::STRING>("chirality").value_or("");
            auto chirality_sv = string_view(chirality);

            // TODO: Add antirectangle support
            if (chirality_sv.starts_with("tetrahedron")) {
                if (adj_list[i].size() <= 2) {
                    warning(
                        "inchi writer",
                        "tetrahedral chirality property set for atom with fewer than 3 bonds"
                    );
                    continue;
                }

                auto stereo = writer.create_stereo_tetrahedron(i, adj_list[i]);
                writer.set_stereo_parity(stereo, chirality_sv.substr(12));

                continue;
            }
        }

        auto& bonds = frame.topology().bonds();
        auto& bond_orders = frame.topology().bond_orders();
        for (size_t i = 0; i < bonds.size(); ++i) {
            if (bond_orders[i] == Bond::DOUBLE) {
                writer.set_double_bond_configuration(i);
            }

            if (bond_orders[i] != Bond::EVEN_RECTANGLE && bond_orders[i] != Bond::ODD_RECTANGLE) {
                continue;
            }

            auto atom1 = bonds[i][0];
            auto atom2 = bonds[i][1];

            auto stereo_atom1 = stereo_verticies(frame, adj_list, atom1, atom2);
            auto stereo_atom2 = stereo_verticies(frame, adj_list, atom2, atom1);

            if (stereo_atom1.size() == 0 || stereo_atom2.size() == 0) {
                warning("inchi writer",
                        "attempt to set chirality on a double bond where an atom is bound to two implicit hydrogens"
                );
                continue;
            }

            auto stereo = writer.create_stereo_rectangle(i, stereo_atom1[0], bonds[i], stereo_atom2[0]);
            writer.set_stereo_parity(stereo, bond_orders[i] == Bond::EVEN_RECTANGLE? "even" : "odd");
        }
    }

    auto result = writer.get_strings();

    file_.print("{}\n", result.first);
    file_.print("{}\n", result.second);
}

optional<uint64_t> InChIFormat::forward() {

    while (!file_.eof()) {
        auto pos = file_.tellpos();
        auto line = file_.readline();
        auto npos = line.find("InChI=");
        if (npos != std::string::npos) {
            return pos;
        }
    }

    return nullopt;
}
