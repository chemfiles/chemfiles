# Chemfiles, an efficient IO library for chemistry files

<div align="center">
<a href="http://chemfiles.readthedocs.io">
    <img alt="Documentation"
    src="https://readthedocs.org/projects/chemfiles/badge/?version=latest" />
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

Chemfiles is a library for reading and writing trajectory files created by
simulations program. These trajectories are created by theoretical chemistry
programs, and contains atomic positions, velocities, names, topology and more.

Running simulations produce enormous amounts of data, which has to be
post-processed in order to extract physical informations about the simulated
system. Chemfiles provides an interface to access this information which is
- **unified**: the same code will work with all the supported formats;
- **simple**: the interface is easy to use and extensively documented.

This repository contains the core of the library which is a programmer
interface, written in C++11. Ready to use program for analysis of trajectories
are provided in the [cfiles](https://github.com/chemfiles/cfiles) project.

Chemfiles is free and open source, and your
[contributions](#contributions-welcome) are very welcome!

## Quick links

- [Features](#features)
- [Supported file formats](#supported-formats)
- [Getting started](#getting-started)
- [Contributing](#contributions-welcome)
- [Full documentation](http://chemfiles.readthedocs.org/en/latest/)
- Documentation for using chemfile from various language:
    - [Python 2 and 3](https://chemfiles.readthedocs.io/projects/python/en/latest/)
    - [Fortran](https://chemfiles.readthedocs.io/projects/fortran/en/latest/)
    - [C and C++](https://chemfiles.readthedocs.io/)
    - [Julia](https://chemfiles.readthedocs.io/projects/julia/en/latest/)
    - [Rust](http://chemfiles.github.io/chemfiles.rs/)

## Features

- Read text and binary file formats (like NetCDF, XTC, TNG and others);
- Automatic recognition of file format based on the extension;
- Support a varying number of atoms;
- Set custom `UnitCell` or `Topology` when reading/writing;

- Usable from Python, C++, C, Fortran 95, Julia and Rust;
- Simple and coherent programming and scripting interface;
- Open-source and freely available under the Mozilla Public License;
- Cross-platform, usable from Linux, OS X and Windows.

### Why you should use chemfiles

You should use chemfiles if one of theses assertions is true for you:

- you write analysis algorithm, and want to read more than one trajectory format;
- you use binary formats, because they are faster and take less disk space;
- you write a simulation software, and want to be able to read and write to mutiple formats.

## File formats

The following formats provide both reading and writing capacities:
- XYZ;
- PDB;
- Amber NetCDF.

The following formats only provide read capacities:
- LAMMPS trajectory;
- Gromacs formats: .gro; .xtc; .trj; .trr;
- CHARMM DCD files

Any format supported by VMD is easy to add. If you need one, just open a new
issue.

See the [issue list](https://github.com/chemfiles/chemfiles/labels/New%20Format)
for additional planned formats. If you want a new format to be added to
chemfiles, create a new issue with a link to the format definition.

## Getting started

This is the way to get started with the C++ and C interface. If you want to use
chemfiles from another language, please refer to the corresponding
documentation.

The [installation documentation][install] contains more information about
pre-built version of the library, and more configuration options.

[install]: http://chemfiles.readthedocs.io/en/latest/installation.html

You will need a recent C++ compiler: gcc 4.8, clang 3.3 and icpc 14 are
known to work, and [cmake](http://cmake.org/).

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
    - Visual Studio 2015 (Visual Studio 2013 DO NOT work)

It is known to also work with the Intel compilers (icc and icpc 14) on both OS X
and Linux, and with mingw64 on Windows. If you manage to compile chemfiles on
any other OS/compiler/architecture combination, please let us know so that I can
add it to this list.


## Contributors and license

Chemfiles is written by Guillaume Fraux, and put to your disposition under the
terms of the Mozilla Public License v2.0.
