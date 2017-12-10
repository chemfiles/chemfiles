add_library(test_helpers STATIC helpers.cpp)
target_include_directories(test_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# We can not direcly link to chemfiles, but we still need it's headers
target_include_directories(test_helpers PUBLIC $<TARGET_PROPERTY:chemfiles,INCLUDE_DIRECTORIES>)

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/boost/LICENSE_1_0.txt")
    message(STATUS "Unpacking Boost sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/boost.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

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

target_include_directories(boost_filesystem SYSTEM BEFORE PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/boost/include)
target_compile_definitions(boost_filesystem PUBLIC BOOST_ALL_NO_LIB)

target_link_libraries(test_helpers boost_filesystem)
