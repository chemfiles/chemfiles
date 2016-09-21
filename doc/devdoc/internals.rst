Chemfiles internals
===================

Sources organisation
--------------------

You will find the following directory in chemfiles source code:

- ``cmake``: CMake modules used for build configuration;
- ``doc``: the source for this documentation;
- ``examples``: usage examples for C and C++ interfaces;
- ``external``: external libraries used by chemfiles;
- ``include``: the headers of chemfiles;
- ``scripts``: some python and bash scripts used in developpement.
- ``src``: the sources of the library;
- ``tests``: the sources of the unit tests;

Classes organisation
--------------------

Chemfiles is written in C++11, in an object-oriented fashion. A ``Trajectory``
is built on the top of two other private classes: a :ref:`File <file-class>` and
a :ref:`Format <format-class>`. These are pure abstract class defining the
interface for reading and writing data.

.. image:: /static/img/classes.*
    :align: center

Adding new formats and tweaking behaviour of existing formats should be done
either in the ``File`` implementation for everything related to interactions
with the actual file, or in the ``Format`` implementation for everything related
with parsing data from the file.

.. toctree::
   :maxdepth: 2

   file
   format

Every ``Format`` class is associated to a ``File`` class, and the associations
are managed by the ``TrajectoryFactory`` class. New file and formats should be
registered with this class.

.. doxygenclass:: chemfiles::TrajectoryFactory
    :members:
