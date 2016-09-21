// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "chemfiles.h"
#include "helpers.h"

bool roughly(double A[3][3], double B[3][3]) {
    double eps = 1e-10;
    return
        (fabs(A[0][0] - B[0][0]) < eps) && (fabs(A[0][1] - B[0][1]) < eps) && (fabs(A[0][2] - B[0][2]) < eps) &&
        (fabs(A[1][0] - B[1][0]) < eps) && (fabs(A[1][1] - B[1][1]) < eps) && (fabs(A[1][2] - B[1][2]) < eps) &&
        (fabs(A[2][0] - B[2][0]) < eps) && (fabs(A[2][1] - B[2][1]) < eps) && (fabs(A[2][2] - B[2][2]) < eps);
}

int main() {
    silent_crash_handlers();
    CHFL_CELL* cell = chfl_cell((chfl_vector_t){2, 3, 4});
    assert(cell != NULL);

    chfl_vector_t data = {0};
    assert(!chfl_cell_lengths(cell, data));
    assert(fabs(data[0] - 2) < 1e-10);
    assert(fabs(data[1] - 3) < 1e-10);
    assert(fabs(data[2] - 4) < 1e-10);

    assert(!chfl_cell_angles(cell, data));
    assert(fabs(data[0] - 90) < 1e-10);
    assert(fabs(data[1] - 90) < 1e-10);
    assert(fabs(data[2] - 90) < 1e-10);

    double volume = 0;
    assert(!chfl_cell_volume(cell, &volume));
    assert(fabs(volume - 2*3*4) < 1e-10);

    assert(!chfl_cell_set_lengths(cell, (chfl_vector_t){10, 20, 30}));
    assert(!chfl_cell_lengths(cell, data));
    assert(fabs(data[0] - 10) < 1e-10);
    assert(fabs(data[1] - 20) < 1e-10);
    assert(fabs(data[2] - 30) < 1e-10);

    chfl_log_silent();
    // This should be an error
    assert(chfl_cell_set_angles(cell, (chfl_vector_t){80, 89, 100}));
    chfl_log_stderr();

    chfl_vector_t expected[3] = {{10, 0, 0}, {0, 20, 0}, {0, 0, 30}};
    chfl_vector_t matrix[3];
    assert(!chfl_cell_matrix(cell, matrix));
    assert(roughly(expected, matrix));

    chfl_cell_shape_t type;
    assert(!chfl_cell_shape(cell, &type));
    assert(type == CHFL_CELL_ORTHORHOMBIC);

    assert(!chfl_cell_set_shape(cell, CHFL_CELL_TRICLINIC));
    assert(!chfl_cell_shape(cell, &type));
    assert(type == CHFL_CELL_TRICLINIC);

    assert(!chfl_cell_set_angles(cell, (chfl_vector_t){80, 89, 100}));
    assert(!chfl_cell_angles(cell, data));
    assert(fabs(data[0] - 80) < 1e-10);
    assert(fabs(data[1] - 89) < 1e-10);
    assert(fabs(data[2] - 100) < 1e-10);

    assert(!chfl_cell_free(cell));
    cell = NULL;
    cell = chfl_cell_triclinic((chfl_vector_t){20, 21, 22}, (chfl_vector_t){90, 100, 120});
    assert(cell != NULL);

    assert(!chfl_cell_lengths(cell, data));
    assert(fabs(data[0] - 20) < 1e-10);
    assert(fabs(data[1] - 21) < 1e-10);
    assert(fabs(data[2] - 22) < 1e-10);

    assert(!chfl_cell_angles(cell, data));
    assert(fabs(data[0] - 90) < 1e-10);
    assert(fabs(data[1] - 100) < 1e-10);
    assert(fabs(data[2] - 120) < 1e-10);

    assert(!chfl_cell_free(cell));

    return EXIT_SUCCESS;
}
