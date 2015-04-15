#==============================================================================#
# Boost.Numpy library will be built as an external project
ExternalProject_add(Boost.Numpy
    PREFIX external/Boost.Numpy
    GIT_REPOSITORY https://github.com/ndarray/Boost.NumPy
    CMAKE_ARGS  -DBUILD_EXAMPLES=OFF
                -DBUILD_TESTS=OFF
                -DLIBRARY_TYPE=STATIC
                -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
    UPDATE_COMMAND ""
)
include_directories(${CMAKE_BINARY_DIR}/include)
add_library(boost_numpy STATIC IMPORTED)
set_property(TARGET boost_numpy
             PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libboost_numpy.a)
#==============================================================================#

find_package(Boost REQUIRED COMPONENTS python)
find_package(PythonLibsNew REQUIRED)

include_directories(SYSTEM ${PYTHON_INCLUDE_DIRS})

file(GLOB python_sources *.cpp)

PYTHON_ADD_MODULE(_chemharp ${python_sources})
add_dependencies(_chemharp Boost.Numpy)
target_link_libraries(_chemharp
    chemharp
    boost_numpy
    ${Boost_LIBRARIES}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/_chemharp.so
        DESTINATION ${PYTHON_SITE_PACKAGES}
        RENAME chemharp.so)

function(chrp_python_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_test(NAME python-${_name_} COMMAND ${PYTHON_EXECUTABLE} ${_file_})
endfunction()

if(BUILD_TESTS)
    file(GLOB python_test_files ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py)

    foreach(test_file IN LISTS python_test_files)
        chrp_python_test(${test_file})
    endforeach(test_file)
endif()
