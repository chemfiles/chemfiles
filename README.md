# Chemfiles, a modern library for chemistry file reading and writing

[![Documentation](https://img.shields.io/badge/docs-latest-brightgreen.svg)](http://chemfiles.github.io/chemfiles/)
[![Build Status -- Linux & OSX](https://img.shields.io/travis/chemfiles/chemfiles/master.svg)](https://travis-ci.org/chemfiles/chemfiles)
[![Build Status -- Windows](https://ci.appveyor.com/api/projects/status/dvn6nr3lsssd23lo/branch/master?svg=true)](https://ci.appveyor.com/project/Luthaf/chemfiles/branch/master)
[![Code Coverage](http://codecov.io/github/chemfiles/chemfiles/coverage.svg?branch=master)](http://codecov.io/github/chemfiles/chemfiles?branch=master)
[![Gitter](https://badges.gitter.im/chemfiles/chemfiles.svg)](https://gitter.im/chemfiles/chemfiles)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.800663.svg)](https://doi.org/10.5281/zenodo.800663)


Chemfiles is a modern and high-quality library for reading and writing
trajectory files created by computational chemistry simulations program. These
trajectories contains atomic positions, velocities, names, topology and
sometimes more.

Running simulations produce enormous amounts of data, which has to be
post-processed to extract physical information about the simulated system.
Chemfiles provides an interface to access this information which is
- **unified**: the same code will work with all the supported formats;
- **simple**: the interface is easy to use and extensively documented.

This repository contains the core of the library which is a programmer
interface, written in C++11. the [cfiles](https://github.com/chemfiles/cfiles)
project provides ready to use programs for analysis of trajectories. You can
also use Chemfiles from other languages, see the
[chemfiles.<xxx>](https://github.com/chemfiles/) repositories.

Chemfiles is free and open source, and your [contributions](Contributing.md) are
always welcome!

If you have questions, suggestions or if you need help with chemfiles, do not
hesitate to open an [issue] or join us on our [Gitter] chat room.

## Quick links

- [Features](#features)
- [Supported file formats](#supported-formats)
- [Getting started](#getting-started)
- [Full documentation](http://chemfiles.github.io/chemfiles/)
- Documentation for using Chemfiles from other languages:
    - [Python 2 and 3](http://chemfiles.github.io/chemfiles.py/)
    - [Fortran](http://chemfiles.github.io/chemfiles.f03/)
    - [C and C++](http://chemfiles.github.io/chemfiles/)
    - [Julia](http://chemfiles.github.io/Chemfiles.jl/)
    - [Rust](http://chemfiles.github.io/chemfiles.rs/)

## Features

- Read both text (XYZ, PDB, ...) and binary (NetCDF, TNG, ...) file formats;
- Filter atoms with a rich selection language;
- Support a varying number of atoms in trajectories;
- Set custom `UnitCell` or `Topology` when reading/writing;
- Easy to use programming interface in Python, C++, C, Fortran 95, Julia and Rust;
- Cross-platform, usable from Linux, OS X and Windows.
- Open-source and freely available (3-clauses BSD license);

### Is chemfiles for you?

You should give a try to chemfiles if one of theses sentence is true for you:

- you do no want to spend time writing and debugging a file parser;
- you use binary formats, because they are faster and take less disk space;
- you write analysis algorithm, and want to read more than one trajectory
  format;
- you write a simulation software, and want to be able to use more than one
  format for input or output.

If you find other uses for chemfiles, let us know! The main author is using it
both for analysis algorithm in [cfiles](https://github.com/chemfiles/cfiles),
and for input/output capacities in two molecular simulation engines.

## File formats

Chemfiles can read and write files in the following formats:
- XYZ;
- PDB;
- Amber NetCDF.

Chemfiles can also read (it can not write yet) files in the following formats:
- TNG;
- LAMMPS data files;
- LAMMPS trajectory;
- GROMACS formats: .gro; .xtc; .trj; .trr;
- CHARMM DCD files

See the [issue list](https://github.com/chemfiles/chemfiles/labels/New%20Format)
for planned formats. Any other format supported by VMD is easy to add; if you
need one, please open a new issue. If you want a new format in chemfiles, open a
new issue with a link to the format definition, or better, write the code and
[contribute](#contributions-welcome)!

## Getting started

This is the way to get started with the C++ and C interface. If you want to use
chemfiles from another language, please refer to the corresponding
documentation.

### Installation of compiled packages

We use [OpenSUSE build][OpenSuseBuild] service to provide compiled packages of
the latest release for some Linux distributions. You can use your package
manager to download them [here][OSB-download].

We also provide conda packages in the `conda-forge` community channel for Linux
and Os X. This package contains provides the C++, C and Python interfaces. You
can install the conda package by running:

```
conda install -c conda-forge chemfiles
```

You can find more information about packages in the [documentation][install].

### Building from source

You will need [cmake](http://cmake.org/) and a C++11 compiler like GCC>=4.8,
clang>=3.3, Intel>=14 or MSVC>=15.

```bash
git clone https://github.com/chemfiles/chemfiles
cd chemfiles
mkdir build
cd build
cmake ..
make
make install
```

### Usage examples

This is how the interface looks like in C++

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

## Contributors and license

If you are using chemfiles in a published scientific study, please cite us using
the following DOI: https://doi.org/10.5281/zenodo.800663.

Chemfiles was created and is maintained by Guillaume Fraux, and put to your
disposition under the terms of the [3 clauses BSD license](LICENSE). By contributing
to Chemfiles, you agree to distribute your contributions under the same license.

All the contributors to chemfiles are listed in the [AUTHORS](AUTHORS) file.
Many thanks to all of them!

[Gitter]: https://gitter.im/chemfiles/chemfiles
[issue]: https://github.com/chemfiles/chemfiles/issues/new
[install]: http://chemfiles.github.io/chemfiles/latest/installation.html
[OpenSuseBuild]: https://build.opensuse.org/package/show/home:Luthaf/chemfiles
[OSB-download]: https://software.opensuse.org/download.html?project=home%3ALuthaf&package=chemfiles
