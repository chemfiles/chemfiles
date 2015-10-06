# 0.4

* Add a Rust binding
* Rewrite the Python binding to use ctypes. The same code can be used with Python 2 & 3,
  and with all numpy versions.
* Easier Python and Julia binding installation, using conda binary packaging.
* Chemharp can now be compiled as a static library! This should allow for easier embedding
  in external code, and easier distribution of binaries.
* All the bindings now live on there own repository:
      - [Fortran](https://github.com/Luthaf/Chemharp.f03)
      - [Python](https://github.com/Luthaf/Chemharp.py)
      - [Julia](https://github.com/Luthaf/Chemharp.jl)
      - [Rust](https://github.com/Luthaf/Chemharp.rs)
* Various bugfixes and code improvements

# 0.3

* Julia binding
* Initial Windows support, with both MSVC and mingw
* Add a binary frontend called `chrp`, implementing some analysis algorithms.
  For more informations, see the [specific repository](https://github.com/Luthaf/chrp).

# 0.2

* Add basic geometrical operations on vectors and implement basic periodic boundaries condition with the `UnitCell::wrap` function;
* Use VMD molfile plugins as a format provider to read trajectories. The following formats are
added through Molfile:
    * PDB;
    * Gromacs gro;
    * Gromacs xtc;
    * Gromacs trj;
    * Gromacs trr;
    * CHARMM dcd;

# 0.1

Initial release. See the documentation for the full API.

Chemharp is usable from four languages:

* C++;
* C;
* Fortran;
* Python;

The following formats are supported:

* XYZ;
* AMBER NetCDF;
