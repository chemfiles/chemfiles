// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#include <toml/get.hpp>
#include <toml/types.hpp>
#include <toml/value.hpp>
#include <toml/parser.hpp>
#include <toml/exception.hpp>

#include "chemfiles/Configuration.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/misc.hpp"
#include "chemfiles/mutex.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/periodic_table.hpp"

#include "chemfiles/external/optional.hpp"

using namespace chemfiles;

// Get the list of directories up to `leaf`. For example, if `leaf` is
// `C:\foo\bar\baz\`, this function returns `{C:\, C:\foo\, C:\foo\bar\,
// C:\foo\bar\baz\}`.
static std::vector<std::string> list_directories(const std::string& leaf);

Configuration& Configuration::instance() {
    static Configuration instance_;
    return instance_;
}

Configuration::Configuration() {
    auto directories = list_directories(current_directory());
    for (auto& dir: directories) {
        auto path = dir + "/" + ".chemfilesrc";
        if (std::ifstream(path)) {
            warning("", "found deprecated configuration file at '{}', please rename it to .chemfiles.toml", path);
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
                    "invalid configuration file at '{}': type for '{}' must be a string",
                    path, old_name
                );
            }
            auto new_name = toml::get<std::string>(entry.second);
            (*types)[std::move(old_name)] = std::move(new_name);
        }
    }
}

optional<std::string> Configuration::atomic_data_string(
    const std::string& path,
    const toml::Table& table,
    const std::string& property,
    const std::string& atomic_type
) {
    auto it = table.find(property);
    if (it == table.end()) {
        return nullopt;
    } else {
        if (it->second.type() != toml::value_t::String) {
            throw configuration_error(
                "invalid configuration file at '{}': {} for '{}' must be a string",
                path, property, atomic_type
            );
        }
        return toml::get<std::string>(it->second);
    }
}

optional<double> Configuration::atomic_data_number(
    const std::string& path,
    const toml::Table& table,
    const std::string& property,
    const std::string& atomic_type
) {
    auto it = table.find(property);
    if (it == table.end()) {
        return nullopt;
    } else {
        if (it->second.type() == toml::value_t::Float) {
            return toml::get<double>(it->second);
        } else if (it->second.type() == toml::value_t::Integer) {
            return static_cast<double>(toml::get<long long>(it->second));
        } else {
            throw configuration_error(
                "invalid configuration file at '{}': {} for '{}' must be a number",
                path, property, atomic_type
            );
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
                    "invalid configuration file at '{}': atomic data for '{}' must be a table",
                    path, type
                );
            }
            auto table = toml::get<toml::Table>(entry.second);

            auto full_name = atomic_data_string(path, table, "full_name", type);
            auto mass = atomic_data_number(path, table, "mass", type);
            auto charge = atomic_data_number(path, table, "charge", type);
            auto vdw_radius = atomic_data_number(path, table, "vdw_radius", type);
            auto covalent_radius = atomic_data_number(path, table, "covalent_radius", type);

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
        throw configuration_error("can not open configuration file at '{}'", path);
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

void chemfiles::add_configuration(const std::string& path) {
    Configuration::add(path);
}
