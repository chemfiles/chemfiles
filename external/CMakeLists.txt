unset(CMAKE_CXX_STANDARD)
unset(CMAKE_C_STANDARD)
# Do not set C standard for external code, as this make some
# symbols (such as strdup) undefined.
string(REPLACE "-std=c99" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

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
    find_package(ZLIB REQUIRED)
    set(ZLIB_LIBRARIES ${ZLIB_LIBRARIES} PARENT_SCOPE)
    set(ZLIB_OBJECTS "" PARENT_SCOPE)
else()
    external_library(zlib)
    set(ZLIB_OBJECTS $<TARGET_OBJECTS:zlib> PARENT_SCOPE)
    set(ZLIB_LIBRARIES "" PARENT_SCOPE)
    set(ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/zlib)
    set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS} PARENT_SCOPE)
endif()
list(APPEND EXTERNAL_INCLUDES ${ZLIB_INCLUDE_DIRS})

# ==========
# lzma: https://github.com/chemfiles/lzma
# ==========
if(${CHFL_SYSTEM_LZMA})
    find_package(LibLZMA REQUIRED)
    set(LZMA_OBJECTS "" PARENT_SCOPE)
else()
    external_library(lzma)
    set(LZMA_OBJECTS $<TARGET_OBJECTS:lzma> PARENT_SCOPE)
    set(LIBLZMA_LIBRARY "" PARENT_SCOPE)
    set(LIBLZMA_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/lzma/liblzma/api ${CMAKE_CURRENT_BINARY_DIR}/lzma)
    set(LIBLZMA_INCLUDE_DIRS ${LIBLZMA_INCLUDE_DIRS} PARENT_SCOPE)
endif()
list(APPEND EXTERNAL_INCLUDES ${LIBLZMA_INCLUDE_DIRS})

# ==========
# bzip2: https://github.com/chemfiles/bzip2
# ==========
if(${CHFL_SYSTEM_BZIP2})
    find_package(BZip2 REQUIRED)
    set(BZIP2_OBJECTS "" PARENT_SCOPE)
else()
    external_library(bzip2)
    set(BZIP2_OBJECTS $<TARGET_OBJECTS:bzip2> PARENT_SCOPE)
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

# Hide FMT variables from CMake GUI
mark_as_advanced(FORCE
    FMT_DOC
    FMT_INSTALL
    FMT_PEDANTIC
    FMT_TEST
    FMT_WERROR
)

# ==========
# toml11
# ==========
unpack_library(toml11)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/toml11)

# ==========
# netcdf: https://github.com/chemfiles/netcdf-c
# ==========
if(${CHFL_SYSTEM_NETCDF})
    find_package(NetCDF REQUIRED)
    set(NETCDF_OBJECTS "" PARENT_SCOPE)
else()
    external_library(netcdf)

    set(NETCDF_INCLUDES
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/include
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf
    )
    set(NETCDF_OBJECTS $<TARGET_OBJECTS:netcdf> PARENT_SCOPE)
    set(NETCDF_LIBRARIES "" PARENT_SCOPE)
    if(${EMSCRIPTEN})
        set(NETCDF_OBJECTS_TESTS "" PARENT_SCOPE)
    else()
        set(NETCDF_OBJECTS_TESTS ${NETCDF_OBJECTS} PARENT_SCOPE)
    endif()
endif()

list(APPEND EXTERNAL_INCLUDES ${NETCDF_INCLUDES})

# ==========
# TNG io: https://github.com/chemfiles/tng
# ==========
external_library(tng)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/tng/include)
target_include_directories(tng_io PUBLIC ${ZLIB_INCLUDE_DIRS})

# Hide TNG variables from CMake GUI
mark_as_advanced(FORCE
    TNG_BUILD_COMPRESSION_TESTS
    TNG_BUILD_EXAMPLES
    TNG_BUILD_FORTRAN
    TNG_BUILD_OWN_ZLIB
    TNG_BUILD_TEST
)

# ==========
# VMD molfile: https://github.com/chemfiles/molfiles
# ==========
set(VMD_MOLFILE_PLUGINS
    dcdplugin gromacsplugin lammpsplugin moldenplugin
)

external_library(molfiles)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/molfiles/include)

# ==========
# Plugins not yet enabled but that should compile without issue:
# ==========

# abinitplugin carplugin cpmdplugin  gamessplugin mdfplugin msmsplugin
# namdbinplugin parm7plugin parmplugin vasp5xdatcarplugin vaspchgcarplugin
# vaspoutcarplugin vaspparchgplugin vaspposcarplugin vaspxdatcarplugin
# vaspxmlplugin dlpolyplugin tinkerplugin
#
# avsplugin babelplugin basissetplugin bgfplugin binposplugin biomoccaplugin
# brixplugin ccp4plugin cifplugin corplugin crdplugin cubeplugin dsn6plugin
# dtrplugin dxplugin edmplugin fs4plugin graspplugin grdplugin gridplugin
# jsplugin maeffplugin mapplugin molemeshplugin offplugin pbeqplugin phiplugin
# pltplugin pqrplugin psfplugin raster3dplugin rst7plugin situsplugin
# spiderplugin stlplugin uhbdplugin vtfplugin xbgfplugin xsfplugin

# ==========
# Plugins with a chemfiles specific implementation
# ==========
#     xyzplugin pdbplugin tngplugin netcdfplugin mol2plugin gromacsplugin (.gro only)

# ==========
# These plugins do not compile or link
# ==========
# error: no member named 'have_esp' in 'molfile_qm_metadata_t'
#       cpmdlogplugin
# Needs to link the sqlite3 library
#       dmsplugin
# error: no member named 'have_esp' in 'molfile_qm_metadata_t'
#       gaussianplugin
# Needs to link to some XML parser
#       hoomdplugin
# The code is not yet written ...
#       mmcif
# Needs to link to TCL interpreter
#       webpdbplugin

# ==========
# mmtf-cpp format: https://github.com/rcsb/mmtf-cpp
# ==========
unpack_library(mmtf-cpp)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/mmtf-cpp/include )
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/mmtf-cpp/msgpack-c/include )

# ==========
# pugixml: http://github.com/chemfiles/pugixml
# ==========
unpack_library(pugixml)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/pugixml/src)
add_library(pugixml OBJECT ${CMAKE_CURRENT_BINARY_DIR}/pugixml/src/pugixml.cpp)

# ==========
# xdrfile
# ==========
external_library(xdrfile)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/xdrfile/include)

set(EXTERNAL_INCLUDES ${EXTERNAL_INCLUDES} PARENT_SCOPE)
