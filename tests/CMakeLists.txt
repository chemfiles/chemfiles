# Update this value if you need to update the data file set
set(TESTS_DATA_GIT "7610e0ba23bd8b03a4f74b039f8fe5f6b0a4873a")

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/data/${TESTS_DATA_GIT}")
    message(STATUS "Downloading test data files")
    file(DOWNLOAD
        "https://github.com/chemfiles/tests-data/archive/${TESTS_DATA_GIT}.tar.gz"
        "${CMAKE_CURRENT_BINARY_DIR}/${TESTS_DATA_GIT}.tar.gz"
        SHOW_PROGRESS
        EXPECTED_HASH SHA1=a2da9634db29dc5a51f084a32b9cf4a35025b324
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

option(CHFL_TESTS_USE_VALGRIND OFF "Run tests using valgrind")
if(CHFL_TESTS_USE_VALGRIND)
    find_program(VALGRIND_EXECUTABLE valgrind)
    set(
        TEST_RUNNER
        ${VALGRIND_EXECUTABLE} "--dsymutil=yes" "--error-exitcode=125"
        "--leak-check=full" "--show-leak-kinds=all" "--track-origins=yes"
        "--suppressions=${CMAKE_CURRENT_SOURCE_DIR}/valgrind.supp"
        # don't warn for replacement of operator new/delete
        "--soname-synonyms=somalloc=nouserintercepts"
    )
else()
    set(TEST_RUNNER "")
endif()

# Pick up CMAKE_CROSSCOMPILING_EMULATOR to run tests if it is defined
set(TEST_RUNNER "${CMAKE_CROSSCOMPILING_EMULATOR};${TEST_RUNNER}")

if(MSVC)
    string(REGEX REPLACE "/Wall" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/Wall" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

add_subdirectory(external)

function(chfl_define_test _name_)
    add_test(NAME ${_name_}
        COMMAND ${TEST_RUNNER} ./${_name_}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    set_tests_properties(${_name_}
        PROPERTIES ENVIRONMENT "CHFL_TESTS_USE_VALGRIND=${CHFL_TESTS_USE_VALGRIND}"
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

find_package(Threads REQUIRED)

function(chfl_unit_test _name_ _file_)
    add_executable(${_name_} ${_file_} ${CHEMFILES_OBJECTS})
    # We need to pretend we are inside the DLL to access all functions
    target_compile_definitions(${_name_} PRIVATE chemfiles_EXPORTS)
    target_link_libraries(${_name_} $<TARGET_PROPERTY:chemfiles,LINK_LIBRARIES>)
    set_target_properties(${_name_} PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(${_name_} SYSTEM PRIVATE ${EXTERNAL_INCLUDES})

    if(THREADS_HAVE_PTHREAD_ARG)
        target_compile_options(${_name_} PUBLIC "-pthread")
    endif()
    if(CMAKE_THREAD_LIBS_INIT)
        target_link_libraries(${_name_} "${CMAKE_THREAD_LIBS_INIT}")
    endif()

    chfl_define_test(${_name_})
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
    ${CMAKE_CURRENT_SOURCE_DIR}/selections/*.cpp
)

foreach(test_file IN LISTS all_test_files)
    chfl_cpp_test(${test_file})
endforeach()

function(chfl_capi_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    set(_name_ "c-${_name_}")
    chfl_unit_test(${_name_} ${_file_})
    target_link_libraries(${_name_} test_helpers)
endfunction()

file(GLOB capi_test_files ${CMAKE_CURRENT_SOURCE_DIR}/capi/*.cpp)
foreach(test_file IN LISTS capi_test_files)
    chfl_capi_test(${test_file})
endforeach()

# Pass extra data to some tests
file(RELATIVE_PATH VERSION_FILE_PATH ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/VERSION)
target_compile_definitions(c-misc PRIVATE "-DVERSION_FILE_PATH=\"${VERSION_FILE_PATH}\"")
target_compile_definitions(netcdf3-file PRIVATE "-DCMAKE_COMMAND=\"${CMAKE_COMMAND}\"")

function(chfl_doctest _name_ _file_)
    if(NOT ${CHFL_BUILD_DOCTESTS})
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
    target_link_libraries(${_name_} ${ZLIB_LIBRARIES} ${LIBLZMA_LIBRARY})
    set_target_properties(${_name_} PROPERTIES LINKER_LANGUAGE CXX)
    set_target_properties(${_name_} PROPERTIES COMPILE_FLAGS "-UNDEBUG")
    target_include_directories(${_name_} SYSTEM PRIVATE ${EXTERNAL_INCLUDES})

    if(${_run_})
        chfl_define_test(${_name_})
    endif()

    if(${CMAKE_CXX_COMPILER_ID} MATCHES "PGI" AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
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
    if(${CHFL_BUILD_DOCTESTS})
        target_link_libraries(${_name_} test_helpers)
    endif()
endfunction()

file(GLOB_RECURSE doctest_files ${CMAKE_CURRENT_SOURCE_DIR}/doc/**.cpp)
foreach(test_file IN LISTS doctest_files)
    chfl_cpp_doctest(${test_file})
endforeach()

file(GLOB_RECURSE capi_doctest_files ${CMAKE_CURRENT_SOURCE_DIR}/doc/capi/**.c)
foreach(test_file IN LISTS capi_doctest_files)
    chfl_c_doctest(${test_file})
endforeach()

# Code linting with Python scripts
find_package(Python REQUIRED)
set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})

function(chfl_lint _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_test(NAME ${_name_}
        COMMAND ${PYTHON_EXECUTABLE} ${_file_}
    )
endfunction()

file(GLOB lint_scripts ${CMAKE_CURRENT_SOURCE_DIR}/lints/*.py)
foreach(script IN LISTS lint_scripts)
    chfl_lint(${script})
endforeach()
