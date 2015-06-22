# Plateform specific configuration

set(OTHER_CHRP_LIBRARIES "")

if(${UNIX})
    find_library(LIBDL_LIBRARY NAMES dl)
    set(OTHER_CHRP_LIBRARIES ${LIBDL_LIBRARY})
endif()
mark_as_advanced(LIBDL_LIBRARY)
