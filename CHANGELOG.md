# 0.5 (19 Feb 2016)

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
* Remove the periodicity handling from UnitCell. It was not implemented in
  boundaries conditions. The corresponding function where removed from the C
  API.
* Rename all setter function from `void xxx(const XXX& value)` to
  `void set_xxx(const XXX& value)` in C++ API.
* It is now possible to provide a callback for logging from the C API. The
  `chfl_log_stdout`, `chfl_log_silent` and `chfl_log_callback` function where
  added to the C API.


# 0.4 (30 Oct 2015)

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
* Various bugfixes and code improvements
* Renamed the library to chemfiles.

# 0.3 (3 Aug 2015)

* Julia binding
* Initial Windows support, with both MSVC and mingw
* Add a binary frontend called `chrp`, implementing some analysis algorithms.
  For more informations, see the [specific repository](https://github.com/Luthaf/chrp).

# 0.2 (31 May 2015)

* Add basic geometrical operations on vectors and implement basic periodic boundaries condition with the `UnitCell::wrap` function;
* Use VMD molfile plugins as a format provider to read trajectories. The following formats are
added through Molfile:
    * PDB;
    * Gromacs gro;
    * Gromacs xtc;
    * Gromacs trj;
    * Gromacs trr;
    * CHARMM dcd;

# 0.1 (16 May 2015)

Initial release. See the documentation for the full API.

Chemharp is usable from four languages:

* C++;
* C;
* Fortran;
* Python;

The following formats are supported:

* XYZ;
* AMBER NetCDF;
