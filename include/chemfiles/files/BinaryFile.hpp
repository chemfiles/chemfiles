#ifndef CHEMFILES_BINARY_FILE
#define CHEMFILES_BINARY_FILE

#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

#include "chemfiles/config.h"
#include "chemfiles/File.hpp"

static_assert(sizeof(char) == sizeof(int8_t), "char must be 8-bits");

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#if (CHEMFILES_SIZEOF_VOID_P == 8)
    // use mmap in 64-bit posix
    #define CHEMFILES_BINARY_FILE_USE_MMAP 1
#else
    // otherwise fallback to an implementation using only <cstdio>
    #define CHEMFILES_BINARY_FILE_USE_MMAP 0
#endif
#else
    #define CHEMFILES_BINARY_FILE_USE_MMAP 0
#endif

namespace chemfiles {

/// A `BinaryFile` provides facilities to read/write a few primitive types
/// from/to binary (i.e. non text) files.
///
/// Depending on the file endianness, you should use one of the two subclasses
/// of this file: `BigEndianFile` or `LittleEndianFile`. All the functions
/// convert from/to the native endianess to the file endianess.
class BinaryFile: public File {
public:
    /// Open the file at the given `path` using the given `mode`
    BinaryFile(std::string path, File::Mode mode);

    virtual ~BinaryFile();

    BinaryFile(const BinaryFile&) = delete;
    BinaryFile& operator=(const BinaryFile&) = delete;

    BinaryFile(BinaryFile&& other): File(std::move(other)) {
        *this = std::move(other);
    }
    BinaryFile& operator=(BinaryFile&&);

    /// Get the current position in the file
    uint64_t tell() const;

    /// Seek to the specified `position` in the file
    void seek(uint64_t position);

    /// Read exactly `count` char, and store them in the `data` array
    void read_char(char* data, size_t count);

    /// Read a single char value from the file
    char read_single_char() {
        char value;
        this->read_char(&value, 1);
        return value;
    }

    /// Read exactly `count` 8-bit signed integers, and store them in the `data`
    /// array
    void read_i8(int8_t* data, size_t count) {
        this->read_char(reinterpret_cast<char*>(data), count);
    }
    /// Read a single 8-bit signed integer from the file
    int8_t read_single_i8() {
        int8_t value;
        this->read_i8(&value, 1);
        return value;
    }

    /// Read exactly `count` 8-bit unsigned integers, and store them in the `data`
    /// array
    void read_u8(uint8_t* data, size_t count) {
        this->read_char(reinterpret_cast<char*>(data), count);
    }
    /// Read a single 8-bit unsigned integer from the file
    uint8_t read_single_u8() {
        uint8_t value;
        this->read_u8(&value, 1);
        return value;
    }

    /// Read exactly `count` 16-bit signed integers, and store them in the
    /// `data` array
    virtual void read_i16(int16_t* data, size_t count) = 0;
    /// Read a single 16-bit signed integer from the file
    int16_t read_single_i16() {
        int16_t value;
        this->read_i16(&value, 1);
        return value;
    }

    /// Read exactly `count` 16-bit unsigned integers, and store them in the
    /// `data` array
    virtual void read_u16(uint16_t* data, size_t count) = 0;
    /// Read a single 16-bit unsigned integer from the file
    uint16_t read_single_u16() {
        uint16_t value;
        this->read_u16(&value, 1);
        return value;
    }

    /// Read exactly `count` 32-bit signed integers, and store them in the
    /// `data` array
    virtual void read_i32(int32_t* data, size_t count) = 0;
    /// Read a single 32-bit signed integer from the file
    int32_t read_single_i32() {
        int32_t value;
        this->read_i32(&value, 1);
        return value;
    }

    /// Read exactly `count` 32-bit unsigned integers, and store them in the
    /// `data` array
    virtual void read_u32(uint32_t* data, size_t count) = 0;
    /// Read a single 32-bit unsigned integer from the file
    uint32_t read_single_u32() {
        uint32_t value;
        this->read_u32(&value, 1);
        return value;
    }

    /// Read exactly `count` 64-bit signed integers, and store them in the
    /// `data` array
    virtual void read_i64(int64_t* data, size_t count) = 0;
    /// Read a single 64-bit signed integer from the file
    int64_t read_single_i64() {
        int64_t value;
        this->read_i64(&value, 1);
        return value;
    }

    /// Read exactly `count` 64-bit unsigned integers, and store them in the
    /// `data` array
    virtual void read_u64(uint64_t* data, size_t count) = 0;
    /// Read a single 64-bit unsigned integer from the file
    uint64_t read_single_u64() {
        uint64_t value;
        this->read_u64(&value, 1);
        return value;
    }

    /// Read exactly `count` 32-bit floating point numbers, and store them in
    /// the `data` array
    virtual void read_f32(float* data, size_t count) = 0;
    /// Read a single 32-bit floating point number from the file
    float read_single_f32() {
        float value;
        this->read_f32(&value, 1);
        return value;
    }

    /// Read exactly `count` 64-bit floating point numbers, and store them in
    /// the `data` array
    virtual void read_f64(double* data, size_t count) = 0;
    /// Read a single 64-bit floating point number from the file
    double read_single_f64() {
        double value;
        this->read_f64(&value, 1);
        return value;
    }



    /// Write exactly `count` char values taken from the `data` array to the
    /// file
    void write_char(const char* data, size_t count);
    /// Write a single char value to the file
    void write_single_char(char value) {
        this->write_char(&value, 1);
    }

    /// Write exactly `count` 8-bit signed integers taken from the `data` array
    /// to the file
    void write_i8(const int8_t* data, size_t count) {
        this->write_char(reinterpret_cast<const char*>(data), count);
    }
    /// Write a single 8-bit signed integer to the file
    void write_single_i8(int8_t value) {
        this->write_i8(&value, 1);
    }

    /// Write exactly `count` 8-bit unsigned integers taken from the `data`
    /// array to the file
    void write_u8(const uint8_t* data, size_t count) {
        this->write_char(reinterpret_cast<const char*>(data), count);
    }
    /// Write a single 8-bit unsigned integer to the file
    void write_single_u8(uint8_t value) {
        this->write_u8(&value, 1);
    }

    /// Write exactly `count` 16-bit signed integers taken from the `data`
    /// array to the file
    virtual void write_i16(const int16_t* data, size_t count) = 0;
    /// Write a single 16-bit signed integer to the file
    void write_single_i16(int16_t value) {
        this->write_i16(&value, 1);
    }

    /// Write exactly `count` 16-bit unsigned integers taken from the `data`
    /// array to the file
    virtual void write_u16(const uint16_t* data, size_t count) = 0;
    /// Write a single 16-bit unsigned integer to the file
    void write_single_u16(uint16_t value) {
        this->write_u16(&value, 1);
    }

    /// Write exactly `count` 32-bit signed integers taken from the `data`
    /// array to the file
    virtual void write_i32(const int32_t* data, size_t count) = 0;
    /// Write a single 32-bit signed integer to the file
    void write_single_i32(int32_t value) {
        this->write_i32(&value, 1);
    }

    /// Write exactly `count` 32-bit unsigned integers taken from the `data`
    /// array to the file
    virtual void write_u32(const uint32_t* data, size_t count) = 0;
    /// Write a single 32-bit unsigned integer to the file
    void write_single_u32(uint32_t value) {
        this->write_u32(&value, 1);
    }

    /// Write exactly `count` 64-bit signed integers taken from the `data`
    /// array to the file
    virtual void write_i64(const int64_t* data, size_t count) = 0;
    /// Write a single 64-bit signed integer to the file
    void write_single_i64(int64_t value) {
        this->write_i64(&value, 1);
    }

    /// Write exactly `count` 64-bit unsigned integers taken from the `data`
    /// array to the file
    virtual void write_u64(const uint64_t* data, size_t count) = 0;
    /// Write a single 64-bit unsigned integer to the file
    void write_single_u64(uint64_t value) {
        this->write_u64(&value, 1);
    }

    /// Write exactly `count` 32-bit floating point numbers taken from the
    /// `data` array to the file
    virtual void write_f32(const float* data, size_t count) = 0;
    /// Write a single 32-bit floating point number to the file
    void write_single_f32(float value) {
        this->write_f32(&value, 1);
    }

    /// Write exactly `count` 64-bit floating point numbers taken from the
    /// `data` array to the file
    virtual void write_f64(const double* data, size_t count) = 0;
    /// Write a single 64-bit floating point number to the file
    void write_single_f64(double value) {
        this->write_f64(&value, 1);
    }

private:
#if CHEMFILES_BINARY_FILE_USE_MMAP
    int file_descriptor_ = -1;
    char* mmap_data_ = nullptr;
    size_t file_size_ = 0;
    size_t mmap_size_ = 0;
    int mmap_prot_ = 0;
    size_t page_size_ = 0;
    uint64_t offset_ = 0;
    uint64_t total_written_size_ = 0;
#else
    FILE* file_ = nullptr;
#endif
};

class LittleEndianFile: public BinaryFile {
public:
    LittleEndianFile(std::string path, File::Mode mode): BinaryFile(std::move(path), mode) {}

    virtual ~LittleEndianFile() override = default;

    LittleEndianFile(const LittleEndianFile&) = delete;
    LittleEndianFile& operator=(const LittleEndianFile&) = delete;

    LittleEndianFile(LittleEndianFile&&) = default;
    LittleEndianFile& operator=(LittleEndianFile&&) = default;

    void read_i16(int16_t* data, size_t count) final override;
    void read_u16(uint16_t* data, size_t count) final override;
    void read_i32(int32_t* data, size_t count) final override;
    void read_u32(uint32_t* data, size_t count) final override;
    void read_i64(int64_t* data, size_t count) final override;
    void read_u64(uint64_t* data, size_t count) final override;
    void read_f32(float* data, size_t count) final override;
    void read_f64(double* data, size_t count) final override;

    void write_i16(const int16_t* data, size_t count) final override;
    void write_u16(const uint16_t* data, size_t count) final override;
    void write_i32(const int32_t* data, size_t count) final override;
    void write_u32(const uint32_t* data, size_t count) final override;
    void write_i64(const int64_t* data, size_t count) final override;
    void write_u64(const uint64_t* data, size_t count) final override;
    void write_f32(const float* data, size_t count) final override;
    void write_f64(const double* data, size_t count) final override;
};

class BigEndianFile: public BinaryFile {
public:
    BigEndianFile(std::string path, File::Mode mode): BinaryFile(std::move(path), mode) {}

    virtual ~BigEndianFile() override = default;

    BigEndianFile(const BigEndianFile&) = delete;
    BigEndianFile& operator=(const BigEndianFile&) = delete;

    BigEndianFile(BigEndianFile&&) = default;
    BigEndianFile& operator=(BigEndianFile&&) = default;

    void read_i16(int16_t* data, size_t count) final override;
    void read_u16(uint16_t* data, size_t count) final override;
    void read_i32(int32_t* data, size_t count) final override;
    void read_u32(uint32_t* data, size_t count) final override;
    void read_i64(int64_t* data, size_t count) final override;
    void read_u64(uint64_t* data, size_t count) final override;
    void read_f32(float* data, size_t count) final override;
    void read_f64(double* data, size_t count) final override;

    void write_i16(const int16_t* data, size_t count) final override;
    void write_u16(const uint16_t* data, size_t count) final override;
    void write_i32(const int32_t* data, size_t count) final override;
    void write_u32(const uint32_t* data, size_t count) final override;
    void write_i64(const int64_t* data, size_t count) final override;
    void write_u64(const uint64_t* data, size_t count) final override;
    void write_f32(const float* data, size_t count) final override;
    void write_f64(const double* data, size_t count) final override;
};

}

#endif
