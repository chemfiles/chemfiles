cmake_minimum_required(VERSION 2.8.12)
include(ExternalProject)

project(chemharp C CXX)
set(CHRP_VERSION_MAJOR 0)
set(CHRP_VERSION_MINOR 2)
set(CHRP_VERSION_PATCH 0)
set(CHRP_VERSION "${CHRP_VERSION_MAJOR}.${CHRP_VERSION_MINOR}.${CHRP_VERSION_PATCH}")

option(BUILD_TESTS "Build harp tests." OFF)
option(CODE_COVERAGE "Build code coverage" OFF)
option(PYTHON_BINDING "Build python interface to Chemharp." OFF)
option(FORTRAN_BINDING "Build FORTRAN interface to Chemharp." OFF)

if(${FORTRAN_BINDING})
    enable_language(Fortran)
endif()

if(CODE_COVERAGE)
    set(BUILD_TESTS ON)
    message(STATUS "Code coverage enabled")
    # Code coverage should use gcc
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
if(${CMAKE_VERSION} VERSION_GREATER 3.0.0)
    cmake_policy(SET CMP0042 NEW)
endif()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'release' as none was specified.")
  set(CMAKE_BUILD_TYPE "release" CACHE STRING "Choose the type of build." FORCE)
endif()

set(HAVE_NETCDF 0)
find_package(NetCDF)

if(${NETCDF_FOUND})
    set(HAVE_NETCDF 1)
    include_directories(${NETCDF_INCLUDES})
    #==========================================================================#
    # NetCDF CXX4 library will be built as an external project
    ExternalProject_add(netcdfcxx
        PREFIX external/netcdfcxx
        URL "https://github.com/Luthaf/netcdf-cxx4/archive/open-close.tar.gz"
        URL_MD5 eebe475d39f3a4a42a783a7045132a0d
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND
            COMMAND autoreconf -if
            COMMAND ./configure --prefix=${CMAKE_BINARY_DIR} --with-pic
                    CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
        BUILD_COMMAND make
    )
    include_directories(BEFORE SYSTEM ${CMAKE_BINARY_DIR}/include)
    add_library(netcdf_cxx4 STATIC IMPORTED)
    set_property(
        TARGET netcdf_cxx4
        PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libnetcdf_c++4${CMAKE_STATIC_LIBRARY_SUFFIX})
    #==========================================================================#
endif()

option(USE_STATIC_BOOST "Force use of static BOOST libraries" NO)
set(Boost_USE_STATIC_LIBS ${USE_STATIC_BOOST})
set(Boost_USE_MULTITHREADED OFF)
if(USE_STATIC_BOOST)
   unset(Boost_INCLUDE_DIR CACHE)
   unset(Boost_LIBRARY_DIRS CACHE)
   message(STATUS "Linking against boost static libraries")
else()
   unset(Boost_INCLUDE_DIR CACHE)
   unset(Boost_LIBRARY_DIRS CACHE)
endif()
find_package(Boost 1.54.0 REQUIRED COMPONENTS filesystem system)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)
include(CompilerFlags)

include_directories(include)
file(GLOB_RECURSE sources src/**.cpp)

configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.in.hpp"
  "${PROJECT_SOURCE_DIR}/include/config.hpp"
)

set(CHEMHARP_PREFIX "")
configure_file ( # This version of the file is used when compiling the C interface
  "${PROJECT_SOURCE_DIR}/include/Chemharp.in.hpp"
  "${PROJECT_SOURCE_DIR}/include/Chemharp.hpp"
)
set(CHEMHARP_PREFIX "chemharp/")
configure_file ( # This version of the file is used for installations
  "${PROJECT_SOURCE_DIR}/include/Chemharp.in.hpp"
  "${CMAKE_BINARY_DIR}/Chemharp.install.hpp"
)

# ============================================================================ #
include(ExternalProject)
ExternalProject_Add(molfile-plugins
    PREFIX external/molfile
    URL https://github.com/Luthaf/VMD-Molfiles/archive/1.9.2.tar.gz
    URL_HASH SHA1=92c094a2536d97d953df03fe14a838382b3ebe5b
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
               -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
)
include_directories(${CMAKE_BINARY_DIR}/include)
# ============================================================================ #

include(Platforms)

add_library(chemharp SHARED ${sources} bindings/c/capi.cpp)

set_property(TARGET chemharp PROPERTY VERSION ${CHRP_VERSION})
set_property(TARGET chemharp PROPERTY SOVERSION ${CHRP_VERSION})

target_link_libraries(chemharp ${Boost_LIBRARIES} ${OTHER_CHRP_LIBRARIES})

if(${HAVE_NETCDF})
    add_dependencies(chemharp netcdfcxx)
    target_link_libraries(chemharp ${NETCDF_LIBRARIES} netcdf_cxx4)
endif()

add_dependencies(chemharp molfile-plugins)
add_definitions("-DCHRP_MOLFILES_DIR=\"${CMAKE_INSTALL_PREFIX}/molfiles/\"")

#==============================================================================#
# Installation configuration
#==============================================================================#

file(GLOB_RECURSE headers include/**.hpp)

# All headers
install(FILES ${headers} DESTINATION include/chemharp)
# C++ main headers
install(FILES "${CMAKE_BINARY_DIR}/Chemharp.install.hpp" DESTINATION include RENAME Chemharp.hpp)
# C main header
install(FILES bindings/c/chemharp.h DESTINATION include)
# Dynamic library
install(TARGETS chemharp DESTINATION lib)

file(GLOB_RECURSE molfiles ${CMAKE_BINARY_DIR}/lib/*plugin.so)
install(FILES ${molfiles} DESTINATION lib/molfiles/
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ             GROUP_EXECUTE
                    WORLD_READ             WORLD_EXECUTE)

add_subdirectory(doc)
enable_testing()
add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(bindings)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.in.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

#========================================================================================#
# Packaging stuff
#========================================================================================#
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Chemharp, efficient chemistry IO library")
set(CPACK_PACKAGE_VENDOR "Guillaume Fraux")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENCE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${CHRP_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CHRP_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CHRP_VERSION_PATCH})
set(CPACK_GENERATOR "TGZ;ZIP")
include(CPack)
