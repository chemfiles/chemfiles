# Change Log

All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## Next Release (current master)

### New features

* Change the license to the 3-clauses BSD license.
* Chemfiles will now read configuration files (by default in `.chemfilesrc`),
  and use the configuration data to rename atomic types to make sure they
  match element names. The `chemfiles::add_configuration` function can be used
  to add additional configuration files.
* Reading a `Frame` (with `Trajectory::read` or `Trajectory::read_step`) will
  now set the frame step.

### Changes in supported formats

* Amber NetCDF format is now activated by default, by embedding the netcdf
  library in chemfiles.
* Added `LAMMPS Data` format, reading [LAMMPS data files].

[LAMMPS data files]: http://lammps.sandia.gov/doc/read_data.html

### C API changes

* Added `chfl_add_configuration` to add more configuration files.

### Deprecation and removals


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

### C API changes

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
