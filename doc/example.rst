Usage examples
==============

All the code here is under the `CC-0 Universal Licence <http://creativecommons.org/publicdomain/zero/1.0/>`_
which means that you are free to do whatever you want with it (*i.e.* it is Public
Domain code)

This page contains the following examples, written in all the languages
supported by Chemharp:

* The ``indexes`` program read the first frame of a trajectory and fill a
  vector called ``indexes`` with the indexes of all the atom obeying the
  :math:`x < 5` relation.
* The ``rmsd`` program read all the frames from a trajectory, and compute the
  root mean square displacement of the 10th atom in the frame.

C++ examples
^^^^^^^^^^^^

Read a single frame
-------------------

.. literalinclude:: ../examples/cpp/indexes.cpp
   :language: cpp
   :lines: 4-

Read Multiple frames
--------------------

.. literalinclude:: ../examples/cpp/rmsd.cpp
   :language: cpp
   :lines: 4-

C examples
^^^^^^^^^^

Read a single frame
-------------------

.. literalinclude:: ../examples/c/indexes.c
   :language: c
   :lines: 4-

Read Multiple frames
--------------------

.. literalinclude:: ../examples/c/rmsd.c
   :language: c
   :lines: 4-

Python examples
^^^^^^^^^^^^^^^

Read a single frame
-------------------

.. literalinclude:: ../examples/python/indexes.py
   :language: python
   :lines: 4-

Read Multiple frames
--------------------

.. literalinclude:: ../examples/python/rmsd.py
   :language: python
   :lines: 4-

Fortran examples
^^^^^^^^^^^^^^^^

Read a single frame
-------------------

.. literalinclude:: ../examples/fortran/indexes.f90
   :language: fortran
   :lines: 4-

Read Multiple frames
--------------------

.. literalinclude:: ../examples/fortran/rmsd.f90
   :language: fortran
   :lines: 4-
