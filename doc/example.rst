Usage examples
==============

All the code here is under the `CC-0 Universal Licence <http://creativecommons.org/publicdomain/zero/1.0/>`_
which means that you are free to do whatever you want with it (*i.e.* it is Public
Domain code)

Read a single frame
^^^^^^^^^^^^^^^^^^^

This example will read a file and fill a vector called ``indexes`` with the
indexes of all the atom obeying the :math:`x < 5` relation, in the first frame
of a trajectory.

C++ example
-----------

.. literalinclude:: ../examples/cpp/indexes.cpp
   :language: cpp
   :lines: 4-

C example
---------

.. literalinclude:: ../examples/c/indexes.c
   :language: c
   :lines: 4-

Fortran example
---------------

.. literalinclude:: ../examples/fortran/indexes.f90
   :language: fortran
   :lines: 4-

Python example
--------------

.. literalinclude:: ../examples/python/indexes.py
   :language: python
   :lines: 4-

.. TODO:: add example with more than one frame
