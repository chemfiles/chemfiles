// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
#include "chemfiles/files/XDRFile.hpp"
using namespace chemfiles;

TEST_CASE("XTC files") {
    SECTION("Read") {
        XDRFile file(XDRFile::XTC, "data/xtc/ubiquitin.xtc", File::READ);

        CHECK(file.natoms() == 20455);
        CHECK(file.nframes() == 251);
        CHECK(file.offset(0) == 0);
        CHECK(file.offset(10) == 730856);

        CHECK_THROWS_WITH(
            file.offset(1150),
            "step 1150 is out of bounds, we have only 251 frames"
        );

        CHECK_THROWS_WITH(
            XDRFile(XDRFile::XTC, "not-there.xtc", File::READ),
            "could not open the file at 'not-there.xtc'"
        );
    }

    SECTION("Write") {
        auto filename = NamedTempPath(".xtc");
        // just checking constructor/destructor
        XDRFile file(XDRFile::XTC, filename, File::WRITE);
    }

    SECTION("Append") {
        auto filename = NamedTempPath(".xtc");
        copy_file("data/xtc/ubiquitin.xtc", filename.path());
        XDRFile file(XDRFile::XTC, filename, File::APPEND);

        CHECK(file.natoms() == 20455);
        CHECK(file.nframes() == 251);
        CHECK(file.offset(0) == 0);
        CHECK(file.offset(10) == 730856);

        auto filename_2 = NamedTempPath(".xtc");
        file = XDRFile(XDRFile::XTC, filename_2, File::APPEND);

        CHECK(file.natoms() == 0);
        CHECK(file.nframes() == 0);
        CHECK_THROWS_WITH(file.offset(0), "step 0 is out of bounds, we have only 0 frames");
    }
}

TEST_CASE("TRR files") {
    SECTION("Read") {
        XDRFile file(XDRFile::TRR, "data/trr/water.trr", File::READ);

        CHECK(file.natoms() == 297);
        CHECK(file.nframes() == 100);
        CHECK(file.offset(0) == 0);
        CHECK(file.offset(10) == 36840);

        CHECK_THROWS_WITH(
            file.offset(150),
            "step 150 is out of bounds, we have only 100 frames"
        );

        CHECK_THROWS_WITH(
            XDRFile(XDRFile::TRR, "not-there.trr", File::READ),
            "could not open the file at 'not-there.trr'"
        );
    }

    SECTION("Write") {
        auto filename = NamedTempPath(".trr");
        // just checking constructor/destructor
        XDRFile file(XDRFile::TRR, filename, File::WRITE);
    }

    SECTION("Append") {
        auto filename = NamedTempPath(".trr");
        copy_file("data/trr/water.trr", filename.path());
        XDRFile file(XDRFile::TRR, filename, File::APPEND);

        CHECK(file.natoms() == 297);
        CHECK(file.nframes() == 100);
        CHECK(file.offset(0) == 0);
        CHECK(file.offset(10) == 36840);

        auto filename_2 = NamedTempPath(".trr");
        file = XDRFile(XDRFile::TRR, filename_2, File::APPEND);

        CHECK(file.natoms() == 0);
        CHECK(file.nframes() == 0);
        CHECK_THROWS_WITH(file.offset(0), "step 0 is out of bounds, we have only 0 frames");
    }
}
