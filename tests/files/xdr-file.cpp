// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "chemfiles/files/XDRFile.hpp"
#include "helpers.hpp"
#include <vector>
using namespace chemfiles;

static void write_xdr_file(XDRFile& file) {
    file.write_gmx_string("Hello!"); // needs 2B padding
    const std::vector<float> array = {1.234f, -5.123f, 100.232f};
    file.write_gmx_compressed_floats(array, 1000.0, false);
}

TEST_CASE("XDR files") {
    SECTION("read") {
        XDRFile file("data/misc/xdr.bin", File::READ);
        CHECK(file.file_size() == 164);

        // read some big-endian data types
        CHECK(file.read_single_i32() == -123);
        CHECK(file.read_single_u32() == 123);
        CHECK(file.read_single_f64() == 5.678);
        CHECK(file.read_single_f32() == -4.567f);

        std::vector<double> darr;
        darr.resize(6);
        file.read_f64(darr);
        const std::vector<double> dexpected = {1.234,    -6.234,    105.232,
                                               1034.346, -5056.465, 10054.475};
        CHECK(darr == dexpected);

        auto array = std::vector<float>();
        array.resize(6);
        file.read_f32(array);
        const std::vector<float> expected = {1.234f,    -5.123f,    100.232f,
                                             1034.346f, -5056.465f, 10054.475f};
        CHECK(array == expected);
        array = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

        // read XDR and GROMACS specific data types
        CHECK(file.read_gmx_string() == "Hello!");
        CHECK(file.read_gmx_compressed_floats(array, false) == 1000.0f);
        for (size_t i = 0; i < 3; ++i) {
            CHECK(approx_eq(array[i], expected[i], 1e-4f));
        }

        // Go back to the beginning to check reading of sizes as i32
        file.seek(0);
        CHECK_THROWS_WITH(file.read_single_size_as_i32(),
                          "invalid value in XDR file: expected a positive integer, got -123");
        CHECK(file.read_single_size_as_i32() == 123);
    }

    // clang-format off
    auto expected = std::vector<uint8_t> {
        0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06,
        'H', 'e', 'l', 'l', 'o', '!', 0x00, 0x00,
        0x44, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x04, 0xd2, 0xff, 0xff, 0xeb, 0xfd,
        0x00, 0x01, 0x87, 0x88, 0x00, 0x00, 0x04, 0xd2, 0xff, 0xff, 0xeb, 0xfd,
        0x00, 0x01, 0x87, 0x88, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x01,
        0x42, 0x00, 0x00, 0x00,
    };
    // clang-format on
    auto expected_2 = expected;
    expected_2.insert(expected_2.end(), expected.begin(), expected.end());

    SECTION("write") {
        auto filename = NamedTempPath(".bin");
        {
            XDRFile file(filename, File::WRITE);
            write_xdr_file(file);
            CHECK(file.file_size() == expected.size());
        }

        auto content = read_binary_file(filename);
        CHECK(content == expected);
    }

    SECTION("write and append") {
        auto filename = NamedTempPath(".bin");
        {
            XDRFile file(filename, File::WRITE);
            write_xdr_file(file);
            CHECK(file.file_size() == expected.size());
        }

        auto content = read_binary_file(filename);
        CHECK(content == expected);

        {
            XDRFile file(filename, File::Mode::APPEND);
            write_xdr_file(file);
            CHECK(file.file_size() == expected_2.size());
        }

        content = read_binary_file(filename);
        CHECK(content == expected_2);
    }

    SECTION("append") {
        auto filename = NamedTempPath(".bin");
        {
            XDRFile file(filename, File::WRITE);
            write_xdr_file(file);
            CHECK(file.file_size() == expected.size());
        }

        auto content = read_binary_file(filename);
        CHECK(content == expected);
    }
}
