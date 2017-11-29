// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include <toml.hpp>

#include "chemfiles/Configuration.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/generic.hpp"
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
            read(path);
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
