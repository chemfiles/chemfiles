if(${PYTHON_BINDING})
    add_subdirectory(python)
    configure_file (
      "${PROJECT_SOURCE_DIR}/scripts/conda-recipe/build.in.sh"
      "${PROJECT_SOURCE_DIR}/scripts/conda-recipe/build.sh"
    )
    configure_file (
      "${PROJECT_SOURCE_DIR}/scripts/conda-recipe/bld.in.bat"
      "${PROJECT_SOURCE_DIR}/scripts/conda-recipe/bld.bat"
    )
    configure_file (
      "${PROJECT_SOURCE_DIR}/scripts/conda-recipe/meta.in.yaml"
      "${PROJECT_SOURCE_DIR}/scripts/conda-recipe/meta.yaml"
    )
endif()

function(chrp_c_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    set(_name_ "c-${_name_}")
    add_executable(${_name_} ${_file_})
    target_link_libraries(${_name_} chemharp)
    add_test(NAME ${_name_} COMMAND ${_name_} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

if(BUILD_TESTS)
    file(GLOB c_test_files ${CMAKE_CURRENT_SOURCE_DIR}/c/tests/*.c)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/c)
    add_definitions("-DDATADIR=\"${CMAKE_SOURCE_DIR}/tests/data/\"")
    foreach(test_file IN LISTS c_test_files)
        chrp_c_test(${test_file})
    endforeach(test_file)
endif()

if(${FORTRAN_BINDING})
    add_subdirectory(fortran)
endif()
