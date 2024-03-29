unset(CMAKE_CXX_STANDARD)
unset(CMAKE_C_STANDARD)
# Do not set C standard for external code, as this make some
# symbols (such as strdup) undefined.
string(REPLACE "-std=c99" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# TODO: update all the external projects to use this and remove it from here
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

set(EXTERNAL_FLAGS "")

# Disable warnings about external code
if(MSVC)
    string(REGEX REPLACE "/Wall" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/Wall" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} /w")
else()
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} -w")
endif()

if(MSVC)
    # Do not use MSVC structured exceptions, and assume extern "C" function
    # do not throw
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} /EHsc")
endif()

if(${BUILD_SHARED_LIBS} AND NOT WIN32)
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} -fPIC")
endif()

if(WIN32)
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} -DWIN32")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTERNAL_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTERNAL_FLAGS}")

function(unpack_library _name_)
    set(_archive_ ${CMAKE_CURRENT_SOURCE_DIR}/${_name_}.tar.gz)
    file(SHA256 ${_archive_} _shasum_)
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${_name_}/${_shasum_}")
        message(STATUS "Unpacking ${_name_} sources")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${_name_}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xf ${_archive_}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${_name_}/${_shasum_}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()

    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${_name_}/.git")
        # .git directories can get huge and increase the size of this repository
        message(FATAL_ERROR "${_name_} archive contains a .git directory, please remove it")
    endif()

    # make sure cmake re-runs whenever the file changes
    get_directory_property(_previous_list_ CMAKE_CONFIGURE_DEPENDS)
    set_directory_properties(PROPERTIES CMAKE_CONFIGURE_DEPENDS "${_previous_list_};${_name_}.tar.gz")
endfunction()

function(external_library _name_)
    unpack_library(${_name_})
    add_subdirectory(
        ${CMAKE_CURRENT_BINARY_DIR}/${_name_}
        ${CMAKE_CURRENT_BINARY_DIR}/${_name_}
        EXCLUDE_FROM_ALL
    )
endfunction()

# Include directories for the dependencies
set(EXTERNAL_INCLUDES "")

# ==========
# zlib: https://github.com/chemfiles/zlib
# ==========

if(${CHFL_SYSTEM_ZLIB})
    message(WARNING "64-bit offsets are often not supported by the system zlib, consider using CHFL_SYSTEM_ZLIB=OFF")
    find_package(ZLIB REQUIRED)
    set(ZLIB_LIBRARIES ${ZLIB_LIBRARIES} PARENT_SCOPE)
    set(ZLIB_OBJECTS "" PARENT_SCOPE)
else()
    external_library(zlib)
    set(ZLIB_OBJECTS $<TARGET_OBJECTS:chemfiles_zlib> PARENT_SCOPE)
    set(ZLIB_LIBRARIES "" PARENT_SCOPE)
    set(ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/zlib)
    set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS} PARENT_SCOPE)
endif()
list(APPEND EXTERNAL_INCLUDES ${ZLIB_INCLUDE_DIRS})

# ==========
# lzma: https://github.com/chemfiles/xz
# ==========
if(${CHFL_SYSTEM_LZMA})
    find_package(LibLZMA REQUIRED)
    set(LIBLZMA_LIBRARY ${LIBLZMA_LIBRARY} PARENT_SCOPE)
    set(LZMA_OBJECTS "" PARENT_SCOPE)
else()
    external_library(lzma)
    set(LZMA_OBJECTS $<TARGET_OBJECTS:chemfiles_lzma> PARENT_SCOPE)
    set(LIBLZMA_LIBRARY "" PARENT_SCOPE)
    set(LIBLZMA_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/lzma/src/liblzma/api ${CMAKE_CURRENT_BINARY_DIR}/lzma)
    set(LIBLZMA_INCLUDE_DIRS ${LIBLZMA_INCLUDE_DIRS} PARENT_SCOPE)
endif()
list(APPEND EXTERNAL_INCLUDES ${LIBLZMA_INCLUDE_DIRS})

# ==========
# bzip2: https://github.com/chemfiles/bzip2
# ==========
if(${CHFL_SYSTEM_BZIP2})
    find_package(BZip2 REQUIRED)
    set(BZIP2_LIBRARIES ${BZIP2_LIBRARIES} PARENT_SCOPE)
    set(BZIP2_OBJECTS "" PARENT_SCOPE)
else()
    external_library(bzip2)
    set(BZIP2_OBJECTS $<TARGET_OBJECTS:chemfiles_bzip2> PARENT_SCOPE)
    set(BZIP2_LIBRARIES "" PARENT_SCOPE)
    set(BZIP2_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/bzip2/)
    set(BZIP2_INCLUDE_DIR ${BZIP2_INCLUDE_DIR} PARENT_SCOPE)
endif()
list(APPEND EXTERNAL_INCLUDES ${BZIP2_INCLUDE_DIR})

# ==========
# fmtlib: https://github.com/chemfiles/fmt
# ==========
external_library(fmt)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/fmt/include/)

# ==========
# pugixml: http://github.com/chemfiles/pugixml
# ==========
external_library(pugixml)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/pugixml/src)

# ==========
# TNG io: https://github.com/chemfiles/tng
# ==========
external_library(tng)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/tng/include)
target_include_directories(chemfiles_tng PUBLIC ${ZLIB_INCLUDE_DIRS})

# ==========
# VMD molfile: https://github.com/chemfiles/molfiles
# ==========
set(VMD_MOLFILE_PLUGINS
    gromacsplugin moldenplugin psfplugin
)

external_library(molfiles)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/molfiles/include)

# ==========
# mmtf-cpp: https://github.com/chemfiles/mmtf-cpp
# ==========
unpack_library(mmtf-cpp)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/mmtf-cpp/include)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/mmtf-cpp/msgpack-c/include)

# ==========
# gemmi: https://github.com/chemfiles/gemmi
# ==========
option(CHFL_DISABLE_GEMMI OFF)
mark_as_advanced(CHFL_DISABLE_GEMMI)

if (NOT ${CHFL_DISABLE_GEMMI})
    unpack_library(gemmi)
    list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/gemmi/include)
endif()

set(EXTERNAL_INCLUDES ${EXTERNAL_INCLUDES} PARENT_SCOPE)
