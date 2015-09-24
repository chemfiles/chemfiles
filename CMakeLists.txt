cmake_minimum_required(VERSION 2.8.7)
project(chemfiles C CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
if(${CMAKE_VERSION} VERSION_GREATER 3.0.0)
    cmake_policy(SET CMP0042 NEW)
endif()

#----------------------------------------------------------------------------------------#
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)

file(READ ${PROJECT_SOURCE_DIR}/VERSION CHEMFILES_VERSION)
string(STRIP ${CHEMFILES_VERSION} CHEMFILES_VERSION)
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" CHEMFILES_VERSION_MAJOR "${CHEMFILES_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" CHEMFILES_VERSION_MINOR "${CHEMFILES_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" CHEMFILES_VERSION_PATCH "${CHEMFILES_VERSION}")
set(CHEMFILES_VERSION_SHORT "${CHEMFILES_VERSION_MAJOR}.${CHEMFILES_VERSION_MINOR}.${CHEMFILES_VERSION_PATCH}")

option(BUILD_TESTS "Build unit tests." OFF)
option(BUILD_FRONTEND "Build the binary frontend." OFF)
option(BUILD_DOCUMENTATION "Build the documentation." OFF)
option(CODE_COVERAGE "Enable code coverage" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static ones" OFF)
option(ENABLE_NETCDF "Enable AMBER NetCDF format." OFF)

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

if(${ENABLE_NETCDF})
    find_package(NetCDF REQUIRED)
    include_directories(${NETCDF_INCLUDES})
    SET(HAVE_NETCDF 1)
else()
    find_package(NetCDF)
    if(${NetCDF_FOUND})
        message(STATUS "Found NetCDF library. Consider activating the associated format with -DENABLE_NETCDF=ON")
    endif()
    SET(HAVE_NETCDF 0)
endif()

add_subdirectory(external)
include_directories(include)
include_directories(BEFORE SYSTEM ${PROJECT_BINARY_DIR}/include)

file(GLOB_RECURSE sources src/**.cpp)
add_library(chemfiles ${sources})

set_property(TARGET chemfiles PROPERTY VERSION ${CHEMFILES_VERSION_SHORT})
set_property(TARGET chemfiles PROPERTY SOVERSION ${CHEMFILES_VERSION_SHORT})

include(GenerateExportHeader)
generate_export_header(chemfiles
          BASE_NAME CHFL
          EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/include/chemfiles/exports.hpp
)
target_link_libraries(chemfiles ${OTHER_CHEMFILES_LIBRARIES})

configure_file (
  "${PROJECT_SOURCE_DIR}/include/chemfiles/config.in.hpp"
  "${PROJECT_SOURCE_DIR}/include/chemfiles/config.hpp"
)

if(${ENABLE_NETCDF})
    add_dependencies(chemfiles netcdfcxx)
    target_link_libraries(chemfiles ${NETCDF_LIBRARIES} netcdf_cxx4)
endif()

add_dependencies(chemfiles molfiles)
add_definitions("-DINSTALL_MOLFILE_DIR=\"${CMAKE_INSTALL_PREFIX}/lib/molfiles/\"")

#----------------------------------------------------------------------------------------#
# Installation configuration
#----------------------------------------------------------------------------------------#

file(GLOB_RECURSE headers include/chemfiles/*.hpp)

# All headers
install(FILES ${headers} DESTINATION include/chemfiles/)
# C++ main headers
install(FILES include/chemfiles.hpp DESTINATION include)
# C main header
install(FILES include/chemfiles.h DESTINATION include)
# Libraries
install(TARGETS chemfiles DESTINATION lib)

file(GLOB molfiles ${PROJECT_BINARY_DIR}/lib/*plugin.so)
install(FILES ${molfiles} DESTINATION lib/molfiles/
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ             GROUP_EXECUTE
                    WORLD_READ             WORLD_EXECUTE)

# uninstall target
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/uninstall.in.cmake"
    "${PROJECT_BINARY_DIR}/uninstall.cmake"
    @ONLY
)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${PROJECT_BINARY_DIR}/uninstall.cmake)

#----------------------------------------------------------------------------------------#
# Packaging stuff
#----------------------------------------------------------------------------------------#
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Chemfiles, an efficient chemistry IO library")
set(CPACK_PACKAGE_VENDOR "Guillaume Fraux")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENCE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${CHEMFILES_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CHEMFILES_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CHEMFILES_VERSION_PATCH})
set(CPACK_GENERATOR "TGZ;ZIP")
include(CPack)

#----------------------------------------------------------------------------------------#
# Sub projects
#----------------------------------------------------------------------------------------#
enable_testing()
if(BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()
if(BUILD_TESTS)
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()
if(BUILD_FRONTEND)
    if(EXISTS "${PROJECT_SOURCE_DIR}/bin/CMakeLists.txt")
        add_subdirectory(bin)
    else()
        message(WARNING
        "The binary frontend source does not exist at ${PROJECT_SOURCE_DIR}/bin/, "
        "the frontend will not be built.")
    endif()
endif()
