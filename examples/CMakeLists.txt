function(chrp_cpp_example _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_executable(example-cpp-${_name_} ${_file_})
    target_link_libraries(example-cpp-${_name_} chemharp)
endfunction()

function(chrp_c_example _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_executable(example-c-${_name_} ${_file_})
    target_link_libraries(example-c-${_name_} chemharp)
endfunction()

function(chrp_fortran_example _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_executable(example-f90-${_name_} ${_file_})
    target_link_libraries(example-f90-${_name_} chemharp chemharpf)



endfunction()


if(BUILD_TESTS)
    file(GLOB cpp_examples cpp/*.cpp)
    foreach(example IN LISTS cpp_examples)
        chrp_cpp_example(${example})
    endforeach()

    file(GLOB c_examples c/*.c)
    include_directories(${CMAKE_SOURCE_DIR}/bindings/c)
    foreach(example IN LISTS c_examples)
        chrp_c_example(${example})
    endforeach()

    if(${FORTRAN_BINDING})
        include_directories(${CMAKE_BINARY_DIR}/bindings/fortran)
        file(GLOB fortran_examples fortran/*.f90)
        foreach(example IN LISTS fortran_examples)
            chrp_fortran_example(${example})
        endforeach()
    endif()
endif()
