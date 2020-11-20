// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"

using namespace chemfiles;

TEST_CASE("Read files in MMTF format") {
    SECTION("Read single step") {
        auto file = Trajectory("data/mmtf/4HHB.mmtf");
        Frame frame = file.read();

        // Check frame properties
        CHECK(frame.get("name")->as_string() ==
              "THE CRYSTAL STRUCTURE OF HUMAN DEOXYHAEMOGLOBIN AT 1.74 "
              "ANGSTROMS RESOLUTION");
        CHECK(frame.get("deposition_date")->as_string() == "1984-03-07");
        CHECK(frame.get("pdb_idcode")->as_string() == "4HHB");

        // If comparing to the RCSB-PDB file,
        // remember that TER increases the number of atoms
        CHECK(frame.size() == 4779);

        // Check reading positions
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(6.204, 16.869, 4.854), 1e-3));
        CHECK(approx_eq(positions[296], Vector3D(10.167, -7.889, -16.138 ), 1e-3));
        CHECK(approx_eq(positions[4778], Vector3D(-1.263, -2.837, -21.251 ), 1e-3));

        // Check the unit cell
        CHECK(approx_eq(frame.cell().lengths(), {63.150, 83.590,53.800}, 1e-3));
        CHECK(approx_eq(frame.cell().angles(), {90.00, 99.34, 90.00}, 1e-3));

        // Check residue information
        CHECK(frame.topology().residues().size() == 801);

        // Iron in Heme
        auto residue = *frame.topology().residue_for_atom(4557);
        CHECK(residue.size() == 43);
        CHECK(residue.name() == "HEM");
        CHECK(residue.get("composition_type")->as_string() == "NON-POLYMER");
        CHECK(!residue.get("is_standard_pdb")->as_bool()); // Should be a hetatm
        CHECK(residue.get("secondary_structure") == nullopt);

        // Nitrogen-Iron Bond
        CHECK(frame.topology().bond_order(4557, 4556) == Bond::SINGLE);

        // Random double bond in HEM group
        CHECK(frame.topology().bond_order(4541, 4542) == Bond::DOUBLE);

        // Check residue connectivity
        const auto& topology = frame.topology();
        CHECK(topology.are_linked(topology.residue(0), topology.residue(1)));
        CHECK(!topology.are_linked(topology.residue(0), topology.residue(2)));

        // Chain information
        CHECK(residue.get("chainid"));
        CHECK(residue.get("chainname"));
        CHECK(residue.get("chainindex"));

        CHECK(residue.get("chainid")->as_string() == "J");
        CHECK(residue.get("chainname")->as_string() == "D");
        CHECK(residue.get("chainindex")->as_double() == 9 );

        CHECK(residue.contains(4525));

        auto water_res = *frame.topology().residue_for_atom(4558);
        CHECK(water_res.size() == 1);
        CHECK(water_res.name() == "HOH");

        CHECK(water_res.get("chainid"));
        CHECK(water_res.get("chainname"));
        CHECK(water_res.get("chainindex"));

        CHECK(water_res.get("chainid")->as_string() == "K");
        CHECK(water_res.get("chainname")->as_string() == "A");
        CHECK(water_res.get("chainindex")->as_double() == 10 );

        auto water_res2 = *frame.topology().residue_for_atom(4614);
        CHECK(water_res2.size() == 1);
        CHECK(water_res2.name() == "HOH");

        CHECK(water_res2.get("chainid"));
        CHECK(water_res2.get("chainname"));
        CHECK(water_res2.get("chainindex"));

        CHECK(water_res2.get("chainid")->as_string() == "L");
        CHECK(water_res2.get("chainname")->as_string() == "B");
        CHECK(water_res2.get("chainindex")->as_double() == 11 );

        // Check the secondary structure
        CHECK(topology.residue(05).get("secondary_structure")->as_string() == "alpha helix");
        CHECK(topology.residue(18).get("secondary_structure")->as_string() == "turn");
        CHECK(topology.residue(36).get("secondary_structure")->as_string() == "3-10 helix");
        CHECK(topology.residue(45).get("secondary_structure")->as_string() == "bend");
        CHECK(topology.residue(143).get("secondary_structure")->as_string() == "coil");
    }

    SECTION("Skip steps") {
        auto file = Trajectory("data/mmtf/1J8K.mmtf");

        auto frame = file.read_step(13);
        CHECK(frame.size() == 1402);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-5.106, 16.212, 4.562), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(5.601, -22.571, -16.631), 1e-3));

        auto topology = frame.topology();
        CHECK(topology.are_linked(topology.residue(0), topology.residue(1)));
        CHECK(!topology.are_linked(topology.residue(0), topology.residue(2)));
        CHECK(topology.residue(0).get("is_standard_pdb")->as_bool());
        CHECK(topology.residue(1).get("is_standard_pdb")->as_bool());
        CHECK(topology.residue(2).get("is_standard_pdb")->as_bool());

        frame = file.read_step(1);
        CHECK(frame.size() == 1402);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D( -9.134, 11.149, 6.990), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(4.437, -13.250, -22.569), 1e-3));

        topology = frame.topology();
        CHECK(topology.are_linked(topology.residue(0), topology.residue(1)));
        CHECK(!topology.are_linked(topology.residue(0), topology.residue(2)));
        CHECK(topology.residue(0).get("composition_type")->as_string() == "L-PEPTIDE LINKING");

        // Check secondary structure
        CHECK(topology.residue(10).get("secondary_structure")->as_string() == "extended");
    }

    SECTION("Bug in 1HTQ") {
        // Fast-forward in `read_step` calculates wrong indices
        // https://github.com/chemfiles/chemfiles/issues/344
        auto file = Trajectory("data/mmtf/1HTQ.mmtf");

        auto frame = file.read_step(9);
        CHECK(frame.size() == 97872);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(103.657, 52.540, 137.019), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(73.297, 19.998, 146.804), 1e-3));

        frame = file.read_step(1);
        CHECK(frame.size() == 97872);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(104.485, 52.282, 139.288), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(73.385, 19.914, 146.528), 1e-3));
    }

    SECTION("Successive steps") {
        auto file = Trajectory("data/mmtf/1J8K.mmtf");

        auto frame = file.read();

        auto topology = frame.topology();
        CHECK(topology.are_linked(topology.residue(0), topology.residue(1)));
        CHECK(!topology.are_linked(topology.residue(0), topology.residue(2)));

        frame = file.read();
        topology = frame.topology();
        CHECK(topology.are_linked(topology.residue(0), topology.residue(1)));
        CHECK(!topology.are_linked(topology.residue(0), topology.residue(2)));

        auto frame3 = file.read();
    }

    SECTION("Alternative locations and symmetry operations") {
        auto file = Trajectory("data/mmtf/5A1I.mmtf");

        auto frame = file.read();
        const auto residues = frame.topology().residues();
        CHECK(!frame[0].get("altloc")); // no alt loc

        const auto& sam = residues[387];
        CHECK(sam.name() == "SAM");
        REQUIRE(frame[*sam.begin()].get("altloc"));
        CHECK(frame[*sam.begin()].get("altloc")->as_string() == "A");

        const auto& adn = residues[388];
        CHECK(adn.name() == "ADN");
        REQUIRE(frame[*adn.begin()].get("altloc"));
        CHECK(frame[*adn.begin()].get("altloc")->as_string() == "C");

        const auto& edo = residues[390];
        CHECK(edo.name() == "EDO");
        REQUIRE(frame[*edo.begin()].get("altloc"));
        CHECK(frame[*edo.begin()].get("altloc")->as_string() == "B");

        // Check to ensure that the symmetry operations are applied
        CHECK(frame.size() == 15912);

        const auto& last_residue = residues[residues.size() - 1];
        CHECK(last_residue.get("chainindex")->as_double() == -1.0);
    }

    SECTION("Read reduced representation") {
        auto file = Trajectory("data/mmtf/1HTQ_reduced.mmtf");
        CHECK(file.nsteps() == 10);

        auto frame = file.read_step(9);
        CHECK(frame.size() == 12336);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(104.656, 52.957, 138.038), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(66.292, -29.336, 158.267), 1e-3));

        frame = file.read_step(1);
        CHECK(frame.size() == 12336);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(105.482, 51.793, 140.282), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(66.033, -29.676, 158.009), 1e-3));
    }

    SECTION("GZ Files") {
        auto file = Trajectory("data/mmtf/1J8K.mmtf.gz");

        auto frame = file.read_step(13);
        CHECK(frame.size() == 1402);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-5.106, 16.212, 4.562), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(5.601, -22.571, -16.631), 1e-3));
        const auto& topology = frame.topology();
        CHECK(topology.are_linked(topology.residue(0), topology.residue(1)));
        CHECK(!topology.are_linked(topology.residue(0), topology.residue(2)));
    }

    SECTION("Large MMTF file") {
    // Test fails on Windows due to timing of MSVC debug builds
    #ifndef CHEMFILES_WINDOWS
        // Test takes too long with valgrind
        if (!is_valgrind_and_travis()) {
            auto file = Trajectory("data/mmtf/3J3Q.mmtf.gz");
            auto frame = file.read_step(0);

            // We just read 2,400,000 atoms and 2,500,000 bonds
            // in ~3s (in release mode) !!!
            CHECK(frame.size() == 2440800);
            CHECK(frame.topology().bonds().size() == 2497752);
        }
    #endif
    }

    SECTION("XZ Files") {
        auto file = Trajectory("data/mmtf/1J8K.mmtf.xz");

        auto frame = file.read_step(13);
        CHECK(frame.size() == 1402);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-5.106, 16.212, 4.562), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(5.601, -22.571, -16.631), 1e-3));
        const auto& topology = frame.topology();
        CHECK(topology.are_linked(topology.residue(0), topology.residue(1)));
        CHECK(!topology.are_linked(topology.residue(0), topology.residue(2)));
    }
}

TEST_CASE("Write files in MMTF format") {
    SECTION("Single model") {
        auto file_r = Trajectory("data/mmtf/4HHB.mmtf");
        Frame frame = file_r.read();

        auto tmpfile = NamedTempPath(".mmtf");
        auto file = Trajectory(tmpfile, 'w');
        file.write(frame);
        file.close();

        auto file_r2 = Trajectory(tmpfile);
        Frame frame2 = file_r2.read();

        CHECK(frame2.size() == 4779);

        auto positions = frame2.positions();
        CHECK(approx_eq(positions[0], Vector3D(6.204, 16.869, 4.854), 1e-3));
        CHECK(approx_eq(positions[296], Vector3D(10.167, -7.889, -16.138 ), 1e-3));
        CHECK(approx_eq(positions[4778], Vector3D(-1.263, -2.837, -21.251 ), 1e-3));

        CHECK(frame2.cell() == frame.cell());
    }

    SECTION("Multiple models") {
        auto file_r = Trajectory("data/mmtf/1J8K.mmtf");

        auto tmpfile = NamedTempPath(".mmtf");
        auto file = Trajectory(tmpfile, 'w');

        file.write(file_r.read());
        file.write(file_r.read());
        file.write(file_r.read());

        auto frame_mod = file_r.read();
        frame_mod.set_cell(UnitCell({10, 10, 10}));
        file.write(frame_mod);

        file.close();

        auto file_r2 = Trajectory(tmpfile);
        CHECK(file_r2.nsteps() == 4);

        auto frame = file_r2.read_step(1);
        CHECK(frame.size() == 1402);
        const auto& positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D( -9.134, 11.149, 6.990), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(4.437, -13.250, -22.569), 1e-3));

        // Check to be sure bonds are copied properly
        const auto& topology = frame.topology();
        CHECK(topology.are_linked(topology.residue(0), topology.residue(1)));
        CHECK(!topology.are_linked(topology.residue(0), topology.residue(2)));
    }

    SECTION("Structure not from RCSB") {
        auto tmpfile = NamedTempPath(".mmtf");

        {
            auto frame1 = Frame();
            frame1.add_atom(Atom("A"), {0, 0, 0});
            frame1.add_atom(Atom("B"), {1, 1, 1});
            frame1.add_atom(Atom("C23456", "HuuuuH"), {2, 2, 2});
            frame1.add_bond(0, 1);
            frame1.add_bond(0, 2, Bond::TRIPLE);

            auto residue = Residue("A");
            residue.add_atom(0);
            residue.add_atom(2);
            frame1.add_residue(std::move(residue));

            residue = Residue("B");
            residue.add_atom(1);
            frame1.add_residue(std::move(residue));

            // No residues, no atom name, nothing!
            auto frame2 = Frame();
            frame2.add_atom(Atom(""), {10, 10, 10});
            frame2.add_atom(Atom(""), {11, 11, 11});

            auto trajectory = Trajectory(tmpfile, 'w');
            trajectory.write(frame1);
            trajectory.write(frame2);
        }

        auto trajectory = Trajectory(tmpfile, 'r');
        REQUIRE(trajectory.nsteps() == 2);

        auto frame = trajectory.read();
        REQUIRE(frame.size() == 3);

        CHECK(frame[0].name() == "A");
        CHECK(frame[1].name() == "B");
        // Truncated name & type
        CHECK(frame[2].name() == "C2345");
        CHECK(frame[2].type() == "Huu");

        auto positions = frame.positions();
        CHECK(positions[0] == Vector3D(0, 0, 0));
        CHECK(positions[1] == Vector3D(1, 1, 1));
        CHECK(positions[2] == Vector3D(2, 2, 2));

        CHECK(frame.topology().bonds() == std::vector<Bond>{{0, 1}, {0, 2}});
        CHECK(frame.topology().bond_orders() == std::vector<Bond::BondOrder>{Bond::UNKNOWN, Bond::TRIPLE});

        frame = trajectory.read();
        REQUIRE(frame.size() == 2);

        CHECK(frame[0].name() == "");
        CHECK(frame[1].name() == "");

        positions = frame.positions();
        CHECK(approx_eq(positions[0], {10, 10, 10}, 1e-6));
        CHECK(approx_eq(positions[1], {11, 11, 11}, 1e-6));

        CHECK(frame.topology().bonds().empty());
    }
}

TEST_CASE("Read memory in MMTF format") {
    SECTION("Plain MMTF Memory") {
        std::ifstream checking("data/mmtf/1J8K.mmtf", std::ifstream::binary);
        std::vector<char> content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());

        auto file = Trajectory::memory_reader(content.data(), content.size(), "MMTF");
        auto frame = file.read_step(13);
        CHECK(frame.size() == 1402);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-5.106, 16.212, 4.562), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(5.601, -22.571, -16.631), 1e-3));
    }

    SECTION("GZ MMTF Memory") {
        std::ifstream checking("data/mmtf/1J8K.mmtf.gz", std::ifstream::binary);
        std::vector<char> content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());

        auto file = Trajectory::memory_reader(content.data(), content.size(), "MMTF/GZ");
        auto frame = file.read_step(13);
        CHECK(frame.size() == 1402);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-5.106, 16.212, 4.562), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(5.601, -22.571, -16.631), 1e-3));
    }

    SECTION("XZ MMTF Memory") {
        std::ifstream checking("data/mmtf/1J8K.mmtf.xz", std::ifstream::binary);
        std::vector<char> content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());

        auto file = Trajectory::memory_reader(content.data(), content.size(), "MMTF/XZ");
        auto frame = file.read_step(13);
        CHECK(frame.size() == 1402);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(-5.106, 16.212, 4.562), 1e-3));
        CHECK(approx_eq(positions[1401], Vector3D(5.601, -22.571, -16.631), 1e-3));
    }
}

TEST_CASE("Error checking") {
    CHECK_THROWS_WITH(
        Trajectory("data/mmtf/1J8K.mmtf", 'a'),
        "append mode ('a') is not supported for the MMTF format"
    );

    CHECK_THROWS_WITH(
        Trajectory::memory_reader("JUNK", 5, "MMTF"),
        "error while decoding MMTF from memory: 'Expected msgpack type to be MAP'"
    );

    CHECK_THROWS_WITH(
        Trajectory::memory_writer("MMTF"),
        "the MMTF format cannot write to memory"
    );
}
