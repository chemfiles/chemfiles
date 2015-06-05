#==============================================================================#
# Boost.Numpy library will be built as an external project
ExternalProject_add(Boost.Numpy
    PREFIX external/Boost.Numpy
    GIT_REPOSITORY https://github.com/ndarray/Boost.NumPy
    CMAKE_ARGS  -DBUILD_EXAMPLES=OFF
                -DBUILD_TESTS=OFF
                -DLIBRARY_TYPE=STATIC
                -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DBoost_USE_STATIC_LIBS=${Boost_USE_STATIC_LIBS}
                -DBoost_USE_MULTITHREADED=${Boost_USE_MULTITHREADED}
    UPDATE_COMMAND ""
)
add_library(boost_numpy STATIC IMPORTED)
set_property(TARGET boost_numpy
             PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libboost_numpy.a)
#==============================================================================#

find_package(Boost REQUIRED COMPONENTS python)
find_package(PythonLibsNew REQUIRED)

include_directories(SYSTEM ${PYTHON_INCLUDE_DIRS})

file(GLOB python_sources *.cpp)

PYTHON_ADD_MODULE(chemharp_python ${python_sources})
add_dependencies(chemharp_python Boost.Numpy)
target_link_libraries(chemharp_python
    chemharp
    boost_numpy
    ${Boost_LIBRARIES}
)

add_custom_command(TARGET chemharp_python POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       $<TARGET_FILE:chemharp_python>
                       chemharp.so
                   COMMENT "Copying Python binding to 'chemharp.so'")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/chemharp.so
        DESTINATION ${PYTHON_SITE_PACKAGES})

function(chrp_python_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_test(NAME python-${_name_} COMMAND ${PYTHON_EXECUTABLE} ${_file_})
    set_tests_properties(python-${_name_}
        PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}")
endfunction()

if(BUILD_TESTS)
    file(GLOB python_test_files ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py)

    foreach(test_file IN LISTS python_test_files)
        chrp_python_test(${test_file})
    endforeach(test_file)
endif()
