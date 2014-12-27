cmake_minimum_required(VERSION 3.0.0)

project(harp)
set(HARP_VERSION_MAJOR 0)
set(HARP_VERSION_MINOR 1)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

enable_testing()

add_subdirectory(doc)
add_subdirectory(tests)

set(HAVE_NETCDF 1)

file(
    GLOB_RECURSE
    sources
    src/**.cpp
)

configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.in.hpp"
  "${PROJECT_SOURCE_DIR}/include/config.hpp"
  )

include_directories(include)

add_library(harp ${sources})
