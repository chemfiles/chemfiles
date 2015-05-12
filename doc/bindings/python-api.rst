Python interface
================

The Python interface follow closely the C++ interface, and defines the same
classes and functions.

Trajectory class
----------------

.. py:class:: Trajectory(filename, mode="", format="")

    .. py:method:: read_next_step()

        :return: the next ``Frame`` in the trajectory

    .. py:method:: read_at_step(idx)

        :param int idx: the index to be read
        :return: the ``Frame`` at index ``idx`` in the trajectory

    .. py:method:: write_step(frame)

        Write a ``Frame`` to the ``Trajectory``

        :param frame Frame: the frame to be writen to the file

Frame class
-----------

.. py:class:: Frame

    .. py:method:: positions()

        :return: the positions of the particles in the system
        :rtype: Numpy ndarray of floats with shape ``natoms`` x3

    .. py:method:: velocities()

        :return: the velocities of the particles in the system
        :rtype: Numpy ndarray of floats with shape ``natoms`` x3

    .. py:attribute:: has_velocities

        A read-only bool indicating wether the current ``Frame`` does have
        velocity information or not

    .. py:attribute:: natoms

        A read-only attribute indicating the number of atoms in the frame. The
        ``len(frame)`` function also returns this value

    .. py:attribute:: topology

        Access to the internal ``Topology`` of the ``Frame``

    .. py:attribute:: cell

        Access to the internal ``UnitCell`` of the ``Frame``

    .. py:attribute:: step

        An integer being the step of the ``Frame`` in the ``Trajectory``.

Atom class
----------

.. py:class:: Atom(name)

    :param name str: the frame to be writen to the file

    .. py:attribute:: name

        A string, the name of the atom

    .. py:attribute:: mass

        A floating point number, the mass of the atom. This is determined
        automatically for elements when creating an Atom.

    .. py:attribute:: charge

        A floating point number, the charge carried by the atom.

    .. py:attribute:: type

        The atom type. This take one value in the ``AtomType`` class.

.. py:class:: AtomType

    The following attributes flags the type of an :ref:`atom <overview-atom>`.

    .. py:attribute:: ELEMENT

    .. py:attribute:: CORSE_GRAIN

    .. py:attribute:: DUMMY

    .. py:attribute:: UNDEFINED

Topology class
--------------

.. py:class:: Topology

    .. py:method:: append(atom)

        Add an atom to the topology.

        :param Atom atom: the new atom

    .. py:method:: add_bond(i, j)

        Add a bond between tha atoms at indexes ``i`` and ``j`` in the topology.

        :param int i: the index of the first atom
        :param int j: the index of the second atom

    .. py:attribute:: natoms

        The number of atoms in the topology. The `len(topology)` function also
        returns this value.

    .. py:attribute:: natom_types

        The number of atomic types in the topology.

    .. py:method:: clear

        Clear the topology and remove everything inside it.

    .. py:method:: resize(natoms)

        Resize the topology to have space for ``natoms`` atoms

        :param int natoms: the new size of the topology

UnitCell class
--------------

.. py:class:: UnitCell([type, a, b, c, alpha, beta, gamma])

    .. py:attribute:: type

        The type of the cell. This takes value form the ``CellType`` class.

    .. py:attribute:: a

        The first length of the cell.

    .. py:attribute:: b

        The second length of the cell.

    .. py:attribute:: c

        The third length of the cell.

    .. py:attribute:: alpha

        The first angle of the cell.

    .. py:attribute:: beta

        The second angle of the cell.

    .. py:attribute:: gamma

        The third angle of the cell.

    .. py:attribute:: periodic_x

        A boolean encoding the periodicity of the cell in the first dimension.

    .. py:attribute:: periodic_y

        A boolean encoding the periodicity of the cell in the second dimension.

    .. py:attribute:: periodic_z

        A boolean encoding the periodicity of the cell in the third dimension.

    .. py:attribute:: full_periodic

        A boolean encoding the full periodicity of the cell, *i.e.* in the
        three dimensions.

.. py:class:: CellType

    The following attributes flags the type of an :ref:`unit cell <overview-cell>`.

    .. py:attribute:: ORTHOROMBIC

    .. py:attribute:: TRICLINIC

    .. py:attribute:: INFINITE
