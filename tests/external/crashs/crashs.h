#ifndef CHEMFILES_CRASHS_HANDLERS_HPP
#define CHEMFILES_CRASHS_HANDLERS_HPP

#ifdef __cplusplus
extern "C" {
#endif

// On Windows, disable the "Application error" dialog box, because it
// requires an human intervention, and there is no one on Appveyor.
//
// On UNIX, does nothing
void silent_crash_handlers(void);

#ifdef __cplusplus
}
#endif

#endif
