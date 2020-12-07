// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_TYPES_H
#define CHEMFILES_CHFL_TYPES_H

#include <stdint.h>
#include <stdbool.h>  // IWYU pragma: keep

#include "chemfiles/exports.h"

#if defined(__cplusplus) && !defined(INCLUDE_WHAT_YOU_USE)
namespace chemfiles {
    class Trajectory;
    class Frame;
    class Atom;
    class UnitCell;
    class Topology;
    class Residue;
    class Property;
}
struct CAPISelection;
typedef chemfiles::Trajectory CHFL_TRAJECTORY;
typedef chemfiles::Frame CHFL_FRAME;
typedef chemfiles::Atom CHFL_ATOM;
typedef chemfiles::UnitCell CHFL_CELL;
typedef chemfiles::Topology CHFL_TOPOLOGY;
typedef chemfiles::Residue CHFL_RESIDUE;
typedef chemfiles::Property CHFL_PROPERTY;
typedef CAPISelection CHFL_SELECTION;

#else

/// An opaque type handling trajectories files.
///
/// The `CHFL_TRAJECTORY` type is the main entry point when using chemfiles. A
/// `CHFL_TRAJECTORY*` behave a bit like a `FILE*` pointer, allowing to read
/// and/or write `CHFL_FRAME*` to a file.
typedef struct CHFL_TRAJECTORY CHFL_TRAJECTORY;

/// An opaque type handling frames.
///
/// A `CHFL_FRAME` contains data from one simulation step: the current unit
/// cell, the topology, the positions, and the velocities of the particles in
/// the system. If some information is missing (topology or velocity or unit
/// cell), the corresponding data is filled with a default value.
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
///
/// A `CHFL_CELL` represent the box containing the atoms, and its periodicity.
///
/// An unit cell is fully represented by three lengths (a, b, c); and three angles
/// (alpha, beta, gamma). The angles are stored in degrees, and the lengths in
/// Angstroms.
///
/// A cell also has a matricial representation, by projecting the three base
/// vector into an orthonormal base. We choose to represent such matrix as an
/// upper triangular matrix:
///
/// ```
/// | a_x   b_x   c_x |
/// |  0    b_y   c_y |
/// |  0     0    c_z |
/// ```
typedef struct CHFL_CELL CHFL_CELL;

/// An opaque type handling a topology.
///
/// A `CHFL_TOPOLOGY` contains the definition of all the atoms in the system,
/// and the liaisons between the atoms (bonds, angles, dihedrals, ...). It will
/// also contain all the residues information if it is available.
typedef struct CHFL_TOPOLOGY CHFL_TOPOLOGY;

/// An opaque type handling a residue.
///
/// A `CHFL_RESIDUE` is a group of atoms belonging to the same logical unit.
/// They can be small molecules, amino-acids in a protein, monomers in polymers,
/// *etc.*
typedef struct CHFL_RESIDUE CHFL_RESIDUE;

/// An opaque type handling a selection.
///
/// `CHFL_SELECTION` allow to select atoms in a `CHFL_FRAME`, from a selection
/// language. The selection language is built by combining basic operations.
/// Each basic operation follows the `<selector>[(<variable>)] <operator>
/// <value>` structure, where `<operator>` is a comparison operator in
/// `== != < <= > >=`.
typedef struct CHFL_SELECTION CHFL_SELECTION;

/// This class holds the data used in properties in `CHFL_FRAME` and
/// `CHFL_ATOM`. A property can have various types: bool, double, string or
/// `chfl_vector3d`.
typedef struct CHFL_PROPERTY CHFL_PROPERTY;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// `chfl_status` list the possible values for the return status code of
/// chemfiles functions.
typedef enum {  // NOLINT: this is both a C and C++ file
    /// Status code for successful operations.
    CHFL_SUCCESS = 0,
    /// Status code for error concerning memory: out of memory, wrong size for
    /// pre-allocated buffers, *etc.*
    CHFL_MEMORY_ERROR = 1,
    /// Status code for error concerning files: the file do not exist, the user
    /// does not have rights to open it, *etc.*
    CHFL_FILE_ERROR = 2,
    /// Status code for error in file formating, i.e. for invalid files.
    CHFL_FORMAT_ERROR = 3,
    /// Status code for invalid selection strings.
    CHFL_SELECTION_ERROR = 4,
    /// Status code for configuration files errors.
    CHFL_CONFIGURATION_ERROR = 5,
    /// Status code for out of bounds errors.
    CHFL_OUT_OF_BOUNDS = 6,
    /// Status code for errors related to properties.
    CHFL_PROPERTY_ERROR = 7,
    /// Status code for any other error from Chemfiles.
    CHFL_GENERIC_ERROR = 254,
    /// Status code for error in the C++ standard library.
    CHFL_CXX_ERROR = 255,
} chfl_status;

/// A 3-dimensional vector for the chemfiles interface
typedef double chfl_vector3d[3];  // NOLINT: this is both a C and C++ file

/// Get the version of the chemfiles library.
///
/// @example{capi/chfl_version.c}
/// @return A null-terminated string containing the version of Chemfiles.
CHFL_EXPORT const char* chfl_version(void);

/// Possible bond orders
typedef enum {  // NOLINT: this is both a C and C++ file
    /// Not specified
    CHFL_BOND_UNKNOWN = 0,
    /// Single bond
    CHFL_BOND_SINGLE = 1,
    /// Double bond
    CHFL_BOND_DOUBLE = 2,
    /// Triple bond
    CHFL_BOND_TRIPLE = 3,
    /// Quadruple bond (present in some metals)
    CHFL_BOND_QUADRUPLE = 4,
    /// Quintuplet bond (present in some metals)
    CHFL_BOND_QUINTUPLET = 5,
    /// Amide bond (required by some file formats)
    CHFL_BOND_AMIDE = 254,
    /// Aromatic bond (required by some file formats)
    CHFL_BOND_AROMATIC = 255,
} chfl_bond_order;

/// Maximal size for a selection match
#define CHFL_MAX_SELECTION_SIZE 4

/// A `chfl_match` is a set of atomic indexes matching a given selection. The
/// size of a match depends on the associated selection, and can vary from 1 to
/// `CHFL_MAX_SELECTION_SIZE`.
typedef struct {  // NOLINT: this is both a C and C++ file
    /// The actual size of the match. Elements in `atoms` are significant up
    /// to this value, and filled with `(uint64_t)-1` for all the other values.
    uint64_t size;
    /// Atomic indexes matching the associated selection
    uint64_t atoms[CHFL_MAX_SELECTION_SIZE];
} chfl_match;


/// A `chfl_format_metadata` contains metadata associated with one format
typedef struct {
    /// Name of the format
    const char* name;
    /// Extension associated with the format, or `NULL` if there is no extension
    /// associated.
    const char* extension;
    /// Extended, user-facing description of the format
    const char* description;
    /// URL pointing to the format definition/reference
    const char* reference;

    /// Is reading files in this format implemented?
    bool read;
    /// Is writing files in this format implemented?
    bool write;
    /// Does this format support in-memory IO?
    bool memory;

    /// Does this format support storing atomic positions?
    bool positions;
    /// Does this format support storing atomic velocities?
    bool velocities;
    /// Does this format support storing unit cell information?
    bool unit_cell;
    /// Does this format support storing atom names or types?
    bool atoms;
    /// Does this format support storing bonds between atoms?
    bool bonds;
    /// Does this format support storing residues?
    bool residues;
} chfl_format_metadata;

#ifdef __cplusplus
}
#endif

#endif
