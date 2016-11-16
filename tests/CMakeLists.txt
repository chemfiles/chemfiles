
# Update this value if you need to update the data file set
set(TESTS_DATA_GIT_HASH "27d89caa55e1f42cdbbed5a322b51c702066d871")

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/data/${TESTS_DATA_GIT_HASH}")
    message(STATUS "Downloading test data files")
    file(DOWNLOAD
        "https://github.com/chemfiles/tests-data/archive/${TESTS_DATA_GIT_HASH}.tar.gz"
        "${CMAKE_CURRENT_BINARY_DIR}/${TESTS_DATA_GIT_HASH}.tar.gz"
        SHOW_PROGRESS
        EXPECTED_HASH SHA1=f16520453515f21ca838c508eea9d38ac4877443
    )

    message(STATUS "Unpacking test data files")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${TESTS_DATA_GIT_HASH}.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E rename tests-data-${TESTS_DATA_GIT_HASH} data
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E touch data/${TESTS_DATA_GIT_HASH}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

set(CHFL_TEST_RUNNER "none" CACHE STRING "External runner for the tests")
set_property(CACHE CHFL_TEST_RUNNER PROPERTY STRINGS none valgrind wine)

if(${CHFL_TEST_RUNNER} STREQUAL "valgrind")
    set(
        RUNNER_COMMAND
        "valgrind" "--leak-check=full" "--dsymutil=yes" "--track-origins=yes"
        "--error-exitcode=125"
        "--suppressions=${CMAKE_CURRENT_SOURCE_DIR}/valgrind.supp"
    )
elseif(${CHFL_TEST_RUNNER} STREQUAL "wine")
    set(RUNNER_COMMAND "wine")
else()
    set(RUNNER_COMMAND "")
endif()

if(MSVC)
    string(REGEX REPLACE "/Wall" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/Wall" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

add_subdirectory(external)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/helpers)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/catch)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR}/external/boost/include)
add_library(catch STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/catch/catch.cpp)
add_library(helpers STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/helpers/helpers.cpp)

function(chfl_cpp_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_executable(${_name_} ${_file_})
    target_link_libraries(${_name_} chemfiles boost_filesystem catch helpers)
    set_property(TARGET ${_name_} PROPERTY CXX_STANDARD 11)
    add_test(NAME ${_name_}
        COMMAND ${RUNNER_COMMAND} ./${_name_}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

file(GLOB all_test_files
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/files/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formats/*.cpp
)

add_definitions("-DBOOST_ALL_NO_LIB")

foreach(test_file IN LISTS all_test_files)
    chfl_cpp_test(${test_file})
endforeach(test_file)

function(chfl_c_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    set(_name_ "c-${_name_}")
    add_executable(${_name_} ${_file_})
    target_link_libraries(${_name_} chemfiles helpers)
    target_compile_definitions(${_name_} PRIVATE "-DSRCDIR=\"${PROJECT_SOURCE_DIR}\"")
    add_test(NAME ${_name_}
        COMMAND ${RUNNER_COMMAND} ./${_name_}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

file(GLOB c_test_files ${CMAKE_CURRENT_SOURCE_DIR}/capi/*.c)
foreach(test_file IN LISTS c_test_files)
    chfl_c_test(${test_file})
endforeach(test_file)
