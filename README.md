# Chemfiles, an efficient IO library for chemistry files

<div align="center">
   <a href="http://chemfiles.readthedocs.org">
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
   <hr />
   <a href="https://github.com/chemfiles/chemfiles/releases">
      <img alt="Latest release"
      src="https://img.shields.io/github/release/chemfiles/chemfiles.svg" />
   </a>
   <a href="https://github.com/chemfiles/chemfiles/blob/master/LICENCE.txt">
      <img alt="License" src="https://img.shields.io/github/license/chemfiles/chemfiles.svg" />
   </a>
</div>

Chemfiles is a library for reading and writing molecular trajectory files.
These files are created by your favorite theoretical chemistry program,
and contains informations about positions, velocities, atomic names, *etc.*

Running simulations produce enormous amounts of data, which has to be
post-processed in order to extract physical informations about the simulated
system. Chemfiles provides an interface to access this information which is
- **unified**: the same code will work with PDB, NetCDF and XTC trajectories;
- **simple**: chemfiles is easy to use and extensively documented.

Chemfiles is written in modern C++, and have interface for Python, Fortran, C, Rust
and Julia languages.


## Quick links

- [Goals of chemfiles](#goals)
- [Features](#features)
- [Supported file formats](#supported-formats)
- [Getting started](#getting-started)
- [Full documentation](http://chemfiles.readthedocs.org/en/latest/)

Using Chemfiles from other languages:
- [Python interface](https://github.com/chemfiles/chemfiles.py)
- [Fortran interface](https://github.com/chemfiles/chemfiles.f03)
- [Julia interface](https://github.com/chemfiles/Chemharp.jl)
- [Rust interface](https://github.com/chemfiles/chemfiles.rs)

## Goals

The chemfiles library tries to:

 - Be easy and simple to use. There are only five main classes, which are really
   easy to understand. The interface is coherent and easy to understand.
 - Be a multi-languages library, usable from whatever language you prefer. If your
   favorite language is not supported, chemfiles have a clean C interface so that you
   can add a binding;
 - Following the UNIX moto, do one thing, and do it well.
 - Be cross-platform, and usable from Linux, OS X and Windows;
 - Have high-quality code, using modern software building technics (unit testing, code
   coverage, git for code versioning, ...).

### Why you should use chemfiles

You should use chemfiles if (at least) one of theses assertions is true for you:

- you want to write analysis algorithm, but not have to worry about reading the data;
- you want to use binary formats, because they are faster or you have to;
- you want to write some analyse code that will work with any format, leaving you free
  to change the simulation software as you need but keeping any code working the same;
- you want to write a simulation software, and provide rich input-output interface to your
  users;

### Non-goals of chemfiles

Chemfiles also have non goals, or explicitly refused goals:

 - It will not contains any code for analysis. You can find implementation of some
   analysis algorithms and a ready-to-use command line program in the
   [cfiles](https://github.com/chemfiles/cfiles) repository.
 - It will not contain molecular dynamics, Monte-Carlo, energy minimization or quantum
   chemistry code. If you want such code with access to the chemfiles features, please see
   the [cymbalum](https://github.com/Luthaf/cymbalum) molecular simulation engine, by the
   same author.

## Features

 - Automatic recognition of file type based on filename extension;
 - Support for a number of atoms which is not constant;
 - Set custom `UnitCell` or `Topology` when reading/writing;
 - Bindings to the most used scientific languages:  Python, C, Fortran 95;
 - Work with binary formats, if the corresponding libraries are available;
 - No dependencies outside of the C++ standard library;
 - Open-source and freely available under the Mozilla Public License.

## File formats

### Supported formats

| Format        | Read ? | Write ? |
| ------------- | ------ | ------- |
| XYZ           | yes    |  yes    |
| PDB           | yes    |  yes    |
| Amber NetCDF  | yes    |  yes    |

The following formats are supported through the VMD molfile plugins, and are read-only:

| Format        | Read ? | Write ? |
| ------------- | ------ | ------- |
| LAMMPS        | yes    |  no     |
| Gromacs .gro  | yes    |  no     |
| Gromacs .xtc  | yes    |  no     |
| Gromacs .trj  | yes    |  no     |
| Gromacs .trr  | yes    |  no     |
| CHARMM DCD    | yes    |  no     |

### Planned formats

See the [issue list](https://github.com/chemfiles/chemfiles/labels/New%20Format) for
planned formats. If you want a new format to be added to chemfiles, you can either
do it by yourself (it is easy !) and create a pull-request to incorporate your
changes, or create a new issue with a link to the format reference and some
example of well-formed files.

## Getting started

The whole installation is documented [here](http://chemfiles.readthedocs.org/en/latest/installation.html), this page only
show the basic steps. Please refer to the link below in case of problem.

If you want to use chemfiles from one of the available bindings, please refer to the
corresponding repository:

- [Python interface](https://github.com/chemfiles/chemfiles.py)
- [Fortran interface](https://github.com/chemfiles/chemfiles.f03)
- [Julia interface](https://github.com/chemfiles/Chemharp.jl)
- [Rust interface](https://github.com/chemfiles/chemfiles.rs)

### Getting the dependencies

Long story made short, just use the right commands for your system:

```bash
# On apt-get based distributions
apt-get update
apt-get install cmake libnetcdf-dev

# On yum based distributions
yum install epel-release # The EPEL repository have the netcdf lib
yum install cmake netcdf-devel netcdf-cxx-devel

# On OS X with Homebrew (brew.sh)
brew tap homebrew/science
brew install cmake netcdf
```

You will also need a recent C++ compiler: gcc 4.9, clang 3.5 and icpc 14 are
known to work.

### Getting the code, building, installing

Get the code from the [release](https://github.com/chemfiles/chemfiles/releases) page,
and extract the archive. Then in a terminal, go to the folder where you put the
code (`~/chemfiles` is assumed here):
```bash
cd ~/chemfiles
```

Then, [`cmake`](http://cmake.org/) is used to build the code. So, create a folder
and go for the build:
```bash
mkdir build
cd build
cmake ..
```

Then, you can build install the library:
```bash
make
make install
```

For more informations about how to configure the build, please [read the doc](http://chemfiles.readthedocs.org/en/latest/installation.html)!

### Usage

The documentation is hosted at [readthedocs](http://chemfiles.readthedocs.org), but here
are some examples of how the usage feels in c++ and in C:

```cpp
// C++ version
#include <iostream>
#include "chemfiles.hpp"

int main() {
    chemfiles::Trajectory trajectory("filename.xyz");
    chemfiles::Frame frame;

    trajectory >> frame;
    std::cout << "There are " << frame.natoms() << " atoms in the frame" << std::endl;
    auto positions = frame.positions();

    // Do awesome things with the positions here !
}
```

```c
// C version
#include <stdint.h>
#include <stdio.h>

#include "chemfiles.h"

int main(){
    CHFL_TRAJECTORY * trajectory = chfl_trajectory_open("filename.xyz", "r");
    CHFL_FRAME * frame = chfl_frame(0);
    size_t natoms = 0;

    if (!traj) {
        printf("Error while opening file: %s", chfl_last_error());
    }

    if (!chfl_trajectory_read(trajectory, frame)){
        // handle error here
    }
    chfl_frame_atoms_count(frame, &natoms);
    printf("There are %d atoms in the frame", natoms);

    float (*positions)[3] = (float(*)[3])malloc(sizeof(float[natoms][3]));

    if (!chfl_frame_positions(frame, positions, natoms)){
        // handle error
    }

    // Do awesome things with the positions here !

    chfl_frame_free(frame);
    chfl_close(trajectory);
    free(positions);
}
```

## Contributing

Any contribution is very welcome, from feedback to pull request.

Please report any bug you find and any feature you may want as a [github issue](https://github.com/chemfiles/chemfiles/issues/new).

## Compilers, architecture and OS support

Chemfiles have been tested on the following platforms, with the following compilers :

 - Linux (64 bit)
    * GCC: gcc/g++ 4.9.2
    * Intel: icc/icpc 14
 - OS X (64 bit)
    * GCC: gcc/g++ 4.9.2
    * LLVM: clang/clang++ 3.5
    * Intel: icc/icpc 14
 - Windows (32 & 64 bit) (only the C++ and C interfaces have been tested)
    * Visual Studio 2015 (Visual Studio 2013 DO NOT work)
    * mingw64 gcc/g++ 4.9.2

If you manage to compile chemfiles on any other OS/compiler/architecture combination,
please let me know so that I can add it to this list.


## License

All this code is copyrighted by Guillaume Fraux, and put to your disposition
under the terms of the Mozilla Public License v2.0.
