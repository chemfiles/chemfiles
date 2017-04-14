// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CONFIGURATION_HPP
#define CHEMFILES_CONFIGURATION_HPP

#include <memory>
#include <string>

#include "chemfiles/exports.hpp"

namespace chemfiles {

class CHFL_EXPORT Configuration {
public:
    std::unordered_map configuration;

    Configuration() = default;
    Configuration& operator=(const Configuration&) = default;
    Configuration(const Configuration&) = default;

    /// Checks if there is a configuration file
    static bool has_configuration();

    /// Reads the configuration file
    Configuration read_configuration();

    /// Returns the type
    const std::string type(std::string name) { return configuration[name]; }
};

} // namespace chemfiles

#endif
