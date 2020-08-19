## Chemfiles: a library for reading and writing chemistry files

[![Documentation](https://img.shields.io/badge/docs-latest-brightgreen.svg)](http://chemfiles.org/chemfiles/)
[![Build Status](https://img.shields.io/travis/chemfiles/chemfiles/master.svg)](https://travis-ci.org/chemfiles/chemfiles)
[![Code Coverage](http://codecov.io/github/chemfiles/chemfiles/coverage.svg?branch=master)](http://codecov.io/github/chemfiles/chemfiles?branch=master)
[![Gitter](https://badges.gitter.im/chemfiles/chemfiles.svg)](https://gitter.im/chemfiles/chemfiles)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3653157.svg)](https://doi.org/10.5281/zenodo.3653157)

Chemfiles is a high-quality library for reading and writing trajectory files
created by computational chemistry simulations programs. To help you access
information (atomic positions, velocities, names, topology, etc.) about these
files, Chemfiles provides a **simple** and **unified** interface to a variety of
file formats.

- **unified**: the same code will work with all supported formats;
- **simple**: the interface is easy to use and extensively documented.

You can use Chemfiles to conduct post-processing analysis and extract physical
information about the systems you're simulating, to convert files from one
format to another, to write trajectories with your own simulation software, and
anything that requires reading or writing the file formats used in computational
chemistry.

Chemfiles is used in multiple scientific software
- [cfiles](https://github.com/chemfiles/cfiles) provides ready-to-use analysis
  algorithms simulations trajectories as a command line tool;
- [lemon](https://github.com/chopralab/lemon) is a framework for rapidly mining
  structural information from the Protein Data Bank;
- [lumol](https://github.com/lumol-org/lumol) is a prototype of universal
  extensible molecular simulation engine, supporting both molecular dynamics
  and Metropolis Monte Carlo simulations;
- [ANA](https://ana.run/) detects cavities, calculates their volume and their
  flexibility in macromolecular structures and molecular dynamics trajectories;

This repository contains the core of the chemfiles library — written in C++11,
with a C99 interface. You can also use chemfiles from other languages: [Python
2&3](https://github.com/chemfiles/chemfiles.py),
[Fortran](https://github.com/chemfiles/chemfiles.f03),
[Rust](https://github.com/chemfiles/chemfiles.rs), and
[Julia](https://github.com/chemfiles/chemfiles.jl).

## Quick Links

- [Is chemfiles for you?](#is-chemfiles-for-you)
- [Main features of chemfiles](#chemfiles-features)
- [Contact / Contribute / Cite](#contact-contribute-cite)
- [Getting Started](#getting-started)
- [Supported File Formats](http://chemfiles.org/chemfiles/latest/formats.html)
- [Full documentation](http://chemfiles.org/chemfiles/)
- Documentation for using Chemfiles from various languages:
    - [Python 2 and 3](http://chemfiles.org/chemfiles.py/)
    - [Fortran](http://chemfiles.org/chemfiles.f03/)
    - [C and C++](http://chemfiles.org/chemfiles/)
    - [Julia](http://chemfiles.org/Chemfiles.jl/)
    - [Rust](http://chemfiles.org/chemfiles.rs/)

## Is chemfiles for you?

You might want to use chemfiles if any of these points appeals to you:

- you don't want to spend time writing and debugging a file parser;
- you use binary formats because they are faster and take up less disk space;
- you write analysis algorithms and want to read more than one trajectory
  format;
- you write simulation software and want to use more than one format for input
  or output.

There are [other libraries](http://chemfiles.org/chemfiles/latest/others.html)
doing the roughly the same job as chemfiles, have a look at them if chemfiles is
not for you. Here we also say why we could not use them instead of creating a
new library.

- [OpenBabel](https://openbabel.org/wiki/Main_Page) is a C++ library providing
  convertions between more than 110 formats. It is more complex than chemfiles,
  and distributed under the GPL license.
- [VMD molfile plugins](http://www.ks.uiuc.edu/Research/vmd/) are a collection
  of plugins witten in C and C++ used by VMD to read/write trajectory files.
  They do not support a variable number of atoms in a trajectory.
- [MDTraj](http://mdtraj.org/latest/), [MDAnalyis](http://www.mdanalysis.org/),
  [cclib](https://cclib.github.io/) are Python libraries providing analysis and
  read capacities for trajectories. Unfortunely, they are only usable from
  Python.

## Chemfiles Features

- Reads both text (XYZ, PDB, ...) and binary (NetCDF, TNG, ...) file formats;
- Transparently read and write compressed files (`.gz`, `.xz` and `.bz2`);
- Filters atoms with a rich selection language, including constrains on
  multiple atoms;
- Supports non-constant numbers of atoms in trajectories;
- Easy-to-use programming interface in Python, C++, C, Fortran 95, Julia and
  Rust;
- Cross-platform and usable from Linux, OS X and Windows;
- Open source and freely available (3-clauses BSD license);

## Contact / Contribute / Cite

Chemfiles is free and open source. Your [contributions](Contributing.md) are
always welcome!

If you have questions or suggestions, or need help, please open an [issue] or
join us on our [Gitter] chat room.

If you are using Chemfiles in a published scientific study, please cite us using
the following DOI: https://doi.org/10.5281/zenodo.3653157.

## Getting Started

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

Find more information about pre-compiled packages in the [documentation][install].

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

### Usage Examples

This is what the interface looks like in C++:

```cpp
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

Chemfiles depends on multiple external libraries, which are distributed under [their
respective licenses](external/README.md). All external libraries licenses should
be compatible with chemfiles's 3 clauses BSD. One notable execption depending on
your use case is [Gemmi](https://gemmi.readthedocs.io) which is distributed
under the Mozilla Public License version 2. You can use `CHFL_DISABLE_GEMMI=ON`
CMake flag to remove this dependency.

The [AUTHORS](AUTHORS) file lists all contributors to Chemfiles. Many thanks to
all of them!

[Gitter]: https://gitter.im/chemfiles/chemfiles
[issue]: https://github.com/chemfiles/chemfiles/issues/new
[install]: http://chemfiles.org/chemfiles/latest/installation.html
[OpenSuseBuild]: https://build.opensuse.org/package/show/home:Luthaf/chemfiles
[OSB-download]: https://software.opensuse.org/download.html?project=home%3ALuthaf&package=chemfiles
