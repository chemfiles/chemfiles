// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "chemfiles/files/Netcdf3File.hpp"
#include "chemfiles/utils.hpp"
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
        CHECK(file.attribute("Conventions")->as_string() == "AMBER");

        // standard dimmension
        CHECK(file.dimensions().size() == 6);
        auto dimension = get_dimension(file, "atom");
        CHECK(dimension->size == 297);
        CHECK(dimension->is_record() == false);
        // record dimension
        dimension = get_dimension(file, "frame");
        CHECK(dimension->size == 0);
        CHECK(dimension->is_record() == true);

        CHECK(file.variables().size() == 9);
        // standard variable
        auto atom_types = file.variable("atom_types").value();
        CHECK(!atom_types.is_record());
        CHECK(atom_types.shape() == std::vector<size_t>{297});
        CHECK(atom_types.type() == netcdf3::constants::NC_INT);

        // record variable
        auto cell_lengths = file.variable("cell_lengths").value();
        CHECK(cell_lengths.is_record());
        CHECK(cell_lengths.attribute("units")->as_string() == "Angstrom");

        auto coordinates = file.variable("coordinates").value();
        CHECK(coordinates.is_record());
        CHECK(coordinates.shape() == std::vector<size_t>{100, 297, 3});
        CHECK(coordinates.type() == netcdf3::constants::NC_FLOAT);

        auto positions = std::vector<float>(297 * 3, 0.0);
        coordinates.read(0, positions);
        CHECK(std::abs(positions[0] - 0.4172191f) < 1e-5f);
        CHECK(std::abs(positions[1] - 8.303366f) < 1e-5f);
        CHECK(std::abs(positions[2] - 11.73717f) < 1e-5f);
    }

    SECTION("Double variables") {
        netcdf3::Netcdf3File file("data/netcdf/water.ncrst", File::READ);

        CHECK(file.attributes().size() == 6);
        CHECK(file.attribute("Conventions")->as_string() == "AMBERRESTART");

        // standard dimmension
        CHECK(file.dimensions().size() == 5);
        auto dimension = get_dimension(file, "atom");
        CHECK(dimension->size == 297);
        CHECK(dimension->is_record() == false);

        // standard variable
        CHECK(file.variables().size() == 7);
        auto cell_lengths = file.variable("cell_lengths").value();
        CHECK(cell_lengths.attribute("units")->as_string() == "angstrom");

        auto coordinates = file.variable("coordinates").value();
        CHECK(coordinates.shape() == std::vector<size_t>{297, 3});
        CHECK(coordinates.type() == netcdf3::constants::NC_DOUBLE);

        auto positions = std::vector<double>(297 * 3, 0.0);
        coordinates.read(0, positions);
        CHECK(std::abs(positions[0] - 0.4172191) < 1e-5);
        CHECK(std::abs(positions[1] - 8.303366) < 1e-5);
        CHECK(std::abs(positions[2] - 11.73717) < 1e-5);
    }

    // TODO error: wrong type/size/step when reading
}

static netcdf3::Netcdf3Builder file_builder() {
    netcdf3::Netcdf3Builder builder;

    builder.add_attribute("global", netcdf3::Value("global.value"));
    auto infinite = builder.add_dimension("infinite", 0);
    auto finite = builder.add_dimension("finite", 42);

    auto A = netcdf3::VariableDefinition {
        netcdf3::constants::NC_FLOAT,
        {infinite, finite},
        {},
    };
    A.attributes.emplace("attribute", netcdf3::Value("hello"));
    builder.add_variable("A", std::move(A));

    auto B = netcdf3::VariableDefinition {
        netcdf3::constants::NC_DOUBLE,
        {finite, finite},
        {},
    };
    B.attributes.emplace("attribute", netcdf3::Value("world"));
    builder.add_variable("B", std::move(B));

    return builder;
}

TEST_CASE("Write NetCDF files") {
    SECTION("writing") {
        auto tmpfile = NamedTempPath(".nc");
        {
            netcdf3::Netcdf3File file(tmpfile, File::WRITE);
            file_builder().initialize(&file);

            file.variable("B").value().write(0, std::vector<double>(42 * 42, 37.4));
            file.add_record();
            file.variable("A").value().write(0, std::vector<float>(42, 38.2f));
        }

#ifndef __EMSCRIPTEN__
        // check the exact content of the file with a MD5 sum
        // this was checked against the official netcdf-c library
        auto md5sum = run_process(
            fmt::format("\"{}\" -E md5sum {}", CMAKE_COMMAND, tmpfile.path())
        );
        CHECK(chemfiles::split(md5sum, ' ')[0] == "b51f5c399e08d1ea2bba9bb468a0dd8b");
#endif

        // re-read the file & check everything
        netcdf3::Netcdf3File file(tmpfile, File::READ);
        CHECK(file.attributes().size() == 1);
        CHECK(file.attribute("global")->as_string() == "global.value");

        const auto& dimensions = file.dimensions();
        CHECK(dimensions.size() == 2);
        CHECK(dimensions[0]->name == "infinite");
        CHECK(dimensions[0]->is_record());

        CHECK(dimensions[1]->name == "finite");
        CHECK(dimensions[1]->size == 42);

        CHECK(file.variables().size() == 2);
        auto A = file.variable("A").value();
        CHECK(A.type() == netcdf3::constants::NC_FLOAT);
        CHECK(A.attributes().size() == 1);
        CHECK(A.attribute("attribute")->as_string() == "hello");
        CHECK(A.shape() == std::vector<size_t>{1, 42});

        auto float_data = std::vector<float>(42);
        A.read(0, float_data);
        CHECK(float_data == std::vector<float>(42, 38.2f));

        auto B = file.variable("B").value();
        CHECK(B.type() == netcdf3::constants::NC_DOUBLE);
        CHECK(B.attributes().size() == 1);
        CHECK(B.attribute("attribute")->as_string() == "world");
        CHECK(B.shape() == std::vector<size_t>{42, 42});

        auto double_data = std::vector<double>(42 * 42);
        B.read(0, double_data);
        CHECK(double_data == std::vector<double>(42 * 42, 37.4));
    }

    SECTION("fill values") {
        auto tmpfile = NamedTempPath(".nc");
        {
            netcdf3::Netcdf3File file(tmpfile, File::WRITE);
            file_builder().initialize(&file);

            file.add_record();
        }

        netcdf3::Netcdf3File file(tmpfile, File::READ);
        auto A = file.variable("A").value();
        auto float_data = std::vector<float>(42);
        A.read(0, float_data);
        CHECK(float_data == std::vector<float>(42, netcdf3::constants::NC_FILL_FLOAT));

        auto B = file.variable("B").value();
        auto double_data = std::vector<double>(42 * 42);
        B.read(0, double_data);
        CHECK(double_data == std::vector<double>(42 * 42, netcdf3::constants::NC_FILL_DOUBLE));
    }

    SECTION("append to existing file") {
        auto tmpfile = NamedTempPath(".nc");
        {
            netcdf3::Netcdf3File file(tmpfile, File::WRITE);
            file_builder().initialize(&file);
            file.variable("B").value().write(0, std::vector<double>(42 * 42, 37.4));

            file.add_record();
            file.variable("A").value().write(0, std::vector<float>(42, 38.2f));
        }

        {
            netcdf3::Netcdf3File file(tmpfile, File::APPEND);
            file.add_record();
            file.variable("A").value().write(1, std::vector<float>(42, 56.8f));
        }

        netcdf3::Netcdf3File file(tmpfile, File::READ);
        auto A = file.variable("A").value();
        auto float_data = std::vector<float>(42);
        A.read(0, float_data);
        CHECK(float_data == std::vector<float>(42, 38.2f));

        A.read(1, float_data);
        CHECK(float_data == std::vector<float>(42, 56.8f));

        auto B = file.variable("B").value();
        auto double_data = std::vector<double>(42 * 42);
        B.read(0, double_data);
        CHECK(double_data == std::vector<double>(42 * 42, 37.4));
    }

    SECTION("append to new file") {
        auto tmpfile = NamedTempPath(".nc");
        {
            netcdf3::Netcdf3File file(tmpfile, File::APPEND);
            file_builder().initialize(&file);
            file.variable("B").value().write(0, std::vector<double>(42 * 42, 37.4));

            file.add_record();
            file.variable("A").value().write(0, std::vector<float>(42, 38.2f));
        }

        netcdf3::Netcdf3File file(tmpfile, File::READ);
        auto A = file.variable("A").value();
        auto float_data = std::vector<float>(42);
        A.read(0, float_data);
        CHECK(float_data == std::vector<float>(42, 38.2f));

        auto B = file.variable("B").value();
        auto double_data = std::vector<double>(42 * 42);
        B.read(0, double_data);
        CHECK(double_data == std::vector<double>(42 * 42, 37.4));
    }
}
