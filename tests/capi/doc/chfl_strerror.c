#include <chemfiles.h>

#include <string.h>
#include <assert.h>

int main() {
    // [example]
    const char* message = chfl_strerror(CHFL_SUCCESS);
    assert(strcmp(message, "operation was sucessfull") == 0);
    // [example]
    return 0;
}
