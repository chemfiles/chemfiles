// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include "chemfiles/shared_allocator.hpp"

// define the global allocator instance
chemfiles::shared_allocator chemfiles::shared_allocator::instance_;
