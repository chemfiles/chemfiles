// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/files/GzFile.hpp"
#include "chemfiles/files/XzFile.hpp"
using namespace chemfiles;

static void check_read_file(Trajectory&& file) {
    REQUIRE(file.nsteps() == 100);

    Frame frame = file.read();

    CHECK(frame.size() == 297);
    auto positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(0.417, 8.303, 11.737), 1e-3));
    CHECK(approx_eq(positions[296], Vector3D(6.664, 11.6148, 12.961), 1e-3));

    file.read(); // Skip a frame
    frame = file.read();

    CHECK(frame.size() == 297);
    positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(0.299, 8.310, 11.721), 1e-3));
    CHECK(approx_eq(positions[296], Vector3D(6.798, 11.509, 12.704), 1e-3));

    file.read();
    file.read();
    // Get back
    frame = file.read_step(2);
    CHECK(frame.size() == 297);
    positions = frame.positions();
    CHECK(approx_eq(positions[0], Vector3D(0.299, 8.310, 11.721), 1e-3));
    CHECK(approx_eq(positions[296], Vector3D(6.798, 11.509, 12.704), 1e-3));
}


TEST_CASE("Read compressed files in XYZ format") {
    // Compression level 6
    check_read_file(Trajectory("data/xyz/water.6.xyz.gz"));
    // Compression level 9
    check_read_file(Trajectory("data/xyz/water.9.xyz.gz"));
    // Multiple streams, compression level 7
    check_read_file(Trajectory("data/xyz/water.multistream.7.xyz.gz"));

    // Compression level 6
    check_read_file(Trajectory("data/xyz/water.6.xyz.bz2"));
    // Compression level 9
    check_read_file(Trajectory("data/xyz/water.9.xyz.bz2"));

    // single stream, single block file
    check_read_file(Trajectory("data/xyz/water.xyz.xz"));
    // single stream, multiple blocks file
    check_read_file(Trajectory("data/xyz/water.blocks.xyz.xz"));
}

TEST_CASE("Explit compression in format name") {
    check_read_file(Trajectory("data/xyz/water.9.xyz.gz", 'r', "XYZ / GZ"));
    check_read_file(Trajectory("data/xyz/water.9.xyz.bz2", 'r', "XYZ / BZ2"));
    check_read_file(Trajectory("data/xyz/water.xyz.xz", 'r', "XYZ / XZ"));
}

static void check_write_file(std::string path, File::Compression compression) {
    auto frame = Frame();
    frame.add_atom(Atom("A","O"), {1, 2, 3});
    frame.add_atom(Atom("B"), {1, 2, 4});
    frame.add_atom(Atom("C"), {1, 2, 5});
    frame.add_atom(Atom("D"), {1, 2, 6});

    {
        auto file = Trajectory(path, 'w');
        file.write(frame);
        file.close();
    }

    TextFile file(path, File::READ, compression);
    CHECK(file.readline() == "4");
    CHECK(file.readline() == "Properties=species:S:1:pos:R:3");
    CHECK(file.readline() == "A 1 2 3");
    CHECK(file.readline() == "B 1 2 4");
    CHECK(file.readline() == "C 1 2 5");
    CHECK(file.readline() == "D 1 2 6");

    CHECK(file.readline() == "");
    CHECK(file.eof());
}


TEST_CASE("Write compressed files in XYZ format") {
    auto gz_path = NamedTempPath(".xyz.gz");
    check_write_file(gz_path, File::GZIP);

    auto bz2_path = NamedTempPath(".xyz.bz2");
    check_write_file(bz2_path, File::BZIP2);

    auto xz_path = NamedTempPath(".xyz.xz");
    check_write_file(xz_path, File::LZMA);
}

static void check_append_file(std::string path, File::Compression compression) {
    auto frame = Frame();
    frame.add_atom(Atom("A","O"), {1, 2, 3});
    frame.add_atom(Atom("B"), {1, 2, 4});
    frame.add_atom(Atom("C"), {1, 2, 5});
    frame.add_atom(Atom("D"), {1, 2, 6});

    {
        auto file = Trajectory(path, 'a');
        file.write(frame);
        CHECK(file.nsteps() == 1);
        file.close();
    }

    frame = Frame();
    frame.add_atom(Atom("D","O"), {3, 2, 1});
    frame.add_atom(Atom("C"), {4, 2, 1});
    frame.add_atom(Atom("B"), {5, 2, 1});
    frame.add_atom(Atom("A"), {6, 2, 1});

    {
        auto file = Trajectory(path, 'a');
        file.write(frame);
        CHECK(file.nsteps() == 2);
        file.close();
    }

    TextFile file(path, File::READ, compression);
    CHECK(file.readline() == "4");
    CHECK(file.readline() == "Properties=species:S:1:pos:R:3");
    CHECK(file.readline() == "A 1 2 3");
    CHECK(file.readline() == "B 1 2 4");
    CHECK(file.readline() == "C 1 2 5");
    CHECK(file.readline() == "D 1 2 6");

    CHECK(file.readline() == "4");
    CHECK(file.readline() == "Properties=species:S:1:pos:R:3");
    CHECK(file.readline() == "D 3 2 1");
    CHECK(file.readline() == "C 4 2 1");
    CHECK(file.readline() == "B 5 2 1");
    CHECK(file.readline() == "A 6 2 1");

    CHECK(file.readline() == "");
    CHECK(file.eof());
}


TEST_CASE("Append to compressed files in XYZ format") {
    auto gz_path = NamedTempPath(".xyz.gz");
    check_append_file(gz_path, File::GZIP);
}
