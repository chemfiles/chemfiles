#include "crashs.h"

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

void silent_crash_handlers() {
#if defined(WIN32) || defined(WIN64)
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    _set_abort_behavior(0, _WRITE_ABORT_MSG);
#endif
}
