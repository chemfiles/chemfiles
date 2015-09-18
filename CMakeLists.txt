cmake_minimum_required(VERSION 2.8.12)
project(chemharp C CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
if(${CMAKE_VERSION} VERSION_GREATER 3.0.0)
    cmake_policy(SET CMP0042 NEW)
endif()

#----------------------------------------------------------------------------------------#
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)

file(READ ${PROJECT_SOURCE_DIR}/VERSION CHRP_VERSION)
string(STRIP ${CHRP_VERSION} CHRP_VERSION)
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" CHRP_VERSION_MAJOR "${CHRP_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" CHRP_VERSION_MINOR "${CHRP_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" CHRP_VERSION_PATCH "${CHRP_VERSION}")
set(CHRP_VERSION_SHORT "${CHRP_VERSION_MAJOR}.${CHRP_VERSION_MINOR}.${CHRP_VERSION_PATCH}")

option(BUILD_TESTS "Build unit tests." OFF)
option(BUILD_FRONTEND "Build the binary frontend." ON)
option(CODE_COVERAGE "Enable code coverage" OFF)
option(PYTHON_BINDING "Build python interface to Chemharp." OFF)
option(FORTRAN_BINDING "Build FORTRAN interface to Chemharp." OFF)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static ones" OFF)

if(${FORTRAN_BINDING})
    enable_language(Fortran)
endif()
if(${PYTHON_BINDING} AND NOT ${BUILD_SHARED_LIBS})
    message(SEND_ERROR "The Python binding can not be built using static libs. Please set BUILD_SHARED_LIBS to ON.")
endif()

include(CompilerFlags)
include(Platforms)

if(CODE_COVERAGE)
    set(BUILD_TESTS ON)
    message(STATUS "Code coverage enabled")
    if(NOT CMAKE_COMPILER_IS_GNUCXX)
        message(FATAL_ERROR "Code coverage can only be used with GCC")
    endif()
    # Code coverage should use gcc
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
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
endif()

add_subdirectory(external)

include_directories(include)
include_directories(BEFORE SYSTEM ${PROJECT_BINARY_DIR}/include)
include_directories(BEFORE SYSTEM ${PROJECT_SOURCE_DIR}/external/boost)

file(GLOB_RECURSE sources src/**.cpp)
add_library(chemharp ${sources} bindings/c/capi.cpp $<TARGET_OBJECTS:boost_system> $<TARGET_OBJECTS:boost_filesystem>)

set_property(TARGET chemharp PROPERTY VERSION ${CHRP_VERSION_SHORT})
set_property(TARGET chemharp PROPERTY SOVERSION ${CHRP_VERSION_SHORT})

include(GenerateExportHeader)
generate_export_header(chemharp
          BASE_NAME CHRP
          EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/include/chemharp/exports.hpp
)
target_link_libraries(chemharp ${OTHER_CHRP_LIBRARIES})

configure_file (
  "${PROJECT_SOURCE_DIR}/include/chemharp/config.in.hpp"
  "${PROJECT_SOURCE_DIR}/include/chemharp/config.hpp"
)

if(${HAVE_NETCDF})
    add_dependencies(chemharp netcdfcxx)
    target_link_libraries(chemharp ${NETCDF_LIBRARIES} netcdf_cxx4)
endif()

add_dependencies(chemharp molfiles)
add_definitions("-DCHRP_MOLFILES_DIR=\"${CMAKE_INSTALL_PREFIX}/molfiles/\"")

#----------------------------------------------------------------------------------------#
# Installation configuration
#----------------------------------------------------------------------------------------#

file(GLOB_RECURSE headers include/chemharp/*.hpp)

# All headers
install(FILES ${headers} DESTINATION include/chemharp/)
# C++ main headers
install(FILES include/Chemharp.hpp DESTINATION include)
# C main header
install(FILES bindings/c/chemharp.h DESTINATION include)
# Dynamic library
install(TARGETS chemharp DESTINATION lib)

file(GLOB molfiles ${PROJECT_BINARY_DIR}/lib/*plugin.so)
install(FILES ${molfiles} DESTINATION lib/molfiles/
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ             GROUP_EXECUTE
                    WORLD_READ             WORLD_EXECUTE)

# uninstall target
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/uninstall.in.cmake"
    "${PROJECT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${PROJECT_BINARY_DIR}/uninstall.cmake)

#----------------------------------------------------------------------------------------#
# Packaging stuff
#----------------------------------------------------------------------------------------#
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Chemharp, efficient chemistry IO library")
set(CPACK_PACKAGE_VENDOR "Guillaume Fraux")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENCE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${CHRP_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CHRP_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CHRP_VERSION_PATCH})
set(CPACK_GENERATOR "TGZ;ZIP")
include(CPack)

#----------------------------------------------------------------------------------------#
# Sub projects
#----------------------------------------------------------------------------------------#
enable_testing()
add_subdirectory(doc)
add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(bindings)
if(${BUILD_FRONTEND})
    if(EXISTS "${PROJECT_SOURCE_DIR}/bin/CMakeLists.txt")
        add_subdirectory(bin)
    else()
        message(WARNING
        "The binary frontend source does not exist at ${PROJECT_SOURCE_DIR}/bin/, "
        "the frontend will not be built.")
    endif()
endif()
