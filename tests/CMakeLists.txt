# Update this value if you need to update the data file set
set(TESTS_DATA_GIT "c26f254cd7bff7e476bf065403fbd0a6a47fea38")

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/data/${TESTS_DATA_GIT}")
    message(STATUS "Downloading test data files")
    file(DOWNLOAD
        "https://github.com/chemfiles/tests-data/archive/${TESTS_DATA_GIT}.tar.gz"
        "${CMAKE_CURRENT_BINARY_DIR}/${TESTS_DATA_GIT}.tar.gz"
        SHOW_PROGRESS
        EXPECTED_HASH SHA1=3b45cce20431c575364f896d763c48f5e9e31705
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
set_property(CACHE CHFL_TEST_RUNNER PROPERTY STRINGS none valgrind wine node)

if(${CHFL_TEST_RUNNER} STREQUAL "valgrind")
    set(
        RUNNER_COMMAND
        "valgrind" "--leak-check=full" "--dsymutil=yes" "--track-origins=yes"
        "--error-exitcode=125"
        "--suppressions=${CMAKE_CURRENT_SOURCE_DIR}/valgrind.supp"
    )
elseif(${CHFL_TEST_RUNNER} STREQUAL "wine")
    set(RUNNER_COMMAND "wine")
elseif(${CHFL_TEST_RUNNER} STREQUAL "node")
    set(RUNNER_COMMAND "node")
else()
    set(RUNNER_COMMAND "")
endif()

if(MSVC)
    string(REGEX REPLACE "/Wall" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/Wall" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

add_subdirectory(external)

function(chfl_define_test _name_)
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
    endif()
endfunction()

function(chfl_unit_test _name_ _file_)
    add_executable(${_name_} ${_file_} ${CHEMFILES_OBJECTS})
    # We need to pretend we are inside the DLL to access all functions
    target_compile_definitions(${_name_} PRIVATE chemfiles_EXPORTS)
    target_link_libraries(${_name_} ${NETCDF_LIBRARIES})
    set_target_properties(${_name_} PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(${_name_} SYSTEM PRIVATE ${EXTERNAL_INCLUDES})

    chfl_define_test(${_name_})

    if(WIN32)
        # Link to the websoket library for the hostname test
        target_link_libraries(${_name_} ws2_32)
    endif()

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "PGI" AND "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
        target_link_libraries(${_name_} atomic)
    endif()
endfunction()

function(chfl_cpp_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    chfl_unit_test(${_name_} ${_file_})
    target_link_libraries(${_name_} test_helpers)
endfunction()

file(GLOB all_test_files
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/files/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formats/*.cpp
)

foreach(test_file IN LISTS all_test_files)
    chfl_cpp_test(${test_file})
endforeach(test_file)

function(chfl_capi_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    set(_name_ "c-${_name_}")
    chfl_unit_test(${_name_} ${_file_})
    target_link_libraries(${_name_} test_helpers)
endfunction()

file(GLOB capi_test_files ${CMAKE_CURRENT_SOURCE_DIR}/capi/*.cpp)
foreach(test_file IN LISTS capi_test_files)
    chfl_capi_test(${test_file})
endforeach(test_file)

file(RELATIVE_PATH VERSION_FILE_PATH ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/VERSION)
target_compile_definitions(c-misc PRIVATE "-DVERSION_FILE_PATH=\"${VERSION_FILE_PATH}\"")

function(chfl_doctest _name_ _file_)
    if(NOT CHFL_BUILD_DOCTESTS)
        return()
    endif()

    set(_run_ TRUE)
    file(STRINGS ${_file_} _FILE_CONTENT_)
    foreach(_line_ ${_FILE_CONTENT_})
        if("${_line_}" MATCHES "\\[no-run\\]")
            set(_run_ FALSE)
        endif()
    endforeach()

    add_executable(${_name_} ${_file_})
    target_link_libraries(${_name_} chemfiles)
    set_target_properties(${_name_} PROPERTIES LINKER_LANGUAGE CXX)
    set_target_properties(${_name_} PROPERTIES COMPILE_FLAGS "-UNDEBUG")
    target_include_directories(${_name_} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external)

    if(${_run_})
        chfl_define_test(${_name_})
    endif()

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "PGI" AND "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
        target_link_libraries(${_name_} atomic)
    endif()
endfunction()

function(chfl_c_doctest _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    get_filename_component(_root_ ${_file_} PATH)
    get_filename_component(_root_ ${_root_} NAME)
    if(${_root_} MATCHES "chfl_")
        set(_name_ "${_root_}_${_name_}")
    endif()

    chfl_doctest(${_name_} ${_file_})

    if(NOT MSVC AND CHFL_BUILD_DOCTESTS)
        target_link_libraries(${_name_} m)
    endif()
endfunction()

function(chfl_cpp_doctest _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    get_filename_component(_root_ ${_file_} PATH)
    get_filename_component(_root_ ${_root_} NAME)
    if(NOT ${_root_} MATCHES "doc")
        set(_name_ "${_root_}-${_name_}")
    endif()

    chfl_doctest(${_name_} ${_file_})
    if(CHFL_BUILD_DOCTESTS)
        target_link_libraries(${_name_} test_helpers)
    endif()
endfunction()

file(GLOB_RECURSE doctest_files ${CMAKE_CURRENT_SOURCE_DIR}/doc/**.cpp)
foreach(test_file IN LISTS doctest_files)
    chfl_cpp_doctest(${test_file})
endforeach(test_file)

file(GLOB_RECURSE capi_doctest_files ${CMAKE_CURRENT_SOURCE_DIR}/capi/doc/**.c)
foreach(test_file IN LISTS capi_doctest_files)
    chfl_c_doctest(${test_file})
endforeach(test_file)

foreach(test IN LISTS CHEMFILES_IGNORED_TESTS)
    set_tests_properties(${test} PROPERTIES WILL_FAIL TRUE)
endforeach(test)

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

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/local-file.toml
"[types]
Oz = \"F\"
")
