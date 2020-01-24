// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles/files/MemoryFile.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

constexpr const char* memory_file =
R"(This is
a test
for the memory file
class!
)";

TEST_CASE("Reading from files in memory") {
    SECTION("Basic reading functionalities") {
        auto buff = MemoryBuffer(memory_file, 43);
        auto file = TextFile(buff, File::READ, File::DEFAULT);

        CHECK(file.readline() == "This is");
        CHECK(file.readline() == "a test");
        CHECK(file.readline() == "for the memory file");

        file.rewind();
        CHECK(file.readline() == "This is");
        CHECK(file.readline() == "a test");

        file.rewind();
        CHECK(file.tellpos() == 0);
        file.readline();
        file.readline();
        CHECK(file.tellpos() == 15);

        file.seekpos(35);
        CHECK(file.readline() == "class!");

        // Count lines

        file.rewind();
        size_t lines = 0;
        while (!file.eof()) {
            file.readline();
            lines++;
        }

        CHECK(lines == 5);
        CHECK(file.eof());

        file.seekpos(6);
        CHECK(file.tellpos() == 6);
        CHECK(file.readline() == "s");

        file.seekpos(100); // go past the end
        CHECK_FALSE(file.eof());

        CHECK(file.readline() == "");
        CHECK(file.eof());

        file.rewind();
        CHECK_FALSE(file.eof());

        CHECK_THROWS_WITH(
            file.seekpos(1000000),
            "error while seeking memory file"
        );
    }
}

TEST_CASE("Write to files in memory") {
    SECTION("Basic writing functionalities") {
        MemoryBuffer buffer(6); // It is 6 as a null character is needed

        auto file = TextFile(buffer, File::WRITE, File::DEFAULT);
        file.print("Test\n");

        auto result = std::string(buffer.data());
        CHECK(result == "Test\n");

        file.print("JUNKJUNK");
        result = std::string(buffer.data());
        CHECK(result == "Test\nJUNKJUNK");
    }

    SECTION("Writing to a compressed memory file") {
        // This currently is not supported
        MemoryBuffer buffer;
        CHECK_THROWS_WITH(
            TextFile(buffer, File::WRITE, File::GZIP),
            "writing to a compressed memory file is not supported"
        );
    }

    SECTION("Appending to a memory file") {
        // This currently is not supported
        MemoryBuffer buffer;
        CHECK_THROWS_WITH(
            TextFile(buffer, File::APPEND, File::DEFAULT),
            "cannot append (mode 'a') to a memory file"
        );
    }
}
