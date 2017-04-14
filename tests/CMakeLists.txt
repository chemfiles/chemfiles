
# Update this value if you need to update the data file set
set(TESTS_DATA_GIT "27d89caa55e1f42cdbbed5a322b51c702066d871")

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/data/${TESTS_DATA_GIT}")
    message(STATUS "Downloading test data files")
    file(DOWNLOAD
        "https://github.com/chemfiles/tests-data/archive/${TESTS_DATA_GIT}.tar.gz"
        "${CMAKE_CURRENT_BINARY_DIR}/${TESTS_DATA_GIT}.tar.gz"
        SHOW_PROGRESS
        EXPECTED_HASH SHA1=f16520453515f21ca838c508eea9d38ac4877443
    )

    message(STATUS "Unpacking test data files")
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
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/)
include_directories(SYSTEM ${PROJECT_BINARY_DIR}/external/tng/include)
include_directories(SYSTEM ${PROJECT_BINARY_DIR}/external/molfiles/include)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR}/external/boost/include)
add_library(test_helpers STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/helpers.cpp)

function(check_dll_export_tests _name_ _result_)
    set(NON_DLL_EXPORT_TESTS tng-file basic-file ncfile)
    list(FIND NON_DLL_EXPORT_TESTS ${_name_} _index_)
    if (${_index_} GREATER -1)
        set(${_result_} TRUE PARENT_SCOPE)
    else()
        set(${_result_} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(chfl_test _name_ _file_)
    check_dll_export_tests(${_name_} _is_not_dll_exported_)
    if(${_is_not_dll_exported_})
        add_executable(${_name_} ${_file_} $<TARGET_OBJECTS:files_impl>)
    else()
        add_executable(${_name_} ${_file_})
    endif()

    target_link_libraries(${_name_} chemfiles boost_filesystem test_helpers)
    set_property(TARGET ${_name_} PROPERTY CXX_STANDARD 11)
    add_test(NAME ${_name_}
        COMMAND ${RUNNER_COMMAND} ./${_name_}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    # We need to set the path to allow access to chemfiles.dll (and any other
    # DLL)
    if(WIN32)
        STRING(REPLACE ";" "\\;" PATH_STRING "$ENV{PATH}")
        set_tests_properties(${_name_}
            PROPERTIES ENVIRONMENT "PATH=${PATH_STRING}\;$<TARGET_FILE_DIR:chemfiles>"
        )
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
    target_compile_definitions(${_name_} PRIVATE "-DSRCDIR=\"${PROJECT_SOURCE_DIR}\"")
endfunction()

file(GLOB capi_test_files ${CMAKE_CURRENT_SOURCE_DIR}/capi/*.cpp)
foreach(test_file IN LISTS capi_test_files)
    chfl_capi_test(${test_file})
endforeach(test_file)

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
