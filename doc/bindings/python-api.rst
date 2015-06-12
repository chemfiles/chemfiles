Python interface
================

The Python interface follow closely the C++ interface. This page list all the
classes availables in the ``chemharp`` module, and for each class all the
methods and attributes of the class.


Trajectory class
----------------

.. py:class:: Trajectory(filename[, mode="r", format=""])

    A trajectory is a file containing informations about the positions of particles
    during a simulation, and sometimes other quantities. The default constructor open
    the file at ``filename``, in read mode, with automatic format guessing.

    :param str filename: path to the file to open
    :param str mode: file opening mode. Can be "r" for read mode or "w" for write
    :param str format: if a specific format is needed, it can be specified here.
                       If ``format`` is ``""``, then the format is automatically
                       guessed from the file extension.

    .. py:method:: read()

        :return: the next :class:`Frame` in the trajectory

    .. py:method:: read_at(idx)

        :param int idx: the index to be read
        :return: the :class:`Frame` at index ``idx`` in the trajectory

    .. py:method:: write(frame)

        Write a :class:`Frame` as the next step of this :class:`Trajectory`

        :param frame Frame: the frame to be writen to the file

    .. py:method:: done()

        Check if all the frames has been read in this trajectory.

        :return: ``True`` if all the frame has been read, ``False`` if there is
                 still some frames to read.

    .. py:method:: nsteps()

        Get the total number of steps in this trajectory.

        :return: The number of steps

    .. py:method:: close()

        Close the file and flush any associated buffer to the hard drive.

    .. py:method:: topology(new_topology)

        Set the topology associated with a trajectory. This topology will be
        used when reading and writing the files, replacing any topology in the
        frames or files.

        If ``new_topology`` is a :class:`Topology`, it is used directly, and if it is the
        path to a trajectory file, the Topology associated with the first frame
        of this trajectory file is used.

    .. py:method:: cell(new_cell)

        Set the :class:`UnitCell` associated with a trajectory. This cell will be
        used when reading and writing the files, replacing any unit cell in the
        frames or files.


Frame class
-----------

.. py:class:: Frame([natoms=0])

    The :class:`Frame` class hold all data from a simulation, and can be constructed
    either from a :class:`Trajectory` while reading a file, or directly.

    :param int natoms: size to be reserved for the frame. This can be used to
                       improve performances with very larges frames, but a :class:`Frame`
                       will be resized internally as needed.

    .. py:method:: guess_topology(bonds)

        Try to guess the bonds, angles and dihedrals in the system. If ``bonds``
        is ``.true.``, guess everything; else only guess the angles and dihedrals from
        the bond list.

    .. py:attribute:: positions

        This attribute hold the positions of the particles in the system as a
        Numpy ndarray of floats. The positions are given in angströms.

    .. py:attribute:: velocities

        This attribute hold  the velocities of the particles in the system as a
        Numpy ndarray of floats. The velocities are given in angströms per
        femptosecond.

    .. py:attribute:: has_velocities

        This read-only attribute is set to ``True`` if the current :class:`Frame`
        does have velocity information or not, and to ``False`` if such information
        is not present.

    .. py:attribute:: natoms

        This read-only attribute indicating the number of atoms in the frame. The
        ``len(frame)`` function also returns this value.

    .. py:attribute:: topology

        The :class:`Topology` of this frame.

    .. py:attribute:: cell

        The :class:`UnitCell` of this frame.

    .. py:attribute:: step

        The step of the current :class:`Frame` in the corresponding :class:`Trajectory`.
        This attribute can be read and set.

UnitCell class
--------------

.. py:class:: UnitCell([type,] [a, b, c, alpha, beta, gamma])

    An UnitCell represent the bounding box of the simulation. It is represented
    by three base vectors of lengthes ``a``, ``b`` and ``c``; and the angles
    between these vectors ``alpha``, ``beta`` and ``gamma``. An optional first
    parameter ``type`` can be used to set a specific :class:`CellType`.

    .. py:attribute:: type

        The type of the cell. This takes value form the :class:`CellType` class.

    .. py:attribute:: a

        The first length of the cell, in angströms.

    .. py:attribute:: b

        The second length of the cell, in angströms.

    .. py:attribute:: c

        The third length of the cell, in angströms.

    .. py:attribute:: alpha

        The first angle of the cell, in degree.

    .. py:attribute:: beta

        The second angle of the cell, in degree.

    .. py:attribute:: gamma

        The third angle of the cell, in degree.

    .. py:attribute:: periodic_x

        A boolean encoding the periodicity of the cell in the first dimension.
        This default to ``True``.

    .. py:attribute:: periodic_y

        A boolean encoding the periodicity of the cell in the second dimension.
        This default to ``True``.

    .. py:attribute:: periodic_z

        A boolean encoding the periodicity of the cell in the third dimension.
        This default to ``True``.

    .. py:attribute:: full_periodic

        A boolean encoding the full periodicity of the cell, *i.e.* in the
        three dimensions.

    .. py:function:: matricial()

        Get the matricial representation of this unit cell, i.e. the representation
        of the three base vectors arranged as:

        .. code-block:: sh

            | a_x b_x c_x |
            |  0  b_y c_y |
            |  0   0  c_z |

        :return: A Numpy ndarray of shape ``3x3`` containing this matrix.


.. py:class:: CellType

    The following attributes flags the type of an :ref:`unit cell <overview-cell>`.

    .. py:attribute:: ORTHOROMBIC

    .. py:attribute:: TRICLINIC

    .. py:attribute:: INFINITE

Topology class
--------------

.. py:function:: dummy_topology(natoms)

    Create a topology containing ``natoms`` dummy atoms.

    :param int natoms: the size of the topology

.. py:class:: Topology([natoms=0])

    The :class:`Topology` class hold data about which atoms are linked together
    to form bonds, angles, *etc.* in a :class:`Frame`. The atoms in a topology
    are represented by they index in the associated :class:`Frame`.

    A topology can be indexed, and ``topology[i]`` is the ith :class:`Atom` in
    the topology.

    .. py:attribute:: natoms

        The number of atoms in the topology. The `len(topology)` function also
        returns this value.

    .. py:attribute:: natom_types

        The number of atomic types in the topology. A topology with Oxygen and
        Hydrogen contains two atomic types for example.

    .. py:method:: append(atom)

        Insert a new atom at the end of this topology.

        :param Atom atom: the new atom

    .. py:method:: remove(i)

        Remove the ``i`` th atom of this topology.

    .. py:method:: add_bond(i, j)

        Add a bond between tha atoms at indexes ``i`` and ``j`` in the topology.

        :param int i: the index of the first atom of the bond
        :param int j: the index of the second atom of the bond

    .. py:method:: remove_bond(i, j)

        Remove the bond between the atom at index ``i`` and ``j`` if this bond
        exists, or do nothing.

        :param int i: the index of the first atom of the bond
        :param int j: the index of the second atom of the bond

    .. py:method:: clear()

        Clear the topology and remove everything inside it.

    .. py:method:: resize(natoms)

        Resize the topology to have space for ``natoms`` atoms

        :param int natoms: the new size of the topology

    .. py:method:: isbond(i, j)

        Tell if the atoms ``i`` and ``j`` are linked together as a bond.

        :param int i: the index of the first atom of the bond
        :param int j: the index of the second atom of the bond

    .. py:method:: isangle(i, j, k)

        Tell if the atoms ``i``, ``j`` and ``k`` are linked together as an angle.
        They will form an angle if and only if ``i`` is linked to ``j``, and ``j``
        is linked to ``k``; with i ≠ k.

        :param int i: the index of the first atom of the angle
        :param int j: the index of the second atom of the angle
        :param int k: the index of the second atom of the angle

    .. py:method:: isdihedral(i, j, k, m)

        Tell if the atoms ``i``, ``j`` and ``k`` are linked together as a dihedral angle.
        They will form a dihedral angle if and only if ``i`` is linked to ``j``, ``j``
        is linked to ``k`` and ``k`` is linked to ``m``; such that (i, j, k) and
        (j, k, m) are two different angles.

        :param int i: the index of the first atom of the dihedral angle
        :param int j: the index of the second atom of the dihedral angle
        :param int k: the index of the third atom of the dihedral angle
        :param int m: the index of the fourth atom of the dihedral angle


Atom class
----------

.. py:class:: Atom(name)

    :param name str: the name of this atom

    .. py:attribute:: name

        A string, the short name of the atom. The short of Oxygen is "O", the one
        of Cadmium is "Cd".

    .. py:attribute:: mass

        A floating point number, the mass of the atom. This is determined
        automatically for elements when creating an Atom.

    .. py:attribute:: charge

        A floating point number, the charge carried by the atom.

    .. py:attribute:: type

        The atom type. This take one value in the ``AtomType`` class.


    .. py:method:: full_name()

        Try to get the atom full name from the short name.

        :return: The full atom name or "" if no name can be found.

    .. py:method:: vdw_radius()

        Try to get the Van der Waald radius of this atom from the short name.

        :return: The Van der Waald radius or -1 if no radius can be found.

    .. py:method:: covalent_radius()

        Try to get the covalent radius of this atom from the short name.

        :return: The covalent radius or -1 if no radius can be found.

    .. py:method:: atomic_number()

        Try to get the atomic number from the short name.

        :return: The atomic number or -1 if no number can be found.

.. py:class:: AtomType

    The following attributes flags the type of an :ref:`atom <overview-atom>`.

    .. py:attribute:: ELEMENT

    .. py:attribute:: CORSE_GRAIN

    .. py:attribute:: DUMMY

    .. py:attribute:: UNDEFINED
