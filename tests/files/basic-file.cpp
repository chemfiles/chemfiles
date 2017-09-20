// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles/files/BasicFile.hpp"
using namespace chemfiles;

TEST_CASE("Read a text file"){
    BasicFile file("data/xyz/helium.xyz", File::READ);

    std::string line = file.readline();
    CHECK(line == "125");

    auto lines = file.readlines(42);
    REQUIRE(lines.size() == 42);
    CHECK(lines[0] == "Helium as a Lennard-Jone fluid");
    CHECK(lines[1] == "He 0.49053 8.41351 0.0777257");

    file.rewind();
    line = file.readline();
    CHECK(line == "125");
    line = file.readline();
    CHECK(line == "Helium as a Lennard-Jone fluid");
}

TEST_CASE("Various line endinds"){
    auto tmpfile = NamedTempPath(".dat");

    std::ofstream file(tmpfile, std::ios_base::binary);
    file << "line one\nline two\rline three\r\nno eol";
    file.close();

    BasicFile basic_file(tmpfile, File::READ);
    CHECK(basic_file.readline() == "line one");
    CHECK(basic_file.readline() == "line two");
    CHECK(basic_file.readline() == "line three");
    CHECK(basic_file.readline() == "no eol");
}


TEST_CASE("Write a text file"){
    auto filename = NamedTempPath(".dat");

    {
        BasicFile file(filename, File::WRITE);
        file << "Test" << std::endl;
        file << 5467 << std::endl;
    }

    std::ifstream verification(filename);
    REQUIRE(verification.is_open());

    std::string line;
    std::getline(verification, line);
    CHECK(line == "Test");

    std::getline(verification, line);
    CHECK(line == "5467");
}
