include(ExternalProject)

#----------------------------------------------------------------------------------------#
set(BOOST_VERSION 1.58.0)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/boost/LICENSE_1_0.txt")
    message(STATUS "Unpacking Boost sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/boost.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

include_directories(BEFORE SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/boost)

file(GLOB_RECURSE boost_system_src boost/libs/system/src/**.cpp)
add_library(boost_system STATIC ${boost_system_src})
set_property(TARGET boost_system PROPERTY VERSION ${BOOST_VERSION})
set_property(TARGET boost_system PROPERTY POSITION_INDEPENDENT_CODE true)

file(GLOB_RECURSE boost_filesystem_src boost/libs/filesystem/src/**.cpp)
add_library(boost_filesystem STATIC ${boost_filesystem_src})
set_property(TARGET boost_filesystem PROPERTY POSITION_INDEPENDENT_CODE true)

#----------------------------------------------------------------------------------------#
if(${NETCDF_FOUND})
    include_directories(${NETCDF_INCLUDES})
    ExternalProject_add(netcdfcxx
        PREFIX netcdfcxx
        URL file://${CMAKE_CURRENT_SOURCE_DIR}/netcdfcxx.tar.gz
        URL_MD5 a50ba13ff6593c3c9293fd205addf288
        CMAKE_ARGS  -DBUILD_SHARED_LIBS=OFF
                    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                    -DBUILD_TESTING=OFF
                    -DNCXX_ENABLE_TESTS=OFF
                    -DCMAKE_BUILD_TYPE=release
                    # Setting compilers
                    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                    -DCMAKE_Fortran_COMPILER=${CMAKE_Fortran_COMPILER}
                    # Setting installation directory
                    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
                    # Support cross-compilation
                    -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                    -DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH}
                    -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=${CMAKE_FIND_ROOT_PATH_MODE_LIBRARY}
                    -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=${CMAKE_FIND_ROOT_PATH_MODE_INCLUDE}
                    -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=${CMAKE_FIND_ROOT_PATH_MODE_PROGRAM}
    )
    add_library(netcdf_cxx4 STATIC IMPORTED GLOBAL)
    set_property(
        TARGET netcdf_cxx4
        PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libnetcdf-cxx4${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
endif()

#----------------------------------------------------------------------------------------#
ExternalProject_Add(molfiles
    PREFIX molfiles
    URL file://${CMAKE_CURRENT_SOURCE_DIR}/molfiles.tar.gz
    URL_HASH SHA1=266b3e1b95ca422428f6a723aab1fcab0ca7b04d
    CMAKE_ARGS  -DCMAKE_BUILD_TYPE=release
                # Setting compilers
                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DCMAKE_Fortran_COMPILER=${CMAKE_Fortran_COMPILER}
                # Setting installation directory
                -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
                # Support cross-compilation
                -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                -DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH}
                -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=${CMAKE_FIND_ROOT_PATH_MODE_LIBRARY}
                -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=${CMAKE_FIND_ROOT_PATH_MODE_INCLUDE}
                -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=${CMAKE_FIND_ROOT_PATH_MODE_PROGRAM}
)
