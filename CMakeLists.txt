cmake_minimum_required(VERSION 2.8)

project(chemharp)
set(HARP_VERSION_MAJOR 0)
set(HARP_VERSION_MINOR 1)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(HAVE_NETCDF 0)
set(NETCDF_CXX "YES")
find_package(NetCDF)
if(${NETCDF_FOUND})
    set(HAVE_NETCDF 1)
    include_directories(${NETCDF_INCLUDES})
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

add_library(chemharp SHARED ${sources} bindings/c/capi.cpp)
target_link_libraries(chemharp ${Boost_LIBRARIES})
if(${HAVE_NETCDF})
    target_link_libraries(chemharp ${NETCDF_LIBRARIES})
endif()

file(GLOB_RECURSE headers include/**.hpp)

# All headers
install(FILES ${headers} DESTINATION include/chemharp)
# C++ main headers
install(FILES "${CMAKE_BINARY_DIR}/Chemharp.install.hpp" DESTINATION include RENAME Chemharp.hpp)
# C main header
install(FILES bindings/c/chemharp.h DESTINATION include)
# Dynamic library
install(TARGETS chemharp DESTINATION lib)

add_subdirectory(doc)
enable_testing()
add_subdirectory(tests)
add_subdirectory(bindings)
