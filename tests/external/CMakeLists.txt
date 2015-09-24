set(BOOST_VERSION 1.58.0)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/boost/LICENSE_1_0.txt")
    message(STATUS "Unpacking Boost sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/boost-${BOOST_VERSION}.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

include_directories(BEFORE SYSTEM boost/include)
add_definitions("-DBOOST_ALL_NO_LIB")

file(GLOB_RECURSE boost_filesystem_src boost/src/*.cpp)
add_library(boost_filesystem STATIC ${boost_filesystem_src})
set_property(TARGET boost_filesystem PROPERTY VERSION ${BOOST_VERSION})
set_property(TARGET boost_filesystem PROPERTY POSITION_INDEPENDENT_CODE true)
