// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Atom.hpp"
#include "chemfiles/periodic.hpp"

using namespace chemfiles;

static optional<const ElementData&> find_element(const std::string& name) {
    auto periodic = PERIODIC_INFORMATION.find(name);
    if (periodic != PERIODIC_INFORMATION.end()) {
        return periodic->second;
    } else {
        return nullopt;
    }
}

Atom::Atom(std::string name): Atom(name, name) {}

Atom::Atom(std::string name, std::string type):
    name_(std::move(name)), type_(std::move(type)) {
    auto element = find_element(type_);
    if (element) {
        mass_ = element->mass;
    }
}

optional<std::string> Atom::full_name() const {
    auto element = find_element(type_);
    if (element) {
        return element->name;
    } else {
        return nullopt;
    }
}

optional<double> Atom::vdw_radius() const {
    auto element = find_element(type_);
    if (element) {
        return element->vdw_radius;
    } else {
        return nullopt;
    }
}

optional<double> Atom::covalent_radius() const {
    auto element = find_element(type_);
    if (element) {
        return element->covalent_radius;
    } else {
        return nullopt;
    }
}

optional<uint64_t> Atom::atomic_number() const {
    auto element = find_element(type_);
    if (element) {
        return element->number;
    } else {
        return nullopt;
    }
}
