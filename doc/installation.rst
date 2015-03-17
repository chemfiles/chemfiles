Installation
============

Core library dependencies
-------------------------

In order to build the core library, you will need a C++11 compiler, like g++>4.7,
clang++>3.3. Chemharp is tested against GCC, Clang and Intel C++ compilers, and
is ISO C++11, so it should compile with other compilers as well. Please report
any sucessfull compilation with other compilers!

Chemharp also needs a compiled version of `Boost`_, with the *filesystem* library.
Boost should be available from your package manager, or you can install it from
the Boost website.

Some functionalities of Chemharp needs aditional library, and will be activated
or not at compile-time if the system can find the appropriate headers and files.
Such optional libraries are listed here:

* The `NetCDF`_ library is needed to read and write the AMBER NetCDF format.
  It is available in all the package managers.

Finally, Chemharp needs uses the `CMake`_ build system, which is also available
in all the package managers.

All these dependencies can be installed in one command:

.. code-block:: bash

    # On apt-get based distributions
    apt-get update
    apt-get install cmake libnetcdf-dev libboost-filesystem-dev libboost-dev

    # On yum based distributions
    yum install epel-release # The EPEL repository have the netcdf lib
    yum install cmake netcdf-devel netcdf-cxx-devel boost-devel boost-filesystem

    # On OS X with Homebrew
    brew tap homebrew/science
    brew install cmake netcdf boost

.. _Boost: http://boost.org/
.. _NetCDF: http://www.unidata.ucar.edu/software/netcdf/
.. _CMake: http://cmake.org/

Build steps
-----------

Getting the source code
^^^^^^^^^^^^^^^^^^^^^^^

You can get the source code from either git, or from the `release`_ page of Github.
In the later case, just unpack the archive wherever you want the source code to
live. To get the latest developpement version, use git:

.. code-block:: bash

    cd where/you/whant/chemharp/to/live
    git clone https://github.com/Luthaf/Chemharp

.. _release: https://github.com/Luthaf/Chemharp/releases

Compiling
^^^^^^^^^

The following command can build and install Chemharp on a standard UNIX environement.

.. code-block:: bash

    cd Chemharp
    mkdir build
    cd build
    cmake .. # other options are allowed
    make
    # if you whant to run the tests before installing:
    ctest
    make install

To use the other generator from CMake (XCode, VisualStudio, …), please refer to
the doccumentation of CMake.

The :command:`cmake` step can be further configured by using the curse-based GUI
(:command:`ccmake .`) or providing some command-line arguments. Here are the
most important options:

+------------------------------------+---------------------+------------------------------+
| Option                             | Default value       | Effect                       |
+====================================+=====================+==============================+
| ``-DCMAKE_INSTALL_PREFIX=prefix``  | :file:`/usr/local`  | Set the installation prefix  |
|                                    |                     | to ``prefix``                |
|                                    |                     |                              |
+------------------------------------+---------------------+------------------------------+
| ``-DCMAKE_BUILD_TYPE=type``        | ``release``         | Set to ``debug`` for debug   |
|                                    |                     | informations                 |
+------------------------------------+---------------------+------------------------------+

For instance, to install to :file:`$HOME/local`, use:

.. code-block:: bash

    cmake -DCMAKE_INSTALL_PREFIX=$HOME/local ..

Building the bindings
---------------------

Only the C interface is always compiled with the core library. All the other
interface needs to be activated with :command:`cmake`.

The other languages bindings for Chemharp have supplementary requirements. The
Python interface requires Boost.Python, the Fortran interface requires a Fortran
compiler, …

TODO: more about this
