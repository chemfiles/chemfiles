# Chemfiles, an efficient IO library for chemistry files

<div align="center">
<a href="http://chemfiles.github.io/chemfiles/">
    <img alt="Documentation"
    src="https://img.shields.io/badge/docs-latest-brightgreen.svg" />
</a>
<a href="https://travis-ci.org/chemfiles/chemfiles">
    <img alt="Build Status"
    src="https://img.shields.io/travis/chemfiles/chemfiles/master.svg" />
</a>
<a href="https://ci.appveyor.com/project/Luthaf/chemfiles/branch/master">
    <img alt="Build Status -- Windows"
    src="https://ci.appveyor.com/api/projects/status/dvn6nr3lsssd23lo/branch/master?svg=true" />
</a>
<a href="http://codecov.io/github/chemfiles/chemfiles?branch=master">
    <img alt="Code Coverage"
    src="http://codecov.io/github/chemfiles/chemfiles/coverage.svg?branch=master"/>
</a>
</div>

Chemfiles is a modern and high-quality library for reading and writing
trajectory files created by simulations program. These trajectories are created
by computational chemistry software, and contains atomic positions, velocities,
names, topology and sometimes more.

Running simulations produce enormous amounts of data, which has to be
post-processed in order to extract physical information about the simulated
system. Chemfiles provides an interface to access this information which is
- **unified**: the same code will work with all the supported formats;
- **simple**: the interface is easy to use and extensively documented.

This repository contains the core of the library which is a programmer
interface, written in C++11. Ready to use program for analysis of trajectories
are provided in the [cfiles](https://github.com/chemfiles/cfiles) project. Other
languages interfaces are provided in their [respective
repositories](https://github.com/chemfiles/).

Chemfiles is free and open source, and your
[contributions](#contributions-welcome) are very welcome!

## Quick links

- [Features](#features)
- [Supported file formats](#supported-formats)
- [Getting started](#getting-started)
- [Contributing](#contributions-welcome)
- [Full documentation](http://chemfiles.github.io/chemfiles/)
- Documentation for using Chemfiles from various language:
    - [Python 2 and 3](http://chemfiles.github.io/chemfiles.py/)
    - [Fortran](http://chemfiles.github.io/chemfiles.f03/)
    - [C and C++](http://chemfiles.github.io/chemfiles/)
    - [Julia](http://chemfiles.github.io/Chemfiles.jl/)
    - [Rust](http://chemfiles.github.io/chemfiles.rs/)

## Features

- Read text and binary file formats (like NetCDF, XTC, TNG and others);
- Automatic recognition of file format based on the extension;
- Support a varying number of atoms;
- Set custom `UnitCell` or `Topology` when reading/writing;
- Usable from Python, C++, C, Fortran 95, Julia and Rust;
- Easy to use programming and scripting interface;
- Open-source and freely available (Mozilla Public License);
- Cross-platform, usable from Linux, OS X and Windows.

### Is chemfiles for you?

You should give a try to chemfiles if one of theses sentence is true for you:

- you do no want to spend time writing and debugging a file parser (CIF files
  anyone?)
- you use binary formats, because they are faster and take less disk space;
- you write analysis algorithm, and want to read more than one trajectory
  format;
- you write a simulation software, and want to be able to read and write to
  multiple formats.

If you find other uses for chemfiles, let us know! The main author is using it
for both analysis algorithm in [cfiles](https://github.com/chemfiles/cfiles),
and for IO in two simulation software.

## File formats

Chemfiles can read and write files in the following formats:
- XYZ;
- PDB;
- Amber NetCDF.

Chemfiles can also read (it can not write yet) files in the following formats:
- LAMMPS trajectory;
- TNG;
- Gromacs formats: .gro; .xtc; .trj; .trr;
- CHARMM DCD files

Any other format supported by VMD is easy to add. If you need one, just open a
new issue.

See the [issue list](https://github.com/chemfiles/chemfiles/labels/New%20Format)
for additional planned formats. If you want a new format to be added to
chemfiles, create a new issue with a link to the format definition.

## Getting started

This is the way to get started with the C++ and C interface. If you want to use
chemfiles from another language, please refer to the corresponding
documentation.

### Compiled packages

We use OpenSUSE build service to provide compiled packages of the latest release
for some Linux distributions. You can use your package manager to download them
[here](https://software.opensuse.org/download.html?project=home%3ALuthaf&package=chemfiles).

We also provide conda packages in the `conda-forge` community channel for Linux
and Os X. This package contains provides the C++, C and Python interfaces. You
can install the conda package by running:

```
conda install -c conda-forge chemfiles
```

You can find more information about packages in the [documentation][install].

[OSB]: https://build.opensuse.org/package/show/home:Luthaf/chemfiles
[install]: http://chemfiles.github.io/chemfiles/latest/installation.html

### Build from source

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
    // Do awesome things with the positions here !
}
```

## Contributions welcome

If this library does something that's surprising, confusing, or just plain weird
you're probably not the only person affected.  Please file an issue, even if
you're not sure it's a bug.

This goes for documentation too. If you think "I wish it had told me that," that
counts as a documentation issue.  New users are especially helpful because your
feedback is the only way we can make the interface easy to learn.

Clean-up, commenting, and missing test cases are always welcome. Try to respect
the overall coding style as much as you can. Do not hesitate to open an [issue]
for feature requests.

[issue]: https://github.com/chemfiles/chemfiles/issues/new

## Compilers, architecture and OS support

Chemfiles is tested at every code addition on the following platforms, with the
following compilers :

- Linux (64 bit)
    - GCC 4.8.5
    - Clang 3.6
- OS X (64 bit)
    - GCC 5.3
    - Clang 3.5
- Windows (32 & 64 bit)
    - Visual Studio 2015 (Visual Studio 2013 DOES NOT support C++11)

It is known to also work with the Intel compilers on both OS X and Linux, and
with mingw64 on Windows. If you manage to compile chemfiles on any other
OS/compiler/architecture combination, please let us know so that I can add it to
this list.

## High quality

The quality of a project can be defined in various ways. Here is what we mean
when speaking about quality.

1. Documentation: always up-to-date, detailed as much as possible, both for API
and user documentations.
2. Unit tests: each functions, each class is unit tested. No new code should be
added without a unit test.
3. Continuous Integration: each set of changes must compile and must not
introduce a regression on all build targets.
4. High code coverage: unit tests and continuous integration are used together
to check that most of the code paths are tested.

## Contributors and license

Chemfiles is written by Guillaume Fraux, and put to your disposition under the
terms of the Mozilla Public License v2.0. By contributing to chemfiles, you
agree to distribute your contributions under the same license.
