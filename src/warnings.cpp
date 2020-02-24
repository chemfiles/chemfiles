// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <string>
#include <iostream>
#include <exception>
#include <functional>

#include "chemfiles/misc.hpp"
#include "chemfiles/mutex.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

static mutex<warning_callback_t> CALLBACK = {[](const std::string& message){ // NOLINT
    // NOLINT: we don't use cerr before it is initialized because we are in a lambda
    std::cerr << "[chemfiles] " << message << std::endl;
}};

void chemfiles::set_warning_callback(warning_callback_t callback) {
    auto guard = CALLBACK.lock();
    *guard = std::move(callback);
}

void chemfiles::send_warning(const std::string& message) noexcept {
    try {
        auto callback = CALLBACK.lock();
        (*callback)(message);
    } catch (const std::exception& e) {
        std::cerr << "exception while sending warning: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception while sending warning" << std::endl;
    }
}
