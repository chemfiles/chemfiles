# Download and compile boost from source
#
# The following variables may be set before calling this macro to
# modify the libraries to build:
#
#  BOOST_VERSION       - dot-seprated version number (like 1.58.0)
#  BOOST_MD5           - md5 sum of the official zip file to download
#  BOOST_PREFIX        - prefix for the installation of headers and libs
#  BOOST_LIBS          - list of libraries to build (example: "filesystem;system")

include(ExternalProject)

function(join values glue output)
  string(REPLACE ";" "${glue}" _tmp_str "${values}")
  set(${output} "${_tmp_str}" PARENT_SCOPE)
endfunction()

join("${BOOST_LIBS}" "," BOOST_LIBS_STR)
list(APPEND BOOST_BOOTSTRAP_ARGS "--with-libraries=${BOOST_LIBS_STR}")

if(WINDOWS)
    set(BOOST_BOOTSTRAP "bootstrap.bat")
else()
    set(BOOST_BOOTSTRAP "./bootstrap.sh")
    list(APPEND BOOST_BOOTSTRAP_ARGS "--prefix=${BOOST_PREFIX}")
endif()

# Get the b2 toolset corresponding to the current compiler.
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    if(APPLE)
        set(B2_TOOLSET "darwin")
    else()
        set(B2_TOOLSET "gcc")
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(B2_TOOLSET "gcc")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(B2_TOOLSET "intel")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(B2_TOOLSET "msvc")
endif()

set(BOOST_BUILD_ARGS "--prefix=${BOOST_PREFIX}" link=static threading=multi runtime-link=shared)
list(APPEND BOOST_BUILD_ARGS "toolset=${B2_TOOLSET}-cmake")

# We want position independent code.
IF("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    list(APPEND BOOST_BUILD_ARGS "cxxflags=-fPIC")
endif()

# Dealing with 32 and 64 bits OS
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    list(APPEND BOOST_BUILD_ARGS "address-model=64")
else()
    list(APPEND BOOST_BUILD_ARGS "address-model=32")
endif()

string(REPLACE "." "_" BOOST_VERSION_UNDERSCORE ${BOOST_VERSION})

ExternalProject_Add(
    Boost
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/boost
    URL http://downloads.sourceforge.net/project/boost/boost/${BOOST_VERSION}/boost_${BOOST_VERSION_UNDERSCORE}.zip
    URL_MD5 ${BOOST_MD5}
    CONFIGURE_COMMAND ${BOOST_BOOTSTRAP} ${BOOST_BOOTSTRAP_ARGS}
    BUILD_COMMAND ./b2 ${BOOST_BUILD_ARGS} install
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
    LOG_BUILD 1
)

# Set the exact compiler path, as needed
ExternalProject_Get_Property(Boost source_dir)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/BoostUserConfig.in.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/BoostUserConfig.cmake"
    IMMEDIATE @ONLY)

ExternalProject_Add_Step(Boost set_compiler
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/BoostUserConfig.cmake
    COMMENT "Setting Boost compiler"
    DEPENDEES configure
    DEPENDERS build
)

set(Boost_INCLUDE_DIRS ${BOOST_PREFIX}/include)
set(Boost_LIBRARIES)
foreach(boost_lib IN LISTS BOOST_LIBS)
    if(WINDOWS)
        # TODO
    else()
        list(APPEND Boost_LIBRARIES "${BOOST_PREFIX}/lib/libboost_${boost_lib}.a")
    endif()
endforeach()
