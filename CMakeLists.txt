cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/InitRules.cmake")

project(chemfiles C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")
if (POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # OSX RPATH handling
endif()

file(READ ${PROJECT_SOURCE_DIR}/VERSION CHEMFILES_VERSION)
string(STRIP ${CHEMFILES_VERSION} CHEMFILES_VERSION)
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" CHEMFILES_VERSION_MAJOR "${CHEMFILES_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" CHEMFILES_VERSION_MINOR "${CHEMFILES_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" CHEMFILES_VERSION_PATCH "${CHEMFILES_VERSION}")
set(CHEMFILES_VERSION_SHORT "${CHEMFILES_VERSION_MAJOR}.${CHEMFILES_VERSION_MINOR}.${CHEMFILES_VERSION_PATCH}")

option(BUILD_SHARED_LIBS "Build shared libraries instead of static ones" OFF)

option(CHFL_BUILD_TESTS "Build unit tests." OFF)
option(CHFL_BUILD_DOCUMENTATION "Build the documentation." OFF)
option(CHFL_CODE_COVERAGE "Enable code coverage" OFF)
option(CHFL_USE_WARNINGS "Compile the code with warnings (default in debug mode)" OFF)
option(CHFL_DEBUG_GLIBCXX "Use the debug version of GNU libstdcxx" OFF)
option(CHFL_SYSTEM_NETCDF "Use the system NetCDF instead of the internal one" OFF)

option(CHFL_BUILD_DOCTESTS "Build documentation tests as well as unit tests." ON)
mark_as_advanced(CHFL_BUILD_DOCTESTS)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'release' as none was specified.")
    set(CMAKE_BUILD_TYPE "release"
        CACHE STRING
        "Choose the type of build, options are: none(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) debug release relwithdebinfo minsizerel."
    FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS release debug relwithdebuginfo minsizerel none)
endif()

include(CompilerFlags)

if(CHFL_USE_WARNINGS)
    # Make sure to have asserts compiled
    add_definitions("-UNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CHEMFILES_CXX_WARNINGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CHEMFILES_C_WARNINGS}")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CHEMFILES_CXX_WARNINGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CHEMFILES_C_WARNINGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CHEMFILES_SANITIZERS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CHEMFILES_SANITIZERS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CHEMFILES_SANITIZERS}")

if(WIN32)
    add_definitions("-DCHEMFILES_WINDOWS")
endif()

if(MSVC)
    add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS")
endif()

if(CHFL_CODE_COVERAGE)
    message(STATUS "Code coverage enabled")
    if(NOT CMAKE_COMPILER_IS_GNUCXX)
        message(FATAL_ERROR "Code coverage can only be used with GCC")
    endif()
    # Code coverage should use gcc
    set(CMAKE_BUILD_TYPE "debug" CACHE STRING "Type of build: release|debug|relwithdebuginfo" FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif()

add_subdirectory(external)

# We need to use a separated library for non-dll-exported classes that have an
# unit test. The chemfiles_objects OBJECT library contains the code for files
# implementation and is linked into both the main chemfiles library and the
# unit tests.
file(GLOB_RECURSE chemfiles_objects_sources src/**.cpp)
add_library(chemfiles_objects OBJECT ${chemfiles_objects_sources})
target_include_directories(chemfiles_objects PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)
target_include_directories(chemfiles_objects SYSTEM BEFORE PRIVATE ${EXTERNAL_INCLUDES})

target_compile_definitions(chemfiles_objects PRIVATE chemfiles_EXPORTS)
if(${BUILD_SHARED_LIBS})
    set_target_properties(chemfiles_objects PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif()

set(CHEMFILES_OBJECTS
    $<TARGET_OBJECTS:chemfiles_objects>
    $<TARGET_OBJECTS:molfiles>
    $<TARGET_OBJECTS:fmt::object>
    $<TARGET_OBJECTS:tng_io>
    $<TARGET_OBJECTS:tng_io_zlib>
    ${NETCDF_OBJECTS}
)

# Add the main chemfiles library
file(GLOB_RECURSE sources src/**.cpp)
add_library(chemfiles ${CHEMFILES_OBJECTS})

set_property(TARGET chemfiles PROPERTY VERSION ${CHEMFILES_VERSION_SHORT})
set_property(TARGET chemfiles PROPERTY SOVERSION ${CHEMFILES_VERSION_SHORT})

target_include_directories(chemfiles PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)
target_include_directories(chemfiles SYSTEM BEFORE PRIVATE ${EXTERNAL_INCLUDES})
target_link_libraries(chemfiles ${NETCDF_LIBRARIES})

configure_file (
  "${PROJECT_SOURCE_DIR}/include/chemfiles/config.in.hpp"
  "${PROJECT_BINARY_DIR}/include/chemfiles/config.hpp"
)

include(GenerateExportHeader)
generate_export_header(chemfiles
    BASE_NAME CHFL
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/chemfiles/exports.hpp
)

#----------------------------------------------------------------------------------------#
# Installation configuration
#----------------------------------------------------------------------------------------#
set(INCLUDE_INSTALL_DIR "include")
install(TARGETS chemfiles EXPORT chemfiles-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY ${PROJECT_BINARY_DIR}/include/ DESTINATION include)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/chemfiles-config-version.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/chemfiles-config-version.cmake"
    @ONLY
)

# Install package files
set(PACKAGE_LOCATION lib${LIB_SUFFIX}/cmake/chemfiles)
install(EXPORT chemfiles-targets
  FILE chemfiles-targets.cmake
  DESTINATION ${PACKAGE_LOCATION}
)
install(FILES
    cmake/chemfiles-config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/chemfiles-config-version.cmake"
    DESTINATION ${PACKAGE_LOCATION}
    COMPONENT Devel
)

# uninstall target
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/uninstall.in.cmake"
    "${PROJECT_BINARY_DIR}/uninstall.cmake"
    @ONLY
)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${PROJECT_BINARY_DIR}/uninstall.cmake
)

if(CHFL_BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()

enable_testing()
if(CHFL_BUILD_TESTS)
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()
