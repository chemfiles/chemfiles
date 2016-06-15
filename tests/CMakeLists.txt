if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data/README.md")
    message(SEND_ERROR "Missing data files for tests. Please run `git submodule update --init` from ${chemfiles_SOURCE_DIR}.")
endif()

if(MSVC)
    string(REGEX REPLACE "/Wall" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/Wall" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

add_subdirectory(external)

function(chfl_cpp_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_executable(${_name_} ${_file_})
    target_link_libraries(${_name_} chemfiles boost_filesystem catch crash-handlers)
    set_property(TARGET ${_name_} PROPERTY CXX_STANDARD 11)
    add_test(NAME ${_name_} COMMAND ${_name_} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/crashs)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/catch)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/boost/include)
add_library(catch STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/catch/catch.cpp)
add_library(crash-handlers STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/crashs/crashs.c)

file(GLOB all_test_files
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/files/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formats/*.cpp
)

add_definitions("-DSRCDIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
add_definitions("-DBINDIR=\"${CMAKE_CURRENT_BINARY_DIR}\"")
add_definitions("-DBOOST_ALL_NO_LIB")

foreach(test_file IN LISTS all_test_files)
    chfl_cpp_test(${test_file})
endforeach(test_file)

function(chfl_c_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    set(_name_ "c-${_name_}")
    add_executable(${_name_} ${_file_})
    target_link_libraries(${_name_} chemfiles crash-handlers)
    add_test(NAME ${_name_} COMMAND ${_name_} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endfunction()

file(GLOB c_test_files ${CMAKE_CURRENT_SOURCE_DIR}/c/*.c)
foreach(test_file IN LISTS c_test_files)
    chfl_c_test(${test_file})
endforeach(test_file)
