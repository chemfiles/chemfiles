cmake_minimum_required(VERSION 3.0.0)

project(harp)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(HAVE_NETCDF 0)
set(NETCDF_CXX "YES")
find_package(NetCDF)
if(${NETCDF_FOUND})
    set(HAVE_NETCDF 1)
endif()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS filesystem system)

set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)

include(HarpCompilerFlags)

set(HARP_VERSION_MAJOR 0)
set(HARP_VERSION_MINOR 1)

include_directories(include)
file(
    GLOB_RECURSE
    sources
    src/**.cpp
)
configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.in.hpp"
  "${PROJECT_SOURCE_DIR}/include/config.hpp"
)

add_library(harp SHARED ${sources})
target_link_libraries(harp ${Boost_LIBRARIES})

add_subdirectory(doc)
enable_testing()
add_subdirectory(tests)
