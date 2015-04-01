function(chrp_cpp_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_executable(${_name_} ${_file_} $<TARGET_OBJECTS:catch>)
    target_link_libraries(${_name_} chemharp)
    set_property(TARGET ${_name_} PROPERTY CXX_STANDARD 11)
    add_test(${_name_} ${_name_})
endfunction()

if(BUILD_TESTS)
    include_directories(${CMAKE_SOURCE_DIR}/include)
    include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/catch)
    add_library(catch OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/catch/catch.cpp)

    add_library(libtest SHARED ${CMAKE_CURRENT_SOURCE_DIR}/test.c)
    set_target_properties(libtest PROPERTIES PREFIX "" SUFFIX ".so")

    file(GLOB all_test_files
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/files/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/formats/*.cpp
    )

    add_definitions("-DSRCDIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
    add_definitions("-DBINDIR=\"${CMAKE_CURRENT_BINARY_DIR}\"")

    foreach(test_file IN LISTS all_test_files)
        chrp_cpp_test(${test_file})
    endforeach(test_file)
endif()
