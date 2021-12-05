// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "chemfiles/files/Netcdf3File.hpp"
#include "helpers.hpp"
using namespace chemfiles;

static std::shared_ptr<netcdf3::Dimension> get_dimension(const netcdf3::Netcdf3File& file, const std::string& name) {
    for (const auto& dimension: file.dimensions()) {
        if (dimension->name == name) {
            return dimension;
        }
    }
    throw std::runtime_error("could not find a dimension named " + name);
}

TEST_CASE("Read a NetCDF file") {
    SECTION("Float variables") {
        netcdf3::Netcdf3File file("data/netcdf/water.nc", File::READ);

        CHECK(file.attributes().size() == 6);
        CHECK(file.attribute("Conventions").as_string().value() == "AMBER");

        // standard dimmension
        CHECK(file.dimensions().size() == 6);
        auto dimension = get_dimension(file, "atom");
        CHECK(dimension->size == 297);
        CHECK(dimension->is_record == false);
        // record dimension
        dimension = get_dimension(file, "frame");
        CHECK(dimension->size == 100);
        CHECK(dimension->is_record == true);

        // standard variable
        CHECK(file.variables().size() == 4);
        auto atom_types = file.variable("atom_types");
        CHECK(atom_types.shape() == std::vector<size_t>{297});
        CHECK(atom_types.type() == netcdf3::constants::NC_INT);

        // record variable
        CHECK(file.record_variables().size() == 5);
        auto cell_lengths = file.record_variable("cell_lengths");
        CHECK(cell_lengths.attribute("units").as_string().value() == "Angstrom");

        auto coordinates = file.record_variable("coordinates");
        CHECK(coordinates.shape() == std::vector<size_t>{100, 297, 3});
        CHECK(coordinates.type() == netcdf3::constants::NC_FLOAT);

        auto positions = std::vector<float>(297 * 3, 0.0);
        coordinates.read_f32(0, positions);
        CHECK(std::abs(positions[0] - 0.4172191f) < 1e-5f);
        CHECK(std::abs(positions[1] - 8.303366f) < 1e-5f);
        CHECK(std::abs(positions[2] - 11.73717f) < 1e-5f);
    }

    SECTION("Double variables") {
        netcdf3::Netcdf3File file("data/netcdf/water.ncrst", File::READ);

        CHECK(file.attributes().size() == 6);
        CHECK(file.attribute("Conventions").as_string().value() == "AMBERRESTART");

        // standard dimmension
        CHECK(file.dimensions().size() == 5);
        auto dimension = get_dimension(file, "atom");
        CHECK(dimension->size == 297);
        CHECK(dimension->is_record == false);

        // standard variable
        CHECK(file.variables().size() == 7);
        CHECK(file.record_variables().size() == 0);
        auto cell_lengths = file.variable("cell_lengths");
        CHECK(cell_lengths.attribute("units").as_string().value() == "angstrom");

        auto coordinates = file.variable("coordinates");
        CHECK(coordinates.shape() == std::vector<size_t>{297, 3});
        CHECK(coordinates.type() == netcdf3::constants::NC_DOUBLE);

        auto positions = std::vector<double>(297 * 3, 0.0);
        coordinates.read_f64(positions);
        CHECK(std::abs(positions[0] - 0.4172191) < 1e-5);
        CHECK(std::abs(positions[1] - 8.303366) < 1e-5);
        CHECK(std::abs(positions[2] - 11.73717) < 1e-5);
    }
}

// TEST_CASE("Write NetCDF files") {
//     auto tmpfile = NamedTempPath(".nc");

//     {
//         NcFile file(tmpfile, File::WRITE);
//         file.set_nc_mode(NcFile::DEFINE);
//         file.add_global_attribute("global", "global.value");
//         file.add_dimension("infinite");
//         file.add_dimension("finite", 42);

//         auto variable = file.add_variable<nc::NcFloat>("variable", "infinite", "finite");
//         variable.add_string_attribute("attribute", "hello");

//         auto variable_d = file.add_variable<nc::NcDouble>("variable_d", "infinite", "finite");
//         variable_d.add_string_attribute("attribute", "world");

//         file.set_nc_mode(NcFile::DATA);
//         variable.add({0, 0}, {1, 42}, std::vector<float>(42, 38.2f));
//         variable_d.add({0, 0}, {1, 42}, std::vector<double>(42, 37.4));
//     }

//     {
//         NcFile file(tmpfile, File::APPEND);
//         auto variable = file.variable<nc::NcFloat>("variable");
//         variable.add({1, 0}, {1, 42}, std::vector<float>(42, 55.1f));
//         auto variable_d = file.variable<nc::NcDouble>("variable_d");
//         variable_d.add({1, 0}, {1, 42}, std::vector<double>(42, 66.3));
//     }

//     {
//         NcFile file(tmpfile, File::READ);
//         CHECK(file.global_attribute("global") == "global.value");
//         CHECK(file.dimension("infinite") == 2);
//         CHECK(file.dimension("finite") == 42);
//         CHECK(file.variable_exists("variable"));
//         CHECK(file.variable_exists("variable_d"));
//         CHECK_FALSE(file.variable_exists("bar"));

//         auto variable = file.variable<nc::NcFloat>("variable");
//         CHECK(variable.string_attribute("attribute") == "hello");
//         CHECK(variable.get({0, 0}, {1, 42}) == std::vector<float>(42, 38.2f));
//         CHECK(variable.get({1, 0}, {1, 42}) == std::vector<float>(42, 55.1f));

//         auto variable_d = file.variable<nc::NcDouble>("variable_d");
//         CHECK(variable_d.string_attribute("attribute") == "world");
//         CHECK(variable_d.get({0, 0}, {1, 42}) == std::vector<double>(42, 37.4));
//         CHECK(variable_d.get({1, 0}, {1, 42}) == std::vector<double>(42, 66.3));
//     }
// }
