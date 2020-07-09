add_library(test_helpers STATIC helpers.cpp)
target_include_directories(test_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# We can not direcly link to chemfiles, but we still need it's headers
target_include_directories(test_helpers PUBLIC $<TARGET_PROPERTY:chemfiles,INCLUDE_DIRECTORIES>)

if(${EMSCRIPTEN})
    target_link_libraries(test_helpers nodefs.js)
endif()
