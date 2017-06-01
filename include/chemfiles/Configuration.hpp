// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CONFIGURATION_HPP
#define CHEMFILES_CONFIGURATION_HPP

#include <string>
#include <unordered_map>
#include "chemfiles/exports.hpp"

namespace chemfiles {

class CHFL_EXPORT Configuration {
public:
    Configuration() = default;
    Configuration& operator=(const Configuration&) = delete;
    Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = default;
    Configuration& operator=(Configuration&&) = default;

    /// Reads the configuration file
    static Configuration get();

    /// Returns true is name is in the map
    bool find(std::string name);

    /// Returns the type
    const std::string type(std::string name) { return rename_[name]; }

private:
    std::unordered_map<std::string, std::string> rename_;
};

} // namespace chemfiles

#endif
