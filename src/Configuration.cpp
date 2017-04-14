// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "chemfiles/Configuration.hpp"
#include "chemfiles/Error.hpp"
#include "cpptoml/cpptoml.h"
#include <fstream>
#include <unordered_map>
using namespace chemfiles;

const char PATH[] = "~/.chemfilesrc";

Configuration Configuration::get() {
    Configuration configuration;
    if (std::ifstream(PATH)) {
        try {
            auto toml = cpptoml::parse_file(PATH);

            for (auto& entry: *toml) {
                auto name = entry.first;
                auto type = entry.second->as<std::string>()->get();
                configuration.rename_.insert({name, type});
            }
        } catch (const cpptoml::parse_exception& e) {
            throw ConfigurationError("Failed to parse " + std::string(PATH) + ": " + e.what());
        }
    }
    return configuration;
}

bool Configuration::find(std::string name) {
    auto search = rename_.find(name);
    return (search != rename_.end());
}
