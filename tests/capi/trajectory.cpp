// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cstring>
#include <fstream>
#include <sstream>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

static CHFL_FRAME* testing_frame();

TEST_CASE("Read trajectory") {
    SECTION("Constructors errors") {
        CHECK(chfl_trajectory_open("not there", 'r') == nullptr);
        CHECK(chfl_trajectory_with_format("not there", 'r', "") == nullptr);

        // Not technically constructors, but close enough
        CHECK(chfl_trajectory_memory_reader("not there", 5, "") == nullptr);
    }

    SECTION("Path") {
        CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("data/xyz/water.xyz", 'r');
        REQUIRE(trajectory);

        char path[256] = {0};
        CHECK_STATUS(chfl_trajectory_path(trajectory, path, sizeof(path)));
        CHECK(std::string(path) == "data/xyz/water.xyz");

        chfl_trajectory_close(trajectory);
    }

    SECTION("Number of steps") {
        CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("data/xyz/water.xyz", 'r');
        REQUIRE(trajectory);

        uint64_t nsteps = 0;
        CHECK_STATUS(chfl_trajectory_nsteps(trajectory, &nsteps));
        CHECK(nsteps == 100);

        chfl_trajectory_close(trajectory);
    }

    SECTION("Open with format") {
        CHFL_TRAJECTORY* trajectory = chfl_trajectory_with_format("data/xyz/helium.xyz.but.not.really", 'r', "XYZ");
        CHFL_FRAME* frame = chfl_frame();
        REQUIRE(trajectory);
        REQUIRE(frame);

        CHECK_STATUS(chfl_trajectory_read(trajectory, frame));

        uint64_t natoms = 0;
        CHECK_STATUS(chfl_frame_atoms_count(frame, &natoms));
        CHECK(natoms == 125);

        chfl_free(frame);
        chfl_trajectory_close(trajectory);
    }

    SECTION("Open memory for reading") {
        CHFL_TRAJECTORY* trajectory = chfl_trajectory_memory_reader("c1ccccc1", 9, "SMI");
        CHFL_FRAME* frame = chfl_frame();
        REQUIRE(trajectory);
        REQUIRE(frame);

        CHECK_STATUS(chfl_trajectory_read(trajectory, frame));

        uint64_t natoms = 0;
        CHECK_STATUS(chfl_frame_atoms_count(frame, &natoms));
        CHECK(natoms == 6);

        chfl_free(frame);
        chfl_trajectory_close(trajectory);
    }

    SECTION("Read next step") {
        CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("data/xyz/water.xyz", 'r');
        CHFL_FRAME* frame = chfl_frame();
        REQUIRE(trajectory);
        REQUIRE(frame);

        CHECK_STATUS(chfl_trajectory_read(trajectory, frame));

        uint64_t natoms = 0;
        CHECK_STATUS(chfl_frame_atoms_count(frame, &natoms));
        CHECK(natoms == 297);

        chfl_vector3d* positions = nullptr;
        // Check for the error when requesting non-existent velocities
        CHECK(chfl_frame_velocities(frame, &positions, &natoms) != CHFL_SUCCESS);

        chfl_vector3d positions_0 = {0.417219, 8.303366, 11.737172};
        chfl_vector3d positions_124 = {5.099554, -0.045104, 14.153846};

        // Check positions in the first frame
        CHECK_STATUS(chfl_frame_positions(frame, &positions, &natoms));
        CHECK(natoms == 297);
        for (unsigned i=0; i<3; i++) {
            CHECK(approx_eq(positions[0][i], positions_0[i], 1e-12));
            CHECK(approx_eq(positions[124][i], positions_124[i], 1e-12));
        }

        chfl_free(frame);
        chfl_trajectory_close(trajectory);
    }

    SECTION("Read specific step") {
        CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("data/xyz/water.xyz", 'r');
        CHFL_FRAME* frame = chfl_frame();
        REQUIRE(trajectory);
        REQUIRE(frame);

        CHECK_STATUS(chfl_trajectory_read_step(trajectory, 41, frame));

        uint64_t natoms = 0;
        CHECK_STATUS(chfl_frame_atoms_count(frame, &natoms));
        CHECK(natoms == 297);

        chfl_vector3d positions_0 = {0.761277, 8.106125, 10.622949};
        chfl_vector3d positions_124 = {5.13242, 0.079862, 14.194161};

        chfl_vector3d* positions = nullptr;
        CHECK_STATUS(chfl_frame_positions(frame, &positions, &natoms));
        CHECK(natoms == 297);
        for (unsigned i=0; i<3; i++) {
            CHECK(approx_eq(positions[0][i], positions_0[i], 1e-12));
            CHECK(approx_eq(positions[124][i], positions_124[i], 1e-12));
        }

        chfl_free(frame);
        chfl_trajectory_close(trajectory);
    }

    SECTION("Get topology") {
        CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("data/xyz/water.xyz", 'r');
        CHFL_FRAME* frame = chfl_frame();
        REQUIRE(trajectory);
        REQUIRE(frame);

        CHECK_STATUS(chfl_trajectory_read(trajectory, frame));

        const CHFL_TOPOLOGY* topology = chfl_topology_from_frame(frame);
        REQUIRE(topology);

        uint64_t natoms = 0;
        CHECK_STATUS(chfl_topology_atoms_count(topology, &natoms));
        CHECK(natoms == 297);

        uint64_t n = 10;
        CHECK_STATUS(chfl_topology_bonds_count(topology, &n));
        CHECK(n == 0);

        const CHFL_ATOM* atom = chfl_atom_from_frame(frame, 0);
        REQUIRE(atom);

        char name[32];
        CHECK_STATUS(chfl_atom_name(atom, name, sizeof(name)));
        CHECK(name == std::string("O"));

        chfl_free(atom);
        chfl_free(topology);
        chfl_free(frame);
        chfl_trajectory_close(trajectory);
    }

    SECTION("Set cell") {
        CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("data/xyz/water.xyz", 'r');
        REQUIRE(trajectory);

        chfl_vector3d lengths = {30, 30, 30};
        CHFL_CELL* cell = chfl_cell(lengths, nullptr);
        CHECK_STATUS(chfl_trajectory_set_cell(trajectory, cell));
        chfl_free(cell);

        CHFL_FRAME* frame = chfl_frame();
        REQUIRE(frame);

        CHECK_STATUS(chfl_trajectory_read(trajectory, frame));
        cell = chfl_cell_from_frame(frame);
        REQUIRE(cell);

        chfl_vector3d data = {0};
        CHECK_STATUS(chfl_cell_lengths(cell, data));
        CHECK(data[0] == 30.0);
        CHECK(data[1] == 30.0);
        CHECK(data[2] == 30.0);

        chfl_free(cell);
        chfl_free(frame);
        chfl_trajectory_close(trajectory);
    }

    SECTION("Set topology") {
        CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("data/xyz/trajectory.xyz", 'r');
        REQUIRE(trajectory);

        CHFL_TOPOLOGY* topology = chfl_topology();
        REQUIRE(topology);
        CHFL_ATOM* atom = chfl_atom("Cs");
        REQUIRE(atom);

        for (size_t i=0; i<9; i++) {
            CHECK_STATUS(chfl_topology_add_atom(topology, atom));
        }

        CHECK_STATUS(chfl_trajectory_set_topology(trajectory, topology));

        chfl_free(atom);
        chfl_free(topology);

        CHFL_FRAME* frame = chfl_frame();
        REQUIRE(frame);
        CHECK_STATUS(chfl_trajectory_read(trajectory, frame));

        atom = chfl_atom_from_frame(frame, 1);
        char name[32];
        CHECK_STATUS(chfl_atom_name(atom, name, sizeof(name)));
        CHECK(name == std::string("Cs"));

        chfl_free(atom);
        chfl_free(frame);
        chfl_trajectory_close(trajectory);
    }

    SECTION("Set topology from file") {
        CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("data/xyz/trajectory.xyz", 'r');
        REQUIRE(trajectory);

        CHECK_STATUS(chfl_trajectory_topology_file(trajectory, "data/xyz/topology.xyz", ""));

        CHFL_FRAME* frame = chfl_frame();
        REQUIRE(frame);
        CHECK_STATUS(chfl_trajectory_read(trajectory, frame));

        CHFL_ATOM* atom = chfl_atom_from_frame(frame, 0);
        char name[32] = {0};
        CHECK_STATUS(chfl_atom_name(atom, name, sizeof(name)));
        CHECK(name == std::string("Zn"));
        chfl_free(atom);

        CHECK_STATUS(chfl_trajectory_topology_file(trajectory, "data/xyz/topology.xyz.topology", "XYZ"));
        CHECK_STATUS(chfl_trajectory_read(trajectory, frame));

        atom = chfl_atom_from_frame(frame, 0);
        CHECK_STATUS(chfl_atom_name(atom, name, sizeof(name)));
        CHECK(name == std::string("Zn"));
        chfl_free(atom);

        chfl_free(frame);
        chfl_trajectory_close(trajectory);
    }
}

TEST_CASE("Write trajectory") {
    auto tmpfile = NamedTempPath(".xyz");
    const char* EXPECTED_CONTENT =
    "4\n"
    "Properties=species:S:1:pos:R:3\n"
    "He 1 2 3\n"
    "He 1 2 3\n"
    "He 1 2 3\n"
    "He 1 2 3\n";

    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open(tmpfile.path().c_str(), 'w');
    REQUIRE(trajectory);

    CHFL_FRAME* frame = testing_frame();
    REQUIRE(frame);

    CHECK_STATUS(chfl_trajectory_write(trajectory, frame));

    chfl_free(frame);
    chfl_trajectory_close(trajectory);

    std::ifstream file(tmpfile);
    REQUIRE(file.is_open());
    std::stringstream content;
    content << file.rdbuf();
    file.close();

    CHECK(content.str() == EXPECTED_CONTENT);
}

TEST_CASE("Write trajectory to memory") {
    // Make sure this fails
    CHECK(chfl_trajectory_memory_writer("") == nullptr);

    const char* EXPECTED_CONTENT =
    "4\n"
    "Properties=species:S:1:pos:R:3\n"
    "He 1 2 3\n"
    "He 1 2 3\n"
    "He 1 2 3\n"
    "He 1 2 3\n";

    CHFL_TRAJECTORY* trajectory = chfl_trajectory_memory_writer("XYZ");
    REQUIRE(trajectory);

    CHFL_FRAME* frame = testing_frame();
    REQUIRE(frame);

    CHECK_STATUS(chfl_trajectory_write(trajectory, frame));

    const char* data = nullptr;
    uint64_t size;
    CHECK_STATUS(chfl_trajectory_memory_buffer(trajectory, &data, &size));
    CHECK(size == std::strlen(EXPECTED_CONTENT));
    CHECK(std::string(data) == EXPECTED_CONTENT);

    chfl_free(frame);
    chfl_trajectory_close(trajectory);

    // Make sure that we can not access memory buffer on standard trajectories
    trajectory = chfl_trajectory_open("data/xyz/trajectory.xyz", 'r');
    REQUIRE(trajectory);
    CHECK(chfl_trajectory_memory_buffer(trajectory, &data, &size) != CHFL_SUCCESS);
    chfl_free(trajectory);
}

static CHFL_FRAME* testing_frame() {
    CHFL_TOPOLOGY* topology = chfl_topology();
    CHFL_ATOM* atom = chfl_atom("He");
    REQUIRE(topology);
    REQUIRE(atom);

    for (unsigned i=0; i<4; i++) {
        CHECK_STATUS(chfl_topology_add_atom(topology, atom));
    }
    chfl_free(atom);

    CHFL_FRAME* frame = chfl_frame();
    REQUIRE(frame);
    CHECK_STATUS(chfl_frame_resize(frame, 4));

    CHECK_STATUS(chfl_frame_set_topology(frame, topology));
    chfl_free(topology);

    chfl_vector3d* positions = nullptr;
    uint64_t natoms = 0;
    CHECK_STATUS(chfl_frame_positions(frame, &positions, &natoms));
    CHECK(natoms == 4);

    for (unsigned i=0; i<4; i++) {
        for (unsigned j=0; j<3; j++) {
            positions[i][j] = j + 1.0;
        }
    }

    return frame;
}
