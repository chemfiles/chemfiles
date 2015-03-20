option(ENABLE_PYTHON "Build python binding." OFF)

if(${ENABLE_PYTHON})
    add_subdirectory(python)
endif()
