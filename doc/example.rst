Usage examples
==============

This example will read a file and fill a vector called ``indexes`` with the
indexes of all the atom obeying the :math:`x < 5` relation, in the first frame
of a trajectory.

C++ example
-----------

.. code-block:: cpp

    #include <iostream>
    #include "Chemharp.cpp"

    int main() {
        harp::Trajectory traj("filename.xyz");
        harp::Frame frame;

        traj >> frame;
        auto positions = frame.positions();
        vector<size_t> indexes;

        for (size_t i=0; i<frame.natoms(); i++) {
            if (positions[i][0] < 5) {
                indexes.push_back(i);
            }
        }

        std::cout << "Atoms with x < 5: " << std::endl;
        for (auto i : indexes) {
            std::cout << "  - " << i << std::endl;
        }
    }

C example
---------

.. code-block:: c

    #include <stdio.h>
    #include <stdlib.h>
    #include "chemharp.h"

    int main() {
        CHRP_TRAJECTORY* traj = chrp_open("tests/files/xyz/helium.xyz", "r");
        CHRP_FRAME* frame = chrp_frame(100);
        int errno = 0;

        if (traj == NULL)
            goto error;

        errno = chrp_read_next_step(traj, frame);
        if (errno)
            goto error;

        size_t natoms = 0;
        chrp_frame_size(frame, &natoms);

        float (*positions)[3] = (float(*)[3])malloc(sizeof(float[natoms][3]));
        unsigned* indexes = (unsigned*)malloc(sizeof(unsigned[natoms]));
        if (positions == NULL || indexes == NULL)
            goto error;

        for (int i=0; i<natoms; i++) {
            indexes[i] = (unsigned)-1;
        }

        errno = chrp_frame_positions(frame, positions, natoms);
        if (errno)
            goto error;

        unsigned last_index = 0;
        for (int i=0; i<natoms; i++) {
            if (positions[i][0] < 5) {
                indexes[last_index] = i;
                last_index++;
            }
        }

        printf("Atoms with x < 5:\n");
        int i = 0;
        while(indexes[i] != (unsigned)-1 && i < natoms) {
            printf("  - %d\n", indexes[i]);
            i++;
        }
        printf("Number of atoms: %d\n", i);

        chrp_close(traj);
        chrp_frame_free(frame);
        return 0;

    error:
        printf("Error, cleaning up …\n");
        chrp_close(traj);
        chrp_frame_free(frame);
        return 1;
    }

Fortran example
---------------

.. code-block:: fortran

    program chemtest
        use chemharp
        implicit none

        ! TODO

    end program

Python example
--------------

.. code-block:: python

    from chemharp import Trajectory

    traj = Trajectory("filename.xyz")
    frame = traj.read_next_step()
    positions = frame.positions()

    indexes = []

    for i in range(len(frame)):
        # positions is a numpy ndarray
        if positions[i, 0] < 5:
            indexes.append(i);

    print("Atoms with x < 5: ")
    for i in indexes:
        print("  - {}".format(i))


TODO: add example with more than one frame
