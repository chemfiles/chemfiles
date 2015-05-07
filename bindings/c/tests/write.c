#include "chemharp.h"

#include <stdio.h>

// Force NDEBUG to be undefined
#undef NDEBUG

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

const char* FILENAME = "test-c.xyz";

const char* CONTENT = "4\n"
                      "Written by Chemharp\n"
                      "He 1 2 3\n"
                      "He 4 5 6\n"
                      "He 1 2 3\n"
                      "He 4 5 6\n";

// Read a whole file at once
static char* read_whole_file(const char* name);

int main(int argc, char** argv){
    CHRP_TRAJECTORY* traj = chrp_open(FILENAME, "w");
    CHRP_TOPOLOGY* topology = chrp_topology();
    CHRP_ATOM* atom = chrp_atom_from_name("He");
    CHRP_FRAME* frame = chrp_frame(4);

    assert(traj != NULL && topology != NULL && atom != NULL && frame != NULL);

    for (int i=0; i<4; i++)
        assert(!chrp_topology_append(topology, atom));

    float positions[4][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {1, 2, 3},
        {4, 5, 6},
    };

    assert(!chrp_frame_positions_set(frame, positions, 4));
    assert(!chrp_frame_topology_set(frame, topology));
    assert(!chrp_trajectory_write(traj, frame));
    assert(!chrp_trajectory_close(traj));

    char *file_content = read_whole_file(FILENAME);
    assert(strcmp(file_content, CONTENT) == 0);
    free(file_content);
    remove(FILENAME);

	return 0;
}


static char* read_whole_file(const char* name) {
    char *buffer;
    FILE *file = fopen(name, "rb");

    if ( file != NULL ){
        fseek(file, 0L, SEEK_END);
        long s = ftell(file);
        rewind(file);
        buffer = (char*)malloc(s);

        if ( buffer != NULL )
            fread(buffer, s, 1, file);
    }

    fclose(file);
    return buffer;
}
