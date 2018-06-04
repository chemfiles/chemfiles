// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
#include "chemfiles/files/GzFile.hpp"
#include "chemfiles/files/XzFile.hpp"
using namespace chemfiles;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;

static void check_read_file(Trajectory&& file) {
    CHECK(file.nsteps() == 100);

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

    // single stream, single block file
    check_read_file(Trajectory("data/xyz/water.xyz.xz"));
    // single stream, multiple blocks file
    check_read_file(Trajectory("data/xyz/water.blocks.xyz.xz"));
}

template<class FileClass>
static void check_write_file(std::string path) {
    auto frame = Frame();
    frame.add_atom(Atom("A","O"), {1, 2, 3});
    frame.add_atom(Atom("B"), {1, 2, 4});
    frame.add_atom(Atom("C"), {1, 2, 5});
    frame.add_atom(Atom("D"), {1, 2, 6});

    auto file = Trajectory(path, 'w');
    file.write(frame);
    file.close();

    FileClass checking(path, File::READ);
    auto lines = checking.readlines(6);
    CHECK(lines[0] == "4");
    CHECK(lines[1] == "Written by the chemfiles library");
    CHECK(lines[2] == "A 1 2 3");
    CHECK(lines[3] == "B 1 2 4");
    CHECK(lines[4] == "C 1 2 5");
    CHECK(lines[5] == "D 1 2 6");
}


TEST_CASE("Write compressed files in XYZ format") {
    // gz compression
    auto gz_path = NamedTempPath(".xyz.gz");
    check_write_file<GzFile>(gz_path);

    // xz compression
    auto xz_path = NamedTempPath(".xyz.xz");
    check_write_file<XzFile>(xz_path);
}
