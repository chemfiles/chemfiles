// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include <toml.hpp>

#include "chemfiles/Configuration.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/utils.hpp"
using namespace chemfiles;

/// Get the list of directories up to `leaf`. For example, if `leaf` is
/// `C:\foo\bar\baz\`, this function returns `{C:\, C:\foo\, C:\foo\bar\,
/// C:\foo\bar\baz\}`.
static std::vector<std::string> list_directories(std::string leaf);

const Configuration& Configuration::instance() {
    static Configuration instance_;
    return instance_;
};

Configuration::Configuration() {
    auto directories = list_directories(current_directory());
    for (auto& dir: directories) {
        auto path = dir + "/" + ".chemfilesrc";
        if (std::ifstream(path)) {
            read_configuration(path);
        }
    }
}

void Configuration::read_configuration(std::string path) {
    toml::Table data;
    try {
        data = toml::parse(path);
    } catch (const toml::exception& e) {
        throw ConfigurationError(
            "configuration file is invalid TOML: " + std::string(e.what())
        );
    }

    if (data.find("rename") != data.end() && data.at("rename").type() == toml::value_t::Table) {
        auto rename = toml::get<toml::Table>(data.at("rename"));
        for (auto& entry: rename) {
            auto old_name = entry.first;
            if (entry.second.type() != toml::value_t::String) {
                throw ConfigurationError(
                    "renaming data for " + old_name + " must be a string"
                );
            }
            auto new_name = toml::get<std::string>(entry.second);
            types_rename_[std::move(old_name)] = std::move(new_name);
        }
    }
}

std::vector<std::string> list_directories(std::string leaf) {
    std::vector<std::string> directories;
    auto slash = leaf.find_first_of("\\/");
    while (slash != std::string::npos) {
        directories.push_back(leaf.substr(0, slash + 1));
        slash = leaf.find_first_of("\\/", slash + 1);
    }
    directories.push_back(leaf);
    return directories;
}
