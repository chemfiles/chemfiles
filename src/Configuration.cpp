// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include <toml.hpp>

#include "chemfiles/Configuration.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/misc.hpp"
using namespace chemfiles;

// Get the list of directories up to `leaf`. For example, if `leaf` is
// `C:\foo\bar\baz\`, this function returns `{C:\, C:\foo\, C:\foo\bar\,
// C:\foo\bar\baz\}`.
static std::vector<std::string> list_directories(const std::string& leaf);

// Check if a toml value is a number (integer or float)
static bool is_toml_number(const toml::value& value);
// Extract a number from a number (integer or float)
static double toml_get_number(const toml::value& value);

Configuration& Configuration::instance() {
    static Configuration instance_;
    return instance_;
}

Configuration::Configuration() {
    auto directories = list_directories(current_directory());
    for (auto& dir: directories) {
        auto path = dir + "/" + ".chemfilesrc";
        if (std::ifstream(path)) {
            warning("found deprecated configuration file at '{}'. Please rename it to .chemfiles.toml", path);
        }
        path = dir + "/" + ".chemfiles.toml";
        if (std::ifstream(path)) {
            read(path);
            continue;
        }
        path = dir + "/" + "chemfiles.toml";
        if (std::ifstream(path)) {
            read(path);
            continue;
        }
    }
}

void Configuration::read(const std::string& path) {
    toml::Table data;
    try {
        data = toml::parse(path);
    } catch (const toml::exception& e) {
        throw configuration_error(
            "configuration file at '{}' is invalid TOML: {}", path, e.what()
        );
    }

    read_types(path, data);
    read_atomic_data(path, data);
}

void Configuration::read_types(const std::string& path, const toml::Table& data) {
    auto types = types_.lock();
    if (data.find("types") != data.end() && data.at("types").type() == toml::value_t::Table) {
        auto rename = toml::get<toml::Table>(data.at("types"));
        for (auto& entry: rename) {
            auto old_name = entry.first;
            if (entry.second.type() != toml::value_t::String) {
                throw configuration_error(
                    "invalid configuration file at '{}': type for {} must be a string",
                    path, old_name
                );
            }
            auto new_name = toml::get<std::string>(entry.second);
            (*types)[std::move(old_name)] = std::move(new_name);
        }
    }
}

void Configuration::read_atomic_data(const std::string& path, const toml::Table& data) {
    auto atoms = atoms_.lock();
    if (data.find("atoms") != data.end() && data.at("atoms").type() == toml::value_t::Table) {
        auto elements = toml::get<toml::Table>(data.at("atoms"));
        for (auto& entry: elements) {
            auto type = entry.first;
            if (entry.second.type() != toml::value_t::Table) {
                throw configuration_error(
                    "invalid configuration file at '{}': atomic data for {} must be a table",
                    path, type
                );
            }
            auto table = toml::get<toml::Table>(entry.second);

            optional<std::string> full_name = nullopt;
            if (table.find("full_name") != table.end()) {
                if (table.at("full_name").type() != toml::value_t::String) {
                    throw configuration_error(
                        "invalid configuration file at '{}': full name for {} must be a string",
                        path, type
                    );
                }
                full_name = toml::get<std::string>(table.at("full_name"));
            }

            optional<double> mass = nullopt;
            if (table.find("mass") != table.end()) {
                if (!is_toml_number(table.at("mass"))) {
                    throw configuration_error(
                        "invalid configuration file at '{}': mass for {} must be a number",
                        path, type
                    );
                }
                mass = toml_get_number(table.at("mass"));
            }

            optional<double> charge = nullopt;
            if (table.find("charge") != table.end()) {
                if (!is_toml_number(table.at("charge"))) {
                    throw configuration_error(
                        "invalid configuration file at '{}': charge for {} must be a number",
                        path, type
                    );
                }
                charge = toml_get_number(table.at("charge"));
            }

            optional<double> vdw_radius = nullopt;
            if (table.find("vdw_radius") != table.end()) {
                if (!is_toml_number(table.at("vdw_radius"))) {
                    throw configuration_error(
                        "invalid configuration file at '{}': vdw_radius for {} must be a number",
                        path, type
                    );
                }
                vdw_radius = toml_get_number(table.at("vdw_radius"));
            }

            optional<double> covalent_radius = nullopt;
            if (table.find("covalent_radius") != table.end()) {
                if (!is_toml_number(table.at("covalent_radius"))) {
                    throw configuration_error(
                        "invalid configuration file at '{}': covalent_radius for {} must be a number",
                        path, type
                    );
                }
                covalent_radius = toml_get_number(table.at("covalent_radius"));
            }

            // If the type is an element, use the default from the periodic
            // table.
            auto element = find_in_periodic_table(type);
            optional<uint64_t> number = nullopt;
            if (element) {
                number = element->number;
                if (!full_name) {
                    full_name = element->full_name;
                }
                if (!mass) {
                    mass = element->mass;
                }
                if (!charge) {
                    charge = element->charge;
                }
                if (!covalent_radius) {
                    covalent_radius = element->covalent_radius;
                }
                if (!vdw_radius) {
                    vdw_radius = element->vdw_radius;
                }
            }

            (*atoms)[std::move(type)] = AtomicData {
                std::move(number),
                std::move(full_name),
                std::move(mass),
                std::move(charge),
                std::move(covalent_radius),
                std::move(vdw_radius),
            };
        }
    }
}

void Configuration::add(const std::string& path) {
    if (std::ifstream(path)) {
        instance().read(path);
    } else {
        throw configuration_error("Can not open configuration file at {}", path);
    }
}

std::vector<std::string> list_directories(const std::string& leaf) {
    std::vector<std::string> directories;
    auto slash = leaf.find_first_of("\\/");
    while (slash != std::string::npos) {
        directories.push_back(leaf.substr(0, slash + 1));
        slash = leaf.find_first_of("\\/", slash + 1);
    }
    directories.push_back(leaf);
    return directories;
}

bool is_toml_number(const toml::value& value) {
    return value.type() == toml::value_t::Float || value.type() == toml::value_t::Integer;
}

double toml_get_number(const toml::value& value) {
    assert(is_toml_number(value));
    if (value.type() == toml::value_t::Float) {
        return toml::get<double>(value);
    } else if (value.type() == toml::value_t::Integer) {
        return static_cast<double>(toml::get<long long>(value));
    }
    return 0;
}
