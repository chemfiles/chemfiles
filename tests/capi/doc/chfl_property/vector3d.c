// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <math.h>

int main() {
    // [example]
    CHFL_PROPERTY* property = chfl_property_vector3d((chfl_vector3d){2, 3.2, -1});

    chfl_vector3d value = {0};
    chfl_property_get_vector3d(property, value);
    assert(fabs(value[0] - 2) < 1e-12);
    assert(fabs(value[1] - 3.2) < 1e-12);
    assert(fabs(value[2] - -1) < 1e-12);

    chfl_property_free(property);
    // [example]
    return 0;
}
