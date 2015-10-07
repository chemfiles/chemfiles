# Plateform specific configuration

set(OTHER_CHEMFILES_LIBRARIES "")

if(${UNIX})
    find_library(LIBDL_LIBRARY NAMES dl)
    set(OTHER_CHEMFILES_LIBRARIES ${LIBDL_LIBRARY})
endif()
mark_as_advanced(LIBDL_LIBRARY)
