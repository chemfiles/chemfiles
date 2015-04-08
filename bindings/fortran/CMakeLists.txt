
enable_language(Fortran)
add_library(chemharpf SHARED chemharp.f90)
target_link_libraries(chemharpf chemharp)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/chemharp.mod DESTINATION include)
install(TARGETS chemharpf DESTINATION lib)


function(chrp_fortran_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    set(_name_ "f-${_name_}")
    add_executable(${_name_} ${_file_})
    target_link_libraries(${_name_} chemharp chemharpf)
    add_test(NAME ${_name_} COMMAND ${_name_})
endfunction()

if(BUILD_TESTS)
    file(GLOB f_test_files ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.f90)

    foreach(test_file IN LISTS f_test_files)
        chrp_fortran_test(${test_file})
    endforeach(test_file)
endif()
