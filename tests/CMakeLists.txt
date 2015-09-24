if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data/README.md")
    message(SEND_ERROR "Missing data files for tests. Please run `git submodule update --init`.")
endif()

add_subdirectory(external)

function(chfl_cpp_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_executable(${_name_} ${_file_} $<TARGET_OBJECTS:catch>)
    target_link_libraries(${_name_} chemfiles boost_filesystem)
    set_property(TARGET ${_name_} PROPERTY CXX_STANDARD 11)
    add_test(NAME ${_name_} COMMAND ${_name_} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/catch)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/boost/include)
add_library(catch OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/external/catch/catch.cpp)

add_library(libtest SHARED ${CMAKE_CURRENT_SOURCE_DIR}/test.c)
set_target_properties(libtest PROPERTIES PREFIX "" SUFFIX ".so")

file(GLOB all_test_files
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/files/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formats/*.cpp
)

add_definitions("-DSRCDIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
add_definitions("-DBINDIR=\"${CMAKE_CURRENT_BINARY_DIR}\"")

foreach(test_file IN LISTS all_test_files)
    chfl_cpp_test(${test_file})
endforeach(test_file)

# Because the files may not be installed yet, we use the environment variable
foreach(_test_ pdb-molfile dcd-molfile gro-molfile xtc-molfile trr-molfile)
    set_tests_properties(${_test_}
    PROPERTIES ENVIRONMENT "MOLFILES_DIRECTORY=${PROJECT_BINARY_DIR}/lib/")
endforeach()

function(chfl_c_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    set(_name_ "c-${_name_}")
    add_executable(${_name_} ${_file_})
    target_link_libraries(${_name_} chemfiles)
    add_test(NAME ${_name_} COMMAND ${_name_} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endfunction()

file(GLOB c_test_files ${CMAKE_CURRENT_SOURCE_DIR}/c/*.c)
foreach(test_file IN LISTS c_test_files)
    chfl_c_test(${test_file})
endforeach(test_file)
