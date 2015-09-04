Installation
============

Pre-compiled binaries
^^^^^^^^^^^^^^^^^^^^^

A few precompiled binary of the latest release are provided through the `conda`_
cross-plateform package manager. These binary provide the C++, C and Python
interfaces, and the :command:`chrp` command. To install them, you can use:

.. code-block:: bash

    conda install -c https://conda.binstar.org/luthaf chemharp

.. _conda: http://conda.pydata.org/docs/

These precompiled binaries are also used for the :ref:`julia-api`, and thus you can
install the Julia ``Chemharp`` module just by running ``Pkg.add("Chemharp")``.

Building from sources
^^^^^^^^^^^^^^^^^^^^^

If you want full control over the way the code is built, you can built Chemharp from
sources.

Core library dependencies
-------------------------

In order to build the core library, you will need a C++11 compiler, like g++>4.9,
clang++>3.3. Chemharp is tested against GCC, Clang and Intel C++ compilers, and
is ISO C++11, so it should compile with other compilers as well. Please report
any sucessfull compilation with other compilers!

Some functionalities of Chemharp needs aditional library, and will be activated
or not at compile-time if the system can find the appropriate headers and files.
Such optional libraries are listed here:

* The `NetCDF`_ library is needed to read and write the AMBER NetCDF format.
  It is available in all the package managers.

Finally, Chemharp needs uses the `CMake`_ build system, which is also available
in all the package managers.

On UNIX-like systems (Linux, OS X, ...)
"""""""""""""""""""""""""""""""""""""""

All these dependencies can be installed in one command:

.. code-block:: bash

    # On apt-get based distributions
    apt-get update
    apt-get install cmake libnetcdf-dev

    # On yum based distributions
    yum install epel-release # The EPEL repository have the netcdf lib
    yum install cmake netcdf-devel netcdf-cxx-devel

    # On OS X with Homebrew
    brew tap homebrew/science
    brew install cmake netcdf

.. _NetCDF: http://www.unidata.ucar.edu/software/netcdf/
.. _CMake: http://cmake.org/

On Windows
""""""""""

You can use either MSVC 2015-rc compiler, or `mingw-w64`_ provided gcc. `MSYS2`_ offer
a package manager to install all the needed libraries. I recomend using it if you
have no preference over your compiler. After the initial installation steps, you can
run the following to install most of the dependencies :

.. code-block:: bash

    # On 64-bits windows
    pacman -S mingw64/mingw-w64-x86_64-gcc

    # On 32-bits windows
    pacman -S mingw32/mingw-w64-i686-gcc

You will also need to install cmake, which can be found `here <http://www.cmake.org/download/>`_.

.. _mingw-w64: http://mingw-w64.org/doku.php
.. _MSYS2: http://msys2.github.io/

Build steps
-----------

You can get the source code from either git, or from the `release`_ page of Github.
In the later case, just unpack the archive wherever you want the source code to
live. To get the latest developpement version, use git:

.. code-block:: bash

    cd where/you/whant/chemharp/to/live
    git clone https://github.com/Luthaf/Chemharp

If you want to run the tests or use the bindings, you will need the corresponding
code. It is available as a git submodule that you can get directly:

.. code-block:: bash

    git clone --recursive https://github.com/Luthaf/Chemharp

.. _release: https://github.com/Luthaf/Chemharp/releases

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
| ``-DBUILD_DOCUMENTATION=ON|OFF``   | ``OFF``             | Build the documentation.     |
|                                    |                     | This needs `sphinx`_ and     |
|                                    |                     | `doxygen`_ to be installed   |
+------------------------------------+---------------------+------------------------------+
| ``-DBUILD_TESTS=ON|OFF``           | ``OFF``             | Build the test suite, for    |
|                                    |                     | later running                |
+------------------------------------+---------------------+------------------------------+

For instance, to install to :file:`$HOME/local`, use:

.. code-block:: bash

    cmake -DCMAKE_INSTALL_PREFIX=$HOME/local ..

.. _doxygen: http://doxygen.org/
.. _sphinx: http://sphinx-doc.org/

Building the bindings
---------------------

Only the C interface is always compiled with the core library. All the other
interface needs to be activated with :command:`cmake` flags.

The other languages bindings for Chemharp have supplementary requirements. The Python
interface requires Boost.Python, the Fortran interface requires a Fortran compiler, …
Please ensure that those requirements are fullfiled before filling an issue.

Python interface
^^^^^^^^^^^^^^^^

The Python interface expose data to Python, and make uses of Numpy arrays. It needs
the following libraries to be installed:

* The CPython developement module, usullay called ``python-dev`` or ``python-devel``;
* The Numpy library, with developement headers.

All these library can be installed in few lines too:

.. code-block:: bash

    # On apt-get based distributions
    apt-get install python-numpy python-dev

    # On yum based distributions
    yum install numpy python-devel

    # On OS X with Homebrew
    brew tap homebrew/python
    brew install numpy

You can also use :command:`pip` or :command:`conda` to install ``numpy`` if you
prefer doing so.


Then, compiling the Python interface to Chemharp can be activated by the
``-DPYTHON_BINDING=ON`` option for :command:`cmake`. The usual :command:`make
install` will then compile and install the python extention at the right place. After
that, you should be able to ``import chemharp`` from Python prompt.

Fortran interface
^^^^^^^^^^^^^^^^^

The Fortran interface needs a Fortran 95 capable compiler, with support for the
``iso_c_binding`` module from Fortran 2003. The vast majority of Fortran 95
compiler does support at least this module from Fortran 2003. In particular,
``gfortran`` version 4.6 and upper and Intel Fortran version 10.0 and upper should
work.

Then, the Fortran interface is activated with the ``-DFORTRAN_BINDING=ON``
option at :command:`cmake` comand line.
