// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CHFL_TYPES_H
#define CHEMFILES_CHFL_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "chemfiles/config.hpp"

#ifdef __cplusplus
extern "C" {
namespace chemfiles {
    class Trajectory;
    class Frame;
    class Atom;
    class UnitCell;
    class Topology;
    class Residue;
}
struct CAPISelection;
typedef chemfiles::Trajectory CHFL_TRAJECTORY;
typedef chemfiles::Frame CHFL_FRAME;
typedef chemfiles::Atom CHFL_ATOM;
typedef chemfiles::UnitCell CHFL_CELL;
typedef chemfiles::Topology CHFL_TOPOLOGY;
typedef chemfiles::Residue CHFL_RESIDUE;
typedef CAPISelection CHFL_SELECTION;
#else
/// An opaque type handling trajectories files.
///
/// The `CHFL_TRAJECTORY` type is the main entry point when using chemfiles. A
/// `CHFL_TRAJECTORY*` behave a bit like a `FILE*` pointer, allowing to read
/// and/or write `CHFL_FRAME*` to a file.
typedef struct CHFL_TRAJECTORY CHFL_TRAJECTORY;

/// An opaque type handling frames, *i.e* data from a step.
typedef struct CHFL_FRAME CHFL_FRAME;

/// An opaque type handling an atom.
///
/// A `CHFL_ATOM` is a particle in the current `CHFL_FRAME`. It stores the
/// following atomic properties:
///
/// - atom name;
/// - atom type;
/// - atom mass;
/// - atom charge.
///
/// The atom name is usually an unique identifier (`"H1"`, `"C_a"`) while the
/// atom type will be shared between all particles of the same type: `"H"`,
/// `"Ow"`, `"CH3"`.
typedef struct CHFL_ATOM CHFL_ATOM;

/// An opaque type handling an unit cell.
typedef struct CHFL_CELL CHFL_CELL;

/// An opaque type handling a topology.
typedef struct CHFL_TOPOLOGY CHFL_TOPOLOGY;

/// An opaque type handling a residue.
typedef struct CHFL_RESIDUE CHFL_RESIDUE;

/// An opaque type handling a selection.
typedef struct CHFL_SELECTION CHFL_SELECTION;
#endif

typedef enum {
    /// Status code for success
    CHFL_SUCCESS = 0,
    /// Memory error: out of memory, wrong size for arrays parameters, ...
    CHFL_MEMORY_ERROR = 1,
    /// File error: file do not exist, you do not have rights to open it, ...
    CHFL_FILE_ERROR = 2,
    /// Error in file formating
    CHFL_FORMAT_ERROR = 3,
    /// Error in selection parsing
    CHFL_SELECTION_ERROR = 4,
    /// Any other error from Chemfiles
    CHFL_GENERIC_ERROR = 5,
    /// Error in the C++ standard library
    CHFL_CXX_ERROR = 6
} chfl_status;

typedef double chfl_vector_t[3];

/// @brief Get the version of the chemfiles library
/// @return A null-terminated string containing the version of Chemfiles.
CHFL_EXPORT const char* chfl_version(void);

#ifdef __cplusplus
}
#endif

#endif
