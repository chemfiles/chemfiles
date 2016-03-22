set(BOOST_VERSION 1.58.0)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/boost/LICENSE_1_0.txt")
    message(STATUS "Unpacking Boost sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/boost-${BOOST_VERSION}.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

set(CMAKE_CXX_FLAGS "")
add_definitions("-DBOOST_ALL_NO_LIB")
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS "-Wno-deprecated")

file(GLOB_RECURSE boost_filesystem_src boost/src/*.cpp)
add_library(boost_filesystem STATIC ${boost_filesystem_src})
set_property(TARGET boost_filesystem PROPERTY VERSION ${BOOST_VERSION})
set_property(TARGET boost_filesystem PROPERTY POSITION_INDEPENDENT_CODE true)

target_include_directories(boost_filesystem BEFORE PUBLIC boost/include)
