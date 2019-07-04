// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include "chemfiles/capi/shared_allocator.hpp"
using namespace chemfiles;

// define the global allocator instance
mutex<shared_allocator> shared_allocator::instance_;
