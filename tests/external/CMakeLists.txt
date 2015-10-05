include_directories(BEFORE SYSTEM ${BOOST_DIR})

message(STATUS ${BOOST_DIR})

file(GLOB_RECURSE boost_system_src ${BOOST_DIR}/libs/system/src/**.cpp)
add_library(boost_system STATIC ${boost_system_src})
set_property(TARGET boost_system PROPERTY VERSION ${BOOST_VERSION})
set_property(TARGET boost_system PROPERTY POSITION_INDEPENDENT_CODE true)

file(GLOB_RECURSE boost_filesystem_src ${BOOST_DIR}/libs/filesystem/src/**.cpp)
add_library(boost_filesystem STATIC ${boost_filesystem_src})
set_property(TARGET boost_filesystem PROPERTY VERSION ${BOOST_VERSION})
set_property(TARGET boost_filesystem PROPERTY POSITION_INDEPENDENT_CODE true)
