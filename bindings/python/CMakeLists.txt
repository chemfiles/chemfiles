include(ExternalProject)

#==============================================================================#
# Boost.Numpy library will be built as an external project
ExternalProject_add(
    Boost.Numpy
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

PYTHON_ADD_MODULE(_chemharp chemharp.cpp)
add_dependencies(_chemharp boost_numpy)
target_link_libraries(_chemharp
    chemharp
    boost_numpy
    ${Boost_LIBRARIES}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/_chemharp.so
        DESTINATION ${PYTHON_SITE_PACKAGES}
        RENAME chemharp.so)
