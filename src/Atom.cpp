// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Atom.hpp"
#include "chemfiles/periodic.hpp"

using namespace chemfiles;

Atom::Atom(std::string name): Atom(name, name) {}

Atom::Atom(std::string name, std::string type):
    name_(std::move(name)), type_(std::move(type)) {
    auto periodic = PERIODIC_INFORMATION.find(type_);
    if (periodic != PERIODIC_INFORMATION.end()) {
        mass_ = periodic->second.mass;
    }
}

std::string Atom::full_name() const {
    auto periodic = PERIODIC_INFORMATION.find(type_);
    if (periodic != PERIODIC_INFORMATION.end()) {
        return periodic->second.name;
    } else {
        return "";
    }
}

double Atom::vdw_radius() const {
    auto periodic = PERIODIC_INFORMATION.find(type_);
    if (periodic != PERIODIC_INFORMATION.end()) {
        return periodic->second.vdw_radius;
    } else {
        return -1;
    }
}

double Atom::covalent_radius() const {
    auto periodic = PERIODIC_INFORMATION.find(type_);
    if (periodic != PERIODIC_INFORMATION.end()) {
        return periodic->second.colvalent_radius;
    } else {
        return -1;
    }
}

int Atom::atomic_number() const {
    auto periodic = PERIODIC_INFORMATION.find(type_);
    if (periodic != PERIODIC_INFORMATION.end()) {
        return periodic->second.number;
    } else {
        return -1;
    }
}

float  Atom::color_r() const{
  auto periodic = PERIODIC_INFORMATION.find(type_);
  if (periodic != PERIODIC_INFORMATION.end()) {
      return periodic->second.elementColor_r;
  } else {
      return -1.0f;
  }
}

float  Atom::color_g() const{
  auto periodic = PERIODIC_INFORMATION.find(type_);
  if (periodic != PERIODIC_INFORMATION.end()) {
      return periodic->second.elementColor_g;
  } else {
      return -1.0f;
  }
}

float  Atom::color_b() const{
  auto periodic = PERIODIC_INFORMATION.find(type_);
  if (periodic != PERIODIC_INFORMATION.end()) {
      return periodic->second.elementColor_b;
  } else {
      return -1.0f;
  }
}
