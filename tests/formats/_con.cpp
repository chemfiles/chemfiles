// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"
using namespace chemfiles;

TEST_CASE("Read files in Con format") {
    SECTION("Read tiny_cuh2.con") {
        auto file = Trajectory("data/con/tiny_cuh2.con");
        CHECK(file.size() == 1);

        auto frame = file.read();
        CHECK(frame.size() == 4);

        // Check Cell
        auto cell = frame.cell();
        auto lengths = cell.lengths();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(lengths[0], 15.3456, 1e-5));
        CHECK(approx_eq(lengths[1], 21.7020, 1e-5));
        CHECK(approx_eq(lengths[2], 100.0000, 1e-5));

        // Check Topology
        auto topology = frame.topology();
        CHECK(topology.size() == 4);

        // Atom 0 (Cu)
        // 0.6394 0.9045 6.9753
        const auto &atom0 = frame[0];
        CHECK(atom0.name() == "Cu");
        CHECK(approx_eq(atom0.mass(), 63.546, 1e-4));
        CHECK(approx_eq(frame.positions()[0], {0.6394, 0.9045, 6.9753}, 1e-4));

        // Properties
        CHECK(atom0.get("is_fixed")->as_bool() == true);
        CHECK(atom0.get("atom_id")->as_double() == 0.0);

        // Atom 3 (H)
        // 7.9421 9.947 11.733
        const auto &atom3 = frame[3];
        CHECK(atom3.name() == "H");
        CHECK(approx_eq(atom3.mass(), 1.00793, 1e-5));
        CHECK(approx_eq(frame.positions()[3], {7.9421, 9.947, 11.733}, 1e-4));

        CHECK(atom3.get("is_fixed")->as_bool() == false);
        CHECK(atom3.get("atom_id")->as_double() == 3.0);
    }

    SECTION("Read tiny_multi_cuh2.con") {
        auto file = Trajectory("data/con/tiny_multi_cuh2.con");
        CHECK(file.size() == 2);

        // --- Frame 1 ---
        auto frame1 = file.read();
        CHECK(frame1.size() == 4);
        CHECK(frame1.index() == 0); // Trajectory index index

        // Check Atom 0 (Cu)
        const auto &f1_atom0 = frame1[0];
        CHECK(f1_atom0.name() == "Cu");
        CHECK(approx_eq(frame1.positions()[0], {0.6394, 0.9045, 6.9753}, 1e-4));
        CHECK(f1_atom0.get("is_fixed")->as_bool() == true);
        CHECK(f1_atom0.get("atom_id")->as_double() == 0.0);

        // --- Frame 2 ---
        auto frame2 = file.read();
        CHECK(frame2.size() == 4);
        CHECK(frame2.index() == 1);

        // Check Atom 1 (Cu) - Matches Rust test expectation
        // 3.1969 0.9045 6.9752
        const auto &f2_atom1 = frame2[1];
        CHECK(f2_atom1.name() == "Cu");
        CHECK(approx_eq(frame2.positions()[1], {3.1969, 0.9045, 6.9752}, 1e-4));
        CHECK(f2_atom1.get("is_fixed")->as_bool() == true);
        CHECK(f2_atom1.get("atom_id")->as_double() == 1.0);
    }

    SECTION("Read sulfolene.con") {
        // Based on snippet: 4 components, 2 4 6 1 atoms -> 13 total
        auto file = Trajectory("data/con/sulfolene.con");
        CHECK(file.size() == 1);

        auto frame = file.read();
        CHECK(frame.size() == 13);

        auto cell = frame.cell();
        CHECK(approx_eq(cell.lengths(), {25.0, 25.0, 25.0}, 1e-5));

        // Component 1: O (2 atoms)
        CHECK(frame[0].name() == "O");
        CHECK(frame[1].name() == "O");
        // Component 2: C (4 atoms)
        CHECK(frame[2].name() == "C");
        // ...
    }
}

TEST_CASE("Write Con file (Roundtrip)") {
    auto tmpfile = NamedTempPath(".con");

    // Read original
    auto file_read = Trajectory("data/con/tiny_multi_cuh2.con");
    auto frames_original = std::vector<Frame>();
    while (!file_read.done()) {
        frames_original.push_back(file_read.read());
    }

    // Write to temp
    {
        auto file_write = Trajectory(tmpfile, 'w');
        for (const auto &frame : frames_original) {
            file_write.write(frame);
        }
    } // file_write closed here

    // Read back
    auto file_roundtrip = Trajectory(tmpfile);
    CHECK(file_roundtrip.size() == frames_original.size());

    for (size_t i = 0; i < frames_original.size(); ++i) {
        const auto &frame_orig = frames_original[i];
        auto frame_rt = file_roundtrip.read();

        CHECK(frame_rt.size() == frame_orig.size());
        CHECK(approx_eq(frame_rt.cell().lengths(), frame_orig.cell().lengths(),
                        1e-5));

        for (size_t j = 0; j < frame_orig.size(); ++j) {
            CHECK(frame_rt[j].name() == frame_orig[j].name());
            CHECK(approx_eq(frame_rt.positions()[j], frame_orig.positions()[j],
                            1e-4));

            auto fixed_orig = frame_orig[j].get("is_fixed");
            auto fixed_rt = frame_rt[j].get("is_fixed");
            if (fixed_orig && fixed_rt) {
                CHECK(fixed_orig->as_bool() == fixed_rt->as_bool());
            }

            auto id_orig = frame_orig[j].get("atom_id");
            auto id_rt = frame_rt[j].get("atom_id");
            if (id_orig && id_rt) {
                CHECK(id_orig->as_double() == id_rt->as_double());
            }
        }
    }
}

TEST_CASE("Con Iterator Forwarding") {
    auto file = Trajectory("data/con/tiny_multi_cuh2.con");

    // Read index 1 directly (skipping index 0)
    auto frame = file.read_at(1);

    CHECK(frame.index() == 1);
    CHECK(frame.size() == 4);

    // Verify content of frame 1 (Atom 1)
    CHECK(approx_eq(frame.positions()[1], {3.1969, 0.9045, 6.9752}, 1e-4));
}
