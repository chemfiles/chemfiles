// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cctype>

#include "chemfiles/Atom.hpp"
#include "chemfiles/periodic.hpp"

using namespace chemfiles;

static char to_upper(char c) {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

static char to_lower(char c) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

static optional<const ElementData&> find_element(const std::string& name) {
    std::map<std::string, ElementData>::const_iterator periodic;
    std::string normalized;
    if (name.length() == 1) {
        normalized = name;
        normalized[0] = to_upper(normalized[0]);

        periodic = PERIODIC_INFORMATION.find(normalized);
    } else if (name.length() == 2) {
        normalized = name;
        normalized[0] = to_upper(normalized[0]);
        normalized[1] = to_lower(normalized[1]);

        periodic = PERIODIC_INFORMATION.find(normalized);
    } else {
        periodic = PERIODIC_INFORMATION.find(name);
    }

    if (periodic != PERIODIC_INFORMATION.end()) {
        return periodic->second;
    } else {
        return nullopt;
    }
}

Atom::Atom(std::string name): name_(std::move(name)), type_(name_) {
    auto element = find_element(type_);
    if (element) {
        mass_ = element->mass;
    }
}

Atom::Atom(std::string name, std::string type): name_(std::move(name)), type_(std::move(type)) {
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

void Atom::set(std::string name, Property value) {
    properties_.set(std::move(name), std::move(value));
}

optional<const Property&> Atom::get(const std::string& name) const {
    return properties_.get(name);
}
