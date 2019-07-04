// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_TYPES_H
#define CHEMFILES_CHFL_TYPES_H

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

/// A 3-dimmensional vector for the chemfiles interface
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
    /// Qintuplet bond (present in some metals)
    CHFL_BOND_QINTUPLET = 5,
    /// Amide bond (required by some file formats)
    CHFL_BOND_AMIDE = 254,
    /// Aromatic bond (required by some file formats)
    CHFL_BOND_AROMATIC = 255,
} chfl_bond_order;

/// Free the memory associated with a chemfiles object.
///
/// This function is NOT equivalent to the standard C function `free`, as memory
/// is acquired and released for all chemfiles objects using a references
/// counter to allow direct modification of C++ objects.
///
/// @example{capi/chfl_atom/chfl_atom.c}
CHFL_EXPORT void chfl_free(const void* objet);

#ifdef __cplusplus
}
#endif

#endif
