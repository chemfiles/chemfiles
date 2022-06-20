# Change Log

All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](https://semver.org/).

## Next Release (current master)

### Deprecation and removals

- Remove support for configuration files (`chemfiles.toml`) and associated
  functions.

## 0.10.3

### Changes in supported formats

- Amber NetCDF files are now read/written with a custom netcdf parser (#443)
- TRR and XTC files are now read/written with a custom parser (#451)
- DCD files are now read/written with a custom parser (#453)

## 0.10.2 (20 Oct 2021)

### New features

- properties are now sorted, and iterating over properties will always yield
  them sorted by the associated key.

### Changes in supported formats

- Added read support for PSF files using VMD molfile plugin.

## 0.10.1 (22 Jul 2021)

### New features

- added `chemfiles::guess_format` and `chfl_guess_format` to get the format
  chemfiles would use for a given file based on its filename

### Changes in supported formats

- Added read and write support for Amber Restart (.ncrst) files.
- Added native read and write support for LAMMPS trajectory (.lammpstrj) files,
  replacing the VMD molfile implementation.

### Changes to the C API

## 0.10.0 (14 Feb 2021)

### Changes in supported formats

* Added read and write support for CIF format.
* Added read and write support for extended XYZ. The XYZ format now default to
  extended output, and read extended files. Extended XYZ allow storing unit
  cell and arbitrary atomic properties.

### New features

* Added ability to read and write files directly in-memory. See
  `Trajectory::memory_reader`; `Trajectory::memory_writer`;
  `Trajectory::memory_buffer`; `chfl_trajectory_memory_reader`;
  `chfl_trajectory_memory_writer` and `chfl_trajectory_memory_buffer`.
* Added support for appending to gzip (.gz) compressed trajectories.
* Added support for sub-selection in numerical functions, for example
  `distance(#1, name O)`.
* Changed the UnitCell representation to the full cell matrix instead of
  a/b/c/alpha/beta/gamma
* Added `chemfiles::formats_list` function to get a list of formats
  and associated metadata.

### Changes to the C API

* Added missing `chfl_frame_clear_bonds` and `chfl_topology_clear_bonds`.
* Added `chfl_cell_from_matrix` and changed parameters to `chfl_cell`, removed
  `chfl_cell_triclinic`.
* Added `chfl_formats_list` function to get a list of formats and
  associated metadata.

## 0.9.3 (5 Feb 2020)

* Fix a bug in the PDB format where no atomic name/type was read from short
  ATOM/HETATM records.
* Fix a few bugs related to UnitCell Matrix and infinite UnitCell construction

## 0.9.2 (18 Dec 2019)

* When compiling chemfiles as a shared library, the dependencies symbols are
  now hidden. This should prevent clashes between say chemfiles's zlib and the
  system zlib.
* Cache sub-selection (the 'name O' in 'is_bonded(#1, name O)'), reducing
  selection evaluation time by a huge margin.

### Changes in supported formats

* Added read and write support for CML (Chemical Markup Language) files, a XML
  based format.
* Added a native implementation of XTC and TRR formats, replacing the VMD
  molfile version. The new code supports reading and writing files, should be
  faster and use less memory.
* Remove the ability to read frames from a trajectory that was opened in
  append mode. This mode is now write only.
* Added support for bzip2 (.bz2) compressed files when reading and writing

## 0.9.1 (13 Mar 2019)

* Fix a bug with memory allocation in the C API. The allocator did not remove
  pointers as soon as `chfl_free` was called, which leaded to an error when the
  system allocator re-used the pointers.

## 0.9.0 (18 Nov 2018)

* Direct reading and writing of compressed files. gzip and lzma (.xz) formats
  are supported.
* GROMACS .gro files now supported through custom implementation.
* Properties are now supported in the `Residue` class. They are accessed using
  `Residue::set` and `Residue::get`.
* The topology of residues is now automatically set using a lookup table for the
  PDB format.
* `Frame::guess_topology` was renamed to `Frame::guess_bonds`.
* The selection engine has been rewritten to add support for more complex
  selections:
    * it is now possible to use mathematical expressions in selections such as
      `x^2 - y ^2 < sqrt(z^2 + 25)`;
    * it is now possible to access geometrical properties in such mathematical
      expressions: `distance(#1, #2)`, `angle(#1, #2, #3)`, `dihedral(#1, #2,
      #3, #4)`, and `out_of_plane(#1, #2, #3, #4)` are supported;
    * it is now possible to add constrains on the topology of the system:
      `bonded(#1, #2)`, `is_angle(#1, #2, #3)`, `is_dihedral(#1, #2, #3, #4)`,
      and `improper(#1, #2, #3, #4)` are supported;
    * the topology constrains support sub-selections: instead of checking is
      `#1` and `#2` are bonded, one can check if `#1` is bonded to any atom
      matching a selection, for example `name O` with `bonded(#1, name O)`.
    * When using numbers as atomic names/types, they must now be inside double
      quotes (`name "45"`). This also allows for more exotic atomic names
      (`name "名"`).
    * Atomic properties can be checked, using the `[property] == Ow` syntax for
      string properties, `[property] == 2.3` for numeric properties and
      `[property]` for boolean properties.
* There is only one constructor for the `Frame` class: `Frame(UnitCell cell =
  UnitCell())`. The constructor taking a topology can be replaced with calls to
  `Frame::add_atom` and `Frame::add_bond`.
* Chemfiles will now read configuration from `.chemfiles.toml` or
  `chemfiles.toml` instead of `.chemfilesrc`
* Added `Trajectory::path` to get the file path used to create a trajectory
* Renamed `Property::get_kind` to `Property::kind`
* Added `Atom::properties`; `Frame::properties`; and `Residue::properties` to
  allow iteration over all the properties in an Atom/Frame/Residue.

### Changes in supported formats

* Added `MarcoMolecule Transmission Format (MMTF)` support, reading via mmtf-cpp.
* Added `Structure-Data File (SDF)` support, reading and writing.
* Added `Cambridge Structure Search and Retrieval (CSSR)` support, reading and writing.
* `LAMMPS Data` format now support triclinic unit cells.

### Changes to the C API

* Added `chfl_residue_get_property` and `chfl_residue_set_property` to provide
  access to residue properties.
* `chfl_frame_guess_topology` was renamed to `chfl_frame_guess_bonds`.
* Function accessing atoms/cell/residue/topology inside a frame/topology no
  longer make a copy. This allows for direct reading and writing inside the
  containing frame/topology.
* Added `chfl_trajectory_path` to get the file path used to create a trajectory
* Added `chfl_{atom,frame,residue}_properties_count` and
  `chfl_{atom,frame,residue}_list_properties` to list all properties in an
  Atom/Frame/Residue
* Replaced `chfl_*_free` by an unique `chfl_free` function

## 0.8 (14 Dec 2017)

### New features

* Change the license to the 3-clauses BSD license.
* Chemfiles will now read configuration files (by default in `.chemfilesrc`),
  and use the configuration data to rename atomic types to make sure they match
  element names. The `chemfiles::add_configuration` function can be used to add
  additional configuration files.
* Reading a `Frame` (with `Trajectory::read` or `Trajectory::read_step`) will
  now set the frame step.
* The `Atom` faillible methods (`atomic_number`, `vdw_radius`, `covalent_radius`
  and `full_name`) returns `optional<T>` instead of `T`.
* Functions taking an atomic index parameter can now throw `OutOfBounds` errors
  if the index is out of bounds.
* `Topology::append` is now called `Topology::add_atom`
* `Topology::natoms` and `Frame::natoms` are now called `Topology::size` and
  `Frame::size`
* `Topology::residue` is now called `Topology::residue_for_atom`
* Added `Frame::distance`, `Frame::angle`, `Frame::dihedral` and
  `Frame::out_of_plane` to get geometric information on the system, accounting
  for periodic boundary conditions.
* Added a `Property` class to store arbitrary properties in `Frame` and `Atom`.
* Added support for improper dihedral angles in `Topology`.
* `chemfiles::add_configuration` and `chemfiles::set_warning_callback` are now
  thread safe, and will block upon concurrent usage.
* `UnitCell::matricial` is renamed to `UnitCell::matrix`.
* The `UnitCell::shape` setter is renamed to `UnitCell::set_shape`.
* The `Trajectory::close` function can be used to close a trajectory and
  synchronize any buffered content with the storage.
* Some of the topology functions are now accsible directly on the frame:
  `Frame::add_bond`, `Frame::remove_bond`, `Frame::clear_bonds`,
  `Frame::add_residue` and `operator[]`. The non const version of
  `Frame::topology` is removed.

### Changes in supported formats

* Amber NetCDF format is now activated by default, by embedding the netcdf
  library in chemfiles.
* Added `LAMMPS Data` format, reading and writing [LAMMPS data files].
* Added `Tinker` format, reading and writing Tinker XYZ file format.
* Added `MOL2` format, reading mol2 files using VMD molfiles plugin.
* Added `Molden` format, reading molden files using VMD molfiles plugin.

[LAMMPS data files]: https://lammps.sandia.gov/doc/read_data.html

### Changes to the C API

* Added `chfl_add_configuration` to add more configuration files.
* Renamed `chfl_vector_t` to `chfl_vector3d`, `chfl_match_t` to `cfl_match`; and
  `chfl_cell_shape_t` to `chfl_cellshape`.
* `chfl_atom_atomic_number`, `chfl_atom_vdw_radius` and
  `chfl_atom_covalent_radius` all returns 0 instead of -1 if the atom does not
  have a known value for this property. This allow `chfl_atom_atomic_number` to
  take a `uint64_t*` parameter instead of an `int64_t*`, following all the other
  functions in the C API.
* Added `CHFL_OUT_OF_BOUNDS` and `CHFL_PROPERTY_ERROR` variants to `chfl_status`
* Added `chfl_frame_distance`, `chfl_frame_angle`, `chfl_frame_dihedral`,
  `chfl_frame_out_of_plane` and `chfl_cell_wrap` to work with periodic boundary
  conditions.
* `chfl_residue` does not take the optional residue id as parameter, instead you
  should use `chfl_residue_with_id`.
* Added `chfl_residue_atoms` to get the list of atoms in a residue.
* Added `chfl_topology_impropers` and `chfl_topology_impropers_count` functions.
* Added `CHFL_PROPERTY` and related functions.
* `chfl_add_configuration` and `chfl_set_warning_callback` are now thread safe,
  and will block upon concurrent usage.
* Added `chfl_frame_add_bond`, `chfl_frame_remove_bond`, and
  `chfl_frame_add_residue`.

### Deprecation and removals

* `Topology::isbond`, `Topology::isangle`, `Topology::isdihedral`, and the
  corresponding C functions `chfl_topology_isbond`, `chfl_topology_isangle`
  `chfl_topology_isdihedral` are removed.

## 0.7 (25 Feb 2017)

### New features

* Add a public `Residue` class to C++ and C API to represent residue data.
  Residues are groups of atoms bonded together, which may or may not correspond
  to molecules; and are often used for bio-molecules.
* Add the `resname` and `resid` selector, to select atoms based on their
  residue.
* Account for the difference between the atom name ("H1") and atom type ("H")
  in some formats (PDB, TNG, ...). This introduces the `Atom::type` member
  function and the `chfl_atom_type` C API function.
* Add the `type` selector, to select atoms based on their type.
* Add "Frame::add_atom" function to add an atom and the corresponding position
  (and velocity) data to a frame, and the C API `chfl_frame_add_atom` function.
* Rename `UnitCell::type` to `UnitCell::shape`. This also affect
  `chfl_cell_shape_t`, `chfl_cell_shape`, and `chfl_cell_set_shape`.
* All the floating point data uses doubles instead of floats. This concerns
  atomic data, positions and velocities.
* Add "Selection::string" function and the corresponding `chfl_selection_string`
  to get the string used to build a selection.
* Selection variables uses the `#3` syntax instead of the `$3` syntax to allow
  passing selection string as shell arguments.
* Add `Frame::remove` and `chfl_frame_remove` to remove an atom in a frame.
* Allow to use chemfiles with Cmake `find_package`.

### Changes in supported formats

* Add read support for TNG files, an new portable and compressed binary format
  used by GROMACS.

### Changes to the C API

* All the integers at C boundary have a fixed size, most of the time using
  `uint64_t`.
* Add missing `chfl_topology_resize` function to C API.
* C API functions taking three lengths/angles now take a `double[3]` parameter
  instead.
* Rename `chfl_topology_are_linked` to `chfl_topology_residues_linked`.
* Rename `chfl_topology_append` to `chfl_topology_add_atom`.
* Remove `chfl_strerror`, as it is redundant with `chfl_last_error`.
* Merge `chfl_trajectory_set_topology_file` and `chfl_trajectory_set_topology_with_format`
  into `chfl_trajectory_topology_file`.
* The `chfl_frame` function no longer take the frame size as argument. It always
  creates an empty frame, that you can resize using `chfl_frame_resize`.
* `chfl_selection_evalutate` was a typo, it is renamed to `chfl_selection_evaluate`.

### Deprecation and removals

* Remove the `Atom::type` enum from C and C++ API.
* Remove the `Trajectory::sync` and the `chfl_trajectory_sync` functions.
  To ensure that all content of a file is written to the disk, the user need to
  close it.
* Remove the `Logger` and all the `chfl_log*` functions. Instead, the users can
  use `chemfiles::set_warning_callback` or `chfl_set_warning_callback` to set a
  global callback to call on warnings events.

## 0.6 (1 July 2016)

* Improve the selection language to allow selecting multiple atoms at once. For
  example, `"pairs: name($1) H and mass($2) > 5"` will select all pairs of atoms
  where the first atom name is `'H'` and the second atom mass is bigger than 5.
    * The implemented modes for selections are `one`, `atoms`, `two`, `pairs`,
      `three`, `four`, `bonds`, `angles` and `dihedrals`;
    * The `Selection` class is now directly exposed to the C API, as
      `CHFL_SELECTION*`. The `chfl_frame_selection` function is replaced by the
      `chfl_selection`, `chfl_selection_size`, `chfl_selection_evalutate`,
      `chfl_selection_matches` and `chfl_selection_free` functions, and the
      `chfl_match_t` helper struct.
* Add the `chfl_clear_errors` function, to cleanup the error state of the C API.
* Molfiles plugins are now incorporated in the Chemfiles library, and no longer
  distributed as shared libraries. The `CHEMFILES_PLUGINS` environment variable
  is a no-op.
* The caching of angles and dihedrals is now an implementation detail. That
  means that `Topology::recalculate` is gone, and that `Frame::guess_topology`
  and `chfl_frame_guess_topology` do not take a boolean parameter anymore.
* The opening mode is now a `char` instead of a string in `Trajectory`
  constructor, `chfl_trajectory_open`, and `chfl_trajectory_with_format`.
* Remove `operator<<` and `operator>>` for `Trajectory`. Users should use
  `Trajectory::read` and `Trajectory::write`
* Users can now specify the format when reading the topology associated with a
  trajectory from a file. The `chfl_trajectory_set_topology_with_format`
  function can be used to do so from the C API.
* The `chfl_atom_from_{frame,topology}` function now return `NULL` in case of
  out-of-bound access.

## 0.5 (19 Feb 2016)

* The C API now provide a direct view into the `positions` and `velocities`
  arrays. This remove the need for copy and separated getter
  (`chfl_frame_{position,velocities}`) and setter
  (`chfl_frame_{position,velocities}_set`) function. This also force usage of
  `chfl_frame_add_velocities` to add velocity data to a frame, and
  `chfl_frame_resize` to change the size of the frame.
* Add constants for error codes in C API. The following macro are defined:
  `CHFL_SUCCESS`, `CHFL_MEMORY_ERROR`, `CHFL_FILE_ERROR`, `CHFL_FORMAT_ERROR`,
  `CHFL_GENERIC_ERROR`, `CHFL_CXX_ERROR`.
* Add the `chfl_version` function in C API.
* Add a small selection language *a la* VMD, allowing to select atoms matching
  a selection string like `"name H and x > 4"`. This is exposed to C++ with the
  public `Selection` class, and to C with the `chfl_frame_selection` function.
* Remove the periodicity handling from `UnitCell`. It was not implemented in
  boundaries conditions. The corresponding function where removed from the C
  API.
* Rename all setter function from `void xxx(const XXX& value)` to
  `void set_xxx(const XXX& value)` in C++ API.
* It is now possible to provide a callback for logging from the C API. The
  `chfl_log_stdout`, `chfl_log_silent` and `chfl_log_callback` function where
  added to the C API.


## 0.4 (30 Oct 2015)

* Chemharp can now be compiled as a static library! This should allow for easier
  embedding in external code, and easier distribution of binaries.
* Add a `chemfiles::Trajectory::sync` method to sync any buffered operation with
  the disk. The `chfl_trajectory_sync` function exposes it to the C API.
* Add a Rust binding
* Rewrite the Python binding to use ctypes. The same code can be used with
  Python 2 & 3, and with all numpy versions.
* Easier Python and Julia binding installation, using conda binary packaging.
* All the bindings now live on their own repository:
      - [Fortran](https://github.com/Luthaf/Chemharp.f03)
      - [Python](https://github.com/Luthaf/Chemharp.py)
      - [Julia](https://github.com/Luthaf/Chemharp.jl)
      - [Rust](https://github.com/Luthaf/Chemharp.rs)
* The library is now continuously tested on Visual Studio
* Various bug fixes and code improvements
* Renamed the library to Chemfiles.

## 0.3 (3 Aug 2015)

* Julia binding
* Initial Windows support, with both MSVC and mingw
* Add a binary frontend called `chrp`, implementing some analysis algorithms.
  For more information, see the [specific repository](https://github.com/Luthaf/chrp).

## 0.2 (31 May 2015)

* Add basic geometrical operations on vectors and implement basic periodic boundaries condition with the `UnitCell::wrap` function;
* Use VMD Molfiles plugins as a format provider to read trajectories. The following formats are
added through Molfiles:
    * PDB;
    * GROMACS gro;
    * GROMACS xtc;
    * GROMACS trj;
    * GROMACS trr;
    * CHARMM dcd;

## 0.1 (16 May 2015)

Initial release. See the documentation for the full API.

Chemharp is usable from four languages:

* C++;
* C;
* Fortran;
* Python;

The following formats are supported:

* XYZ;
* AMBER NetCDF;
