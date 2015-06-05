Chemharp internals
==================

Sources organisation
--------------------

You may find the following directory in the Chemharp sources :

- ``bin``: the sources of the binary frontend to Chemharp;
- ``bindings``: the sources of the bindings, with one directory by language: C, Fortran,
  Python, Julia.
- ``cmake``: some CMake modules used for build configuration;
- ``doc``: this documentation;
- ``examples``: usage examples, with one folder by supported language;
- ``include``: the headers of Chemharp;
- ``scripts``: some python scripts used for developpement. The most important is the
  Fortran binding generator;
- ``src``: the sources of the library;
- ``tests``: the sources of the unit tests;

Classes organisation
--------------------

Chemharp is written in C++11, in an object-oriented fashion. In addition to the
:ref:`5 public classes <classes-reference>`, other classes are used. A ``Trajectory``
is built on the top of two other classes: a :ref:`File <file-class>` and  a
:ref:`Format <format-class>`. The ``File`` classes provides IO operation, and the
``Format`` classes do all the work for reading/writing information from the ``File``.
For more informations, read the following:

.. toctree::
   :maxdepth: 2

   file
   format

All the File <-> Format associations are managed by the TrajectoryFactory class.

.. doxygenclass:: harp::TrajectoryFactory
    :members:
