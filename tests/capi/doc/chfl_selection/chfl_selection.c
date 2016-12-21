#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_SELECTION* selection = chfl_selection("pairs: name(#1) O and name(#2) H");

    if (selection == NULL) {
        /* handle error */
    }

    chfl_selection_free(selection);
    // [example]
    return 0;
}
