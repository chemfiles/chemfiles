// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

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
