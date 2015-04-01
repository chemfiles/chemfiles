option(PYTHON_BINDING "Build python binding." OFF)

if(${PYTHON_BINDING})
    add_subdirectory(python)
endif()


function(chrp_c_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_executable(${_name_} ${_file_})
    target_link_libraries(${_name_} chemharp)
    add_test(NAME c-${_name_} COMMAND ${_name_})
endfunction()

function(chrp_python_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_test(NAME python-${_name_} COMMAND ${PYTHON_EXECUTABLE} ${_file_})
endfunction()

if(BUILD_TESTS)
    file(GLOB c_test_files ${CMAKE_CURRENT_SOURCE_DIR}/c/tests/*.c)
    foreach(test_file IN LISTS c_test_files)
        chrp_c_test(${test_file})
    endforeach(test_file)

    if(${PYTHON_BINDING})
        file(GLOB python_test_files ${CMAKE_CURRENT_SOURCE_DIR}/python/tests/*.py)
        foreach(test_file IN LISTS python_test_files)
            chrp_python_test(${test_file})
        endforeach(test_file)
    endif()
endif()
