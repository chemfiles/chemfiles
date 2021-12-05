// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <cassert>

#include "chemfiles/File.hpp"
#include "chemfiles/files/Netcdf3File.hpp"

#include "chemfiles/error_fmt.hpp"
#include "chemfiles/unreachable.hpp"

using namespace chemfiles;
using namespace chemfiles::netcdf3;

Value& Value::operator=(Value&& other) {
    this->~Value();

    this->kind_ = other.kind_;
    if (kind_ == kind::BYTE) {
        this->byte_ = other.byte_;
    } else if (kind_ == kind::SHORT) {
        this->short_ = other.short_;
    } else if (kind_ == kind::INT) {
        this->int_ = other.int_;
    } else if (kind_ == kind::FLOAT) {
        this->float_ = other.float_;
    } else if (kind_ == kind::DOUBLE) {
        this->double_ = other.double_;
    } else if (kind_ == kind::STRING) {
        // use placement new for string, assigning to a string requires a
        // valid string, but `this->string_` might not be valid yet
        new(&string_) std::string(std::move(other.string_));
    } else {
        unreachable();
    }

    return *this;
}

Value& Value::operator=(const Value& other) {
    this->~Value();

    this->kind_ = other.kind_;
    if (kind_ == kind::BYTE) {
        this->byte_ = other.byte_;
    } else if (kind_ == kind::SHORT) {
        this->short_ = other.short_;
    } else if (kind_ == kind::INT) {
        this->int_ = other.int_;
    } else if (kind_ == kind::FLOAT) {
        this->float_ = other.float_;
    } else if (kind_ == kind::DOUBLE) {
        this->double_ = other.double_;
    } else if (kind_ == kind::STRING) {
        // use placement new for string, assigning to a string requires a
        // valid string, but `this->string_` might not be valid yet
        new(&string_) std::string(other.string_);
    } else {
        unreachable();
    }

    return *this;
}

size_t VariableLayout::size_for_type() const {
    if (this->type == constants::NC_BYTE || this->type == constants::NC_CHAR) {
        return sizeof(char);
    } else if (this->type == constants::NC_SHORT) {
        return sizeof(int16_t);
    } else if (this->type == constants::NC_INT) {
        return sizeof(int32_t);
    } else if (this->type == constants::NC_FLOAT) {
        return sizeof(float);
    } else if (this->type == constants::NC_DOUBLE) {
        return sizeof(double);
    } else {
        unreachable();
    }
}

size_t VariableLayout::count() const {
    return static_cast<uint64_t>(this->size) / this->size_for_type();
}

std::string VariableLayout::type_name() const {
    if (this->type == constants::NC_BYTE) {
        return "byte";
    } else if (this->type == constants::NC_CHAR) {
        return "char";
    } else if (this->type == constants::NC_SHORT) {
        return "short/i16";
    } else if (this->type == constants::NC_INT) {
        return "int/i32";
    } else if (this->type == constants::NC_FLOAT) {
        return "float/f32";
    } else if (this->type == constants::NC_DOUBLE) {
        return "double/f64";
    } else {
        unreachable();
    }
}

BaseVariable::BaseVariable(
    Netcdf3File& file,
    std::vector<std::shared_ptr<Dimension>> dimensions,
    std::map<std::string, Value> attributes,
    VariableLayout layout
):
    file_(file),
    dimensions_(std::move(dimensions)),
    attributes_(std::move(attributes)),
    layout_(std::move(layout))
{
    auto layout_size = layout_.size;
    // the size coming from the file (in layout) is not reliable since it can
    // only go up to 2^32 - 1. Fortunately, we can compute the information
    // we need from the dimension sizes.
    layout_.size = static_cast<int32_t>(layout_.size_for_type());
    for (const auto& dimension: dimensions_) {
        if (!dimension->is_record) {
            layout_.size *= dimension->size;
        }
    }

    // sanity check w.r.t. the size stored in the file when possible
    if (layout_.size < INT32_MAX) {
        auto padding = (4 - (layout_.size % 4)) % 4;
        assert(
            layout_.size + padding == layout_size
            && "declared size does not match dimension size for this variable"
        );
    }
}

std::vector<size_t> BaseVariable::shape() const {
    auto results = std::vector<size_t>();
    for (const auto& dimension: dimensions_) {
        results.emplace_back(dimension->size);
    }
    return results;
}

Variable::Variable(BaseVariable base): BaseVariable(std::move(base)) {
    bool check_is_record = false;
    for (const auto& dimension: dimensions_) {
        if (dimension->is_record) {
            check_is_record = true;
        }
    }
    assert(!check_is_record && "Variable refers to a record dimension");
}

RecordVariable::RecordVariable(BaseVariable base):
    BaseVariable(std::move(base)),
    // initialize record size to 0 here, it will be changed to the real value by
    // the reader before handing back control to the user
    record_size_(0),
    record_length_(0)
{
    bool check_is_record = false;
    for (const auto& dimension: dimensions_) {
        if (dimension->is_record) {
            check_is_record = true;
            record_length_ = static_cast<size_t>(dimension->size);
        }
    }

    assert(check_is_record && "RecordVariable does not refer to a record dimension");
}

#define VARIABLE_READ(_name_, _type_, _nc_type_)                               \
    void Variable::_name_(std::vector<_type_>& data) {                         \
        data.resize(layout_.count());                                          \
        this->_name_(data.data(), data.size());                                \
    }                                                                          \
                                                                               \
    void Variable::_name_(_type_* data, size_t count) {                        \
        if (layout_.type != _nc_type_) {                                       \
            throw file_error(                                                  \
                "internal error: the code called Variable::{}, but this "      \
                "variable contains {} values",                                 \
                #_name_, layout_.type_name()                                   \
            );                                                                 \
        }                                                                      \
                                                                               \
        if (count != layout_.count()) {                                        \
            throw file_error(                                                  \
                "wrong array size in Variable::{}: expected {}, got {}",       \
                #_name_, layout_.count(), count                                \
            );                                                                 \
        }                                                                      \
                                                                               \
        file_.get().seek(static_cast<uint64_t>(layout_.begin));                \
        file_.get()._name_(data, count);                                       \
    }

VARIABLE_READ(read_i16, int16_t, constants::NC_SHORT)
VARIABLE_READ(read_i32, int32_t, constants::NC_INT)
VARIABLE_READ(read_f32, float,   constants::NC_FLOAT)
VARIABLE_READ(read_f64, double,  constants::NC_DOUBLE)

/******************************************************************************/

#define RECORD_VARIABLE_READ(_name_, _type_, _nc_type_)                        \
    void RecordVariable::_name_(size_t step, std::vector<_type_>& data) {      \
        data.resize(layout_.count());                                          \
        this->_name_(step, data.data(), data.size());                          \
    }                                                                          \
                                                                               \
    void RecordVariable::_name_(size_t step, _type_* data, size_t count) {     \
        if (step >= record_length_) {                                          \
            throw file_error(                                                  \
                "out of bounds: trying to read record variable at step {}, "   \
                "but there are only {} steps in this file",                    \
                step, record_length_                                           \
            );                                                                 \
        }                                                                      \
                                                                               \
        if (layout_.type != _nc_type_) {                                       \
            throw file_error(                                                  \
                "internal error: the code called RecordVariable::{}, but this "\
                "variable contains {} values",                                 \
                #_name_, layout_.type_name()                                   \
            );                                                                 \
        }                                                                      \
                                                                               \
        if (count != layout_.count()) {                                        \
            throw file_error(                                                  \
                "wrong array size in RecordVariable::{}: expected {}, got {}", \
                #_name_, layout_.count(), count                                \
            );                                                                 \
        }                                                                      \
                                                                               \
        auto begin = static_cast<uint64_t>(layout_.begin);                     \
        begin += static_cast<uint64_t>(step) * record_size_;                   \
        file_.get().seek(begin);                                               \
        file_.get()._name_(data, count);                                       \
    }

RECORD_VARIABLE_READ(read_i16, int16_t, constants::NC_SHORT)
RECORD_VARIABLE_READ(read_i32, int32_t, constants::NC_INT)
RECORD_VARIABLE_READ(read_f32, float,   constants::NC_FLOAT)
RECORD_VARIABLE_READ(read_f64, double,  constants::NC_DOUBLE)

/******************************************************************************/

Netcdf3File::Netcdf3File(std::string filename, File::Mode mode):
    BigEndianFile(std::move(filename), mode),
    dimensions_(),
    attributes_(),
    variables_()
{
    if (mode != 'r') {
        throw file_error("only read mode is implemented");
    }

    this->seek(0);
    auto buffer = std::string(3, '\0');
    this->read_char(&buffer[0], 3);
    if (buffer != "CDF") {
        throw file_error("the file at '{}' is not a valid NetCDF 3 file", this->path());
    }

    char version = this->read_single_char();
    if (version != 2) {
        throw file_error("only 64-bit netcdf3 files are supported");
    }

    record_length_ = this->read_single_i32();

    // read dimensions
    auto header = this->read_single_i32();
    if (header != constants::NC_DIMENSION && header != 0) {
        throw file_error(
            "invalid netcdf3 file: expected a NC_DIMENSION header, got {}",
            header
        );
    }
    auto count = this->read_single_i32();

    for (int i=0; i<count; i++) {
        auto name = this->read_pascal_string();
        auto length = this->read_single_i32();
        auto is_record = length == 0;
        if (is_record) {
            length = record_length_;
        }
        dimensions_.push_back(std::make_shared<Dimension>(
            std::move(name),
            length,
            is_record
        ));
    }

    attributes_ = this->read_attributes();

    auto all_variables = this->read_variable_list();
    variables_ = std::move(all_variables.globals);
    record_variables_ = std::move(all_variables.records);
}

void Netcdf3File::skip_padding(size_t count) {
    auto padding = (4 - (count % 4)) % 4;
    for (size_t i=0; i<padding; i++){
        this->read_single_char();
    }
}

std::string Netcdf3File::read_pascal_string() {
    auto size = static_cast<size_t>(this->read_single_i32());
    auto value = std::string(size, '\0');
    this->read_char(&value[0], size);
    this->skip_padding(size);
    return value;
}

std::map<std::string, Value> Netcdf3File::read_attributes() {
    auto header = this->read_single_i32();
    if (header != constants::NC_ATTRIBUTE && header != 0) {
        throw file_error(
            "invalid netcdf3 file: expected a NC_ATTRIBUTE header, got {}",
            header
        );
    }
    auto count = this->read_single_i32();

    auto attributes = std::map<std::string, Value>();
    for (int i=0; i<count; i++) {
        auto name = this->read_pascal_string();
        auto value = this->read_attribute_value();
        attributes.emplace(std::move(name), std::move(value));
    }

    return attributes;
}

Value Netcdf3File::read_attribute_value() {
    auto type = this->read_single_i32();
    auto count = static_cast<size_t>(this->read_single_i32());

    if (count != 1 && type != constants::NC_CHAR) {
        throw file_error("not implemented: attributes with more than one value");
    }

    size_t size = 0;
    auto value = Value(0);
    if (type == constants::NC_BYTE) {
        size = sizeof(char);
        value = Value(this->read_single_char());
    } else if (type == constants::NC_CHAR) {
        size = sizeof(char);
        auto str = std::string(static_cast<size_t>(count), '\0');
        this->read_char(&str[0], count);
        // should we remove trailing NULL if there is any?
        value = Value(std::move(str));
    } else if (type == constants::NC_SHORT) {
        size = sizeof(int16_t);
        value = Value(this->read_single_i16());
    } else if (type == constants::NC_INT) {
        size = sizeof(int32_t);
        value = Value(this->read_single_i32());
    } else if (type == constants::NC_FLOAT) {
        size = sizeof(float);
        value = Value(this->read_single_f32());
    } else if (type == constants::NC_DOUBLE) {
        size = sizeof(double);
        value = Value(this->read_single_f64());
    } else {
        throw file_error(
            "invalid netcdf3 file: attribute data type {} is unknown",
            type
        );
    }

    this->skip_padding(count * size);

    return value;
}

Netcdf3File::variable_list Netcdf3File::read_variable_list() {
    auto header = this->read_single_i32();
    if (header != constants::NC_VARIABLE && header != 0) {
        throw file_error(
            "invalid netcdf3 file: expected a NC_VARIABLE header, got {}",
            header
        );
    }
    auto count = this->read_single_i32();

    auto variables = std::map<std::string, Variable>();
    auto record_variables = std::map<std::string, RecordVariable>();
    for (int var_i=0; var_i<count; var_i++) {
        auto name = this->read_pascal_string();

        auto n_dims = this->read_single_i32();
        auto dimensions = std::vector<std::shared_ptr<Dimension>>();
        auto is_record_variable = false;
        for (int i=0; i<n_dims; i++) {
            auto id = static_cast<size_t>(this->read_single_i32());

            auto dimension = this->dimensions_[id];
            dimensions.emplace_back(dimension);
            if (dimension->is_record) {
                is_record_variable = true;
            }
        }

        auto attributes = this->read_attributes();

        auto type = this->read_single_i32();
        auto size = this->read_single_i32();
        // this is where the 64-bit offset changes something to the format
        auto begin = this->read_single_i64();

        auto base = BaseVariable(
            *this,
            std::move(dimensions),
            std::move(attributes),
            VariableLayout { type, size, begin }
        );

        if (is_record_variable) {
            record_variables.emplace(
                std::move(name), RecordVariable(std::move(base))
            );
        } else {
            variables.emplace(std::move(name), Variable(std::move(base)));
        }
    }

    // now that we know all the variables, compute the size of a single record
    // entry, which includes all record variables
    size_t record_size = 0;
    for (const auto& it: record_variables) {
        const auto& variable = it.second;
        auto padding = (4 - (variable.layout_.size % 4)) % 4;
        record_size += static_cast<size_t>(variable.layout_.size) + static_cast<size_t>(padding);
    }

    for (auto& variable: record_variables) {
        variable.second.record_size_ = record_size;
    }

    return {record_variables, variables};
}

const Value& Netcdf3File::attribute(const std::string& name) const {
    auto it = this->attributes_.find(name);
    if (it == this->attributes_.end()) {
        throw file_error("could not find the '{}' attribute in this file", name);
    }
    return it->second;
}

Variable& Netcdf3File::variable(const std::string& name) {
    auto it = this->variables_.find(name);
    if (it == this->variables_.end()) {
        throw file_error("could not find the '{}' variable in this file", name);
    }
    return it->second;
}

RecordVariable& Netcdf3File::record_variable(const std::string& name) {
    auto it = this->record_variables_.find(name);
    if (it == this->record_variables_.end()) {
        throw file_error("could not find the '{}' record variable in this file", name);
    }
    return it->second;
}
