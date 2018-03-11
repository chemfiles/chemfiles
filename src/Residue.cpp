// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Residue.hpp"

using namespace chemfiles;

Residue::Residue(std::string name): name_(std::move(name)), id_(nullopt) {}

Residue::Residue(std::string name, uint64_t resid): name_(std::move(name)), id_(resid) {}

void Residue::add_atom(size_t i) {
    atoms_.insert(i);
}

bool Residue::contains(size_t i) const {
    return atoms_.find(i) != atoms_.end();
}

void Residue::set(std::string name, Property value) {
    properties_.set(std::move(name), std::move(value));
}

optional<const Property&> Residue::get(const std::string& name) const {
    return properties_.get(name);
}
