if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/boost/LICENSE_1_0.txt")
    message(STATUS "Unpacking Boost sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/boost.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

add_definitions("-DBOOST_ALL_NO_LIB")
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

# Disable warnings for boost
if(MSVC)
    string(REGEX REPLACE "/Wall" "/w" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()

file(GLOB_RECURSE boost_filesystem_src ${CMAKE_CURRENT_BINARY_DIR}/boost/src/*.cpp)
add_library(boost_filesystem STATIC ${boost_filesystem_src})
set_property(TARGET boost_filesystem PROPERTY POSITION_INDEPENDENT_CODE true)

target_include_directories(boost_filesystem BEFORE PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/boost/include)
