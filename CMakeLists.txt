cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/InitRules.cmake")

project(chemfiles C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")
if (POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # OSX RPATH handling
endif()
if (POLICY CMP0063)
    # Use of `<LANG>_VISIBILITY_PRESET` in OBJECT libraries
    cmake_policy(SET CMP0063 NEW)
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
option(CHFL_USE_WARNINGS "Compile the code with warnings (default in debug mode)" OFF)
option(CHFL_USE_INCLUDE_WHAT_YOU_USE "Compile the code with include-what-you-use warnings" OFF)
option(CHFL_SYSTEM_NETCDF "Use the system NetCDF instead of the internal one" OFF)
option(CHFL_SYSTEM_ZLIB "Use the system zlib instead of the internal one" OFF)
option(CHFL_SYSTEM_LZMA "Use the system lzma instead of the internal one" OFF)
option(CHFL_SYSTEM_BZIP2 "Use the system bzip2 instead of the internal one" OFF)

option(CHFL_BUILD_DOCTESTS "Build documentation tests as well as unit tests." ON)

set(LIB_INSTALL_DIR "lib" CACHE PATH "Path relative to CMAKE_INSTALL_PREFIX where to install libraries")
set(BIN_INSTALL_DIR "bin" CACHE PATH "Path relative to CMAKE_INSTALL_PREFIX where to install headers")
set(INCLUDE_INSTALL_DIR "include" CACHE PATH "Path relative to CMAKE_INSTALL_PREFIX where to install DLL/binaries")

mark_as_advanced(CHFL_BUILD_DOCTESTS)

# Set a default build type if none was specified
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    if("${CMAKE_BUILD_TYPE}" STREQUAL "" AND "${CMAKE_CONFIGURATION_TYPES}" STREQUAL "")
        message(STATUS "Setting build type to 'release' as none was specified.")
        set(CMAKE_BUILD_TYPE "release"
            CACHE STRING
            "Choose the type of build, options are: none(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) debug release relwithdebinfo minsizerel."
        FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS release debug relwithdebuginfo minsizerel none)
    endif()
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
include(CompilerFlags)

include(IncludeWhatYouUse)

if(${CHFL_USE_WARNINGS})
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

add_subdirectory(external)

# We need to use a separated library for non-dll-exported classes that have an
# unit test. The chemfiles_objects OBJECT library contains the code for files
# implementation and is linked into both the main chemfiles library and the
# unit tests.
file(GLOB_RECURSE chemfiles_objects_sources src/**.cpp)
add_library(chemfiles_objects OBJECT ${chemfiles_objects_sources})

target_include_directories(chemfiles_objects SYSTEM BEFORE PRIVATE ${EXTERNAL_INCLUDES})
target_include_directories(chemfiles_objects SYSTEM BEFORE PRIVATE ${ZLIB_INCLUDE_DIRS} ${LIBLZMA_INCLUDE_DIRS})

target_include_directories(chemfiles_objects PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)
target_include_directories(chemfiles_objects SYSTEM BEFORE PRIVATE ${EXTERNAL_INCLUDES})

target_compile_definitions(chemfiles_objects PRIVATE chemfiles_EXPORTS)
if(NOT ${CHFL_SYSTEM_LZMA})
    target_compile_definitions(chemfiles_objects PRIVATE LZMA_API_STATIC)
endif()

if(${BUILD_SHARED_LIBS})
    set_target_properties(chemfiles_objects PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif()

# Multiple different flags are required by different platform for 64-bit support
# These flags are also set in external libraries (zlib & lzma), but they might
# not be carried over when using system libraries.

# Default flag:
target_compile_definitions(chemfiles_objects PRIVATE -D_FILE_OFFSET_BITS=64)
# Required for zlib 64-bit support:
target_compile_definitions(chemfiles_objects PRIVATE -D_LARGEFILE64_SOURCE=1)
target_compile_definitions(chemfiles_objects PRIVATE -D_LFS64_LARGEFILE=1)

target_use_include_what_you_use(chemfiles_objects)

set(CHEMFILES_OBJECTS
    $<TARGET_OBJECTS:chemfiles_objects>
    $<TARGET_OBJECTS:molfiles>
    $<TARGET_OBJECTS:tng_io>
    $<TARGET_OBJECTS:fmt>
    $<TARGET_OBJECTS:pugixml>
    $<TARGET_OBJECTS:xdrfile>
    ${ZLIB_OBJECTS}
    ${LZMA_OBJECTS}
    ${BZIP2_OBJECTS}
    ${NETCDF_OBJECTS}
)

# Add the main chemfiles library
add_library(chemfiles ${CHEMFILES_OBJECTS})

if(WIN32)
    # MMTF (and thus chemfiles) uses endianness conversion function from ws2_32
    target_link_libraries(chemfiles ws2_32)
endif()

set_property(TARGET chemfiles PROPERTY VERSION ${CHEMFILES_VERSION_SHORT})
set_property(TARGET chemfiles PROPERTY SOVERSION ${CHEMFILES_VERSION_SHORT})

target_include_directories(chemfiles PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(chemfiles
    ${NETCDF_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${LIBLZMA_LIBRARY}
    ${BZIP2_LIBRARIES}
)
if(WIN32)
    target_link_libraries(chemfiles ws2_32)
endif()

configure_file (
  "${PROJECT_SOURCE_DIR}/include/chemfiles/config.in.h"
  "${PROJECT_BINARY_DIR}/include/chemfiles/config.h"
)

include(GenerateExportHeader)
generate_export_header(chemfiles
    BASE_NAME CHFL
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/chemfiles/exports.h
)

if(${CHFL_BUILD_DOCUMENTATION})
    add_subdirectory(doc)
endif()

enable_testing()
if(${CHFL_BUILD_TESTS})
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()

#----------------------------------------------------------------------------------------#
# Installation configuration
#----------------------------------------------------------------------------------------#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/chemfiles-config-version.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/chemfiles-config-version.cmake"
    @ONLY
)

install(TARGETS chemfiles
    EXPORT chemfiles-targets
    LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT Runtime
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Development
    RUNTIME DESTINATION ${BIN_INSTALL_DIR} COMPONENT Runtime
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/chemfiles-config.cmake.in"
    "${PROJECT_BINARY_DIR}/chemfiles-config.cmake"
    INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/chemfiles
)

install(EXPORT chemfiles-targets DESTINATION ${LIB_INSTALL_DIR}/cmake/chemfiles)
install(FILES "${PROJECT_BINARY_DIR}/chemfiles-config-version.cmake"
              "${PROJECT_BINARY_DIR}/chemfiles-config.cmake"
        DESTINATION ${LIB_INSTALL_DIR}/cmake/chemfiles)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${INCLUDE_INSTALL_DIR})
install(DIRECTORY ${PROJECT_BINARY_DIR}/include/ DESTINATION ${INCLUDE_INSTALL_DIR})

# uninstall target
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/uninstall.in.cmake"
    "${PROJECT_BINARY_DIR}/uninstall.cmake"
    @ONLY
)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${PROJECT_BINARY_DIR}/uninstall.cmake
)
