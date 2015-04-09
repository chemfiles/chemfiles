Chemharp, an efficient IO library for chemistry file formats
============================================================

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

.. note::

    Chemharp is still is alpha stage, and no backward compatibility is assured.
    I hope I can reach a stable API pretty soon, once it have been validated on
    various formats.

User manual
-----------

Even if Chemharp is written in C++, it can be used from the most popular
scientific programming languages: C, Fortran, Python, … You can just pick up your
favorite language to use it. This part of the documentation presents the data
model used by Chemharp to store information about the trajectories, and how to
acess that data in each of the supported languages.

Basic usage looks like this in C++:

.. code-block:: cpp

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


The interfaces for the other supported languages are described in the links below.

.. toctree::
    :maxdepth: 2

    installation
    overview
    example
    formats
    bindings/cpp-api
    bindings/c-api
    bindings/python-api
    bindings/fortran-api

.. _classes-reference:

Class reference
---------------

.. toctree::
   :maxdepth: 2

   classes/trajectory
   classes/frame
   classes/topology
   classes/atom
   classes/unitcell
   classes/logger

Developer documentation
-----------------------

Coming soon …
