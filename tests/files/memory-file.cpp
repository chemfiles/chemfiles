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

    SECTION("Read a compressed GZip file") {
        auto gz_file = std::vector<uint8_t>{
            0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x0b, 0x49,
            0x2d, 0x2e, 0xe1, 0x32, 0x35, 0x31, 0x33, 0xe7, 0x02, 0x00, 0x8a, 0x43,
            0x5e, 0x98, 0x0a, 0x00, 0x00, 0x00,
        };

        auto buffer = MemoryBuffer(reinterpret_cast<char*>(gz_file.data()), gz_file.size());
        auto file = TextFile(buffer, File::READ, File::GZIP);

        CHECK(file.readline() == "Test");
        CHECK(file.readline() == "5467");

        gz_file[23] = 0x00;
        auto buffer2 = MemoryBuffer(reinterpret_cast<char*>(gz_file.data()), gz_file.size());
        CHECK_THROWS_WITH(
            TextFile(buffer2, File::READ, File::GZIP),
            "error inflating GZ file: 'incorrect data check'"
        );

        gz_file[0] = 0x00;
        auto buffer3 = MemoryBuffer(reinterpret_cast<char*>(gz_file.data()), gz_file.size());
        CHECK_THROWS_WITH(
            TextFile(buffer3, File::READ, File::GZIP),
            "error inflating GZ file: 'incorrect header check'"
        );
    }

    SECTION("Read a compressed BZip2 file") {
        auto buffer = MemoryBuffer("JUNK", 5);
        CHECK_THROWS_WITH(
            TextFile(buffer, File::READ, File::BZIP2),
            "unsupported decompression format for in memory files"
        );
    }

    SECTION("Read a compressed LZMA file") {
        auto xz_file = std::vector<uint8_t> {
            0xfd, 0x37, 0x7a, 0x58, 0x5a, 0x00, 0x00, 0x04, 0xe6, 0xd6, 0xb4, 0x46,
            0x02, 0x00, 0x21, 0x01, 0x16, 0x00, 0x00, 0x00, 0x74, 0x2f, 0xe5, 0xa3,
            0x01, 0x00, 0x09, 0x54, 0x65, 0x73, 0x74, 0x0a, 0x35, 0x34, 0x36, 0x37,
            0x0a, 0x00, 0x00, 0x00, 0xbd, 0xb5, 0x7a, 0x14, 0x41, 0x54, 0x79, 0xbe,
            0x00, 0x01, 0x22, 0x0a, 0x15, 0x1a, 0xe1, 0x67, 0x1f, 0xb6, 0xf3, 0x7d,
            0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x59, 0x5a
        };

        auto buffer = MemoryBuffer(reinterpret_cast<char*>(xz_file.data()), xz_file.size());
        auto file = TextFile(buffer, File::READ, File::LZMA);

        CHECK(file.readline() == "Test");
        CHECK(file.readline() == "5467");

        xz_file[23] = 0x00;
        auto buffer2 = MemoryBuffer(reinterpret_cast<char*>(xz_file.data()), xz_file.size());
        CHECK_THROWS_WITH(
            TextFile(buffer2, File::READ, File::LZMA),
            "lzma: compressed file is corrupted (code: 9)"
        );

        xz_file[0] = 0x00;
        auto buffer3 = MemoryBuffer(reinterpret_cast<char*>(xz_file.data()), xz_file.size());
        CHECK_THROWS_WITH(
            TextFile(buffer3, File::READ, File::LZMA),
            "lzma: input not in .xz format (code: 7)"
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
            "cannot write a compressed memory file"
        );
    }

    SECTION("Appending to a memory file") {
        // This currently is not supported
        MemoryBuffer buffer;
        CHECK_THROWS_WITH(
            TextFile(buffer, File::APPEND, File::DEFAULT),
            "cannot append 'a' to a memory file"
        );
    }
}
