// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"

#include <algorithm>

using namespace chemfiles;

static void check_traj(const char* path) {
    auto file = Trajectory(path);
    auto frame = file.read();

    auto file_ref = Trajectory("data/tpr/reference.pdb");
    auto frame_ref = file_ref.read();

    CHECK(frame.size() == 293);
    auto cell = frame.cell();
    CHECK(cell.shape() == UnitCell::TRICLINIC);
    CHECK(approx_eq(cell.lengths(), {22.500, 33.549, 44.700}, 1e-3));
    CHECK(approx_eq(cell.angles(), {96.66, 80.20, 109.45}, 1e-2));

    auto positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(29.0200, 21.7100, 11.3700), 1e-4));
    CHECK(approx_eq(positions[10], Vector3D(25.8400, 21.5300, 35.2600), 1e-4));
    CHECK(approx_eq(positions[200], Vector3D(9.9500, -0.7900, 30.9400), 1e-4));
    CHECK(approx_eq(positions[292], Vector3D(17.4500, -5.4200, 17.7700), 1e-4));

    auto velocities = *frame.velocities();
    CHECK(approx_eq(velocities[0], Vector3D(0.0, 0.0, 0.0), 1e-4));
    CHECK(approx_eq(velocities[10], Vector3D(0.0, 0.0, 0.0), 1e-4));
    CHECK(approx_eq(velocities[200], Vector3D(0.0, 0.0, 0.0), 1e-4));
    CHECK(approx_eq(velocities[292], Vector3D(0.0, 0.0, 0.0), 1e-4));

    CHECK(frame.get("name").value().as_string() == "MySystemName");

    CHECK(frame.size() == frame_ref.size());
    auto positions_ref = frame_ref.positions();
    for (size_t i = 0; i < frame.size(); ++i) {
        CHECK(approx_eq(positions[i], positions_ref[i], 1e-4));
        auto& atom = frame[i];
        auto& atom_ref = frame_ref[i];
        CHECK(atom.name() == atom_ref.name());
        CHECK(atom.type() == atom_ref.type());
    }

    const std::vector<Residue>& residues = frame.topology().residues();
    const std::vector<Residue>& residues_ref = frame_ref.topology().residues();
    CHECK(residues.size() == residues_ref.size());
    for (size_t i = 0; i < residues.size(); ++i) {
        const auto& res = residues[i];
        const auto& res_ref = residues_ref[i];
        CHECK(res.name() == res_ref.name());
        // Do not compare residue ids because PDB is numbered sequentially
        // and TPR uses internal numbering with possible duplicates.
        // For this specific test file, the id is either `id == 1`
        // for new versions or `1 >= id <= 4` for old versions.
        CHECK((*res.id() >= 1 && *res.id() <= 4));
        CHECK(res.size() == res_ref.size());
        CHECK(std::equal(res.begin(), res.end(), res_ref.begin()));
        CHECK(res.properties().size() == 1);
        CHECK(*res.get("insertion_code") == " ");
    }

    // First Li
    CHECK(frame[0].name() == "Li");
    CHECK(frame[0].type() == "Li");
    auto res = *frame.topology().residue_for_atom(0);
    CHECK(res.name() == "LI");
    CHECK(*res.id() == 1);
    CHECK(approx_eq(frame[0].mass(), 6.9410, 1e-4));
    CHECK(approx_eq(frame[0].charge(), 0.8000, 1e-4));
    CHECK(*frame[0].get("ff_type") == "LI");

    // First protein, first residue, first N
    CHECK(frame[2].name() == "N");
    CHECK(frame[2].type() == "N");
    res = *frame.topology().residue_for_atom(2);
    CHECK(res.name() == "RSK");
    CHECK(*res.id() == 2); // from ITP, differs from PDB
    CHECK(approx_eq(frame[2].mass(), 14.0027, 1e-4));
    CHECK(approx_eq(frame[2].charge(), -0.3000, 1e-4));
    CHECK(*frame[2].get("ff_type") == "opls_287");

    // Third protein, last residue, first O
    CHECK(frame[63].name() == "O1");
    CHECK(frame[63].type() == "O");
    res = *frame.topology().residue_for_atom(63);
    CHECK(res.name() == "RSR");
    CHECK(*res.id() == 4); // from ITP, differs from PDB
    CHECK(approx_eq(frame[63].mass(), 15.9994, 1e-4));
    CHECK(approx_eq(frame[63].charge(), -0.8000, 1e-4));
    CHECK(*frame[63].get("ff_type") == "opls_272");

    // Fourth Li
    CHECK(frame[192].name() == "Li");
    CHECK(frame[192].type() == "Li");
    res = *frame.topology().residue_for_atom(192);
    CHECK(res.name() == "LI");
    CHECK(*res.id() == 1);
    CHECK(approx_eq(frame[192].mass(), 6.9410, 1e-4));
    CHECK(approx_eq(frame[192].charge(), 0.8000, 1e-4));
    CHECK(*frame[192].get("ff_type") == "LI");

    // First THF, second C
    CHECK(frame[196].name() == "C");
    CHECK(frame[196].type() == "C");
    res = *frame.topology().residue_for_atom(196);
    CHECK(res.name() == "THF");
    CHECK(*res.id() == 1);
    CHECK(approx_eq(frame[196].mass(), 12.0110, 1e-4));
    CHECK(approx_eq(frame[196].charge(), -0.1200, 1e-4));
    CHECK(*frame[196].get("ff_type") == "opls_136");

    // First THF, O
    CHECK(frame[205].name() == "O");
    CHECK(frame[205].type() == "O");
    res = *frame.topology().residue_for_atom(205);
    CHECK(res.name() == "THF");
    CHECK(*res.id() == 1);
    CHECK(approx_eq(frame[205].mass(), 15.9994, 1e-4));
    CHECK(approx_eq(frame[205].charge(), -0.4000, 1e-4));
    CHECK(*frame[205].get("ff_type") == "opls_180");

    const std::vector<Bond>& bonds = frame.topology().bonds();
    const std::vector<Bond>& bonds_ref = frame_ref.topology().bonds();
    if (bonds.size() > bonds_ref.size()) {
        // new TPR format
        // one intermolecular bond which is not dumped in the reference
        CHECK(bonds.size() == bonds_ref.size() + 1);
        Bond intermolecular_bond = {0, 99};
        auto it = std::lower_bound(bonds.begin(), bonds.end(), intermolecular_bond);
        CHECK(it != bonds.end());
        CHECK(*it == intermolecular_bond);
    } else {
        // old TPR format
        // no intermolecular bonds
        CHECK(bonds.size() == bonds_ref.size());
    }
    for (const auto& bond : bonds_ref) {
        auto it = std::lower_bound(bonds.begin(), bonds.end(), bond);
        CHECK(it != bonds.end());
        CHECK(*it == bond);
    }
}

TEST_CASE("Read files in TPR format") {
    SECTION("Read TPR Version 2023") {
        check_traj("data/tpr/gmx_v2023_s.tpr");
        check_traj("data/tpr/gmx_v2023_d.tpr");
    }

    SECTION("Read TPR Version 2022") {
        check_traj("data/tpr/gmx_v2022_s.tpr");
        check_traj("data/tpr/gmx_v2022_d.tpr");
    }

    SECTION("Read TPR Version 2021") {
        check_traj("data/tpr/gmx_v2021_s.tpr");
        check_traj("data/tpr/gmx_v2021_d.tpr");
    }

    SECTION("Read TPR Version 2020") {
        check_traj("data/tpr/gmx_v2020_s.tpr");
        check_traj("data/tpr/gmx_v2020_d.tpr");
    }

    SECTION("Read TPR Version 2019") {
        check_traj("data/tpr/gmx_v2019_s.tpr");
        check_traj("data/tpr/gmx_v2019_d.tpr");
    }

    SECTION("Read TPR Version 2018") {
        check_traj("data/tpr/gmx_v2018_s.tpr");
        check_traj("data/tpr/gmx_v2018_d.tpr");
    }

    SECTION("Read TPR Version 2016") {
        check_traj("data/tpr/gmx_v2016_s.tpr");
        check_traj("data/tpr/gmx_v2016_d.tpr");
    }

    SECTION("Read TPR Version v5.1") {
        check_traj("data/tpr/gmx_v5.1_s.tpr");
        check_traj("data/tpr/gmx_v5.1_d.tpr");
    }

    SECTION("Read TPR Version v5.0") {
        check_traj("data/tpr/gmx_v5.0_s.tpr");
        check_traj("data/tpr/gmx_v5.0_d.tpr");
    }

    SECTION("Read TPR Version v4.6") {
        check_traj("data/tpr/gmx_v4.6_s.tpr");
        check_traj("data/tpr/gmx_v4.6_d.tpr");
    }

    SECTION("Read TPR Version v4.5") {
        check_traj("data/tpr/gmx_v4.5_s.tpr");
        check_traj("data/tpr/gmx_v4.5_d.tpr");
    }
}

TEST_CASE("Errors") {
    SECTION("Read multiple frames") {
        auto file = Trajectory("data/tpr/gmx_v2021_s.tpr");
        file.read();
        CHECK_THROWS_WITH(
            file.read(),
            "can not read file 'data/tpr/gmx_v2021_s.tpr' at index 1: "
            "there are 1 frames in this file"
        );
    }

    SECTION("Write error") {
        auto tmpfile = NamedTempPath(".tpr");
        CHECK_THROWS_WITH(Trajectory(tmpfile, 'w'), "TPR format does not support write & append");
    }

    SECTION("Append error") {
        auto tmpfile = NamedTempPath(".tpr");
        CHECK_THROWS_WITH(Trajectory(tmpfile, 'a'), "TPR format does not support write & append");
    }
}
