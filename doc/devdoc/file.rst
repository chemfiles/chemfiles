.. _file-class:

``File`` classes
================

The ``File`` classes provide abstraction of the IO operation, allowing for the same
format to be used with on-disk files, network files, memory-mapped files,
compressed files, *etc.*

Depending on the kind of format, either a ``TextFile`` or a ``BinaryFile`` is needed,
and will provide different informations.

Abstract classes
----------------

These classes only define the interface, which as to be implemented by all sub-classes.

.. doxygenclass:: harp::File
    :members:
    :protected-members:

.. doxygenclass:: harp::TextFile
    :members:
    :protected-members:

.. doxygenclass:: harp::BinaryFile
    :members:
    :protected-members:

Implemented classes
-------------------

These classes implement the interface defined previously.

.. doxygenclass:: harp::BasicFile
    :members:

.. doxygenclass:: harp::NCFile
    :members:

.. TODO:: adding a new file class
