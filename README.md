# Chemharp, an efficient IO library for chemistry file formats

<div align="center">
   <a href="http://chemharp.readthedocs.org">
      <img alt="Documentation" src="https://readthedocs.org/projects/chemharp/badge/?version=latest"></img>
   </a>
   <a href="https://travis-ci.org/Luthaf/Chemharp">
      <img alt="Build Status" src="https://img.shields.io/travis/Luthaf/Chemharp/master.svg"></img>
   </a>
   <a href="https://coveralls.io/r/Luthaf/Chemharp">
      <img alt="Coverage Status" src="https://img.shields.io/coveralls/Luthaf/Chemharp/master.svg"></img>
   </a>
   <hr />
   <a href="https://github.com/Luthaf/Chemharp/releases">
      <img alt="Latest release" src="https://img.shields.io/github/release/Luthaf/Chemharp.svg"></img>
   </a>
   <a href="https://github.com/Luthaf/Chemharp/blob/master/LICENCE.txt">
      <img alt="License" src="https://img.shields.io/github/license/Luthaf/Chemharp.svg"></img>
   </a>
</div>

Chemharp is a multi-language library written in modern C++ for reading and writing
from and to molecular trajectory files. These files are created by your favorite
theoretical chemistry program, and contains informations about atomic or residues
names and positions. Some format also have additional informations, such as
velocities, forces, energy, …

The main targeted audience of Chemharp (*libchemharp*) is chemistry researcher
working on their own code to do some kind of awesome science, without wanting to
bother about handling all the format that may exist in the world.

Running simulation (either Quantum Dynamic, Monte Carlo, Molecular Dynamic, or
any other method) often produce enormous amounts of data, which had to be
post-processed in order to extract informations. This post-processing step involve
reading and parsing the data, and computing physical values with the help of
statistical thermodynamic. Chemharp tries to help you on the first point, by providing
the same interface to all the trajectory formats. If you ever need to change your
output format, your analysis tools will still work the same way. Chemharp is
efficient because it allow you to write and debug your code only once, and then
to re-use it as needed.

## Goals

The Chemharp library tries to:

 - Be easy and simple to use. There are only five main classes, which are really
   easy to understand. No messy templates and uninformative functions names;
 - Be a multi-languages library, usable from whatever language you prefer. If your
   favorite language is not supported, Chemharp have a clean C interface so that you
   can add a binding;
 - Do one thing, and do it well. Following the UNIX idea, Chemharp tries to do
   only one thing, but doing it well.

## Features

 - Automatic recognition of file type based on filename extension;
 - Open-source under the Mozilla Public License;
 - Cross platform: it runs on Windows, Linux, Mac OS X, and maybe others systems;
 - Support for a number of atoms which is not constant;
 - Bindings to the most used scientific languages:  Python, C, Fortran 95;
 - Work with binary formats, if the corresponding libraries are available.

## Planned Features

 - Binding to even more languages: Julia, Lua, …
 - More formats !
 - More speed !
 - Support for other files types: very big files, over network, compressed text files.

## File formats

### Supported formats

| Format        | Read ? | Write ? |
| ------------- | ------ | ------- |
| XYZ           | yes    |  yes    |
| Amber NetCDF  | yes    |  yes    |

The following formats are supported through the VMD molfile plugins, and are read-only:

| Format        | Read ? | Write ? |
| ------------- | ------ | ------- |
| PDB           | yes    |  no     |
| Gromacs .gro  | yes    |  no     |
| Gromacs .xtc  | yes    |  no     |
| Gromacs .trj  | yes    |  no     |
| Gromacs .trr  | yes    |  no     |
| CHARMM DCD    | yes    |  no     |

### Planned formats

See the [issue list](https://github.com/Luthaf/Chemharp/labels/New%20Format) for
planned formats. If you want a new format to be added to Chemharp, you can either
do it by yourself (it is easy !) and create a pull-request to incorporate your
changes, or create a new issue with a link to the format reference and some
example of well-formed files.

## Getting started

The whole installation is documented [here](http://chemharp.readthedocs.org/en/latest/installation.html), this page only
show the basic steps. Please refer to the link below in case of problem.

### Getting the dependencies

Long story made short, just use the right commands for your system:

```bash
# On apt-get based distributions
apt-get update
apt-get install cmake libnetcdf-dev libboost-filesystem-dev libboost-dev

# On yum based distributions
yum install epel-release # The EPEL repository have the netcdf lib
yum install cmake netcdf-devel netcdf-cxx-devel boost-devel boost-filesystem

# On OS X with Homebrew (brew.sh)
brew tap homebrew/science
brew install cmake netcdf boost
```

You will also need a recent C++ compiler: gcc 4.9, clang 3.5 and icpc 14 are
known to work.

### Getting the code, building, installing

Get the code from the [release](https://github.com/Luthaf/Chemharp/releases) page,
and extract the archive. Then in a terminal, go to the folder where you put the
code (`~/Chemharp` is assumed here):
```bash
cd ~/Chemharp
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

For more informations about how to configure the build, and how to build the
bindings to Python or Fortran, please [read the doc](http://chemharp.readthedocs.org/en/latest/installation.html)!

### Usage

The documentation is hosted at [readthedocs](http://chemharp.readthedocs.org), but here is an example of how the API feels like in C++:
```cpp
#include <iostream>

#include "Chemharp.cpp"
using namespace harp;

int main() {
    Trajectory traj("filename.xyz");
    Frame frame;

    traj >> frame;
    std::cout << "There is " << frame.natoms() << " atoms in the frame" << std::endl;
    auto positions = frame.positions();

    // Do stuff here with the positions
}
```

And in C:
```c
#include <stdint.h>
#include <stdio.h>

#include "chemharp.h"

int main(){
    CHRP_TRAJECTORY *traj = chrp_open("filename.xyz", "r");
    CHRP_FRAME *frame = chrp_frame(0);
    size_t natoms = 0;
    int status;

    if (!traj) {
        printf("Error while reading: %s", chrp_last_error());
    }

    status = chrp_trajectory_read(traj, frame);
    if (!status){
        /* handle error here */
    }
    chrp_frame_size(frame, &natoms);
    printf("There is %d atoms in the frame", natoms);

    float (*positions)[3] = (float(*)[3])malloc(sizeof(float[natoms][3]));

    if (!chrp_frame_positions(frame, positions, natoms)){
        /* handle error */
    }

    /* Do awesome things with the positions here ! */

    chrp_frame_free(frame);
    chrp_close(traj);
}
```

## License

All this code is copyrighted by Guillaume Fraux, and put to your disposition
under the terms of the Mozilla Public License v2.0.
