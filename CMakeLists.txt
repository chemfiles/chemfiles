cmake_minimum_required(VERSION 3.0.0)

project(chemharp)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(HAVE_NETCDF 0)
set(NETCDF_CXX "YES")
find_package(NetCDF)
if(${NETCDF_FOUND})
    set(HAVE_NETCDF 1)
    include_directories(${NETCDF_INCLUDES})
endif()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)

include(CompilerFlags)

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

add_library(chemharp SHARED ${sources} bindings/c/capi.cpp)
target_link_libraries(chemharp ${Boost_LIBRARIES})
if(${NETCDF_FOUND})
    target_link_libraries(chemharp ${NETCDF_LIBRARIES})
endif()

add_subdirectory(doc)
enable_testing()
add_subdirectory(tests)
