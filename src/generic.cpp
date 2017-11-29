// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <iostream>

#include "chemfiles/warnings.hpp"
#include "chemfiles/Configuration.hpp"
#include "chemfiles/generic.hpp"
#include "chemfiles/mutex.hpp"

using namespace chemfiles;

static mutex<warning_callback> CALLBACK = {[](const std::string& message){ // NOLINT
    // NOLINT: we don't reference cerr before it is initialized because we are in a lambda
    std::cerr << "[chemfiles] " << message << std::endl;
}};

void chemfiles::set_warning_callback(warning_callback callback) {
    auto guard = CALLBACK.lock();
    *guard = std::move(callback);
}

void chemfiles::warning(const std::string& message) {
    auto callback = CALLBACK.lock();
    (*callback)(message);
}

void chemfiles::add_configuration(const std::string& path) {
    Configuration::add(path);
}
