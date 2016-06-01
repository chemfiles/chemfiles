cmake_minimum_required(VERSION 2.8.11)
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
option(BUILD_DOCUMENTATION "Build the documentation." OFF)
option(CODE_COVERAGE "Enable code coverage" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static ones" OFF)
option(ENABLE_NETCDF "Enable AMBER NetCDF format." OFF)
option(USE_WARNINGS "Compile the code with warnings (default in debug mode)" OFF)

include(CompilerFlags)

if(USE_WARNINGS)
    # Make sure to have asserts compiled
    add_definitions("-UNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CHEMFILES_CXX_WARNINGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CHEMFILES_C_WARNINGS}")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CHEMFILES_CXX_WARNINGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CHEMFILES_C_WARNINGS}")

if(CODE_COVERAGE)
    set(BUILD_TESTS ON)
    message(STATUS "Code coverage enabled")
    if(NOT CMAKE_COMPILER_IS_GNUCXX)
        message(FATAL_ERROR "Code coverage can only be used with GCC")
    endif()
    # Code coverage should use gcc
    set(CMAKE_BUILD_TYPE "debug" CACHE STRING "Type of build: release|debug|relwithdebuginfo" FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'release' as none was specified.")
  set(CMAKE_BUILD_TYPE "release" CACHE STRING "Type of build: release|debug|relwithdebuginfo" FORCE)
endif()

if(NOT CMAKE_CONFIGURATION_TYPES)
    if(${CMAKE_BUILD_TYPE} STREQUAL "debug")
        # Set CMAKE_CXX_FLAGS to pass this definition to all external libraries too
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG")
    endif()
endif()

add_subdirectory(external)

file(GLOB_RECURSE sources src/**.cpp)
add_library(chemfiles ${sources} $<TARGET_OBJECTS:molfiles> $<TARGET_OBJECTS:fmt>)

set_property(TARGET chemfiles PROPERTY VERSION ${CHEMFILES_VERSION_SHORT})
set_property(TARGET chemfiles PROPERTY SOVERSION ${CHEMFILES_VERSION_SHORT})

target_include_directories(chemfiles PUBLIC include)
target_include_directories(chemfiles PUBLIC external/molfiles/include)
target_include_directories(chemfiles PUBLIC external/fmt)

if(${ENABLE_NETCDF})
    find_package(NetCDF REQUIRED)
    target_include_directories(chemfiles PUBLIC ${NETCDF_INCLUDES})
    target_link_libraries(chemfiles ${NETCDF_LIBRARIES})
    set(HAVE_NETCDF 1)
else()
    find_package(NetCDF)
    if(${NetCDF_FOUND})
        message(STATUS "Found NetCDF library. Consider activating the associated format with -DENABLE_NETCDF=ON")
    endif()
    set(HAVE_NETCDF 0)
endif()

configure_file (
  "${PROJECT_SOURCE_DIR}/include/chemfiles/config.in.hpp"
  "${PROJECT_SOURCE_DIR}/include/chemfiles/config.hpp"
)

include(GenerateExportHeader)
generate_export_header(chemfiles
    BASE_NAME CHFL
    EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/include/chemfiles/exports.hpp
)

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

# uninstall target
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/uninstall.in.cmake"
    "${PROJECT_BINARY_DIR}/uninstall.cmake"
    @ONLY
)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${PROJECT_BINARY_DIR}/uninstall.cmake)

# Testing
enable_testing()
if(BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()
if(BUILD_TESTS)
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()
