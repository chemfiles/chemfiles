// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include "catch.hpp"
#include "helpers.hpp"

#include "chemfiles/files/BinaryFile.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

#define CHECK_BINARY_ARRAY(_array_)  \
    CHECK(_array_[0] == 0);   \
    CHECK(_array_[1] == 1);   \
    CHECK(_array_[2] == 2);   \
    CHECK(_array_[3] == 3);   \
    CHECK(_array_[4] == 4);   \
    CHECK(_array_[5] == 5);   \
    CHECK(_array_[6] == 6);   \
    CHECK(_array_[7] == 7);   \
    CHECK(_array_[8] == 8);   \
    CHECK(_array_[9] == 9)

static void check_read_binary_file(BinaryFile& file) {
    char buffer[8] = {0};
    file.read_char(buffer, 4);
    CHECK(buffer == std::string("DATA"));

    CHECK(file.read_single_char() == '2');

    CHECK(file.read_single_i16() == -42);
    CHECK(file.read_single_u16() == 42);

    CHECK(file.read_single_i32() == -573);
    CHECK(file.read_single_u32() == 573);

    CHECK(file.read_single_i64() == -123456);
    CHECK(file.read_single_u64() == 123456);

    CHECK(file.read_single_f32() == 33.3f);
    CHECK(file.read_single_f64() == -55.8);

    auto size = file.read_single_i32();
    CHECK(size == 5);
    file.read_char(buffer, size);
    CHECK(buffer == std::string("hello"));

    int16_t i16[10];
    file.read_i16(i16, 10);
    CHECK_BINARY_ARRAY(i16);

    uint16_t u16[10];
    file.read_u16(u16, 10);
    CHECK_BINARY_ARRAY(u16);

    int32_t i32[10];
    file.read_i32(i32, 10);
    CHECK_BINARY_ARRAY(i32);

    uint32_t u32[10];
    file.read_u32(u32, 10);
    CHECK_BINARY_ARRAY(u32);

    int64_t i64[10];
    file.read_i64(i64, 10);
    CHECK_BINARY_ARRAY(i64);

    uint64_t u64[10];
    file.read_u64(u64, 10);
    CHECK_BINARY_ARRAY(u64);

    float f32[10];
    file.read_f32(f32, 10);
    CHECK_BINARY_ARRAY(f32);

    double f64[10];
    file.read_f64(f64, 10);
    CHECK_BINARY_ARRAY(f64);
}

TEST_CASE("Read binary files") {
    SECTION("big endian") {
        auto file = BigEndianFile("data/misc/big-endian.dat", File::Mode::READ);
        check_read_binary_file(file);
    }

    SECTION("little endian") {
        auto file = LittleEndianFile("data/misc/little-endian.dat", File::Mode::READ);
        check_read_binary_file(file);
    }
}

static void write_binary_file(BinaryFile& file) {
    file.write_char("ABCD", 4);
    file.write_single_i16(-42);
    file.write_single_u16(42);

    file.write_single_i32(-573);
    file.write_single_u32(573);

    file.write_single_i64(-123456);
    file.write_single_u64(123456);

    file.write_single_f32(33.3f);
    file.write_single_f64(-55.8);
}

TEST_CASE("Write binary files") {
    SECTION("big endian") {
        auto filename = NamedTempPath(".data");
        {
            auto file = BigEndianFile(filename, File::Mode::WRITE);
            write_binary_file(file);
        }

        auto content = read_binary_file(filename);
        auto expected = std::vector<uint8_t> {
            'A', 'B', 'C', 'D',
            0xff, 0xd6,
            0x0,  0x2a,
            0xff, 0xff, 0xfd, 0xc3,
            0x0,  0x0,  0x2,  0x3d,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1d, 0xc0,
            0x0,  0x0,  0x0,  0x0,  0x0,  0x1,  0xe2, 0x40,
            0x42, 0x5, 0x33, 0x33,
            0xc0, 0x4b, 0xe6, 0x66, 0x66, 0x66, 0x66, 0x66
        };
        CHECK(content == expected);

        auto file = BigEndianFile(filename, File::Mode::READ);
        CHECK(file.read_single_char() == 'A');
        CHECK(file.read_single_char() == 'B');
        CHECK(file.read_single_char() == 'C');
        CHECK(file.read_single_char() == 'D');
        CHECK(file.read_single_i16() == -42);
        CHECK(file.read_single_u16() == 42);
        CHECK(file.read_single_i32() == -573);
        CHECK(file.read_single_u32() == 573);
        CHECK(file.read_single_i64() == -123456);
        CHECK(file.read_single_u64() == 123456);
        CHECK(file.read_single_f32() == 33.3f);
        CHECK(file.read_single_f64() == -55.8);
    }

    SECTION("little endian") {
        auto filename = NamedTempPath(".data");
        {
            auto file = LittleEndianFile(filename, File::Mode::WRITE);
            write_binary_file(file);
        }

        auto content = read_binary_file(filename);
        auto expected = std::vector<uint8_t> {
            'A', 'B', 'C', 'D',
            0xd6, 0xff,
            0x2a, 0x0,
            0xc3, 0xfd, 0xff, 0xff,
            0x3d, 0x2,  0x0,  0x0,
            0xc0, 0x1d, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
            0x40, 0xe2, 0x1,  0x0,  0x0,  0x0,  0x0,  0x0,
            0x33, 0x33, 0x5,  0x42,
            0x66, 0x66, 0x66, 0x66, 0x66, 0xe6, 0x4b, 0xc0,
        };
        CHECK(content == expected);

        auto file = LittleEndianFile(filename, File::Mode::READ);
        CHECK(file.read_single_char() == 'A');
        CHECK(file.read_single_char() == 'B');
        CHECK(file.read_single_char() == 'C');
        CHECK(file.read_single_char() == 'D');
        CHECK(file.read_single_i16() == -42);
        CHECK(file.read_single_u16() == 42);
        CHECK(file.read_single_i32() == -573);
        CHECK(file.read_single_u32() == 573);
        CHECK(file.read_single_i64() == -123456);
        CHECK(file.read_single_u64() == 123456);
        CHECK(file.read_single_f32() == 33.3f);
        CHECK(file.read_single_f64() == -55.8);
    }
}
