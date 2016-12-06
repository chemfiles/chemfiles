// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <iostream>
#include "chemfiles/warnings.hpp"
#include "chemfiles/Error.hpp"

static chemfiles::warning_callback CALLBACK = [](std::string message){
    std::cerr << "[chemfiles] " << message << std::endl;
};

void chemfiles::set_warning_callback(warning_callback callback) {
    CALLBACK = callback;
}

void chemfiles::warning(std::string message) {
    CALLBACK(std::move(message));
}
