#include <string>
#include <vector>
#include <fstream>
#include <cstdio>

#include "catch.hpp"

#include "Chemharp.hpp"
#include "files/File.hpp"
using namespace harp;

#define FILESDIR SRCDIR "/files/"

TEST_CASE("Read a text file", "[Files]"){
    BasicFile file(FILESDIR "xyz/helium.xyz");
    REQUIRE(file.is_open());

    CHECK(file.nlines() == 128);

    std::string line = file.getline();
    CHECK(line == "125");

    auto lines = file.readlines(42);
    REQUIRE(lines.size() == 42);
    CHECK(lines[0] == "Helium as a Lennard-Jone fluid");
    CHECK(lines[1] == "He 0.49053 8.41351 0.0777257");

    // Geting line count after some operations
    CHECK(file.nlines() == 128);

    file.rewind();
    line = file.getline();
    CHECK(line == "125");

    // State is preserved by the nlines function
    file.nlines();
    line = file.getline();
    CHECK(line == "Helium as a Lennard-Jone fluid");

    // Check stream version
    file.rewind();
    file >> line;
    CHECK(line == "125");
}

TEST_CASE("Write a text file", "[Files]"){
    BasicFile file(SRCDIR"/tmp.dat");
    REQUIRE(file.is_open());

    file << "Test";

    file.writeline("Test again");

    std::vector<std::string> lines;
    lines.push_back("Hello");
    lines.push_back("world");
    file.writelines(lines);

    file.close();

    std::ifstream verification(SRCDIR"/tmp.dat");
    REQUIRE(verification.is_open());

    std::string line;
    std::getline(verification, line);
    CHECK(line == "Test");

    std::getline(verification, line);
    CHECK(line == "Test again");

    std::getline(verification, line);
    CHECK(line == "Hello");
    std::getline(verification, line);
    CHECK(line == "world");

    verification.close();

    remove(SRCDIR"/tmp.dat");
}

TEST_CASE("Errors", "[Files]"){

}
