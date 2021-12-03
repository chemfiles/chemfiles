#include <cerrno>
#include <cstdio>
#include <cstring>

#include "chemfiles/error_fmt.hpp"
#include "chemfiles/unreachable.hpp"

#include "chemfiles/files/BinaryFile.hpp"

#ifdef __CYGWIN__
    #include <sys/types.h>
    #define fseek64 fseek
    #define ftell64 ftell
    #define off64_t off_t
#elif defined(_MSC_VER)
    #define fseek64 _fseeki64
    #define ftell64 _ftelli64
    #define off64_t __int64
#else
    // assume unix by default
    #include <sys/types.h>
    #define fseek64 fseeko
    #define ftell64 ftello
    #define off64_t off_t
    static_assert(_FILE_OFFSET_BITS == 64, "_FILE_OFFSET_BITS must be 64");
#endif

using namespace chemfiles;

BinaryFile::BinaryFile(std::string path, File::Mode mode):
    File(std::move(path), mode, File::Compression::DEFAULT)
{
    const char* open_mode;
    if (mode == Mode::READ) {
        open_mode = "rb";
    } else if (mode == Mode::APPEND) {
        open_mode = "r+b";
    } else if (mode == Mode::WRITE) {
        open_mode = "wb";
    } else {
        unreachable();
    }

    file_ = std::fopen(this->path().c_str(), open_mode);
    if (file_ == nullptr) {
        throw file_error(
            "could not open file at '{}': {}", this->path(), std::strerror(errno)
        );
    }
}

BinaryFile::~BinaryFile() {
    if (this->mode() != Mode::READ) {
        std::fflush(file_);
    }

    std::fclose(file_);
}

BinaryFile& BinaryFile::operator=(BinaryFile&& other) {
    File::operator=(std::move(other));

    std::fclose(this->file_);
    this->file_ = other.file_;
    other.file_ = nullptr;

    return *this;
}

void BinaryFile::read_char(char* data, size_t count) {
    auto read = std::fread(data, 1, count, file_);
    if (read != count) {
        throw file_error(
            "failed to read {} bytes from the file at '{}': {}",
            count, this->path(), std::strerror(errno)
        );
    }
}


void BinaryFile::write_char(const char* data, size_t count) {
    auto written = std::fwrite(data, 1, count, file_);
    if (written != count) {
        throw file_error(
            "failed to write {} bytes to the file at '{}': {}",
            count, this->path(), std::strerror(errno)
        );
    }
}


uint64_t BinaryFile::tell() const {
    return static_cast<uint64_t>(ftell64(file_));
}


void BinaryFile::seek(uint64_t position) {
    fseek64(file_, static_cast<off64_t>(position), SEEK_SET);
}


/******************************************************************************/

#define CHEMFILES_LITTLE_ENDIAN 0
#define CHEMFILES_BIG_ENDIAN 1

/***** adapted from boost ******/
// GNU libc offers the helpful header <endian.h> which defines
// __BYTE_ORDER
#if defined (__GLIBC__)
# include <endian.h>
# if (__BYTE_ORDER == __LITTLE_ENDIAN)
#  define CHEMFILES_BYTE_ORDER CHEMFILES_LITTLE_ENDIAN
# elif (__BYTE_ORDER == __BIG_ENDIAN)
#  define CHEMFILES_BYTE_ORDER CHEMFILES_BIG_ENDIAN
# else
#  error Unknown machine endianness detected.
# endif
#elif (defined(_BIG_ENDIAN) || defined(__BIG_ENDIAN__)) && !(defined(_LITTLE_ENDIAN) || defined(__LITTLE_ENDIAN__))
# define CHEMFILES_BYTE_ORDER CHEMFILES_BIG_ENDIAN
#elif (defined(_LITTLE_ENDIAN) || defined(__LITTLE_ENDIAN__)) && !(defined(_BIG_ENDIAN) || defined(__BIG_ENDIAN__))
# define CHEMFILES_BYTE_ORDER CHEMFILES_LITTLE_ENDIAN
#elif defined(__sparc) || defined(__sparc__) \
   || defined(_POWER) || defined(__powerpc__) \
   || defined(__ppc__) || defined(__hpux) || defined(__hppa) \
   || defined(_MIPSEB) || defined(_POWER) \
   || defined(__s390__)
# define CHEMFILES_BYTE_ORDER CHEMFILES_BIG_ENDIAN
#elif defined(__i386__) || defined(__alpha__) \
   || defined(__ia64) || defined(__ia64__) \
   || defined(_M_IX86) || defined(_M_IA64) \
   || defined(_M_ALPHA) || defined(__amd64) \
   || defined(__amd64__) || defined(_M_AMD64) \
   || defined(__x86_64) || defined(__x86_64__) \
   || defined(_M_X64) || defined(__bfin__)
# define CHEMFILES_BYTE_ORDER CHEMFILES_LITTLE_ENDIAN
#else
# error Unknown target machine endianness
#endif
/***** adapted from boost ******/

inline uint16_t swap_u16(uint16_t value) {
    return static_cast<uint16_t>(
        ((static_cast<uint32_t>(value) & 0xff) << 8) |
        ((static_cast<uint32_t>(value) >> 8) & 0xff)
    );
}

inline uint32_t swap_u32(uint32_t value) {
    return (
        (value << 24) |
        ((value <<  8) & 0x00ff0000) |
        ((value >>  8) & 0x0000ff00) |
        ((value >> 24) & 0x000000ff)
    );
}

inline uint64_t swap_u64(uint64_t value) {
    return (
        ((value & 0x00000000000000FFULL) << 56) |
        ((value & 0x000000000000FF00ULL) << 40) |
        ((value & 0x0000000000FF0000ULL) << 24) |
        ((value & 0x00000000FF000000ULL) <<  8) |
        ((value & 0x000000FF00000000ULL) >>  8) |
        ((value & 0x0000FF0000000000ULL) >> 24) |
        ((value & 0x00FF000000000000ULL) >> 40) |
        ((value & 0xFF00000000000000ULL) >> 56)
    );
}

/******************************************************************************/

inline uint16_t u16_from_big_endian(char buffer[2]) {
    uint16_t value;
    std::memcpy(&value, buffer, sizeof(uint16_t));
#if CHEMFILES_BYTE_ORDER == CHEMFILES_LITTLE_ENDIAN
    value = swap_u16(value);
#endif
    return value;
}

inline uint32_t u32_from_big_endian(char buffer[4]) {
    uint32_t value;
    std::memcpy(&value, buffer, sizeof(uint32_t));
#if CHEMFILES_BYTE_ORDER == CHEMFILES_LITTLE_ENDIAN
    value = swap_u32(value);
#endif
    return value;
}

inline uint64_t u64_from_big_endian(char buffer[8]) {
    uint64_t value;
    std::memcpy(&value, buffer, sizeof(uint64_t));
#if CHEMFILES_BYTE_ORDER == CHEMFILES_LITTLE_ENDIAN
    value = swap_u64(value);
#endif
    return value;
}

inline void u16_to_big_endian(uint16_t value, char buffer[2]) {
#if CHEMFILES_BYTE_ORDER == CHEMFILES_LITTLE_ENDIAN
    value = swap_u16(value);
#endif
    std::memcpy(buffer, &value, sizeof(uint16_t));
}

inline void u32_to_big_endian(uint32_t value, char buffer[4]) {
#if CHEMFILES_BYTE_ORDER == CHEMFILES_LITTLE_ENDIAN
    value = swap_u32(value);
#endif
    std::memcpy(buffer, &value, sizeof(uint32_t));
}

inline void u64_to_big_endian(uint64_t value, char buffer[8]) {
#if CHEMFILES_BYTE_ORDER == CHEMFILES_LITTLE_ENDIAN
    value = swap_u64(value);
#endif
    std::memcpy(buffer, &value, sizeof(uint64_t));
}

void BigEndianFile::read_i16(int16_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(int16_t));
    for (size_t i=0; i<count; i++) {
        auto value = u16_from_big_endian(char_data + sizeof(int16_t) * i);
        std::memcpy(data + i, &value, sizeof(int16_t));
    }
}


void BigEndianFile::read_u16(uint16_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(uint16_t));
    for (size_t i=0; i<count; i++) {
        data[i]  = u16_from_big_endian(char_data + sizeof(uint16_t) * i);
    }
}


void BigEndianFile::read_i32(int32_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(int32_t));
    for (size_t i=0; i<count; i++) {
        auto value = u32_from_big_endian(char_data + sizeof(int32_t) * i);
        std::memcpy(data + i, &value, sizeof(int32_t));
    }
}


void BigEndianFile::read_u32(uint32_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(uint32_t));
    for (size_t i=0; i<count; i++) {
        data[i]  = u32_from_big_endian(char_data + sizeof(uint32_t) * i);
    }
}


void BigEndianFile::read_i64(int64_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(int64_t));
    for (size_t i=0; i<count; i++) {
        auto value = u64_from_big_endian(char_data + sizeof(int64_t) * i);
        std::memcpy(data + i, &value, sizeof(int64_t));
    }
}


void BigEndianFile::read_u64(uint64_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(uint64_t));
    for (size_t i=0; i<count; i++) {
        data[i]  = u64_from_big_endian(char_data + sizeof(uint64_t) * i);
    }
}


void BigEndianFile::read_f32(float* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(float));
    for (size_t i=0; i<count; i++) {
        auto value = u32_from_big_endian(char_data + sizeof(float) * i);
        std::memcpy(data + i, &value, sizeof(float));
    }
}


void BigEndianFile::read_f64(double* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(double));
    for (size_t i=0; i<count; i++) {
        auto value = u64_from_big_endian(char_data + sizeof(double) * i);
        std::memcpy(data + i, &value, sizeof(double));
    }
}

void BigEndianFile::write_i16(const int16_t* data, size_t count) {
    char buffer[sizeof(int16_t)];
    uint16_t unsigned_value;
    for (size_t i=0; i<count; i++) {
        std::memcpy(&unsigned_value, data + i, sizeof(int16_t));
        u16_to_big_endian(unsigned_value, buffer);
        this->write_char(buffer, sizeof(int16_t));
    }
}

void BigEndianFile::write_u16(const uint16_t* data, size_t count) {
    char buffer[sizeof(uint16_t)];
    for (size_t i=0; i<count; i++) {
        u16_to_big_endian(data[i], buffer);
        this->write_char(buffer, sizeof(uint16_t));
    }
}

void BigEndianFile::write_i32(const int32_t* data, size_t count) {
    char buffer[sizeof(int32_t)];
    uint32_t unsigned_value;
    for (size_t i=0; i<count; i++) {
        std::memcpy(&unsigned_value, data + i, sizeof(int32_t));
        u32_to_big_endian(unsigned_value, buffer);
        this->write_char(buffer, sizeof(int32_t));
    }
}

void BigEndianFile::write_u32(const uint32_t* data, size_t count) {
    char buffer[sizeof(uint32_t)];
    for (size_t i=0; i<count; i++) {
        u32_to_big_endian(data[i], buffer);
        this->write_char(buffer, sizeof(uint32_t));
    }
}

void BigEndianFile::write_i64(const int64_t* data, size_t count) {
    char buffer[sizeof(int64_t)];
    uint64_t unsigned_value;
    for (size_t i=0; i<count; i++) {
        std::memcpy(&unsigned_value, data + i, sizeof(int64_t));
        u64_to_big_endian(unsigned_value, buffer);
        this->write_char(buffer, sizeof(int64_t));
    }
}

void BigEndianFile::write_u64(const uint64_t* data, size_t count) {
    char buffer[sizeof(uint64_t)];
    for (size_t i=0; i<count; i++) {
        u64_to_big_endian(data[i], buffer);
        this->write_char(buffer, sizeof(uint64_t));
    }
}

void BigEndianFile::write_f32(const float* data, size_t count) {
    char buffer[sizeof(float)];
    uint32_t unsigned_value;
    for (size_t i=0; i<count; i++) {
        std::memcpy(&unsigned_value, data + i, sizeof(float));
        u32_to_big_endian(unsigned_value, buffer);
        this->write_char(buffer, sizeof(float));
    }
}

void BigEndianFile::write_f64(const double* data, size_t count) {
    char buffer[sizeof(double)];
    uint64_t unsigned_value;
    for (size_t i=0; i<count; i++) {
        std::memcpy(&unsigned_value, data + i, sizeof(double));
        u64_to_big_endian(unsigned_value, buffer);
        this->write_char(buffer, sizeof(double));
    }
}


/******************************************************************************/

inline uint16_t u16_from_little_endian(char buffer[2]) {
    uint16_t value;
    std::memcpy(&value, buffer, sizeof(uint16_t));
#if CHEMFILES_BYTE_ORDER == CHEMFILES_BIG_ENDIAN
    value = swap_u16(value);
#endif
    return value;
}

inline uint32_t u32_from_little_endian(char buffer[4]) {
    uint32_t value;
    std::memcpy(&value, buffer, sizeof(uint32_t));
#if CHEMFILES_BYTE_ORDER == CHEMFILES_BIG_ENDIAN
    value = swap_u32(value);
#endif
    return value;
}

inline uint64_t u64_from_little_endian(char buffer[8]) {
    uint64_t value;
    std::memcpy(&value, buffer, sizeof(uint64_t));
#if CHEMFILES_BYTE_ORDER == CHEMFILES_BIG_ENDIAN
    value = swap_u64(value);
#endif
    return value;
}

inline void u16_to_little_endian(uint16_t value, char buffer[2]) {
#if CHEMFILES_BYTE_ORDER == CHEMFILES_BIG_ENDIAN
    value = swap_u16(value);
#endif
    std::memcpy(buffer, &value, sizeof(uint16_t));
}

inline void u32_to_little_endian(uint32_t value, char buffer[4]) {
#if CHEMFILES_BYTE_ORDER == CHEMFILES_BIG_ENDIAN
    value = swap_u32(value);
#endif
    std::memcpy(buffer, &value, sizeof(uint32_t));
}

inline void u64_to_little_endian(uint64_t value, char buffer[8]) {
#if CHEMFILES_BYTE_ORDER == CHEMFILES_BIG_ENDIAN
    value = swap_u64(value);
#endif
    std::memcpy(buffer, &value, sizeof(uint64_t));
}


void LittleEndianFile::read_i16(int16_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(int16_t));
    for (size_t i=0; i<count; i++) {
        auto value = u16_from_little_endian(char_data + sizeof(int16_t) * i);
        std::memcpy(data + i, &value, sizeof(int16_t));
    }
}


void LittleEndianFile::read_u16(uint16_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(uint16_t));
    for (size_t i=0; i<count; i++) {
        data[i]  = u16_from_little_endian(char_data + sizeof(uint16_t) * i);
    }
}


void LittleEndianFile::read_i32(int32_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(int32_t));
    for (size_t i=0; i<count; i++) {
        auto value = u32_from_little_endian(char_data + sizeof(int32_t) * i);
        std::memcpy(data + i, &value, sizeof(int32_t));
    }
}


void LittleEndianFile::read_u32(uint32_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(uint32_t));
    for (size_t i=0; i<count; i++) {
        data[i]  = u32_from_little_endian(char_data + sizeof(uint32_t) * i);
    }
}


void LittleEndianFile::read_i64(int64_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(int64_t));
    for (size_t i=0; i<count; i++) {
        auto value = u64_from_little_endian(char_data + sizeof(int64_t) * i);
        std::memcpy(data + i, &value, sizeof(int64_t));
    }
}


void LittleEndianFile::read_u64(uint64_t* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(uint64_t));
    for (size_t i=0; i<count; i++) {
        data[i]  = u64_from_little_endian(char_data + sizeof(uint64_t) * i);
    }
}


void LittleEndianFile::read_f32(float* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(float));
    for (size_t i=0; i<count; i++) {
        auto value = u32_from_little_endian(char_data + sizeof(float) * i);
        std::memcpy(data + i, &value, sizeof(float));
    }
}


void LittleEndianFile::read_f64(double* data, size_t count) {
    auto char_data = reinterpret_cast<char*>(data);
    this->read_char(char_data, count * sizeof(double));
    for (size_t i=0; i<count; i++) {
        auto value = u64_from_little_endian(char_data + sizeof(double) * i);
        std::memcpy(data + i, &value, sizeof(double));
    }
}

void LittleEndianFile::write_i16(const int16_t* data, size_t count) {
    char buffer[sizeof(int16_t)];
    uint16_t unsigned_value;
    for (size_t i=0; i<count; i++) {
        std::memcpy(&unsigned_value, data + i, sizeof(int16_t));
        u16_to_little_endian(unsigned_value, buffer);
        this->write_char(buffer, sizeof(int16_t));
    }
}

void LittleEndianFile::write_u16(const uint16_t* data, size_t count) {
    char buffer[sizeof(uint16_t)];
    for (size_t i=0; i<count; i++) {
        u16_to_little_endian(data[i], buffer);
        this->write_char(buffer, sizeof(uint16_t));
    }
}

void LittleEndianFile::write_i32(const int32_t* data, size_t count) {
    char buffer[sizeof(int32_t)];
    uint32_t unsigned_value;
    for (size_t i=0; i<count; i++) {
        std::memcpy(&unsigned_value, data + i, sizeof(int32_t));
        u32_to_little_endian(unsigned_value, buffer);
        this->write_char(buffer, sizeof(int32_t));
    }
}

void LittleEndianFile::write_u32(const uint32_t* data, size_t count) {
    char buffer[sizeof(uint32_t)];
    for (size_t i=0; i<count; i++) {
        u32_to_little_endian(data[i], buffer);
        this->write_char(buffer, sizeof(uint32_t));
    }
}

void LittleEndianFile::write_i64(const int64_t* data, size_t count) {
    char buffer[sizeof(int64_t)];
    uint64_t unsigned_value;
    for (size_t i=0; i<count; i++) {
        std::memcpy(&unsigned_value, data + i, sizeof(int64_t));
        u64_to_little_endian(unsigned_value, buffer);
        this->write_char(buffer, sizeof(int64_t));
    }
}

void LittleEndianFile::write_u64(const uint64_t* data, size_t count) {
    char buffer[sizeof(uint64_t)];
    for (size_t i=0; i<count; i++) {
        u64_to_little_endian(data[i], buffer);
        this->write_char(buffer, sizeof(uint64_t));
    }
}

void LittleEndianFile::write_f32(const float* data, size_t count) {
    char buffer[sizeof(float)];
    uint32_t unsigned_value;
    for (size_t i=0; i<count; i++) {
        std::memcpy(&unsigned_value, data + i, sizeof(float));
        u32_to_little_endian(unsigned_value, buffer);
        this->write_char(buffer, sizeof(float));
    }
}

void LittleEndianFile::write_f64(const double* data, size_t count) {
    char buffer[sizeof(double)];
    uint64_t unsigned_value;
    for (size_t i=0; i<count; i++) {
        std::memcpy(&unsigned_value, data + i, sizeof(double));
        u64_to_little_endian(unsigned_value, buffer);
        this->write_char(buffer, sizeof(double));
    }
}
