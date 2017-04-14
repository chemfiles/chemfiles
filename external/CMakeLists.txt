set(EXTERNAL_FLAGS "")
# Disable warnings about external code
if(MSVC)
    string(REGEX REPLACE "/Wall" "/w" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/Wall" "/w" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} /EHsc")
else()
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} -w")
endif()

if(${BUILD_SHARED_LIBS})
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} -fPIC")
endif()

if(WIN32)
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS}
set(WIN32")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTERNAL_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTERNAL_FLAGS}")

# ============================================================================ #

# cppformat library
include_directories(fmt)
file(GLOB FMT_SOURCES fmt/fmt/*.cc)
add_library(fmt OBJECT ${FMT_SOURCES})
set_target_properties(fmt PROPERTIES COMPILE_FLAGS ${EXTERNAL_FLAGS})

# ============================================================================ #

if(CHFL_SYSTEM_NETCDF)
    find_package(NetCDF REQUIRED)
    set(NETCDF_OBJECTS "")
else()
    # Build internal NetCDF library
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/netcdf/CMakeLists.txt")
        message(STATUS "Unpacking NetCDF sources")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/netcdf3-4.4.1.1.tar.gz
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf
    )

    add_library(netcdf3-chemfiles OBJECT
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/liblib/nc_initialize.c
    )
    target_include_directories(netcdf3-chemfiles PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/include
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/
    )

    # Hide NetCDF variables from CMake GUI
    mark_as_advanced(FORCE
        BUILDNAME BUILDNAME_PREFIX BUILDNAME_SUFFIX BUILD_TESTING BUILD_UTILITIES
        CHUNK_CACHE_NELEMS CHUNK_CACHE_PREEMPTION CHUNK_CACHE_SIZE
        DEFAULT_CHUNKS_IN_CACHE DEFAULT_CHUNK_SIZE ENABLE_CONVERSION_WARNINGS
        ENABLE_DAP ENABLE_DAP_GROUPS ENABLE_DISKLESS ENABLE_DOXYGEN ENABLE_EXAMPLES
        ENABLE_EXAMPLE_TESTS ENABLE_EXTRA_TESTS ENABLE_EXTREME_NUMBERS ENABLE_FFIO
        ENABLE_FSYNC ENABLE_HDF4 ENABLE_JNA ENABLE_LARGE_FILE_SUPPORT
        ENABLE_LARGE_FILE_TESTS ENABLE_LOGGING ENABLE_MMAP ENABLE_NETCDF_4
        ENABLE_PARALLEL4 ENABLE_PARALLEL_TESTS ENABLE_PNETCDF
        ENABLE_REMOTE_FORTRAN_BOOTSTRAP ENABLE_RPC ENABLE_STDIO ENABLE_TESTS
        ENABLE_V2_API HAVE_LIBM MAKEDIST_CMAKEFLAGS MAKEDIST_TARFLAGS
        MAX_DEFAULT_CACHE_SIZE NCIO_MINBLOCKSIZE NC_EXTRA_DEPS NC_FIND_SHARED_LIBS
        NC_M4 NETCDF_LIB_NAME PACKAGE TEMP_LARGE UNAME
    )

    set(NETCDF_INCLUDES
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/include
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/
        PARENT_SCOPE
    )
    set(NETCDF_OBJECTS
        $<TARGET_OBJECTS:netcdf3-chemfiles>
        $<TARGET_OBJECTS:netcdf3>
        $<TARGET_OBJECTS:dispatch>
        PARENT_SCOPE
    )
    set(NETCDF_LIBRARIES "")
endif()


# ============================================================================ #

# TNG library
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/tng/CMakeLists.txt")
    message(STATUS "Unpacking TNG sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/tng.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

add_subdirectory(
    ${CMAKE_CURRENT_BINARY_DIR}/tng
    ${CMAKE_CURRENT_BINARY_DIR}/tng
)

# Hide TNG variables from CMake GUI
mark_as_advanced(FORCE
    TNG_BUILD_COMPRESSION_TESTS
    TNG_BUILD_EXAMPLES
    TNG_BUILD_FORTRAN
    TNG_BUILD_OWN_ZLIB
    TNG_BUILD_TEST
)

# ============================================================================ #

# VMD Molfile plugins
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/molfiles/LICENSE")
    message(STATUS "Unpacking molfiles sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/molfiles.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

set(VMD_MOLFILE_PLUGINS_ENABLED
    molfiles/src/dcdplugin.c
    molfiles/src/gromacsplugin.cxx
    molfiles/src/pdbplugin.c
    molfiles/src/lammpsplugin.c
)

# These plugins are not yet activated, but can be compiled into chemfiles easily
set(VMD_MOLFILE_PLUGINS_DISABLED
    molfiles/src/abinitplugin.c
    molfiles/src/avsplugin.cxx
    molfiles/src/babelplugin.c
    molfiles/src/basissetplugin.c
    molfiles/src/bgfplugin.cxx
    molfiles/src/binposplugin.c
    molfiles/src/biomoccaplugin.cxx
    molfiles/src/brixplugin.cxx
    molfiles/src/carplugin.c
    molfiles/src/ccp4plugin.cxx
    molfiles/src/cifplugin.cxx
    molfiles/src/corplugin.c
    molfiles/src/cpmdplugin.c
    molfiles/src/crdplugin.c
    molfiles/src/cubeplugin.cxx
    molfiles/src/dlpolyplugin.c
    molfiles/src/dsn6plugin.cxx
    molfiles/src/dtrplugin.cxx
    molfiles/src/dxplugin.cxx
    molfiles/src/edmplugin.cxx
    molfiles/src/fs4plugin.cxx
    molfiles/src/gamessplugin.c
    molfiles/src/graspplugin.cxx
    molfiles/src/grdplugin.cxx
    molfiles/src/gridplugin.cxx
    molfiles/src/jsplugin.c
    molfiles/src/maeffplugin.cxx
    molfiles/src/mapplugin.cxx
    molfiles/src/mdfplugin.cxx
    molfiles/src/mol2plugin.cxx
    molfiles/src/moldenplugin.c
    molfiles/src/molemeshplugin.cxx
    molfiles/src/msmsplugin.cxx
    molfiles/src/namdbinplugin.c
    molfiles/src/netcdfplugin.c
    molfiles/src/offplugin.cxx
    molfiles/src/parm7plugin.cxx
    molfiles/src/parmplugin.cxx
    molfiles/src/pbeqplugin.cxx
    molfiles/src/phiplugin.cxx
    molfiles/src/pltplugin.cxx
    molfiles/src/pqrplugin.c
    molfiles/src/psfplugin.c
    molfiles/src/raster3dplugin.cxx
    molfiles/src/rst7plugin.c
    molfiles/src/situsplugin.cxx
    molfiles/src/spiderplugin.cxx
    molfiles/src/stlplugin.cxx
    molfiles/src/tinkerplugin.c
    molfiles/src/uhbdplugin.cxx
    molfiles/src/vasp5xdatcarplugin.c
    molfiles/src/vaspchgcarplugin.c
    molfiles/src/vaspoutcarplugin.c
    molfiles/src/vaspparchgplugin.c
    molfiles/src/vaspposcarplugin.c
    molfiles/src/vaspxdatcarplugin.c
    molfiles/src/vaspxmlplugin.c
    molfiles/src/vtfplugin.c
    molfiles/src/xbgfplugin.cxx
    molfiles/src/xsfplugin.cxx
    molfiles/src/xyzplugin.c
)

# These plugins do not compile or link
set(VMD_MOLFILE_PLUGINS_FAILED
    # error: no member named 'have_esp' in 'molfile_qm_metadata_t'
    molfiles/src/cpmdlogplugin.c
    # Needs to link the sqlite3 library
    molfiles/src/dmsplugin.cxx
    # error: no member named 'have_esp' in 'molfile_qm_metadata_t'
    molfiles/src/gaussianplugin.c
    # Needs to link to some XML parser
    molfiles/src/hoomdplugin.c
    # The code is not yet written ...
    molfiles/src/mmcif.cxx
    # Needs to link the TNGio library
    molfiles/src/tngplugin.cxx
    # Needs to link to TCL interpreter
    molfiles/src/webpdbplugin.c
)

set(MOLFILE_FLAGS "-DSTATIC_PLUGIN ${EXTERNAL_FLAGS}")
foreach(file IN ITEMS ${VMD_MOLFILE_PLUGINS_ENABLED})
    get_filename_component(plugin ${file} NAME_WE)
    set_source_files_properties(${file} PROPERTIES COMPILE_FLAGS "-DVMDPLUGIN=${plugin} ${MOLFILE_FLAGS}")
endforeach()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/molfiles/include)
add_library(molfiles OBJECT ${VMD_MOLFILE_PLUGINS_ENABLED})
