## Chemfiles: a library for reading and writing chemistry files

[![Documentation](https://img.shields.io/badge/docs-latest-brightgreen.svg)](http://chemfiles.org/chemfiles/)
[![Build Status -- Linux & OSX](https://img.shields.io/travis/chemfiles/chemfiles/master.svg)](https://travis-ci.org/chemfiles/chemfiles)
[![Build Status -- Windows](https://ci.appveyor.com/api/projects/status/dvn6nr3lsssd23lo/branch/master?svg=true)](https://ci.appveyor.com/project/Luthaf/chemfiles/branch/master)
[![Code Coverage](http://codecov.io/github/chemfiles/chemfiles/coverage.svg?branch=master)](http://codecov.io/github/chemfiles/chemfiles?branch=master)
[![Gitter](https://badges.gitter.im/chemfiles/chemfiles.svg)](https://gitter.im/chemfiles/chemfiles)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.800663.svg)](https://doi.org/10.5281/zenodo.800663)


Chemfiles is a modern and high-quality library for reading and writing
trajectory files created by computational chemistry simulations programs. To
help you access information (atomic positions, velocities, names, topology,
etc.) about these files, Chemfiles provides an interface that is
- **unified**: the same code will work with all supported formats;
- **simple**: the interface is easy to use and extensively documented.

You can use Chemfiles to conduct post-processing analysis and extract physical
information about the systems you're simulating, to convert files from one
format to another, or to write trajectories with your own simulation software.

This repository contains the core of the Chemfiles library — a programmer
interface written in C++11, with a C99 interface. If you want a ready-to-use
programs for trajectory analysis, use [cfiles](https://github.com/chemfiles/cfiles).
You can also use Chemfiles from other languages: [Python 2&3](https://github.com/chemfiles/chemfiles.py),
[Fortran](https://github.com/chemfiles/chemfiles.f03), [Rust](https://github.com/chemfiles/chemfiles.rs),
and [Julia](https://github.com/chemfiles/chemfiles.jl).

## Is Chemfiles for You?

We created Chemfiles to be able to write analysis algorithms once and use them
with all the existing trajectory formats in
[cfiles](https://github.com/chemfiles/cfiles); and to provide input/output
capacities in two molecular simulation engines. You might want to use it too, if
any of these apply to your use case:

- you don't want to spend time writing and debugging a file parser;
- you use binary formats because they are faster and take up less disk space;
- you write analysis algorithms and want to read more than one trajectory
  format;
- you write simulation software and want to use more than one format for input
  or output.

There are [other libraries](http://chemfiles.org/chemfiles/latest/others.html)
doing the roughly the same job as Chemfiles, have a look at them if Chemfiles is
not for you. Here we also say why we could not use them instead of creating a
new library.

- [OpenBabel](https://openbabel.org/wiki/Main_Page) is a C++ library providing
  convertions between more than 110 formats. It is more complex than chemfiles,
  and distributed under the GPL license.
- [VMD molfile plugins](http://www.ks.uiuc.edu/Research/vmd/) are a collection
  of plugins witten in C and C++ used by VMD to read/write trajectory files.
  It does not support non-constant number of atoms in a trajectory, and do not
  provide a lot of documentation.
- [MDTraj](http://mdtraj.org/latest/), [MDAnalyis](http://www.mdanalysis.org/),
  [cclib](https://cclib.github.io/) are Python libraries providing analysis and
  read capacities for trajectories. Unfortunely, they are only usable from
  Python.

## Contact/Contribute

Chemfiles is free and open source. Your [contributions](Contributing.md) are
always welcome!

If you have questions or suggestions, or need help, please open an [issue] or
join us on our [Gitter] chat room.

If you are using Chemfiles in a published scientific study, please cite us using
the following DOI: https://doi.org/10.5281/zenodo.800663.

## Quick Links

- [Features](#chemfiles-features)
- [Supported File Formats](#supported-file-formats)
- [Getting Started](#getting-started)
- [Full documentation](http://chemfiles.org/chemfiles/)
- Documentation for using Chemfiles from other languages:
    - [Python 2 and 3](http://chemfiles.org/chemfiles.py/)
    - [Fortran](http://chemfiles.org/chemfiles.f03/)
    - [C and C++](http://chemfiles.org/chemfiles/)
    - [Julia](http://chemfiles.org/Chemfiles.jl/)
    - [Rust](http://chemfiles.org/chemfiles.rs/)

### Chemfiles Features

- Reads both text (XYZ, PDB, ...) and binary (NetCDF, TNG, ...) file formats;
- Transparently read and write compressed files (.gz and .xz compressions);
- Filters atoms with a rich selection language;
- Supports non-constant numbers of atoms in trajectories;
- Easy-to-use programming interface in Python, C++, C, Fortran 95, Julia and
  Rust;
- Cross-platform and usable from Linux, OS X and Windows;
- Open source and freely available (3-clauses BSD license);

### Supported File Formats

Chemfiles can read and write files in the following formats:

- XYZ;
- PDB;
- CML;
- Amber NetCDF
- LAMMPS data files;
- Tinker XYZ files (.arc files);
- BIOVIA SDF files;
- SERC Daresbury Laboratory CSSR files;
- GROMACS formats: .gro; .trr; .xtc;
- TRIPOS mol2 files;
- MacroMolecular Crystallographic Information Framework (CIF) files;
- MacroMolecular Transmission Format (MMTF) files;
- Daylight SMILES: .smi;

For text-based formats (XYZ, PDB, LAMMPS Data, Tinker, ...) Chemfiles will
transparently read compressed files if the file extension matches the expected
one (`file.arc.gz`, `trajectory.pdb.xz`).

Chemfiles can also read but not (yet) write files in the following formats:

- TNG;
- LAMMPS trajectory;
- GROMACS formats: .trj;
- CHARMM DCD files;
- Molden files;

See the [issue list](https://github.com/chemfiles/chemfiles/labels/A-formats)
for planned formats. If you need another format, open a new issue with a link to
the format definition, or — even better — write the code and
[contribute](Contributing.md) it!

### Getting Started

Here, we'll help you get started with the C++ and C interface. If you want to
use Chemfiles with another language, please refer to the corresponding
documentation.

### Installing Compiled Packages

We provide compiled packages of the latest Chemfiles release for Linux
distributions. You can use your package manager to download them
[here][OSB-download].

We also provide conda packages in the `conda-forge` community channel for Linux
and OS X. This package provides the C++, C and Python interfaces. Install the conda package by running:

```
conda install -c conda-forge chemfiles
```

Find more information about packages in the [documentation][install].

### Building from Source

You will need [cmake](http://cmake.org/) and a C++11 compiler.

```bash
git clone https://github.com/chemfiles/chemfiles
cd chemfiles
mkdir build
cd build
cmake ..
make
make install
```

## Usage Examples

This is what the interface looks like in C++:

```cpp
// C++ version
#include <iostream>
#include "chemfiles.hpp"

int main() {
    chemfiles::Trajectory trajectory("filename.xyz");

    auto frame = trajectory.read();
    std::cout << "There are " << frame.size() << " atoms in the frame" << std::endl;

    auto positions = frame.positions();
    // Do awesome science with the positions here !
}
```

## License

Guillaume Fraux created and maintains Chemfiles, which is distributed under the
[3 clauses BSD license](LICENSE). By contributing to Chemfiles, you agree to
distribute your contributions under the same license.

The [AUTHORS](AUTHORS) file lists all contributors to Chemfiles. Many thanks to
all of them!

[Gitter]: https://gitter.im/chemfiles/chemfiles
[issue]: https://github.com/chemfiles/chemfiles/issues/new
[install]: http://chemfiles.org/chemfiles/latest/installation.html
[OpenSuseBuild]: https://build.opensuse.org/package/show/home:Luthaf/chemfiles
[OSB-download]: https://software.opensuse.org/download.html?project=home%3ALuthaf&package=chemfiles
