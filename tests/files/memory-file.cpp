// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles/files/MemoryFile.hpp"
#include "chemfiles/files/MemoryBuffer.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

const auto TEST_DATA = std::string(
R"(This is
a test
for the memory file
class!
)");

TEST_CASE("Reading from files in memory") {
    SECTION("Basic reading functionalities") {
        auto buffer = std::make_shared<MemoryBuffer>(TEST_DATA.data(), TEST_DATA.size());
        auto file = TextFile(buffer, File::READ, File::DEFAULT);

        CHECK(file.readline() == "This is");
        CHECK(file.readline() == "a test");
        CHECK(file.readline() == "for the memory file");
        CHECK(file.readline() == "class!");
        CHECK(file.readline() == ""); // Need to go past the end to get eol
        CHECK(file.eof());

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

        file.seekpos(5);
        CHECK(file.tellpos() == 5);
        CHECK(file.readline() == "is");
        CHECK(file.readline() == "a test");
        CHECK(file.tellpos() == 15);

        file.seekpos(100); // go past the end
        CHECK_FALSE(file.eof());

        CHECK(file.readline() == "");
        CHECK(file.eof());

        file.rewind();
        CHECK_FALSE(file.eof());

        CHECK_THROWS_WITH(
            file.print("JUNK"),
            "cannot write to a memory file unless it is opened in write mode"
        );

        // Note that a large file is tested with the text-based formats!
        // This way, we can be sure the file works with buffers greater than
        // 8192 in size
    }
}

TEST_CASE("Write to files in memory") {
    SECTION("Basic writing functionalities") {
        // Size 6 as this is the minimal size needed to store "Test\n"
        auto buffer = std::make_shared<MemoryBuffer>(6);
        auto file = TextFile(buffer, File::WRITE, File::DEFAULT);
        CHECK(file.tellpos() == buffer->size());
        CHECK(buffer->size() == 0);
        CHECK(buffer->capacity() == 6);

        file.print("Test\n");
        CHECK(std::string(buffer->data(), buffer->size()) == "Test\n");
        CHECK(std::strlen(buffer->data()) == buffer->size());
        CHECK(file.tellpos() == buffer->size());
        CHECK(buffer->size() == 5);
        CHECK(buffer->capacity() == 6);

        // Check reallocation (more than twice the previous size)
        file.print("JUNKJUNKJUNKJUNKJUNK");
        CHECK(std::string(buffer->data(), buffer->size()) == "Test\nJUNKJUNKJUNKJUNKJUNK");
        CHECK(std::strlen(buffer->data()) == buffer->size());
        CHECK(file.tellpos() == buffer->size());
        CHECK(buffer->size() == 25);
        CHECK(buffer->capacity() > buffer->size());

        CHECK_THROWS_WITH(
            file.seekpos(5),
            "cannot seek a memory file unless it is opened in read mode"
        );

        CHECK_THROWS_WITH(
            file.readline(),
            "cannot read a memory file unless it is opened in read mode"
        );
    }

    SECTION("Writing to a compressed memory file") {
        // This currently is not supported
        auto buffer = std::make_shared<MemoryBuffer>(4096);
        CHECK_THROWS_WITH(
            TextFile(buffer, File::WRITE, File::GZIP),
            "writing to a compressed memory file is not supported"
        );

        CHECK_THROWS_WITH(
            TextFile(buffer, File::WRITE, File::LZMA),
            "writing to a compressed memory file is not supported"
        );

        CHECK_THROWS_WITH(
            TextFile(buffer, File::WRITE, File::BZIP2),
            "writing to a compressed memory file is not supported"
        );
    }

    SECTION("Appending to a memory file") {
        // This currently is not supported
        auto buffer = std::make_shared<MemoryBuffer>(4096);
        CHECK_THROWS_WITH(
            TextFile(buffer, File::APPEND, File::DEFAULT),
            "cannot append (mode 'a') to a memory file"
        );
    }
}
