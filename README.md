## Chemfiles: a library for reading and writing chemistry files

[![Documentation](https://img.shields.io/badge/docs-latest-brightgreen.svg)](http://chemfiles.github.io/chemfiles/)
[![Build Status -- Linux & OSX](https://img.shields.io/travis/chemfiles/chemfiles/master.svg)](https://travis-ci.org/chemfiles/chemfiles)
[![Build Status -- Windows](https://ci.appveyor.com/api/projects/status/dvn6nr3lsssd23lo/branch/master?svg=true)](https://ci.appveyor.com/project/Luthaf/chemfiles/branch/master)
[![Code Coverage](http://codecov.io/github/chemfiles/chemfiles/coverage.svg?branch=master)](http://codecov.io/github/chemfiles/chemfiles?branch=master)
[![Gitter](https://badges.gitter.im/chemfiles/chemfiles.svg)](https://gitter.im/chemfiles/chemfiles)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.800663.svg)](https://doi.org/10.5281/zenodo.800663)


Chemfiles is a modern and high-quality library for reading and writing
trajectory files created by computational chemistry simulations programs. To help you access information (atomic positions, velocities, names, topology, etc.) about these files, Chemfiles provides an interface that is
- **unified**: the same code will work with all supported formats;
- **simple**: the interface is easy to use and extensively documented.

Use Chemfiles to conduct post-processing and then extract physical information about the systems you're simulating.

This repository contains the core of the Chemfiles library—a programmer
interface written in C++11. Use it with the [cfiles](https://github.com/chemfiles/cfiles)
project, which provides ready-to-use programs for analyzing trajectories. You can
also use Chemfiles with other languages; [go here](https://github.com/chemfiles/) to view all related repositories.

## Is Chemfiles for You?

We created Chemfiles to analyze algorithms in [cfiles](https://github.com/chemfiles/cfiles) and for
input/output capacities in two molecular simulation engines. You might use it too, if any of these apply to your use case:

- you don't want to spend time writing and debugging a file parser;
- you use binary formats because they are faster and take up less disk space;
- you write analysis algorithms and want to read more than one trajectory
  format;
- you write simulation software and want to use more than one format for input or output.

## Contact/Contribute

Chemfiles is free and open source. Your [contributions](Contributing.md) are
always welcome!

If you have questions or suggestions, or need help, please open an [issue] or join us on our [Gitter] chat room.

If you are using Chemfiles in a published scientific study, please cite us using the following DOI: https://doi.org/10.5281/zenodo.800663. And if you find other uses for Chemfiles, drop us a line.

## Quick Links

- [Features](#chemfiles-features)
- [Supported File Formats](#supported-file-formats)
- [Getting Started](#getting-started)
- [Full documentation](http://chemfiles.github.io/chemfiles/)
- Documentation for using Chemfiles from other languages:
    - [Python 2 and 3](http://chemfiles.github.io/chemfiles.py/)
    - [Fortran](http://chemfiles.github.io/chemfiles.f03/)
    - [C and C++](http://chemfiles.github.io/chemfiles/)
    - [Julia](http://chemfiles.github.io/Chemfiles.jl/)
    - [Rust](http://chemfiles.github.io/chemfiles.rs/)

### Chemfiles Features

- Reads both text (XYZ, PDB, ...) and binary (NetCDF, TNG, ...) file formats;
- Filters atoms with a rich selection language;
- Supports different numbers of atoms in trajectories;
- Sets custom `UnitCell` or `Topology` when reading/writing;
- Easy-to-use programming interface in Python, C++, C, Fortran 95, Julia and Rust;
- Cross-platform and usable from Linux, OS X and Windows;
- Open source and freely available (3-clauses BSD license);

### Supported File Formats

Chemfiles can read and write files in the following formats:
- XYZ;
- PDB;
- Amber NetCDF

Chemfiles can also read (but not yet write) files in the following formats:
- TNG;
- LAMMPS data files;
- LAMMPS trajectory;
- GROMACS formats: .gro; .xtc; .trj; .trr;
- CHARMM DCD files

See the [issue list](https://github.com/chemfiles/chemfiles/labels/New%20Format)
for planned formats. Any other format supported by VMD is easy to add: simply open a new issue with a link to the format definition, or—even better—write the code and [contribute](Contributing.md) it!
new issue !

### Getting Started

We'll help you get started with the C++ and C interface. If you want to use
Chemfiles with another language, please refer to the corresponding
documentation.

### Installing Compiled Packages

We use the [OpenSUSE build][OpenSuseBuild] service to provide compiled packages of
the latest Chemfiles release for Linux distributions. You can use your package
manager to download them [here][OSB-download].

We also provide conda packages in the `conda-forge` community channel for Linux
and OS X. This package provides the C++, C and Python interfaces. Install the conda package by running:

```
conda install -c conda-forge chemfiles
```

Find more information about packages in the [documentation][install].

### Building from Source

You will need [cmake](http://cmake.org/) and a C++11 compiler like GCC>=4.8,
clang>=3.3, Intel>=14 or MSVC>=15:

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
    std::cout << "There are " << frame.natoms() << " atoms in the frame" << std::endl;

    auto positions = frame.positions();
    // Do awesome science with the positions here !
}
```

## License

Guillaume Fraux created and maintains Chemfiles, which applies the [3 clauses BSD license](LICENSE). By contributing
to Chemfiles, you agree to distribute your contributions under the same license.

The [AUTHORS](AUTHORS) file lists all past contributors to Chemfiles.
Many thanks to all of them!

[Gitter]: https://gitter.im/chemfiles/chemfiles
[issue]: https://github.com/chemfiles/chemfiles/issues/new
[install]: http://chemfiles.github.io/chemfiles/latest/installation.html
[OpenSuseBuild]: https://build.opensuse.org/package/show/home:Luthaf/chemfiles
[OSB-download]: https://software.opensuse.org/download.html?project=home%3ALuthaf&package=chemfiles
