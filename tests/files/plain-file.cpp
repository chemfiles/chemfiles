// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles/files/PlainFile.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

TEST_CASE("Read a text file") {
    SECTION("Basic reading functionalities") {
        auto file = TextFile("data/xyz/helium.xyz", File::READ, File::DEFAULT);
        CHECK_FALSE(file.eof());

        CHECK(file.readline() == "125");
        // skip a line
        file.readline();
        CHECK(file.readline() == "He 0.49053 8.41351 0.0777257");
    }

    SECTION("Constructor errors") {
        CHECK_THROWS_WITH(
            PlainFile("not existing", File::READ),
            "could not open the file at 'not existing'"
        );
    }

    SECTION("Count lines in file") {
        auto file = TextFile("data/xyz/helium.xyz", File::READ, File::DEFAULT);

        size_t lines = 0;
        while (!file.eof()) {
            file.readline();
            lines++;
        }

        CHECK(lines == 50419);
        CHECK(file.tellpos() == 1521940);
        CHECK(file.eof());
    }

    SECTION("Handle eof") {
        auto file = TextFile("data/xyz/helium.xyz", File::READ, File::DEFAULT);
        while (!file.eof()) {
            file.readline();
        }

        CHECK(file.tellpos() == 1521940);
        CHECK(file.eof());

        // Reading after EOF gives empty strings
        CHECK(file.readline() == "");
        for (size_t i=0; i<100; i++) {
            CHECK(file.readline() == "");
        }

        file.clear();
        CHECK_FALSE(file.eof());
        CHECK(file.tellpos() == 1521940);

        // If we don't rewind, we get back to the eof
        CHECK(file.readline() == "");
        CHECK(file.eof());

        file.seekpos(0);
        CHECK_FALSE(file.eof());

        // Try to seek past the end
        file.seekpos(1521940 + 10000);
        CHECK_FALSE(file.eof());

        CHECK(file.readline() == "");
        CHECK(file.eof());


        file.rewind();
        CHECK_FALSE(file.eof());
    }

    SECTION("Lines offsets") {
        // Compare offset with std::getline
        std::ifstream stream("data/xyz/helium.xyz", std::ios::binary);
        std::string line;
        auto positions = std::vector<uint64_t>();
        while (stream) {
            positions.push_back(static_cast<uint64_t>(stream.tellg()));
            std::getline(stream, line);
        }
        stream.close();
        CHECK(positions.back() == static_cast<uint64_t>(-1));
        positions.pop_back();

        auto file = TextFile("data/xyz/helium.xyz", File::READ, File::DEFAULT);
        for (size_t i=0; i<positions.size(); i++) {
            CHECK(positions[i] == file.tellpos());
            file.readline();
        }
        CHECK(file.eof());
    }

    SECTION("File seeking") {
        TextFile file("data/xyz/helium.xyz", File::READ, File::DEFAULT);
        CHECK(file.tellpos() == 0);

        for (int i=0; i<120; i++) {
            file.readline();
        }

        auto position = file.tellpos();
        CHECK(file.readline() == "He 3.3945 5.48606 7.20384");

        file.rewind();
        CHECK(file.tellpos() == 0);
        file.seekpos(position);
        CHECK(file.tellpos() == position);
        CHECK(file.readline() == "He 3.3945 5.48606 7.20384");

        for (int i=0; i<10; i++) {
            file.readline();
        }

        // Small seeking, this should not reset the file buffer
        position = file.tellpos();
        CHECK(file.readline() == "He 8.480960 0.867459 4.217944");

        file.seekpos(position);
        CHECK(file.tellpos() == position);
        CHECK(file.readline() == "He 8.480960 0.867459 4.217944");
    }

    SECTION("Read full file") {
        TextFile file("data/xyz/helium.xyz", File::READ, File::DEFAULT);
        auto content = file.readall();

        CHECK(content.size() == 1521940);
        // Check that we don't use extra memory storing '\0'
        CHECK(content.size() == std::strlen(content.c_str()));

        std::ifstream checking("data/xyz/helium.xyz");
        std::string expected((std::istreambuf_iterator<char>(checking)),
                              std::istreambuf_iterator<char>());

        CHECK(content == expected);
    }
}

TEST_CASE("Various line endings") {
    auto tmpfile = NamedTempPath(".dat");

    std::ofstream file(tmpfile, std::ios_base::binary);
    file << "line one\nline two\r\nno eol";
    file.close();

    TextFile plain_file(tmpfile, File::READ, File::DEFAULT);
    CHECK(plain_file.readline() == "line one");
    CHECK(plain_file.readline() == "line two");
    CHECK(plain_file.readline() == "no eol");
}

TEST_CASE("Write a text file") {
    auto filename = NamedTempPath(".dat");

    {
        TextFile file(filename, File::WRITE, File::DEFAULT);
        file.print("Test\n");
        file.print("{}\n", 5467);
        CHECK(file.tellpos() == 10);
    }

    std::ifstream verification(filename);
    REQUIRE(verification.is_open());

    std::string line;
    std::getline(verification, line);
    CHECK(line == "Test");

    std::getline(verification, line);
    CHECK(line == "5467");
}
