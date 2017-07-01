
# Update this value if you need to update the data file set
set(TESTS_DATA_GIT "881aca3b892cd1a81063ae74dec5a6b32bae07c9")

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/data/${TESTS_DATA_GIT}")
    message(STATUS "Downloading test data files")
    file(DOWNLOAD
        "https://github.com/chemfiles/tests-data/archive/${TESTS_DATA_GIT}.tar.gz"
        "${CMAKE_CURRENT_BINARY_DIR}/${TESTS_DATA_GIT}.tar.gz"
        SHOW_PROGRESS
        EXPECTED_HASH SHA1=d97114cf746044097b3f8bd221cccd6bd2963283
    )

    message(STATUS "Unpacking test data files")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E remove_directory data
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${TESTS_DATA_GIT}.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E rename tests-data-${TESTS_DATA_GIT} data
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E touch data/${TESTS_DATA_GIT}
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
include_directories(SYSTEM BEFORE ${EXTERNAL_INCLUDES})
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR}/external/boost/include)
add_library(test_helpers STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/helpers.cpp)

function(chfl_test _name_ _file_)
    add_executable(${_name_} ${_file_} ${CHEMFILES_OBJECTS})
    # We need to pretend we are inside the DLL to access all functions
    target_compile_definitions(${_name_} PRIVATE chemfiles_EXPORTS)

    target_link_libraries(${_name_} test_helpers boost_filesystem ${NETCDF_LIBRARIES})
    set_property(TARGET ${_name_} PROPERTY CXX_STANDARD 11)
    add_test(NAME ${_name_}
        COMMAND ${RUNNER_COMMAND} ./${_name_}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    if(WIN32)
        # We need to set the path to allow access to chemfiles.dll
        # (and any other DLL)
        STRING(REPLACE ";" "\\;" PATH_STRING "$ENV{PATH}")
        set_tests_properties(${_name_}
            PROPERTIES ENVIRONMENT "PATH=${PATH_STRING}\;$<TARGET_FILE_DIR:chemfiles>"
        )

        # Link to the websoket library for the hostname test
        target_link_libraries(${_name_} ws2_32)
    endif()
endfunction()

function(chfl_cpp_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    chfl_test(${_name_} ${_file_})
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

function(chfl_capi_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    set(_name_ "c-${_name_}")
    chfl_test(${_name_} ${_file_})
endfunction()

file(GLOB capi_test_files ${CMAKE_CURRENT_SOURCE_DIR}/capi/*.cpp)
foreach(test_file IN LISTS capi_test_files)
    chfl_capi_test(${test_file})
endforeach(test_file)

file(RELATIVE_PATH VERSION_FILE_PATH ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/VERSION)
target_compile_definitions(c-misc PRIVATE "-DVERSION_FILE_PATH=\"${VERSION_FILE_PATH}\"")

set(CHEMFILES_TESTS_IGNORE "")
function(chfl_doctest _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    get_filename_component(_root_ ${_file_} PATH)
    get_filename_component(_root_ ${_root_} NAME)
    if(${_root_} MATCHES "chfl_")
        set(_name_ "${_root_}_${_name_}")
    endif()

    file(STRINGS ${_file_} _FILE_CONTENT_)
    foreach(_line_ ${_FILE_CONTENT_})
        if("${_line_}" MATCHES "\\[no-run\\]")
            set(CHEMFILES_TESTS_IGNORE "${CHEMFILES_TESTS_IGNORE}\n${_name_}" PARENT_SCOPE)
        endif()
    endforeach()

    chfl_test(${_name_} ${_file_})

    set_target_properties(${_name_} PROPERTIES COMPILE_FLAGS "-UNDEBUG")
endfunction()

file(GLOB_RECURSE capi_doctest_files ${CMAKE_CURRENT_SOURCE_DIR}/capi/doc/**.c)
foreach(test_file IN LISTS capi_doctest_files)
    chfl_doctest(${test_file})
endforeach(test_file)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in
    ${CMAKE_BINARY_DIR}/CTestCustom.cmake
    @ONLY
)


# Write sample configuration files for tests
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/../.chemfilesrc
"[types]
Oh = \"O\"
Oz = \"O\"
N2 = \"N\"
")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/.chemfilesrc
"[types]
# This override the root .chemfilesrc file
N2 = \"N4\"
")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/another_config
"[types]
Oz = \"F\"
")
