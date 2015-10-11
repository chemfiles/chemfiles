.. _python-api:

Python interface
================

The Python interface is built on top of the C interface, using the `ctypes`_
standard module.

.. _ctypes: https://docs.python.org/3/library/ctypes.html

This interface is contained in the :py:mod:`chemfiles` module, and this page list all
the classes and methods in this module.

Error and logging functions
---------------------------

.. automodule:: chemfiles.logging
    :members:

.. automodule:: chemfiles.errors
    :members:

Trajectory class
----------------

.. set current module to chemfiles

.. automodule:: chemfiles


.. autoclass:: Trajectory
    :members:

Frame class
-----------

.. autoclass:: Frame
    :members:

UnitCell class
--------------

.. autoclass:: CellType
    :members:

.. autoclass:: UnitCell
    :members:

Topology class
--------------

.. autoclass:: Topology
    :members:

Atom class
----------

.. autoclass:: AtomType
    :members:

.. autoclass:: Atom
    :members:
