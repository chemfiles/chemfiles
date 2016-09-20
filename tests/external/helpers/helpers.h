#ifndef CHEMFILES_TESTS_HELPERS_H
#define CHEMFILES_TESTS_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

// On Windows, disable the "Application error" dialog box, because it
// requires an human intervention, and there is no one on Appveyor.
//
// On UNIX, does nothing
void silent_crash_handlers(void);

// Read a whole file at once, normalizing lines ending to '\n'
char* read_whole_file(const char* path);

#ifdef __cplusplus
}
#endif

#endif
